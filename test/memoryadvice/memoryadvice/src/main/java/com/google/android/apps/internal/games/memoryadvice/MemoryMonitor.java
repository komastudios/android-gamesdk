package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice.Utils.getOomScore;
import static com.google.android.apps.internal.games.memoryadvice.Utils.processMemFormatFile;

import android.app.ActivityManager;
import android.content.Context;
import android.os.Build;
import android.os.Debug;
import android.os.Process;
import androidx.lifecycle.Lifecycle;
import androidx.lifecycle.LifecycleObserver;
import androidx.lifecycle.OnLifecycleEvent;
import androidx.lifecycle.ProcessLifecycleOwner;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 * A class to provide metrics of current memory usage to an application in JSON format.
 */
class MemoryMonitor {
  private static final String TAG = MemoryMonitor.class.getSimpleName();

  private static final long BYTES_IN_KILOBYTE = 1024;
  private static final long BYTES_IN_MEGABYTE = BYTES_IN_KILOBYTE * 1024;
  private final MapTester mapTester;
  private final ActivityManager activityManager;
  private final CanaryProcessTester canaryProcessTester;
  private boolean appBackgrounded;
  private int latestOnTrimLevel;
  private final int pid = Process.myPid();

  /**
   * Create an Android memory metrics fetcher.
   *
   * @param context The Android context to employ.
   * @param metrics The constant metrics to fetch - constant and variable.
   */
  MemoryMonitor(Context context, Map<String, Object> metrics) {
    mapTester = new MapTester(context.getCacheDir());
    activityManager = (ActivityManager) context.getSystemService((Context.ACTIVITY_SERVICE));

    if (metrics != null) {
      Map<String, Object> variable = (Map<String, Object>) metrics.get("variable");
      if (variable != null) {
        Map<String, Object> canaryProcessParams =
            (Map<String, Object>) variable.get("canaryProcessTester");
        if (canaryProcessParams == null) {
          canaryProcessTester = null;
        } else {
          canaryProcessTester = new CanaryProcessTester(context, canaryProcessParams);
        }
      } else {
        canaryProcessTester = null;
      }
    } else {
      canaryProcessTester = null;
    }

    ProcessLifecycleOwner.get().getLifecycle().addObserver(new LifecycleObserver() {
      @OnLifecycleEvent(Lifecycle.Event.ON_STOP)
      public void onAppBackgrounded() {
        appBackgrounded = true;
      }

      @OnLifecycleEvent(Lifecycle.Event.ON_START)
      public void onAppForegrounded() {
        appBackgrounded = false;
      }
    });
  }

