package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getMemoryQuantity;
import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getOrDefault;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.DeadObjectException;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectReader;
import java.io.IOException;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.atomic.AtomicReference;
import org.json.JSONObject;

/**
 * A system to discover the limits of the device using a service running in a different process.
 * The limits are compatible with the lab test stress results.
 */
class OnDeviceStressTester {
  // Message codes for inter-process communication.
  public static final int GET_BASELINE_METRICS = 1;
  public static final int GET_BASELINE_METRICS_RETURN = 2;
  public static final int OCCUPY_MEMORY = 3;
  public static final int OCCUPY_MEMORY_OK = 4;
  public static final int OCCUPY_MEMORY_FAILED = 5;
  public static final int SERVICE_CHECK_FREQUENCY = 250;
  public static final int NO_PID_TIMEOUT = 1000 * 2;
  private static final long NO_RESPONSE_TIMEOUT = 5000;
  private static final String TAG = OnDeviceStressTester.class.getSimpleName();
  private final AtomicReference<ServiceConnection> serviceConnection = new AtomicReference<>();
  private final AtomicReference<Timer> timeoutTimer = new AtomicReference<>();
  private final ObjectReader objectReader = new ObjectMapper().reader();

  /**
   * Construct an on-device stress tester.
   *
   * @param context  The current context.
   * @param params   The configuration parameters for the tester.
   * @param consumer The handler to call back when the test is complete.
   */
  OnDeviceStressTester(Context context, Map<String, Object> params, Consumer consumer) {
    Intent launchIntent = new Intent(context, StressService.class);
    launchIntent.putExtra("params", JSONObject.wrap(params).toString());
    serviceConnection.set(new ServiceConnection() {
      private final AtomicReference<Timer> serviceWatcherTimer = new AtomicReference<>();
      private Map<String, Object> baseline;
      private Map<String, Object> limit;
      private int connectionCount;

      public void onServiceConnected(ComponentName name, IBinder service) {
        if (connectionCount > 0) {
          // If the service connects after the first time it is because the service was killed
          // on the first occasion.
          stopService(false);
          return;
        }
        connectionCount++;
        Messenger messenger = new Messenger(service);

        {
          // Ask the service for its pid and baseline metrics.
          Message message = Message.obtain(null, GET_BASELINE_METRICS, 0, 0);
          message.replyTo = new Messenger(new Handler(new Handler.Callback() {
            @Override
            public boolean handleMessage(Message msg) {
              if (msg.what == GET_BASELINE_METRICS_RETURN) {
                Bundle bundle = (Bundle) msg.obj;
                try {
                  baseline = objectReader.readValue(bundle.getString("metrics"), Map.class);
                } catch (IOException e) {
                  throw new IllegalStateException(e);
                }
                int servicePid = msg.arg1;
                Timer newTimer = new Timer();
                Timer oldServiceTimer = serviceWatcherTimer.getAndSet(newTimer);
                if (oldServiceTimer != null) {
                  Log.w(TAG, "Service watcher timer set twice");
                  oldServiceTimer.cancel();
                }
                newTimer.scheduleAtFixedRate(new TimerTask() {
                  private long lastSawPid;
                  @Override
                  public void run() {
                    int oomScore = Utils.getOomScore(servicePid);
                    if (oomScore == -1) {
                      if (lastSawPid > 0
                          && System.currentTimeMillis() - lastSawPid > NO_PID_TIMEOUT) {
                        // The prolonged lack of OOM score is almost certainly because the process
                        // was killed.
                        stopService(false);
                      }
                    } else {
                      lastSawPid = System.currentTimeMillis();
                    }
                  }
                }, 0, SERVICE_CHECK_FREQUENCY);
              } else {
                throw new MemoryAdvisorException("Unexpected return code " + msg.what);
              }
              return true;
            }
          }));
          sendMessage(message, messenger);
        }
        allocateMemory(messenger);
      }

      private void allocateMemory(Messenger messenger) {
        int toAllocate = (int) getMemoryQuantity(getOrDefault(
            (Map<String, Object>) params.get("onDeviceStressTest"), "segmentSize", "4M"));
        Message message = Message.obtain(null, OCCUPY_MEMORY, toAllocate, 0);
        message.replyTo = new Messenger(new Handler(new Handler.Callback() {
          @Override
          public boolean handleMessage(Message msg) {
            ServiceConnection connection = serviceConnection.get();
            if (connection == null) {
              Log.e(TAG, "Message received although service had been stopped");
              return true;
            }
            // The service's return message includes metrics.
            Bundle bundle = (Bundle) msg.obj;

            Map<String, Object> received = null;
            try {
              received = objectReader.readValue(bundle.getString("metrics"), Map.class);
            } catch (IOException e) {
              throw new IllegalStateException(e);
            }
            long limitTime = limit == null
                ? Long.MIN_VALUE
                : ((Number) ((Map<String, Object>) limit.get("meta")).get("time")).longValue();
            long metaTime =
                ((Number) ((Map<String, Object>) received.get("meta")).get("time")).longValue();
            if (limitTime < metaTime) {
              consumer.progress(received);
              // Metrics can be received out of order. The latest only is recorded as the limit.
              limit = received;
            }
            if (msg.what == OCCUPY_MEMORY_FAILED) {
              stopService(false);
            } else if (msg.what == OCCUPY_MEMORY_OK) {
              allocateMemory(messenger);
            } else {
              throw new MemoryAdvisorException("Unexpected return code " + msg.what);
            }
            return true;
          }
        }));
        sendMessage(message, messenger);
      }

      public void onServiceDisconnected(ComponentName name) {
      }

      private void sendMessage(Message message, Messenger messenger) {
        Timer newTimer = new Timer();
        Timer oldTimer = timeoutTimer.getAndSet(newTimer);
        if (oldTimer != null) {
          oldTimer.cancel();
        }
        newTimer.schedule(new TimerTask() {
          @Override
          public void run() {
            Log.w(TAG, "Timed out: stopping service");
            stopService(true);
          }
        }, NO_RESPONSE_TIMEOUT);
        try {
          messenger.send(message);
        } catch (DeadObjectException e) {
          // Complete the test because the messenger could not find the service - probably because
          // it was terminated by the system.
          stopService(false);
        } catch (RemoteException e) {
          Log.e(TAG, "Error sending message", e);
        }
      }

      private void stopService(boolean timedOut) {
        Timer oldTimer = timeoutTimer.getAndSet(null);
        if (oldTimer != null) {
          oldTimer.cancel();
        }
        ServiceConnection connection = serviceConnection.getAndSet(null);
        if (connection != null) {
          Log.i(TAG, "Unbinding service");
          Timer oldServiceTimer = serviceWatcherTimer.getAndSet(null);
          if (oldServiceTimer != null) {
            oldServiceTimer.cancel();
          }
          context.unbindService(connection);
          if (context.stopService(launchIntent)) {
            Log.i(TAG, "Service reported stopped");
          } else {
            Log.i(TAG, "Service could not be stopped");
          }
          consumer.accept(baseline, limit, timedOut);
        }
      }
    });

    context.startService(launchIntent);
    context.bindService(launchIntent, serviceConnection.get(), Context.BIND_AUTO_CREATE);
  }

  interface Consumer {
    void progress(Map<String, Object> metrics);
    void accept(Map<String, Object> baseline, Map<String, Object> limit, boolean timedOut);
  }
}
