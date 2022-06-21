package com.google.android.apps.internal.games.memorytest;

import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getMemoryQuantity;
import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getOrDefault;
import static com.google.android.apps.internal.games.memorytest.DurationUtils.getDuration;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;
import android.view.View;
import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;
import com.google.android.apps.internal.games.memoryadvice.MemoryWatcher;
import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

public class MemoryTest implements MemoryWatcher.Client {
  private static final String TAG = MemoryTest.class.getSimpleName();

  static {
    System.loadLibrary("memory-test");
  }

  private final boolean yellowLightTesting;
  private final long mallocBytesPerMillisecond;
  private final long glAllocBytesPerMillisecond;
  private final long vkAllocBytesPerMillisecond;
  private final long mmapFileBytesPerMillisecond;
  private final long mmapAnonBytesPerMillisecond;
  private final long mmapAnonBlock;
  private final long mmapFileBlock;
  private final long memoryToFreePerCycle;
  private final long delayBeforeRelease;
  private final long delayAfterRelease;
  private final ResultsReceiver resultsReceiver;
  private final MemoryAdvisor memoryAdvisor;
  private final Timer timer = new Timer();
  private final TestSurface testSurface;
  private final MmapFileGroup mmapFiles;
  private long allocationStartedTime = -1;
  private long nativeAllocatedByTest;
  private long vkAllocatedByTest;
  private long mmapAnonAllocatedByTest;
  private long mmapFileAllocatedByTest;

  public MemoryTest(Context context, MemoryAdvisor memoryAdvisor, TestSurface testSurface,
      Map<String, Object> params, ResultsReceiver resultsReceiver) {
    this.testSurface = testSurface;
    this.resultsReceiver = resultsReceiver;
    this.memoryAdvisor = memoryAdvisor;

    initNative();

    yellowLightTesting = Boolean.TRUE.equals(params.get("yellowLightTesting"));
    mallocBytesPerMillisecond = getMemoryQuantity(getOrDefault(params, "malloc", 0));
    glAllocBytesPerMillisecond = getMemoryQuantity(getOrDefault(params, "glTest", 0));
    vkAllocBytesPerMillisecond = getMemoryQuantity(getOrDefault(params, "vkTest", 0));

    delayBeforeRelease = getDuration(getOrDefault(params, "delayBeforeRelease", "1s"));
    delayAfterRelease = getDuration(getOrDefault(params, "delayAfterRelease", "1s"));

    memoryToFreePerCycle = getMemoryQuantity(getOrDefault(params, "memoryToFreePerCycle", "500M"));

    Map<String, Object> mmapFile = (Map<String, Object>) params.get("mmapFile");
    Map<String, Object> mmapAnon = (Map<String, Object>) params.get("mmapAnon");

    if (mmapAnon != null && mmapFile != null) {
      initMMap();
    }

    if (mmapAnon == null) {
      mmapAnonBlock = 0;
      mmapAnonBytesPerMillisecond = 0;
    } else {
      mmapAnonBlock = getMemoryQuantity(getOrDefault(mmapAnon, "blockSize", "2M"));
      mmapAnonBytesPerMillisecond =
          getMemoryQuantity(getOrDefault(mmapAnon, "allocPerMillisecond", 0));
    }

    if (mmapFile == null) {
      mmapFiles = null;
      mmapFileBlock = 0;
      mmapFileBytesPerMillisecond = 0;
    } else {
      int mmapFileCount = ((Number) getOrDefault(mmapFile, "count", 10)).intValue();
      long mmapFileSize = getMemoryQuantity(getOrDefault(mmapFile, "fileSize", "4K"));
      String mmapPath = context.getCacheDir().toString();
      try {
        mmapFiles = new MmapFileGroup(mmapPath, mmapFileCount, mmapFileSize);
      } catch (IOException e) {
        throw new IllegalStateException(e);
      }
      mmapFileBlock = getMemoryQuantity(getOrDefault(mmapFile, "blockSize", "2M"));
      mmapFileBytesPerMillisecond =
          getMemoryQuantity(getOrDefault(mmapFile, "allocPerMillisecond", 0));
    }

    long glFixed = getMemoryQuantity(getOrDefault(params, "glFixed", 0));
    if (glAllocBytesPerMillisecond > 0 || glFixed > 0) {
      initGl();
      testSurface.getRenderer().setMaxConsumerSize(
          getMemoryQuantity(getOrDefault(params, "maxConsumer", "2048K")));
      testSurface.setVisibility(View.VISIBLE);
      testSurface.getRenderer().setTarget(glFixed);
    }

    nativeConsume(getMemoryQuantity(getOrDefault(params, "mallocFixed", 0)));

    IntentFilter filter = new IntentFilter(Intent.ACTION_SCREEN_ON);
    filter.addAction(Intent.ACTION_SCREEN_OFF);
    BroadcastReceiver receiver = new BroadcastReceiver() {
      @Override
      public void onReceive(Context context, Intent intent) {
        Map<String, Object> report = new LinkedHashMap<>();
        report.put("action", intent.getAction());
        report.put("time", System.currentTimeMillis());
        resultsReceiver.accept(report);
      }
    };
    context.registerReceiver(receiver, filter);

    new Timer().schedule(new TimerTask() {
      @Override
      public void run() {
        context.unregisterReceiver(receiver);
        Map<String, Object> report = new LinkedHashMap<>();
        report.put("exiting", true);
        report.put("time", System.currentTimeMillis());
        resultsReceiver.accept(report);
      }
    }, getDuration(getOrDefault(params, "timeout", "10m")));

    runAfterDelay(() -> {
      allocationStartedTime = System.currentTimeMillis();
    }, getDuration(getOrDefault(params, "delayBeforeAllocation", "2s")));
  }

