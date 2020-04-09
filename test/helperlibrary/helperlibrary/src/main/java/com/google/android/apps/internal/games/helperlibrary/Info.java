package com.google.android.apps.internal.games.helperlibrary;

import android.app.ActivityManager;
import android.content.Context;
import android.os.Build;
import android.os.Debug;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import static com.google.android.apps.internal.games.helperlibrary.Utils.getDebugMemoryInfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.getOomScore;
import static com.google.android.apps.internal.games.helperlibrary.Utils.lowMemoryCheck;
import static com.google.android.apps.internal.games.helperlibrary.Utils.processMeminfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.processStatus;

/** A class to provide metrics of current memory usage to an application in JSON format. */
public class Info {

  private static final List<String> MEMINFO_FIELDS =
      Arrays.asList("Cached", "MemFree", "MemAvailable", "SwapFree");
  private static final List<String> STATUS_FIELDS =
      Arrays.asList("VmSize", "VmRSS", "VmData");
  private static final String[] SUMMARY_FIELDS = {
      "summary.graphics", "summary.native-heap", "summary.total-pss"
  };
  private final long startTime = System.currentTimeMillis();
  /**
   * Gets Android memory metrics.
   *
   * @param context The current Android context.
   * @return A JSONObject containing current memory metrics.
   */
  public JSONObject getMemoryMetrics(Context context) throws JSONException {
    JSONObject report = new JSONObject();
    report.put("time", System.currentTimeMillis() - startTime);
    report.put("nativeAllocated", Debug.getNativeHeapAllocatedSize());

    ActivityManager activityManager = (ActivityManager)
        Objects.requireNonNull(context.getSystemService((Context.ACTIVITY_SERVICE)));
    ActivityManager.MemoryInfo memoryInfo = Utils.getMemoryInfo(activityManager);
    report.put("availMem", memoryInfo.availMem);
    boolean lowMemory = lowMemoryCheck(activityManager);
    if (lowMemory) {
      report.put("lowMemory", true);
    }

    report.put("oom_score", getOomScore(activityManager));

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
      Debug.MemoryInfo debugMemoryInfo = getDebugMemoryInfo(activityManager)[0];
      for (String key : SUMMARY_FIELDS) {
        report.put(key, debugMemoryInfo.getMemoryStat(key));
      }
    }

    Map<String, Long> values = processMeminfo();
    for (Map.Entry<String, Long> pair : values.entrySet()) {
      String key = pair.getKey();
      if (MEMINFO_FIELDS.contains(key)) {
        report.put(key, pair.getValue());
      }
    }

    for (Map.Entry<String, Long> pair : processStatus(activityManager).entrySet()) {
      String key = pair.getKey();
      if (STATUS_FIELDS.contains(key)) {
        report.put(key, pair.getValue());
      }
    }
    return report;
  }

  public long getStartTime() {
    return startTime;
  }
}
