package com.google.android.apps.internal.games.memoryadvice;

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
import java.util.Timer;
import java.util.TimerTask;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * A system to discover the limits of the device using a service running in a different process.
 * The limits are compatible with the lab test stress results.
 */
class OnDeviceStressTester {
  private static final String TAG = OnDeviceStressTester.class.getSimpleName();

  // Message codes for inter-process communication.
  public static final int GET_BASELINE_METRICS = 1;
  public static final int GET_BASELINE_METRICS_RETURN = 2;
  public static final int OCCUPY_MEMORY = 3;
  public static final int OCCUPY_MEMORY_OK = 4;
  public static final int OCCUPY_MEMORY_FAILED = 5;

  public static final int SERVICE_CHECK_FREQUENCY = 250;

  private final ServiceConnection serviceConnection;

  /**
   * Construct an on-device stress tester.
   * @param context The current context.
   * @param params The configuration parameters for the tester.
   * @param consumer The handler to call back when the test is complete.
   */
  OnDeviceStressTester(final Context context, final JSONObject params, final Consumer consumer) {
    final Intent launchIntent = new Intent(context, StressService.class);
    serviceConnection = new ServiceConnection() {
      private Messenger messenger;
      private JSONObject baseline;
      private JSONObject limit;
      private Timer serviceWatcherTimer;
      private int connectionCount;

      public void onServiceConnected(ComponentName name, IBinder service) {
        if (connectionCount > 0) {
          // If the service connects after the first time it is because the service was killed
          // on the first occasion.
          stopService();
          return;
        }
        connectionCount++;
        messenger = new Messenger(service);

        {
          // Ask the service for its pid and baseline metrics.
          Message message = Message.obtain(null, GET_BASELINE_METRICS, 0, 0);
          message.replyTo = new Messenger(new Handler(new Handler.Callback() {
            @Override
            public boolean handleMessage(Message msg) {
              if (msg.what == GET_BASELINE_METRICS_RETURN) {
                Bundle bundle = (Bundle) msg.obj;
                try {
                  baseline = new JSONObject(bundle.getString("metrics"));
                } catch (JSONException e) {
                  throw new MemoryAdvisorException(e);
                }
                final int servicePid = msg.arg1;
                serviceWatcherTimer = new Timer();
                serviceWatcherTimer.scheduleAtFixedRate(new TimerTask() {
                  @Override
                  public void run() {
                    int oomScore = Utils.getOomScore(servicePid);
                    if (oomScore == -1) {
                      // The lack of OOM score is almost certainly because the process was killed.
                      stopService();
                    }
                  }
                }, 0, SERVICE_CHECK_FREQUENCY);
              } else {
                throw new MemoryAdvisorException("Unexpected return code " + msg.what);
              }
              return true;
            }
          }));
          sendMessage(message);
        }
        {
          int toAllocate =
              (int) Utils.getMemoryQuantity(Utils.getOrDefault(params, "segmentSize", "4M"));
          Message message = Message.obtain(null, OCCUPY_MEMORY, toAllocate, 0);
          message.replyTo = new Messenger(new Handler(new Handler.Callback() {
            @Override
            public boolean handleMessage(Message msg) {
              // The service's return message includes metrics.
              Bundle bundle = (Bundle) msg.obj;
              try {
                JSONObject received = new JSONObject(bundle.getString("metrics"));
                if (limit == null
                    || limit.getJSONObject("meta").getLong("time")
                        < received.getJSONObject("meta").getLong("time")) {
                  // Metrics can be received out of order. The latest only is recorded as the limit.
                  limit = received;
                }
              } catch (JSONException e) {
                throw new MemoryAdvisorException(e);
              }
              if (msg.what == OCCUPY_MEMORY_FAILED) {
                stopService();
              } else if (msg.what != OCCUPY_MEMORY_OK) {
                throw new MemoryAdvisorException("Unexpected return code " + msg.what);
              }
              return true;
            }
          }));
          sendMessage(message);
        }
      }

      public void onServiceDisconnected(ComponentName name) {
        messenger = null;
      }

      private void sendMessage(Message message) {
        try {
          messenger.send(message);
        } catch (DeadObjectException e) {
          // Complete the test because the messenger could not find the service - probably because
          // it was terminated by the system.
          stopService();
        } catch (RemoteException e) {
          Log.e(TAG, "Error sending message", e);
        }
      }

      private void stopService() {
        serviceWatcherTimer.cancel();
        context.stopService(launchIntent);
        context.unbindService(serviceConnection);
        consumer.accept(baseline, limit);
      }
    };

    context.startService(launchIntent);
    context.bindService(launchIntent, serviceConnection, Context.BIND_AUTO_CREATE);
  }

  abstract static class Consumer { abstract void accept(JSONObject baseline, JSONObject limit); }
}
