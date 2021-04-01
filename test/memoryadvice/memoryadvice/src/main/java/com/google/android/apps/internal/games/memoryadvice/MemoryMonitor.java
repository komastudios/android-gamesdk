package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor.getDefaultParams;
import static com.google.android.apps.internal.games.memoryadvice.Utils.getOomScore;
import static com.google.android.apps.internal.games.memoryadvice.Utils.processMeminfo;
import static com.google.android.apps.internal.games.memoryadvice.Utils.processStatus;
import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getOrDefault;

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
  private static final long BYTES_IN_GIGABYTE = BYTES_IN_MEGABYTE * 1024;
  protected final Map<String, Object> baseline;
  private final MapTester mapTester;
  private final ActivityManager activityManager;
  private final Map<String, Object> metrics;
  private final CanaryProcessTester canaryProcessTester;
  protected final Map<String, Object> build;
  private Predictor realtimePredictor;
  private Predictor availablePredictor;
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
    build = BuildInfo.getBuild(context);
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

      Map<String, Object> baseline = (Map<String, Object>) metrics.get("baseline");
      if (baseline != null) {
        this.baseline = getMemoryMetrics(baseline);
        Map<String, Object> constant = (Map<String, Object>) metrics.get("constant");
        if (constant != null) {
          this.baseline.put("constant", getMemoryMetrics(constant));
        }
      } else {
        this.baseline = null;
      }
    } else {
      canaryProcessTester = null;
      baseline = null;
    }

    this.metrics = metrics;
    activityManager = context.getSystemService(ActivityManager.class);

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
   * Create an Android memory metrics fetcher to collect the default selection of metrics.
   *
   * @param context The Android context to employ.
   */
  public MemoryMonitor(Context context) {
    this(context, (Map<String, Object>) getDefaultParams(context.getAssets()).get("metrics"));
  }

  /**
   * Gets Android memory metrics using the default fields.
   *
   * @return A map containing current memory metrics.
   */
  public Map<String, Object> getMemoryMetrics() {
    return getMemoryMetrics((Map<String, Object>) metrics.get("variable"));
  }

  /**
   * Gets Android memory metrics.
   *
   * @param fields The fields to fetch in a JSON dictionary.
   * @return A map containing current memory metrics.
   */
  public Map<String, Object> getMemoryMetrics(Map<String, Object> fields) {
    Map<String, Object> report = new LinkedHashMap<>();

    if (fields.containsKey("debug")) {
      long time = System.nanoTime();
      Map<String, Object> metricsOut = new LinkedHashMap<>();
      Object debugFieldsValue = fields.get("debug");
      Map<String, Object> debug =
          debugFieldsValue instanceof Map ? (Map<String, Object>) debugFieldsValue : null;
      boolean allFields = debugFieldsValue instanceof Boolean && (Boolean) debugFieldsValue;
      if (allFields || (debug != null && getOrDefault(debug, "nativeHeapAllocatedSize", false))) {
        metricsOut.put("NativeHeapAllocatedSize", Debug.getNativeHeapAllocatedSize());
      }
      if (allFields || (debug != null && getOrDefault(debug, "NativeHeapFreeSize", false))) {
        metricsOut.put("NativeHeapFreeSize", Debug.getNativeHeapFreeSize());
      }
      if (allFields || (debug != null && getOrDefault(debug, "NativeHeapSize", false))) {
        metricsOut.put("NativeHeapSize", Debug.getNativeHeapSize());
      }
      if (allFields || (debug != null && getOrDefault(debug, "Pss", false))) {
        metricsOut.put("Pss", Debug.getPss() * BYTES_IN_KILOBYTE);
      }

      Map<String, Object> meta = new LinkedHashMap<>();
      meta.put("duration", System.nanoTime() - time);
      metricsOut.put("_meta", meta);
      report.put("debug", metricsOut);
    }

    if (fields.containsKey("MemoryInfo")) {
      long time = System.nanoTime();
      Object memoryInfoValue = fields.get("MemoryInfo");
      Map<String, Object> memoryInfoFields =
          memoryInfoValue instanceof Map ? (Map<String, Object>) memoryInfoValue : null;
      boolean allFields = memoryInfoValue instanceof Boolean && (Boolean) memoryInfoValue;
      if (allFields || (memoryInfoFields != null && !memoryInfoFields.isEmpty())) {
        Map<String, Object> metricsOut = new LinkedHashMap<>();
        ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
        activityManager.getMemoryInfo(memoryInfo);
        if (allFields || memoryInfoFields.containsKey("availMem")) {
          metricsOut.put("availMem", memoryInfo.availMem);
        }
        if ((allFields || memoryInfoFields.containsKey("lowMemory")) && memoryInfo.lowMemory) {
          metricsOut.put("lowMemory", true);
        }
        if (allFields || memoryInfoFields.containsKey("totalMem")) {
          metricsOut.put("totalMem", memoryInfo.totalMem);
        }
        if (allFields || memoryInfoFields.containsKey("threshold")) {
          metricsOut.put("threshold", memoryInfo.threshold);
        }

        Map<String, Object> meta = new LinkedHashMap<>();
        meta.put("duration", System.nanoTime() - time);
        metricsOut.put("_meta", meta);
        report.put("MemoryInfo", metricsOut);
      }
    }

    if (fields.containsKey("ActivityManager")) {
      Object activityManagerValue = fields.get("ActivityManager");
      Map<String, Object> activityManagerFields =
          activityManagerValue instanceof Map ? (Map<String, Object>) activityManagerValue : null;
      boolean allFields = activityManagerValue instanceof Boolean && (Boolean) activityManagerValue;
      if (allFields || activityManagerFields.containsKey("MemoryClass")) {
        report.put("MemoryClass", activityManager.getMemoryClass() * BYTES_IN_MEGABYTE);
      }
      if (allFields || activityManagerFields.containsKey("LargeMemoryClass")) {
        report.put("LargeMemoryClass", activityManager.getLargeMemoryClass() * BYTES_IN_MEGABYTE);
      }
      if (allFields || activityManagerFields.containsKey("LowRamDevice")) {
        report.put("LowRamDevice", activityManager.isLowRamDevice());
      }
    }

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

    if (fields.containsKey("proc")) {
      long time = System.nanoTime();
      Object procFieldsValue = fields.get("proc");
      Map<String, Object> procFields =
          procFieldsValue instanceof Map ? (Map<String, Object>) procFieldsValue : null;
      boolean allFields = procFieldsValue instanceof Boolean && (Boolean) procFieldsValue;
      Map<String, Object> metricsOut = new LinkedHashMap<>();
      if (allFields || getOrDefault(procFields, "oom_score", false)) {
        metricsOut.put("oom_score", getOomScore(pid));
      }

      Map<String, Object> meta = new LinkedHashMap<>();
      meta.put("duration", System.nanoTime() - time);
      metricsOut.put("_meta", meta);
      report.put("proc", metricsOut);
    }

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && fields.containsKey("summary")) {
      long time = System.nanoTime();
      Object summaryValue = fields.get("summary");
      Map<String, Object> summary =
          summaryValue instanceof Map ? (Map<String, Object>) summaryValue : null;
      boolean allFields = summaryValue instanceof Boolean && (Boolean) summaryValue;
      if (allFields || (summary != null && !summary.isEmpty())) {
        Debug.MemoryInfo[] debugMemoryInfos = activityManager.getProcessMemoryInfo(new int[] {pid});
        Map<String, Object> metricsOut = new LinkedHashMap<>();
        for (Debug.MemoryInfo debugMemoryInfo : debugMemoryInfos) {
          for (Map.Entry<String, String> entry : debugMemoryInfo.getMemoryStats().entrySet()) {
            String key = entry.getKey();
            if (allFields || summary.containsKey(key)) {
              long value = Long.parseLong(entry.getValue()) * BYTES_IN_KILOBYTE;
              metricsOut.put(key, getOrDefault(metricsOut, key, 0L) + value);
            }
          }
        }

        Map<String, Object> meta = new LinkedHashMap<>();
        meta.put("duration", System.nanoTime() - time);
        metricsOut.put("_meta", meta);
        report.put("summary", metricsOut);
      }
    }

    if (fields.containsKey("meminfo")) {
      long time = System.nanoTime();
      Object meminfoFieldsValue = fields.get("meminfo");
      Map<String, Object> meminfoFields =
          meminfoFieldsValue instanceof Map ? (Map<String, Object>) meminfoFieldsValue : null;
      boolean allFields = meminfoFieldsValue instanceof Boolean && (Boolean) meminfoFieldsValue;
      if (allFields || (meminfoFields != null && !meminfoFields.isEmpty())) {
        Map<String, Object> metricsOut = new LinkedHashMap<>();
        Map<String, Long> memInfo = processMeminfo();
        for (Map.Entry<String, Long> pair : memInfo.entrySet()) {
          String key = pair.getKey();
          if (allFields || getOrDefault(meminfoFields, key, false)) {
            metricsOut.put(key, pair.getValue());
          }
        }

        Map<String, Object> meta = new LinkedHashMap<>();
        meta.put("duration", System.nanoTime() - time);
        metricsOut.put("_meta", meta);
        report.put("meminfo", metricsOut);
      }
    }

    if (fields.containsKey("status")) {
      long time = System.nanoTime();
      Object statusValue = fields.get("status");
      Map<String, Object> status =
          statusValue instanceof Map ? (Map<String, Object>) statusValue : null;
      boolean allFields = statusValue instanceof Boolean && (Boolean) statusValue;
      if (allFields || (status != null && !status.isEmpty())) {
        Map<String, Object> metricsOut = new LinkedHashMap<>();
        for (Map.Entry<String, Long> pair : processStatus(pid).entrySet()) {
          String key = pair.getKey();
          if (allFields || getOrDefault(status, key, false)) {
            metricsOut.put(key, pair.getValue());
          }
        }

        Map<String, Object> meta = new LinkedHashMap<>();
        meta.put("duration", System.nanoTime() - time);
        metricsOut.put("_meta", meta);
        report.put("status", metricsOut);
      }
    }

    Map<String, Object> _meta = new LinkedHashMap<>();
    _meta.put("time", System.currentTimeMillis());
    report.put("meta", _meta);

    if (getOrDefault(fields, "predictRealtime", false)) {
      if (realtimePredictor == null) {
        realtimePredictor = new Predictor("/realtime.tflite", "/realtime_features.json");
      }

      long time = System.nanoTime();
      Map<String, Object> data = new LinkedHashMap<>();
      data.put("baseline", baseline);
      data.put("build", build);
      data.put("sample", report);
      report.put("predictedUsage", realtimePredictor.predict(data));
      Map<String, Object> meta = new LinkedHashMap<>();
      meta.put("duration", System.nanoTime() - time);
      report.put("_predictedUsageMeta", meta);
    }

    if (getOrDefault(fields, "availableRealtime", false)) {
      if (availablePredictor == null) {
        availablePredictor = new Predictor("/available.tflite", "/available_features.json");
      }

      long time = System.nanoTime();
      Map<String, Object> data = new LinkedHashMap<>();
      data.put("baseline", baseline);
      data.put("build", build);
      data.put("sample", report);
      long available = (long) (BYTES_IN_GIGABYTE * availablePredictor.predict(data));
      report.put("predictedAvailable", available);
      Map<String, Object> meta = new LinkedHashMap<>();
      meta.put("duration", System.nanoTime() - time);
      report.put("_predictedAvailableMeta", meta);
    }

    return report;
  }

  public void setOnTrim(int level) {
    if (level > latestOnTrimLevel) {
      latestOnTrimLevel = level;
    }
  }
}
