package com.google.android.apps.internal.games.helperlibrary;

import static com.google.android.apps.internal.games.helperlibrary.Utils.getDebugMemoryInfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.getOomScore;
import static com.google.android.apps.internal.games.helperlibrary.Utils.processMeminfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.processStatus;

import android.app.ActivityManager;
import android.content.Context;
import android.os.Build;
import android.os.Debug;
import android.util.Log;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import org.json.JSONException;
import org.json.JSONObject;

/** A class to provide metrics of current memory usage to an application in JSON format. */
public final class Info {
  private static final String TAG = Info.class.getSimpleName();

  private static final Collection<String> MEMINFO_FIELDS = new HashSet<>(Arrays.asList("Active",
      "Active(anon)", "Active(file)", "AnonPages", "MemAvailable", "MemFree", "VmData", "VmRSS"));
  private static final Collection<String> MEMINFO_FIELDS_CONSTANT =
      new HashSet<>(Arrays.asList("CommitLimit", "HighTotal", "LowTotal", "MemTotal"));
  private static final Collection<String> STATUS_FIELDS =
      new HashSet<>(Arrays.asList("VmRSS", "VmSize"));
  private static final String[] SUMMARY_FIELDS = {
      "summary.native-heap", "summary.graphics", "summary.total-pss", "summary.total-swap"};
  private static final long BYTES_IN_KILOBYTE = 1024;
  private final boolean fetchDebug;
  private final MapTester mapTester;
  private final JSONObject baseline;
  private final ActivityManager activityManager;
  private final JSONObject deviceSettings;

  /**
   * Create an Android memory metrics fetcher.
   *
   * @param context The Android context to employ.
   * @param fetchDebug Whether to fetch debug-based params.
   */
  public Info(Context context, boolean fetchDebug) {
    this.fetchDebug = fetchDebug;
    deviceSettings = DeviceSettings.getDeviceSettings(context.getAssets());
    mapTester = new MapTester(context.getCacheDir());
    activityManager = (ActivityManager) context.getSystemService((Context.ACTIVITY_SERVICE));
    baseline = getMemoryMetrics(true);
  }

  /**
   * Gets Android memory metrics.
   *
   * @param fetchConstants Whether to fetch metrics that never change.
   * @return A JSONObject containing current memory metrics.
   */
  public JSONObject getMemoryMetrics(boolean fetchConstants) {
    long time = System.currentTimeMillis();
    JSONObject report = new JSONObject();
    try {
      report.put("nativeAllocated", Debug.getNativeHeapAllocatedSize());

      ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
      activityManager.getMemoryInfo(memoryInfo);
      report.put("availMem", memoryInfo.availMem);
      if (memoryInfo.lowMemory) {
        report.put("lowMemory", true);
      }

      if (mapTester.warning()) {
        report.put("mapTester", true);
        mapTester.reset();
      }

      report.put("oom_score", getOomScore());
      if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && fetchDebug) {
        Debug.MemoryInfo[] debugMemoryInfos = getDebugMemoryInfo(activityManager);
        if (debugMemoryInfos.length > 0) {
          for (String key : SUMMARY_FIELDS) {
            long value = 0;
            for (Debug.MemoryInfo debugMemoryInfo : debugMemoryInfos) {
              value += Long.parseLong(debugMemoryInfo.getMemoryStat(key));
            }
            report.put(key, value * BYTES_IN_KILOBYTE);
          }
        }
      }

      Map<String, Long> memInfo = processMeminfo();
      for (Map.Entry<String, Long> pair : memInfo.entrySet()) {
        String key = pair.getKey();
        if (MEMINFO_FIELDS.contains(key)) {
          report.put(key, pair.getValue());
        }
      }

      for (Map.Entry<String, Long> pair : processStatus().entrySet()) {
        String key = pair.getKey();
        if (STATUS_FIELDS.contains(key)) {
          report.put(key, pair.getValue());
        }
      }
      if (fetchConstants) {
        JSONObject constant = new JSONObject();
        for (Map.Entry<String, Long> pair : memInfo.entrySet()) {
          String key = pair.getKey();
          if (MEMINFO_FIELDS_CONSTANT.contains(key)) {
            constant.put(key, pair.getValue());
          }
        }
        constant.put("totalMem", memoryInfo.totalMem);
        constant.put("threshold", memoryInfo.threshold);
        report.put("constant", constant);
      }
      JSONObject meta = new JSONObject();
      meta.put("duration", System.currentTimeMillis() - time);
      report.put("meta", meta);
    } catch (JSONException ex) {
      Log.w(TAG, "Problem getting memory metrics", ex);
    }
    return report;
  }

  public JSONObject getBaseline() {
    return baseline;
  }

  public JSONObject getDeviceSettings() {
    return deviceSettings;
  }
}
