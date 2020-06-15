package net.jimblackler.istresser;

import static com.google.android.apps.internal.games.helperlibrary.Utils.getDebugMemoryInfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.getMemoryInfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.getOomScore;
import static com.google.android.apps.internal.games.helperlibrary.Utils.processMeminfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.processStatus;
import static net.jimblackler.istresser.MainActivity.tryAlloc;

import android.app.ActivityManager;
import android.os.Build;
import android.os.Debug;
import java.util.Map;
import org.json.JSONException;
import org.json.JSONObject;

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
            vmSize > commitLimit * heuristics.getDouble("vmsize") ? Indicator.RED
                                                                  : Indicator.GREEN);
      }
    }

    if (heuristics.has("oom")) {
      reporter.report(
          "oom", oomScore <= heuristics.getLong("oom") ? Indicator.GREEN : Indicator.RED);
    }

    if (heuristics.has("try")) {
      reporter.report("try",
          tryAlloc((int) Utils.getMemoryQuantity(heuristics.get("try"))) ? Indicator.GREEN
                                                                         : Indicator.RED);
    }

    if (heuristics.has("low")) {
      reporter.report(
          "low", getMemoryInfo(activityManager).lowMemory ? Indicator.RED : Indicator.GREEN);
    }

    if (heuristics.has("cl")) {
      if (commitLimit == null) {
        reporter.report("cl", Indicator.GREEN);
      } else {
        reporter.report("cl",
            Debug.getNativeHeapAllocatedSize() > commitLimit * heuristics.getDouble("cl")
                ? Indicator.RED
                : Indicator.GREEN);
      }
    }

    if (heuristics.has("avail")) {
      reporter.report("avail",
          availMem < Utils.getMemoryQuantity(heuristics.get("avail")) ? Indicator.RED
                                                                      : Indicator.GREEN);
    }

    if (heuristics.has("cached")) {
      if (cached == null || cached == 0) {
        reporter.report("cached", Indicator.GREEN);
      } else {
        reporter.report("cached",
            cached * heuristics.getDouble("cached") < memoryInfo.threshold / 1024
                ? Indicator.RED
                : Indicator.GREEN);
      }
    }

    if (heuristics.has("avail2")) {
      if (memAvailable == null) {
        reporter.report("avail2", Indicator.GREEN);
      } else {
        reporter.report("avail2",
            memAvailable < Utils.getMemoryQuantity(heuristics.get("avail2")) ? Indicator.RED
                                                                             : Indicator.GREEN);
      }
    }

    JSONObject nativeAllocatedParams = heuristics.optJSONObject("nativeAllocated");
    long nativeAllocatedLimit = deviceSettings.optLong("nativeAllocated");
    if (nativeAllocatedParams != null && nativeAllocatedLimit > 0) {
      Indicator level = Indicator.GREEN;
      if (Debug.getNativeHeapAllocatedSize()
          > nativeAllocatedLimit * nativeAllocatedParams.getDouble("red")) {
        level = Indicator.RED;
      } else if (Debug.getNativeHeapAllocatedSize()
          > nativeAllocatedLimit * nativeAllocatedParams.getDouble("yellow")) {
        level = Indicator.YELLOW;
      }
      reporter.report("nativeAllocated", level);
    }

    JSONObject vmSizeParams = heuristics.optJSONObject("VmSize");
    long vmSizeLimit = deviceSettings.optLong("VmSize");
    if (vmSizeParams != null && vmSize != null && vmSizeLimit > 0) {
      Indicator level = Indicator.GREEN;
      if (vmSize > vmSizeLimit * vmSizeParams.getDouble("red")) {
        level = Indicator.RED;
      } else if (vmSize > vmSizeLimit * vmSizeParams.getDouble("yellow")) {
        level = Indicator.YELLOW;
      }
      reporter.report("VmSize", level);
    }

    JSONObject oomScoreParams = heuristics.optJSONObject("oom_score");
    long oomScoreLimit = deviceSettings.optLong("oom_score");
    if (oomScoreParams != null && oomScoreLimit > 0) {
      Indicator level = Indicator.GREEN;
      if (oomScore > oomScoreLimit * oomScoreParams.getDouble("red")) {
        level = Indicator.RED;
      } else if (oomScore > oomScoreLimit * oomScoreParams.getDouble("yellow")) {
        level = Indicator.YELLOW;
      }
      reporter.report("oom_score", level);
    }
    Debug.MemoryInfo debugMemoryInfo = getDebugMemoryInfo(activityManager)[0];

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
      JSONObject summaryGraphicsParams = heuristics.optJSONObject("summary.graphics");
      long summaryGraphicsLimit = deviceSettings.optLong("summary.graphics");
      if (summaryGraphicsParams != null && summaryGraphicsLimit > 0) {
        Indicator level = Indicator.GREEN;
        long summaryGraphics = Long.parseLong(debugMemoryInfo.getMemoryStat("summary.graphics"));
        if (summaryGraphics > summaryGraphicsLimit * summaryGraphicsParams.getDouble("red")) {
          level = Indicator.RED;
        } else if (summaryGraphics
            > summaryGraphicsLimit * summaryGraphicsParams.getDouble("yellow")) {
          level = Indicator.YELLOW;
        }
        reporter.report("summary.graphics", level);
      }

      JSONObject summaryTotalPssParams = heuristics.optJSONObject("summary.total-pss");
      long summaryTotalPssLimit = deviceSettings.optLong("summary.total-pss");
      if (summaryTotalPssParams != null && summaryTotalPssLimit > 0) {
        Indicator level = Indicator.GREEN;
        long summaryTotalPss = Long.parseLong(debugMemoryInfo.getMemoryStat("summary.total-pss"));
        if (summaryTotalPss > summaryTotalPssLimit * summaryTotalPssParams.getDouble("red")) {
          level = Indicator.RED;
        } else if (summaryTotalPss
            > summaryTotalPssLimit * summaryTotalPssParams.getDouble("yellow")) {
          level = Indicator.YELLOW;
        }
        reporter.report("summary.total-pss", level);
      }
    }

    JSONObject availMemParams = heuristics.optJSONObject("availMem");
    long availMemLimit = deviceSettings.optLong("availMem");
    if (availMemParams != null && availMemLimit > 0) {
      Indicator level = Indicator.GREEN;
      if (availMem * availMemParams.getDouble("red") < availMemLimit) {
        level = Indicator.RED;
      } else if (availMem * availMemParams.getDouble("yellow") < availMemLimit) {
        level = Indicator.YELLOW;
      }
      reporter.report("availMem", level);
    }

    JSONObject cachedParams = heuristics.optJSONObject("Cached");
    long cachedLimit = deviceSettings.optLong("Cached");
    if (cachedParams != null && cached != null && cachedLimit > 0) {
      Indicator level = Indicator.GREEN;
      if (cached * cachedParams.getDouble("red") < cachedLimit) {
        level = Indicator.RED;
      } else if (cached * cachedParams.getDouble("yellow") < cachedLimit) {
        level = Indicator.YELLOW;
      }
      reporter.report("Cached", level);
    }

    JSONObject memAvailableParams = heuristics.optJSONObject("MemAvailable");
    long memAvailableLimit = deviceSettings.optLong("MemAvailable");
    if (memAvailableParams != null && memAvailable != null && memAvailableLimit > 0) {
      Indicator level = Indicator.GREEN;
      if (memAvailable * memAvailableParams.getDouble("red") < memAvailableLimit) {
        level = Indicator.RED;
      } else if (memAvailable * memAvailableParams.getDouble("yellow") < memAvailableLimit) {
        level = Indicator.YELLOW;
      }
      reporter.report("MemAvailable", level);
    }
  }

  public enum Indicator { GREEN, YELLOW, RED }

  interface Reporter {
    void report(String heuristic, Indicator indicator);
  }
}
