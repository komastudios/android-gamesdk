package com.google.android.apps.internal.games.memoryadvice;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

/**
 * The service associated with the CanaryProcessTester. It consumes memory specifically to make it
 * a target for the platform to kill when memory is becoming low.
 */
public class CanaryService extends Service {
  private static final String TAG = CanaryService.class.getSimpleName();

  @Override
  public int onStartCommand(Intent intent, int flags, int startId) {
    return START_NOT_STICKY;
  }

  @Override
  public IBinder onBind(Intent intent) {
    int memory = intent.getIntExtra("memory", 0);
    if (!TryAllocTester.occupyMemory(memory)) {
      Log.e(TAG, "occupyMemory failed to allocate");
    }
    return new Binder();
  }
}
