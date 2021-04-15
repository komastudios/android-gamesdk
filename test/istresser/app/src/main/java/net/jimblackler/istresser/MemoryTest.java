package net.jimblackler.istresser;

import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getMemoryQuantity;
import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getOrDefault;
import static net.jimblackler.istresser.Utils.getDuration;

import android.content.Context;
import android.util.Log;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;
import com.google.android.apps.internal.games.memoryadvice.MemoryWatcher;
import java.io.IOException;
import java.io.PrintStream;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

class MemoryTest implements MemoryWatcher.Client {
  private static final String TAG = MemoryTest.class.getSimpleName();

  private static final int BYTES_IN_MEGABYTE = 1024 * 1024;
  private static final int MMAP_ANON_BLOCK_BYTES = 2 * BYTES_IN_MEGABYTE;
  private static final int MMAP_FILE_BLOCK_BYTES = 2 * BYTES_IN_MEGABYTE;
  private static final int MEMORY_TO_FREE_PER_CYCLE_MB = 500;

  private final boolean yellowLightTesting;
  private final long mallocBytesPerMillisecond;
  private final long glAllocBytesPerMillisecond;
  private final long vkAllocBytesPerMillisecond;
  private final long mmapFileBytesPerMillisecond;
  private final long mmapAnonBytesPerMillisecond;
  private final long delayBeforeRelease;
  private final long delayAfterRelease;
  private final PrintStream resultsStream;

  private final ObjectMapper objectMapper = new ObjectMapper();
  private final ResultsReceiver resultsReceiver;
  private final MemoryAdvisor memoryAdvisor;
  private final Timer timer = new Timer();
  private final TestSurface testSurface;
  private final MmapFileGroup mmapFiles;
  private long allocationStartedTime = System.currentTimeMillis();
  private long nativeAllocatedByTest;
  private long vkAllocatedByTest;
  private long mmapAnonAllocatedByTest;
  private long mmapFileAllocatedByTest;

  MemoryTest(Context context, MemoryAdvisor memoryAdvisor, TestSurface testSurface,
      PrintStream resultsStream, Map<String, Object> params, ResultsReceiver resultsReceiver) {
    this.testSurface = testSurface;
    this.resultsStream = resultsStream;
    this.resultsReceiver = resultsReceiver;
    this.memoryAdvisor = memoryAdvisor;

    yellowLightTesting = Boolean.TRUE.equals(params.get("yellowLightTesting"));
    mallocBytesPerMillisecond = getMemoryQuantity(getOrDefault(params, "malloc", 0));
    glAllocBytesPerMillisecond = getMemoryQuantity(getOrDefault(params, "glTest", 0));
    vkAllocBytesPerMillisecond = getMemoryQuantity(getOrDefault(params, "vkTest", 0));
    mmapFileBytesPerMillisecond = getMemoryQuantity(getOrDefault(params, "mmapFile", 0));
    mmapAnonBytesPerMillisecond = getMemoryQuantity(getOrDefault(params, "mmapAnon", 0));
    delayBeforeRelease = getDuration(getOrDefault(params, "delayBeforeRelease", "1s"));
    delayAfterRelease = getDuration(getOrDefault(params, "delayAfterRelease", "1s"));

    if (params.containsKey("mmapFile")) {
      String mmapPath = context.getCacheDir().toString();
      int mmapFileCount = ((Number) getOrDefault(params, "mmapFileCount", 10)).intValue();
      long mmapFileSize = getMemoryQuantity(getOrDefault(params, "mmapFileSize", "4K"));
      try {
        mmapFiles = new MmapFileGroup(mmapPath, mmapFileCount, mmapFileSize);
      } catch (IOException e) {
        throw new IllegalStateException(e);
      }
    } else {
      mmapFiles = null;
    }
  }

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
              MainActivity.freeMemory(MEMORY_TO_FREE_PER_CYCLE_MB * BYTES_IN_MEGABYTE);
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
              boolean succeeded = MainActivity.nativeConsume(owed);
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
              long allocated = MainActivity.vkAlloc(owed);
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
            if (owed > MMAP_ANON_BLOCK_BYTES) {
              long allocated = MainActivity.mmapAnonConsume(owed);
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
            if (owed > MMAP_FILE_BLOCK_BYTES) {
              MmapFileInfo file = mmapFiles.alloc(owed);
              long allocated = MainActivity.mmapFileConsume(
                  file.getPath(), file.getAllocSize(), file.getOffset());
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

    TestRenderer renderer = testSurface.getRenderer();
    long glAllocated = renderer.getAllocated();
    if (glAllocated > 0) {
      testMetrics.put("gl_allocated", glAllocated);
    }
    if (renderer.getFailed()) {
      report.put("allocFailed", true);
    }
    report.put("testMetrics", testMetrics);

    try {
      resultsStream.println(objectMapper.writeValueAsString(report));
    } catch (JsonProcessingException e) {
      throw new IllegalStateException(e);
    }

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
      report2.put("metrics", memoryAdvisor.getMemoryMetrics());

      try {
        resultsStream.println(objectMapper.writeValueAsString(report2));
      } catch (JsonProcessingException e) {
        throw new IllegalStateException(e);
      }
      if (nativeAllocatedByTest > 0) {
        nativeAllocatedByTest = 0;
        MainActivity.freeAll();
      }
      MainActivity.mmapAnonFreeAll();
      mmapAnonAllocatedByTest = 0;

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
        MainActivity.vkRelease();
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
          try {
            resultsStream.println(objectMapper.writeValueAsString(report));
          } catch (JsonProcessingException e) {
            throw new IllegalStateException(e);
          }
        }
      }, delayAfterRelease);
    }, delayBeforeRelease);
  }

  interface ResultsReceiver {
    void accept(Map<String, Object> results);
  }
}