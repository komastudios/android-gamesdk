package net.jimblackler.istresser;

import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Debug;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;

import com.google.common.collect.HashMultiset;
import com.google.common.collect.Lists;
import com.google.common.collect.Multiset;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.FileNotFoundException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Objects;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {
  private static final String TAG = MainActivity.class.getSimpleName();

  static {
    System.loadLibrary("native-lib");
  }

  private ArrayList<byte[]> data = Lists.newArrayList();

  private Multiset<Integer> onTrims = HashMultiset.create();
  private long nativeAllocatedByTest;
  private long recordNativeHeapAllocatedSize;
  private int totalTrims = 0;
  private PrintStream resultsStream = System.out;
  private boolean pauseAllocation = false;
  private long startTime;

  private static String memoryString(long bytes) {
    return String.format(Locale.getDefault(), "%.1f MB", (float) bytes / (1024 * 1024));
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    try {
      JSONObject report = new JSONObject();
      report.put("version", 5);
      Intent launchIntent = getIntent();
      if ("com.google.intent.action.TEST_LOOP".equals(launchIntent.getAction())) {
        Uri logFile = launchIntent.getData();
        if (logFile != null) {
          String logPath = logFile.getEncodedPath();
          if (logPath != null) {
            Log.i(TAG, "Log file " + logPath);
            try {
              resultsStream = new PrintStream(
                  Objects.requireNonNull(getContentResolver().openOutputStream(logFile)));
            } catch (FileNotFoundException e) {
              e.printStackTrace();
            }
          }
        }
        int scenario = launchIntent.getIntExtra("scenario", 0);
        report.put("scenario", scenario);
      }
      JSONObject build = new JSONObject();

      build.put("ID", Build.ID);
      build.put("DISPLAY", Build.DISPLAY);
      build.put("PRODUCT", Build.PRODUCT);
      build.put("DEVICE", Build.DEVICE);
      build.put("BOARD", Build.BOARD);
      build.put("MANUFACTURER", Build.MANUFACTURER);
      build.put("BRAND", Build.BRAND);
      build.put("MODEL", Build.MODEL);
      build.put("BOOTLOADER", Build.BOOTLOADER);
      build.put("HARDWARE", Build.HARDWARE);
      build.put("BASE_OS", Build.VERSION.BASE_OS);
      build.put("CODENAME", Build.VERSION.CODENAME);
      build.put("INCREMENTAL", Build.VERSION.INCREMENTAL);
      build.put("RELEASE", Build.VERSION.RELEASE);
      build.put("SDK_INT", Build.VERSION.SDK_INT);
      build.put("PREVIEW_SDK_INT", Build.VERSION.PREVIEW_SDK_INT);
      build.put("SECURITY_PATCH", Build.VERSION.SECURITY_PATCH);
      report.put("build", build);

      resultsStream.println(report);
    } catch (JSONException e) {
      e.printStackTrace();
    }

    this.startTime = System.currentTimeMillis();

    new Timer().schedule(new TimerTask() {
      @Override
      public void run() {
        if (!pauseAllocation) {
          int bytes = 1024 * 1024 * 8;
          nativeAllocatedByTest += bytes;
          nativeConsume(bytes);
          //jvmConsume(1024 * 512);
        }

        try {
          JSONObject report = standardInfo();
          report.put("freeing", true);
          resultsStream.println(report);
          updateInfo();
        } catch (JSONException e) {
          e.printStackTrace();
        }
      }
    }, 0, 1000 / 50);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    Log.i(TAG, "onDestroy");
  }

  private void updateRecords() {
    long nativeHeapAllocatedSize = Debug.getNativeHeapAllocatedSize();
    if (nativeHeapAllocatedSize > recordNativeHeapAllocatedSize) {
      recordNativeHeapAllocatedSize = nativeHeapAllocatedSize;
    }
  }

  private void updateInfo() {
    runOnUiThread(() -> {
      updateRecords();
      TextView freeMemory = findViewById(R.id.freeMemory);
      freeMemory.setText(memoryString(Runtime.getRuntime().freeMemory()));

      TextView totalMemory = findViewById(R.id.totalMemory);
      totalMemory.setText(memoryString(Runtime.getRuntime().totalMemory()));

      TextView maxMemory = findViewById(R.id.maxMemory);
      maxMemory.setText(memoryString(Runtime.getRuntime().maxMemory()));

      TextView nativeHeap = findViewById(R.id.nativeHeap);
      nativeHeap.setText(memoryString(Debug.getNativeHeapSize()));

      TextView nativeAllocated = findViewById(R.id.nativeAllocated);
      long nativeHeapAllocatedSize = Debug.getNativeHeapAllocatedSize();
      nativeAllocated.setText(memoryString(nativeHeapAllocatedSize));

      TextView recordNativeAllocated = findViewById(R.id.recordNativeAllocated);
      recordNativeAllocated.setText(memoryString(recordNativeHeapAllocatedSize));

      TextView nativeAllocatedByTestTextView = findViewById(R.id.nativeAllocatedByTest);
      nativeAllocatedByTestTextView.setText(memoryString(nativeAllocatedByTest));

      TextView trimMemoryComplete = findViewById(R.id.trimMemoryComplete);
      trimMemoryComplete.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_COMPLETE)));

      TextView trimMemoryModerate = findViewById(R.id.trimMemoryModerate);
      trimMemoryModerate.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_MODERATE)));

      TextView trimMemoryBackground = findViewById(R.id.trimMemoryBackground);
      trimMemoryBackground.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_BACKGROUND)));

      TextView trimMemoryUiHidden = findViewById(R.id.trimMemoryUiHidden);
      trimMemoryUiHidden.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_UI_HIDDEN)));

      TextView trimMemoryRunningCritical = findViewById(R.id.trimMemoryRunningCritical);
      trimMemoryRunningCritical.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_RUNNING_CRITICAL)));

      TextView trimMemoryRunningLow = findViewById(R.id.trimMemoryRunningLow);
      trimMemoryRunningLow.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_RUNNING_LOW)));

      TextView trimMemoryRunningModerate = findViewById(R.id.trimMemoryRunningModerate);
      trimMemoryRunningModerate.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_RUNNING_MODERATE)));
    });
  }

  void jvmConsume(int bytes) {
    try {
      byte[] array = new byte[bytes];
      for (int count = 0; count < array.length; count++) {
        array[count] = (byte) count;
      }
      data.add(array);
    } catch (OutOfMemoryError e) {
      e.printStackTrace();
    }
  }

  @Override
  public void onTrimMemory(int level) {
    try {
      JSONObject report = standardInfo();
      report.put("onTrimMemory", level);
      nativeAllocatedByTest = 0;

      if (!pauseAllocation) {
        pauseAllocation = true;
        int delay = 1000;///500 * totalTrims;
        report.put("delay", delay);
        new Timer().schedule(
            new TimerTask() {
              @Override
              public void run() {
                try {
                  JSONObject report = standardInfo();
                  report.put("freeing", true);
                  resultsStream.println(report);
                  freeAll();
                  data.clear();
                  System.gc();
                  new Timer().schedule(new TimerTask() {
                    @Override
                    public void run() {
                      try {
                        JSONObject report = standardInfo();
                        report.put("resuming", true);
                        resultsStream.println(report);

                        pauseAllocation = false;
                      } catch (JSONException e) {
                        e.printStackTrace();
                      }

                    }
                  }, 1000);
                } catch (JSONException e) {
                  e.printStackTrace();
                }
              }
            },
            delay
        );
      }
      resultsStream.println(report);

      onTrims.add(level);

      updateInfo();
      super.onTrimMemory(level);
      totalTrims++;
      if (totalTrims == 50) {
        try {
          report = standardInfo();
          report.put("exiting", true);
          resultsStream.println(report);
        } catch (JSONException e) {
          e.printStackTrace();
        }
        resultsStream.close();
        finish();
      }
    } catch (JSONException e) {
      throw new RuntimeException(e);
    }
  }

  private JSONObject standardInfo() throws JSONException {
    updateRecords();
    JSONObject report = new JSONObject();
    report.put("time", System.currentTimeMillis() - this.startTime);
    report.put("totalTrims", totalTrims);
    report.put("freeMemory", Runtime.getRuntime().freeMemory());
    report.put("totalMemory", Runtime.getRuntime().totalMemory());
    report.put("maxMemory", Runtime.getRuntime().maxMemory());
    report.put("nativeHeap", Debug.getNativeHeapSize());
    report.put("nativeAllocated", Debug.getNativeHeapAllocatedSize());
    report.put("recordNativeAllocated", recordNativeHeapAllocatedSize);
    report.put("paused", pauseAllocation);
    return report;
  }

  public native void freeAll();

  public native void nativeConsume(int bytes);
}
