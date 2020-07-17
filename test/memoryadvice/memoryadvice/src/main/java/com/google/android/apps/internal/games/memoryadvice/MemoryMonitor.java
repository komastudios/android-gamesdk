package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice.Utils.getOomScore;
import static com.google.android.apps.internal.games.memoryadvice.Utils.processMeminfo;
import static com.google.android.apps.internal.games.memoryadvice.Utils.processStatus;

import android.app.ActivityManager;
import android.content.Context;
import android.os.Build;
import android.os.Debug;
import android.util.Log;
import java.util.Map;
import org.json.JSONException;
import org.json.JSONObject;

/** A class to provide metrics of current memory usage to an application in JSON format. */
class MemoryMonitor {
  private static final String TAG = MemoryMonitor.class.getSimpleName();

  private static final long BYTES_IN_KILOBYTE = 1024;
  private final MapTester mapTester;
  protected final JSONObject baseline;
  private final ActivityManager activityManager;
  private int latestOnTrimLevel;

  /**
   * Create an Android memory metrics fetcher.
   *
   * @param context  The Android context to employ.
   * @param metrics The constant metrics to fetch - constant and variable.
   */
  MemoryMonitor(Context context, JSONObject metrics) {
    mapTester = new MapTester(context.getCacheDir());
    activityManager = (ActivityManager) context.getSystemService((Context.ACTIVITY_SERVICE));

    try {
      baseline = getMemoryMetrics(metrics.getJSONObject("variable"));
      baseline.put("constant", getMemoryMetrics(metrics.getJSONObject("constant")));
    } catch (JSONException e) {
      throw new IllegalStateException(e);
    }
  }

  /**
   * Gets Android memory metrics.
   *
   * @param fields The fields to fetch in a JSON dictionary.
   * @return A JSONObject containing current memory metrics.
   */
  public JSONObject getMemoryMetrics(JSONObject fields) {
    JSONObject report = new JSONObject();
    try {
      if (fields.has("debug")) {
        Object debugFieldsValue = fields.get("debug");
        JSONObject debug =
            debugFieldsValue instanceof JSONObject ? (JSONObject) debugFieldsValue : null;
        boolean allFields = debugFieldsValue instanceof Boolean && (Boolean) debugFieldsValue;
        if (allFields || (debug != null && debug.optBoolean("nativeHeapAllocatedSize"))) {
          report.put("NativeHeapAllocatedSize", Debug.getNativeHeapAllocatedSize());
        }
        if (allFields || (debug != null && debug.optBoolean("NativeHeapFreeSize"))) {
          report.put("NativeHeapFreeSize", Debug.getNativeHeapFreeSize());
        }
        if (allFields || (debug != null && debug.optBoolean("NativeHeapSize"))) {
          report.put("NativeHeapSize", Debug.getNativeHeapSize());
        }
        if (allFields || (debug != null && debug.optBoolean("Pss"))) {
          report.put("Pss", Debug.getNativeHeapSize());
        }
      }

      if (fields.has("MemoryInfo")) {
        Object memoryInfoValue = fields.get("MemoryInfo");
        JSONObject memoryInfoFields =
            memoryInfoValue instanceof JSONObject ? (JSONObject) memoryInfoValue : null;
        boolean allFields = memoryInfoValue instanceof Boolean && (Boolean) memoryInfoValue;
        if (allFields || (memoryInfoFields != null && memoryInfoFields.length() > 0)) {
          ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
          activityManager.getMemoryInfo(memoryInfo);
          if (allFields || memoryInfoFields.has("availMem")) {
            report.put("availMem", memoryInfo.availMem);
          }
          if ((allFields || memoryInfoFields.has("lowMemory")) && memoryInfo.lowMemory) {
            report.put("lowMemory", true);
          }
          if (allFields || memoryInfoFields.has("totalMem")) {
            report.put("totalMem", memoryInfo.totalMem);
          }
          if (allFields || memoryInfoFields.has("threshold")) {
            report.put("threshold", memoryInfo.threshold);
          }
        }
      }

      if (mapTester.warning()) {
        report.put("mapTester", true);
        mapTester.reset();
      }

      if (latestOnTrimLevel > 0) {
        report.put("onTrim", latestOnTrimLevel);
        latestOnTrimLevel = 0;
      }

      int pid = android.os.Process.myPid();
      if (fields.has("proc")) {
        Object procFieldsValue = fields.get("proc");
        JSONObject procFields =
            procFieldsValue instanceof JSONObject ? (JSONObject) procFieldsValue : null;
        boolean allFields = procFieldsValue instanceof Boolean && (Boolean) procFieldsValue;
        if (allFields || procFields.optBoolean("oom_score")) {
          report.put("oom_score", getOomScore(pid));
        }
      }

      if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && fields.has("summary")) {
        Object summaryValue = fields.get("summary");
        JSONObject summary = summaryValue instanceof JSONObject ? (JSONObject) summaryValue : null;
        boolean allFields = summaryValue instanceof Boolean && (Boolean) summaryValue;
        if (allFields || (summary != null && summary.length() > 0)) {
          Debug.MemoryInfo[] debugMemoryInfos =
              activityManager.getProcessMemoryInfo(new int[] {pid});
          for (Debug.MemoryInfo debugMemoryInfo : debugMemoryInfos) {
            for (Map.Entry<String, String> entry : debugMemoryInfo.getMemoryStats().entrySet()) {
              String key = entry.getKey();
              if (allFields || summary.has(key)) {
                long value = Long.parseLong(entry.getValue()) * BYTES_IN_KILOBYTE;
                report.put(key, report.optLong(key) + value);
              }
            }
          }
        }
      }

      if (fields.has("meminfo")) {
        Object meminfoFieldsValue = fields.get("meminfo");
        JSONObject meminfoFields =
            meminfoFieldsValue instanceof JSONObject ? (JSONObject) meminfoFieldsValue : null;
        boolean allFields = meminfoFieldsValue instanceof Boolean && (Boolean) meminfoFieldsValue;
        if (allFields || (meminfoFields != null && meminfoFields.length() > 0)) {
          Map<String, Long> memInfo = processMeminfo();
          for (Map.Entry<String, Long> pair : memInfo.entrySet()) {
            String key = pair.getKey();
            if (allFields || meminfoFields.optBoolean(key)) {
              report.put(key, pair.getValue());
            }
          }
        }
      }

      if (fields.has("status")) {
        Object statusValue = fields.get("status");
        JSONObject status = statusValue instanceof JSONObject ? (JSONObject) statusValue : null;
        boolean allFields = statusValue instanceof Boolean && (Boolean) statusValue;
        if (allFields || (status != null && status.length() > 0)) {
          for (Map.Entry<String, Long> pair : processStatus(pid).entrySet()) {
            String key = pair.getKey();
            if (allFields || status.optBoolean(key)) {
              report.put(key, pair.getValue());
            }
          }
        }
      }

      JSONObject meta = new JSONObject();
      meta.put("time", System.currentTimeMillis());
      report.put("meta", meta);
    } catch (JSONException ex) {
      Log.w(TAG, "Problem getting memory metrics", ex);
    }
    return report;
  }

  public void setOnTrim(int level) {
    if (level > latestOnTrimLevel) {
      latestOnTrimLevel = level;
    }
  }
}