  /**
   * Gets Android memory metrics.
   *
   * @param fields The fields to fetch in a JSON dictionary.
   * @return A map containing current memory metrics.
   */
  public Map<String, Object> getMemoryMetrics(Map<String, Object> fields) {
    Map<String, Object> report = new LinkedHashMap<>();

    Map<String, Object> _meta = new LinkedHashMap<>();
    _meta.put("time", System.currentTimeMillis());
    report.put("meta", _meta);

    if (mapTester.warning()) {
      report.put("mapTester", true);
      mapTester.reset();
    }

    if (canaryProcessTester != null && canaryProcessTester.warning()) {
      report.put("canaryProcessTester", "red");
      canaryProcessTester.reset();
    }

    if (appBackgrounded) {
      report.put("backgrounded", true);
    }

    if (latestOnTrimLevel > 0) {
      report.put("onTrim", latestOnTrimLevel);
      latestOnTrimLevel = 0;
    }

    if (fields == null) {
      // All remaining data requires a fields spec.
      return report;
    }

    boolean recordTimings = Boolean.TRUE.equals(fields.get("timings"));
    Object debugFieldsValue = fields.get("debug");
    if (debugFieldsValue != null) {
      long time = System.nanoTime();
      Map<String, Object> metricsOut = new LinkedHashMap<>();
      Map<String, Object> debug =
          debugFieldsValue instanceof Map ? (Map<String, Object>) debugFieldsValue : null;
      boolean allFields = Boolean.TRUE.equals(debugFieldsValue);
      if (allFields || debug != null) {
        if (allFields || Boolean.TRUE.equals(debug.get("nativeHeapAllocatedSize"))) {
          metricsOut.put("NativeHeapAllocatedSize", Debug.getNativeHeapAllocatedSize());
        }
        if (allFields || Boolean.TRUE.equals(debug.get("NativeHeapFreeSize"))) {
          metricsOut.put("NativeHeapFreeSize", Debug.getNativeHeapFreeSize());
        }
        if (allFields || Boolean.TRUE.equals(debug.get("NativeHeapSize"))) {
          metricsOut.put("NativeHeapSize", Debug.getNativeHeapSize());
        }
        if (allFields || Boolean.TRUE.equals(debug.get("Pss"))) {
          metricsOut.put("Pss", Debug.getPss() * BYTES_IN_KILOBYTE);
        }
      }
      if (recordTimings) {
        Map<String, Object> meta = new LinkedHashMap<>();
        meta.put("duration", System.nanoTime() - time);
        metricsOut.put("_meta", meta);
      }
      report.put("debug", metricsOut);
    }

    Object memoryInfoValue = fields.get("MemoryInfo");
    if (memoryInfoValue != null) {
      long time = System.nanoTime();
      Map<String, Object> memoryInfoFields =
          memoryInfoValue instanceof Map ? (Map<String, Object>) memoryInfoValue : null;
      boolean allFields = Boolean.TRUE.equals(memoryInfoValue);
      if (allFields || (memoryInfoFields != null && !memoryInfoFields.isEmpty())) {
        Map<String, Object> metricsOut = new LinkedHashMap<>();
        ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
        activityManager.getMemoryInfo(memoryInfo);
        if (allFields || Boolean.TRUE.equals(memoryInfoFields.get("availMem"))) {
          metricsOut.put("availMem", memoryInfo.availMem);
        }
        if (memoryInfo.lowMemory
            && (allFields || Boolean.TRUE.equals(memoryInfoFields.get("lowMemory")))) {
          metricsOut.put("lowMemory", true);
        }
        if (allFields || Boolean.TRUE.equals(memoryInfoFields.get("totalMem"))) {
          metricsOut.put("totalMem", memoryInfo.totalMem);
        }
        if (allFields || Boolean.TRUE.equals(memoryInfoFields.get("threshold"))) {
          metricsOut.put("threshold", memoryInfo.threshold);
        }

        if (recordTimings) {
          Map<String, Object> meta = new LinkedHashMap<>();
          meta.put("duration", System.nanoTime() - time);
          metricsOut.put("_meta", meta);
        }
        report.put("MemoryInfo", metricsOut);
      }
    }

    Object activityManagerValue = fields.get("ActivityManager");
    if (activityManagerValue != null) {
      Map<String, Object> activityManagerFields =
          activityManagerValue instanceof Map ? (Map<String, Object>) activityManagerValue : null;
      boolean allFields = Boolean.TRUE.equals(activityManagerValue);
      if (allFields || activityManagerFields != null) {
        if (allFields || Boolean.TRUE.equals(activityManagerFields.get("MemoryClass"))) {
          report.put("MemoryClass", activityManager.getMemoryClass() * BYTES_IN_MEGABYTE);
        }
        if (allFields || Boolean.TRUE.equals(activityManagerFields.get("LargeMemoryClass"))) {
          report.put("LargeMemoryClass", activityManager.getLargeMemoryClass() * BYTES_IN_MEGABYTE);
        }
        if (allFields || Boolean.TRUE.equals(activityManagerFields.get("LowRamDevice"))) {
          report.put("LowRamDevice", activityManager.isLowRamDevice());
        }
      }
    }

    Object procFieldsValue = fields.get("proc");
    if (procFieldsValue != null) {
      long time = System.nanoTime();
      Map<String, Object> procFields =
          procFieldsValue instanceof Map ? (Map<String, Object>) procFieldsValue : null;
      boolean allFields = Boolean.TRUE.equals(procFieldsValue);
      Map<String, Object> metricsOut = new LinkedHashMap<>();
      if (allFields || (procFields != null && Boolean.TRUE.equals(procFields.get("oom_score")))) {
        metricsOut.put("oom_score", getOomScore(pid));
      }
      if (recordTimings) {
        Map<String, Object> meta = new LinkedHashMap<>();
        meta.put("duration", System.nanoTime() - time);
        metricsOut.put("_meta", meta);
      }
      report.put("proc", metricsOut);
    }

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && fields.containsKey("summary")) {
      long time = System.nanoTime();
      Object summaryValue = fields.get("summary");
      Map<String, Object> summary =
          summaryValue instanceof Map ? (Map<String, Object>) summaryValue : null;
      boolean allFields = Boolean.TRUE.equals(summaryValue);
      if (allFields || (summary != null && !summary.isEmpty())) {
        Debug.MemoryInfo[] debugMemoryInfos = activityManager.getProcessMemoryInfo(new int[] {pid});
        Map<String, Object> metricsOut = new LinkedHashMap<>();
        for (Debug.MemoryInfo debugMemoryInfo : debugMemoryInfos) {
          for (Map.Entry<String, String> entry : debugMemoryInfo.getMemoryStats().entrySet()) {
            String key = entry.getKey();
            if (allFields || summary.containsKey(key)) {
              long value = Long.parseLong(entry.getValue()) * BYTES_IN_KILOBYTE;
              Number number = (Number) metricsOut.get(key);
              metricsOut.put(key, number == null ? value : number.longValue() + value);
            }
          }
        }

        if (recordTimings) {
          Map<String, Object> meta = new LinkedHashMap<>();
          meta.put("duration", System.nanoTime() - time);
          metricsOut.put("_meta", meta);
        }
        report.put("summary", metricsOut);
      }
    }

