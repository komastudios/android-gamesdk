package net.jimblackler.istresser;

import android.app.ActivityManager;
import android.os.Build;
import android.os.Debug;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Map;

import static com.google.android.apps.internal.games.helperlibrary.Utils.getDebugMemoryInfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.getMemoryInfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.getOomScore;
import static com.google.android.apps.internal.games.helperlibrary.Utils.processMeminfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.processStatus;
import static net.jimblackler.istresser.MainActivity.tryAlloc;

/**
 * Wrapper class for methods related to memory management heuristics.
 */
class Heuristic {

  /**
   * The value a heuristic returns when asked for memory pressure on the device through the
   * getSignal method. GREEN indicates it is safe to allocate further, YELLOW indicates further
   * allocation shouldn't happen, and RED indicates high memory pressure.
   */
  static void checkHeuristics(ActivityManager activityManager, JSONObject params,
                              JSONObject deviceSettings, Reporter reporter) throws JSONException {
    if (!params.has("heuristics")) {
      return;
    }
    JSONObject heuristics = params.getJSONObject("heuristics");

    int oomScore = getOomScore(activityManager);
    Map<String, Long> memInfo = processMeminfo();
    Long commitLimit = memInfo.get("CommitLimit");
    Long vmSize = processStatus(activityManager).get("VmSize");
    Long cached = memInfo.get("Cached");
    Long memAvailable = memInfo.get("MemAvailable");
    ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activityManager);
    long availMem = memoryInfo.availMem;

    if (heuristics.has("vmsize")) {

      if (commitLimit == null || vmSize == null) {
        reporter.report("cl", Indicator.GREEN);
      } else {
        reporter.report("vmsize",
            vmSize > commitLimit * heuristics.getDouble("vmsize") ? Indicator.RED : Indicator.GREEN);
      }
    }

    if (heuristics.has("oom")) {
      reporter.report("oom",
          oomScore <= heuristics.getLong("oom") ?
              Indicator.GREEN : Indicator.RED);
    }

    if (heuristics.has("try")) {
      reporter.report("try",
          tryAlloc((int) Utils.getMemoryQuantity(heuristics, "try")) ?
              Indicator.GREEN : Indicator.RED);
    }

    if (heuristics.has("low")) {
      reporter.report("low",
          getMemoryInfo(activityManager).lowMemory ? Indicator.RED : Indicator.GREEN);
    }

    if (heuristics.has("cl")) {
      if (commitLimit == null) {
        reporter.report("cl", Indicator.GREEN);
      } else {
        reporter.report("cl",
            Debug.getNativeHeapAllocatedSize() / 1024 > commitLimit * heuristics.getDouble("cl")
                ? Indicator.RED : Indicator.GREEN);
      }
    }

    if (heuristics.has("avail")) {
      reporter.report("avail",
          availMem < Utils.getMemoryQuantity(heuristics, "avail") ?
              Indicator.RED : Indicator.GREEN);
    }

    if (heuristics.has("cached")) {
      if (cached == null || cached == 0) {
        reporter.report("cached", Indicator.GREEN);
      } else {
        reporter.report("cached",
            cached * heuristics.getDouble("cached") <
                memoryInfo.threshold / 1024 ? Indicator.RED : Indicator.GREEN);
      }
    }

    if (heuristics.has("avail2")) {
      if (memAvailable == null) {
        reporter.report("avail2", Indicator.GREEN);
      } else {
        reporter.report("avail2",
            memAvailable < Utils.getMemoryQuantity(heuristics, "avail2") ?
                Indicator.RED : Indicator.GREEN);
      }
    }

    if (heuristics.has("nativeAllocated")) {
      reporter.report("nativeAllocated",
          Debug.getNativeHeapAllocatedSize() >
              deviceSettings.getLong("nativeAllocated") * heuristics.getDouble("nativeAllocated") ?
              Indicator.RED : Indicator.GREEN);
    }

    if (heuristics.has("VmSize")) {
      reporter.report("VmSize",
          vmSize > deviceSettings.getLong("VmSize") * heuristics.getDouble("VmSize") ?
              Indicator.RED : Indicator.GREEN);
    }

    if (heuristics.has("oom_score")) {
      reporter.report("oom_score",
          oomScore > deviceSettings.getLong("oom_score") * heuristics.getDouble("oom_score") ?
              Indicator.RED : Indicator.GREEN);
    }
    Debug.MemoryInfo debugMemoryInfo = getDebugMemoryInfo(activityManager)[0];

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
      if (heuristics.has("summary.graphics")) {
        reporter.report("summary.graphics",
            Long.parseLong(debugMemoryInfo.getMemoryStat("summary.graphics")) >
                deviceSettings.getLong("summary.graphics") *
                    heuristics.getDouble("summary.graphics") ? Indicator.RED : Indicator.GREEN);
      }

      if (heuristics.has("summary.total-pss")) {
        reporter.report("summary.total-pss",
            Long.parseLong(debugMemoryInfo.getMemoryStat("summary.total-pss")) >
                deviceSettings.getLong("summary.total-pss") *
                    heuristics.getDouble("summary.total-pss") ? Indicator.RED : Indicator.GREEN);
      }
    }

    if (heuristics.has("availMem")) {
      reporter.report("availMem",
          availMem * heuristics.getDouble("availMem") < deviceSettings.getLong("availMem") ?
              Indicator.RED : Indicator.GREEN);
    }

    if (heuristics.has("Cached")) {
      reporter.report("Cached",
          cached * heuristics.getDouble("Cached") < deviceSettings.getLong("Cached") ?
              Indicator.RED : Indicator.GREEN);
    }

    if (heuristics.has("MemAvailable")) {
      reporter.report("MemAvailable",
          memAvailable * heuristics.getDouble("MemAvailable") <
              deviceSettings.getLong("MemAvailable") ? Indicator.RED : Indicator.GREEN);
    }
  }


  public enum Indicator {
    GREEN,
    YELLOW,
    RED
  }

  interface Reporter {
    void report(String heuristic, Indicator indicator);
  }
}
