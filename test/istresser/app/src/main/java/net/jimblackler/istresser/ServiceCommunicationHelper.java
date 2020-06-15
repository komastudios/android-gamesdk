package net.jimblackler.istresser;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.Build.VERSION;
import android.os.Build.VERSION_CODES;
import android.util.Log;

class ServiceCommunicationHelper {
  private static final String ACTION_TYPE = "action";
  private static final String QUERY_ACTION = "query";
  private static final String ALLOCATE_MEMORY_ACTION = "allocate";
  private static final String FREE_MEMORY_ACTION = "free";
  private static final String FIRST_TIME = "first_time";

  private static final String TAG = MainActivity.class.getSimpleName();

  static final String CRASHED_BEFORE = "crashed_before";
  static final String TOTAL_MEMORY_MB = "total_memory_mb";

  private Context context;
  private boolean isFirstTime;

  ServiceCommunicationHelper(Context context) {
    this.context = context;
    this.isFirstTime = true;
  }

  void allocateServerMemory(int megabytes) {
    if (VERSION.SDK_INT >= VERSION_CODES.O) {
      Intent intent = new Intent();
      intent.setComponent(new ComponentName(
          "com.google.gamesdk.grabber", "com.google.gamesdk.grabber.MemoryPressureService"));
      intent.putExtra(ACTION_TYPE, ALLOCATE_MEMORY_ACTION);
      intent.putExtra(TOTAL_MEMORY_MB, megabytes);
      intent.putExtra(FIRST_TIME, isFirstTime);
      isFirstTime = false;
      context.startForegroundService(intent);
    } else {
      Log.w(TAG, "Unable to request server memory, version is below 26");
    }
  }

  void queryServerMemory() {
    if (VERSION.SDK_INT >= VERSION_CODES.O) {
      Intent intent = new Intent();
      intent.setComponent(new ComponentName(
          "com.google.gamesdk.grabber", "com.google.gamesdk.grabber.MemoryPressureService"));
      intent.putExtra(ACTION_TYPE, QUERY_ACTION);
      intent.putExtra(FIRST_TIME, isFirstTime);
      isFirstTime = false;
      context.startForegroundService(intent);
    } else {
      Log.w(TAG, "Unable to query server memory, version is below 26");
    }
  }

  void freeServerMemory() {
    if (VERSION.SDK_INT >= VERSION_CODES.O) {
      Intent intent = new Intent();
      intent.setComponent(new ComponentName(
          "com.google.gamesdk.grabber", "com.google.gamesdk.grabber.MemoryPressureService"));
      intent.putExtra(ACTION_TYPE, FREE_MEMORY_ACTION);
      intent.putExtra(FIRST_TIME, isFirstTime);
      isFirstTime = false;
      context.startForegroundService(intent);
    } else {
      Log.w(TAG, "Unable to free server memory, version is below 26");
    }
  }
}