  public static native void initNative();

  public static native void initGl();

  public static native void initMMap();

  public static native int nativeDraw(int toAllocate);

  public static native void release();

  public static native long vkAlloc(long size);

  public static native void vkRelease();

  public static native boolean writeRandomFile(String path, long bytes);

  public static native void freeAll();

  public static native void freeMemory(long bytes);

  public static native boolean nativeConsume(long bytes);

  public static native void mmapAnonFreeAll();

  public static native long mmapAnonConsume(long bytes);

  public static native long mmapFileConsume(String path, long bytes, long offset);

  @Override
  public void newState(MemoryAdvisor.MemoryState state) {
    Log.i(TAG, "New memory state: " + state.name());
  }

  @Override
  public void receiveAdvice(Map<String, Object> advice) {
    Map<String, Object> report = new LinkedHashMap<>();
    report.put("advice", advice);
    if (allocationStartedTime == -1) {
      report.put("paused", true);
    } else {
      long sinceAllocationStarted = System.currentTimeMillis() - allocationStartedTime;
      if (sinceAllocationStarted > 0) {
        boolean shouldAllocate = true;
        switch (MemoryAdvisor.getMemoryState(advice)) {
          case APPROACHING_LIMIT:
            if (yellowLightTesting) {
              shouldAllocate = false;
            }
            break;
          case CRITICAL:
            shouldAllocate = false;
            if (yellowLightTesting) {
              freeMemory(memoryToFreePerCycle);
            } else {
              // Allocating 0 MB
              releaseMemory();
            }
            break;
        }
        if (shouldAllocate) {
          if (mallocBytesPerMillisecond > 0) {
            long owed = sinceAllocationStarted * mallocBytesPerMillisecond - nativeAllocatedByTest;
            if (owed > 0) {
              boolean succeeded = nativeConsume(owed);
              if (succeeded) {
                nativeAllocatedByTest += owed;
              } else {
                report.put("allocFailed", true);
              }
            }
          }
          if (glAllocBytesPerMillisecond > 0) {
            long target = sinceAllocationStarted * glAllocBytesPerMillisecond;
            testSurface.getRenderer().setTarget(target);
          }

          if (vkAllocBytesPerMillisecond > 0) {
            long owed = sinceAllocationStarted * vkAllocBytesPerMillisecond - vkAllocatedByTest;
            if (owed > 0) {
              long allocated = vkAlloc(owed);
              if (allocated >= owed) {
                vkAllocatedByTest += owed;
              } else {
                report.put("allocFailed", true);
              }
            }
          }

          if (mmapAnonBytesPerMillisecond > 0) {
            long owed =
                sinceAllocationStarted * mmapAnonBytesPerMillisecond - mmapAnonAllocatedByTest;
            if (owed > mmapAnonBlock) {
              long allocated = mmapAnonConsume(owed);
              if (allocated == 0) {
                report.put("mmapAnonFailed", true);
              } else {
                mmapAnonAllocatedByTest += allocated;
              }
            }
          }
          if (mmapFileBytesPerMillisecond > 0) {
            long owed =
                sinceAllocationStarted * mmapFileBytesPerMillisecond - mmapFileAllocatedByTest;
            if (owed > mmapFileBlock) {
              MmapFileInfo file = mmapFiles.alloc(owed);
              long allocated =
                  mmapFileConsume(file.getPath(), file.getAllocSize(), file.getOffset());
              if (allocated == 0) {
                report.put("mmapFileFailed", true);
              } else {
                mmapFileAllocatedByTest += allocated;
              }
            }
          }
        }
      }
    }

    Map<String, Object> testMetrics = new LinkedHashMap<>();
    if (vkAllocatedByTest > 0) {
      testMetrics.put("vkAllocatedByTest", vkAllocatedByTest);
    }
    if (nativeAllocatedByTest > 0) {
      testMetrics.put("nativeAllocatedByTest", nativeAllocatedByTest);
    }
    if (mmapAnonAllocatedByTest > 0) {
      testMetrics.put("mmapAnonAllocatedByTest", mmapAnonAllocatedByTest);
    }
    if (mmapAnonAllocatedByTest > 0) {
      testMetrics.put("mmapFileAllocatedByTest", mmapFileAllocatedByTest);
    }

    if (testSurface != null) {
      TestRenderer renderer = testSurface.getRenderer();
      long glAllocated = renderer.getAllocated();
      if (glAllocated > 0) {
        testMetrics.put("gl_allocated", glAllocated);
      }
      if (renderer.getFailed()) {
        report.put("allocFailed", true);
      }
    }
    report.put("testMetrics", testMetrics);

    resultsReceiver.accept(report);
  }

