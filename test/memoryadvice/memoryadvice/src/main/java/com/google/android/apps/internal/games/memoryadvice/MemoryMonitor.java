package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice.Utils.getDebugMemoryInfo;
import static com.google.android.apps.internal.games.memoryadvice.Utils.getOomScore;
import static com.google.android.apps.internal.games.memoryadvice.Utils.processMeminfo;
import static com.google.android.apps.internal.games.memoryadvice.Utils.processStatus;

import android.app.ActivityManager;
import android.content.Context;
import android.os.Build;
import android.os.Debug;
import android.util.Log;
import java.util.Iterator;
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
        JSONObject debug = fields.getJSONObject("debug");
        if (debug.optBoolean("nativeHeapAllocatedSize")) {
          report.put("nativeAllocated", Debug.getNativeHeapAllocatedSize());
        }
      }

      if (fields.has("MemoryInfo")) {
        JSONObject memoryInfoFields = fields.getJSONObject("MemoryInfo");
        if (memoryInfoFields.length() > 0) {
          ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
          activityManager.getMemoryInfo(memoryInfo);
          if (memoryInfoFields.has("availMem")) {
            report.put("availMem", memoryInfo.availMem);
          }
          if (memoryInfoFields.has("lowMemory") && memoryInfo.lowMemory) {
            report.put("lowMemory", true);
          }
          if (memoryInfoFields.has("totalMem")) {
            report.put("totalMem", memoryInfo.totalMem);
          }
          if (memoryInfoFields.has("threshold")) {
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

      if (fields.has("proc")) {
        JSONObject procFields = fields.getJSONObject("proc");
        if (procFields.optBoolean("oom_score")) {
          report.put("oom_score", getOomScore());
        }
      }

      if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && fields.has("summary")) {
        JSONObject summary = fields.getJSONObject("summary");
        if (summary.length() > 0) {
          Debug.MemoryInfo[] debugMemoryInfos = getDebugMemoryInfo(activityManager);
          if (debugMemoryInfos.length > 0) {
            Iterator<String> it = summary.keys();
            while (it.hasNext()) {
              String key = it.next();
              long value = 0;
              for (Debug.MemoryInfo debugMemoryInfo : debugMemoryInfos) {
                value += Long.parseLong(debugMemoryInfo.getMemoryStat(key));
              }
              report.put(key, value * BYTES_IN_KILOBYTE);
            }
          }
        }
      }

      if (fields.has("meminfo")) {
        JSONObject meminfoFields = fields.getJSONObject("meminfo");
        if (meminfoFields.length() > 0) {
          Map<String, Long> memInfo = processMeminfo();
          for (Map.Entry<String, Long> pair : memInfo.entrySet()) {
            String key = pair.getKey();
            if (meminfoFields.optBoolean(key)) {
              report.put(key, pair.getValue());
            }
          }
        }
      }

      if (fields.has("status")) {
        JSONObject status = fields.getJSONObject("status");
        if (status.length() > 0) {
          for (Map.Entry<String, Long> pair : processStatus().entrySet()) {
            String key = pair.getKey();
            if (status.optBoolean(key)) {
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
