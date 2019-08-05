package net.jimblackler.istresser;

import android.os.Bundle;
import android.os.Debug;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;

import com.google.common.collect.HashMultiset;
import com.google.common.collect.Lists;
import com.google.common.collect.Multiset;

import java.util.ArrayList;
import java.util.Locale;
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

  private static String memoryString(long bytes) {
    return String.format(Locale.getDefault(), "%.1f MB", (float) bytes / (1024 * 1024));
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    System.out.println("test");

    new Timer().schedule(new TimerTask() {
      @Override
      public void run() {
        int bytes = 1024 * 1024 * 8;
        nativeAllocatedByTest += bytes;
        nativeConsume(bytes);
        //jvmConsume(1024 * 512);

        updateInfo();
      }
    }, 0, 1000 / 50);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    Log.i(TAG, "onDestroy");
  }

  private void updateInfo() {
    runOnUiThread(() -> {
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
      if (nativeHeapAllocatedSize > recordNativeHeapAllocatedSize) {
        recordNativeHeapAllocatedSize = nativeHeapAllocatedSize;
      }

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
    Log.i(TAG, String.format("onTrimMemory %d", level));

    nativeAllocatedByTest = 0;
    freeAll();
    data.clear();
    System.gc();

    onTrims.add(level);

    updateInfo();
    super.onTrimMemory(level);
    totalTrims++;
    if (totalTrims == 6) {
      finish();
    }
  }

  public native void freeAll();
  public native void nativeConsume(int bytes);
}
