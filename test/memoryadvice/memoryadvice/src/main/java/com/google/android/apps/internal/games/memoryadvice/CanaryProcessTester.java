package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice.Utils.getMemoryQuantity;
import static com.google.android.apps.internal.games.memoryadvice.Utils.getOrDefault;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.util.Log;
import org.json.JSONObject;

/**
 * A method of predicting critical memory pressure ahead of time, by creating a service that
 * allocates a significant quantity of memory, that will be killed by the system when memory is
 * becoming scarce. This occurrence can be detected easily as the connection will be disconnected.
 */
class CanaryProcessTester {
  private static final String TAG = CanaryProcessTester.class.getSimpleName();

  private final Context context;
  private final JSONObject params;
  private ServiceConnection serviceConnection;

  CanaryProcessTester(Context context, JSONObject params) {
    this.context = context;
    this.params = params;
    reset();
  }

  /**
   * Returns true if the connection has been lost, and therefore a low memory situation is likely
   * to be in effect.
   * @return If a low memory situation is likely to be in effect.
   */
  public boolean warning() {
    return serviceConnection == null;
  }

  public void reset() {
    Intent launchIntent = new Intent(context, CanaryService.class);
    launchIntent.putExtra("memory", (int) getMemoryQuantity(getOrDefault(params, "memory", "32M")));
    serviceConnection = new ServiceConnection() {
      public void onServiceConnected(ComponentName name, IBinder service) {
        Log.i(TAG, "onServiceConnected");
      }

      public void onServiceDisconnected(ComponentName name) {
        Log.i(TAG, "onServiceDisconnected");
        context.unbindService(serviceConnection);
        serviceConnection = null;
      }
    };

    context.startService(launchIntent);
    context.bindService(launchIntent, serviceConnection, Context.BIND_AUTO_CREATE);
  }
}
