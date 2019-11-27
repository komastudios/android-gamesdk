package net.jimblackler.istresser;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;

class ServiceCommunicationHelper {

  private static final String ACTION_TYPE = "action";
  private static final String QUERY_ACTION = "query";
  private static final String ALLOCATE_MEMORY_ACTION = "allocate";
  private static final String FREE_MEMORY_ACTION = "free";
  private static final String FIRST_TIME = "first_time";

  static final String CRASHED_BEFORE = "crashed_before";
  static final String TOTAL_MEMORY_MB = "total_memory_mb";

  private Context context;
  private boolean isFirstTime;

  ServiceCommunicationHelper(Context context) {
    this.context = context;
    this.isFirstTime = true;
  }

  void allocateServerMemory(int megaBytes) {

    Intent intent = new Intent();
    intent.setComponent(
        new ComponentName(
            "experimental.users.bkaya.grabber",
            "experimental.users.bkaya.grabber.MemoryPressureService"));
    intent.putExtra(ACTION_TYPE, ALLOCATE_MEMORY_ACTION);
    intent.putExtra(TOTAL_MEMORY_MB, megaBytes);
    intent.putExtra(FIRST_TIME, isFirstTime);
    isFirstTime = false;
    context.startForegroundService(intent);
  }

  void queryServerMemory() {
    Intent intent = new Intent();
    intent.setComponent(
        new ComponentName(
            "experimental.users.bkaya.grabber",
            "experimental.users.bkaya.grabber.MemoryPressureService"));
    intent.putExtra(ACTION_TYPE, QUERY_ACTION);
    intent.putExtra(FIRST_TIME, isFirstTime);
    isFirstTime = false;
    context.startForegroundService(intent);
  }

  void freeServerMemory() {
    Intent intent = new Intent();
    intent.setComponent(
        new ComponentName(
            "experimental.users.bkaya.grabber",
            "experimental.users.bkaya.grabber.MemoryPressureService"));
    intent.putExtra(ACTION_TYPE, FREE_MEMORY_ACTION);
    intent.putExtra(FIRST_TIME, isFirstTime);
    isFirstTime = false;
    context.startForegroundService(intent);
  }
}