  private void runAfterDelay(Runnable runnable, long delay) {
    timer.schedule(new TimerTask() {
      @Override
      public void run() {
        runnable.run();
      }
    }, delay);
  }

  private void releaseMemory() {
    allocationStartedTime = -1;

    runAfterDelay(() -> {
      Map<String, Object> report2 = new LinkedHashMap<>();
      report2.put("paused", true);
      report2.put("time", System.currentTimeMillis());

      resultsReceiver.accept(report2);

      if (nativeAllocatedByTest > 0) {
        nativeAllocatedByTest = 0;
        freeAll();
      }

      if (mmapAnonAllocatedByTest > 0) {
        mmapAnonFreeAll();
        mmapAnonAllocatedByTest = 0;
      }

      if (glAllocBytesPerMillisecond > 0) {
        if (testSurface != null) {
          testSurface.queueEvent(() -> {
            TestRenderer renderer = testSurface.getRenderer();
            renderer.release();
          });
        }
      }
      if (vkAllocBytesPerMillisecond > 0) {
        vkAllocatedByTest = 0;
        vkRelease();
      }

      runAfterDelay(new Runnable() {
        @Override
        public void run() {
          Map<String, Object> report = new LinkedHashMap<>();
          Map<String, Object> advice = memoryAdvisor.getAdvice();
          report.put("advice", advice);
          if (MemoryAdvisor.anyWarnings(advice)) {
            report.put("failedToClear", true);
            report.put("paused", true);
            runAfterDelay(this, delayAfterRelease);
          } else {
            allocationStartedTime = System.currentTimeMillis();
          }
          resultsReceiver.accept(report);
        }
      }, delayAfterRelease);
    }, delayBeforeRelease);
  }

  public interface ResultsReceiver {
    void accept(Map<String, Object> results);
  }
}