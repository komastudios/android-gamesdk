package com.google.android.apps.internal.games.helperlibrary;

import static com.google.android.apps.internal.games.helperlibrary.Utils.getDebugMemoryInfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.getOomScore;
import static com.google.android.apps.internal.games.helperlibrary.Utils.processMeminfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.processStatus;

import android.app.ActivityManager;
import android.content.Context;
import android.os.Build;
import android.os.Debug;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import org.json.JSONException;
import org.json.JSONObject;

/** A class to provide metrics of current memory usage to an application in JSON format. */
public class Info {
  private static final List<String> MEMINFO_FIELDS = Arrays.asList("Active", "Active(anon)",
      "Active(file)", "AnonPages", "MemAvailable", "MemFree", "VmData", "VmRSS");
  private static final List<String> MEMINFO_FIELDS_CONSTANT =
      Arrays.asList("CommitLimit", "HighTotal", "LowTotal", "MemTotal");
  private static final List<String> STATUS_FIELDS = Arrays.asList("VmRSS", "VmSize");
  private static final String[] SUMMARY_FIELDS = {
      "summary.native-heap", "summary.graphics", "summary.total-pss", "summary.total-swap"};
  private static final long BYTES_IN_KILOBYTE = 1024;
  private MapTester mapTester;
  private JSONObject baseline;

  /**
   * Gets Android memory metrics.
   *
   * @param context The current Android context.
   * @return A JSONObject containing current memory metrics.
   */
  public JSONObject getMemoryMetrics(Context context) throws JSONException {
    JSONObject report = new JSONObject();
    report.put("nativeAllocated", Debug.getNativeHeapAllocatedSize());

    ActivityManager activityManager = (ActivityManager) Objects.requireNonNull(
        context.getSystemService((Context.ACTIVITY_SERVICE)));
    ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
    activityManager.getMemoryInfo(memoryInfo);
    report.put("availMem", memoryInfo.availMem);
    if (memoryInfo.lowMemory) {
      report.put("lowMemory", true);
    }

    if (mapTester == null) {
      mapTester = new MapTester(context.getCacheDir());
    }

    if (mapTester.warning()) {
      report.put("mapTester", true);
      mapTester.reset();
    }

    report.put("oom_score", getOomScore());
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
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
    if (baseline == null) {
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
      baseline = new JSONObject(report.toString());
    }
    return report;
  }

  public JSONObject getBaseline() {
    return baseline;
  }
}
