package com.google.gamesdk.grabber;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

/** Service to mock memory pressure */
public class MemoryPressureService extends Service {

  public static final String TOTAL_MEMORY_MB = "total_memory_mb";
  public static final String REQUEST_SUCCEEDED = "success";
  public static final String ACTION_TYPE = "action";
  public static final String ALLOCATE_MEMORY_ACTION = "allocate";
  public static final String FREE_MEMORY_ACTION = "free";
  public static final String NOTIFICATION_CHANNEL = "memory";
  public static final String FIRST_TIME = "first_time";
  public static final String CRASHED_BEFORE = "crashed_before";
  Notification notification;
  int mb = 0;
  boolean crashedBefore = true;

  @Override
  public int onStartCommand(Intent intent, int flags, int startId) {
    String action = intent.getStringExtra(ACTION_TYPE);

    if (intent.getBooleanExtra(FIRST_TIME, false)) {
      crashedBefore = false;
    }
    NotificationManager notificationManager =
        (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
    notificationManager.createNotificationChannel(
        new NotificationChannel(
            NOTIFICATION_CHANNEL, NOTIFICATION_CHANNEL, NotificationManager.IMPORTANCE_HIGH));
    notification =
        new Notification.Builder(this, NOTIFICATION_CHANNEL)
            .setSmallIcon(R.drawable.ic_launcher_background)
            .setContentTitle("Last memory request proceeding")
            .setContentText("Total memory used: " + mb + " megabytes")
            .build();
    startForeground(startId, notification);

    boolean success = false;

    if (ALLOCATE_MEMORY_ACTION.equals(action)) {
      int newMb = intent.getIntExtra(TOTAL_MEMORY_MB, 0);
      success = true;
      while (newMb > 0) {
        if (nativeConsume(1024 * 1024)) {
          newMb--;
          mb++;
        } else {
          success = false;
          break;
        }
      }

    } else if (FREE_MEMORY_ACTION.equals(action)) {
      success = true;
      freeAll();
      mb = 0;
    }

    Intent returnIntent = new Intent("com.google.gamesdk.grabber.RETURN");
    returnIntent.putExtra(TOTAL_MEMORY_MB, mb);
    returnIntent.putExtra(REQUEST_SUCCEEDED, success);
    returnIntent.putExtra(ACTION_TYPE, action);
    returnIntent.putExtra(CRASHED_BEFORE, crashedBefore);
    sendBroadcast(returnIntent);

    return Service.START_NOT_STICKY;
  }

  @Override
  public IBinder onBind(Intent intent) {
    return null;
  }

  public native boolean nativeConsume(int bytes);

  public native void freeAll();

  static {
    System.loadLibrary("grabber");
  }
}