    if (fields.containsKey("meminfo")) {
      long time = System.nanoTime();
      Object meminfoFieldsValue = fields.get("meminfo");
      Map<String, Object> meminfoFields =
          meminfoFieldsValue instanceof Map ? (Map<String, Object>) meminfoFieldsValue : null;
      boolean allFields = Boolean.TRUE.equals(meminfoFieldsValue);
      if (allFields || (meminfoFields != null && !meminfoFields.isEmpty())) {
        Map<String, Object> metricsOut = new LinkedHashMap<>();
        processMemFormatFile("/proc/meminfo", (key, bytes) -> {
          if (allFields || Boolean.TRUE.equals(meminfoFields.get(key))) {
            metricsOut.put(key, bytes);
          }
        });

        if (recordTimings) {
          Map<String, Object> meta = new LinkedHashMap<>();
          meta.put("duration", System.nanoTime() - time);
          metricsOut.put("_meta", meta);
        }
        report.put("meminfo", metricsOut);
      }
    }

    if (fields.containsKey("smaps_rollup")) {
      long time = System.nanoTime();
      Object smapsRollupFieldsValue = fields.get("smaps_rollup");
      Map<String, Object> smapsRollupFields = smapsRollupFieldsValue instanceof Map
          ? (Map<String, Object>) smapsRollupFieldsValue
          : null;
      boolean allFields = Boolean.TRUE.equals(smapsRollupFieldsValue);
      if (allFields || (smapsRollupFields != null && !smapsRollupFields.isEmpty())) {
        Map<String, Object> metricsOut = new LinkedHashMap<>();
        processMemFormatFile("/proc/" + pid + "/smaps_rollup", (key, bytes) -> {
          if (allFields || Boolean.TRUE.equals(smapsRollupFields.get(key))) {
            metricsOut.put(key, bytes);
          }
        });

        if (recordTimings) {
          Map<String, Object> meta = new LinkedHashMap<>();
          meta.put("duration", System.nanoTime() - time);
          metricsOut.put("_meta", meta);
        }
        report.put("smaps_rollup", metricsOut);
      }
    }

    if (fields.containsKey("status")) {
      long time = System.nanoTime();
      Object statusValue = fields.get("status");
      Map<String, Object> status =
          statusValue instanceof Map ? (Map<String, Object>) statusValue : null;
      boolean allFields = Boolean.TRUE.equals(statusValue);
      if (allFields || (status != null && !status.isEmpty())) {
        Map<String, Object> metricsOut = new LinkedHashMap<>();
        processMemFormatFile("/proc/" + pid + "/status", (key, bytes) -> {
          if (allFields || Boolean.TRUE.equals(status.get(key))) {
            metricsOut.put(key, bytes);
          }
        });

        if (recordTimings) {
          Map<String, Object> meta = new LinkedHashMap<>();
          meta.put("duration", System.nanoTime() - time);
          metricsOut.put("_meta", meta);
        }
        report.put("status", metricsOut);
      }
    }

    return report;
  }

  public void setOnTrim(int level) {
    if (level > latestOnTrimLevel) {
      latestOnTrimLevel = level;
    }
  }
}
