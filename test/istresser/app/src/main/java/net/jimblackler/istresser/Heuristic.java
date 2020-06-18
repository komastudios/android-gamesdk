package net.jimblackler.istresser;

import static net.jimblackler.istresser.MainActivity.tryAlloc;
import static net.jimblackler.istresser.Utils.optLong;

import android.os.Build;
import android.os.Debug;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Collections;
import java.util.Iterator;
import java.util.List;

/**
 * Wrapper class for methods related to memory management heuristics.
 */
class Heuristic {
  private static final List<String> PREDICTION_FIELDS = Collections.singletonList("oom_score");

  /**
   * The value a heuristic returns when asked for memory pressure on the device through the
   * getSignal method. GREEN indicates it is safe to allocate further, YELLOW indicates further
   * allocation shouldn't happen, and RED indicates high memory pressure.
   */
  static JSONObject checkHeuristics(JSONObject metrics, JSONObject baseline, JSONObject params,
                                    JSONObject deviceSettings) throws JSONException {
    JSONObject results = new JSONObject();
    if (!params.has("heuristics")) {
      return results;
    }

    JSONObject deviceLimit = deviceSettings.getJSONObject("limit");
    JSONObject deviceBaseline = deviceSettings.getJSONObject("baseline");

    if (params.has("heuristics")) {
      int oomScore = metrics.getInt("oom_score");
      JSONObject constant = baseline.getJSONObject("constant");
      Long commitLimit = optLong(constant, "CommitLimit");
      Long vmSize = optLong(metrics, "VmSize");
      Long cached = optLong(metrics, "Cached");
      Long memAvailable = optLong(metrics, "MemAvailable");
      long availMem = metrics.getLong("availMem");

      JSONArray warnings = new JSONArray();
      JSONObject heuristics = params.getJSONObject("heuristics");
      if (heuristics.has("vmsize")) {
        if (commitLimit != null && vmSize != null &&
            vmSize > commitLimit * heuristics.getDouble("vmsize")) {
          JSONObject warning = new JSONObject();
          warning.put("vmsize", heuristics.get("vmsize"));
          warning.put("level", "red");
          warnings.put(warning);
        }
      }

      if (heuristics.has("oom")) {
        if (oomScore > heuristics.getLong("oom")) {
          JSONObject warning = new JSONObject();
          warning.put("oom", heuristics.get("oom"));
          warning.put("level", "red");
          warnings.put(warning);
        }
      }

      if (heuristics.has("try")) {
        if (!tryAlloc((int) Utils.getMemoryQuantity(heuristics.get("try")))) {
          JSONObject warning = new JSONObject();
          warning.put("try", heuristics.get("try"));
          warning.put("level", "red");
          warnings.put(warning);
        }
      }

      if (heuristics.has("low")) {
        if (metrics.optBoolean("lowMemory")) {
          JSONObject warning = new JSONObject();
          warning.put("low", heuristics.get("low"));
          warning.put("level", "red");
          warnings.put(warning);
        }
      }

      if (heuristics.has("cl")) {
        if (commitLimit != null && Debug.getNativeHeapAllocatedSize() >
            commitLimit * heuristics.getDouble("cl")) {
          JSONObject warning = new JSONObject();
          warning.put("cl", heuristics.get("cl"));
          warning.put("level", "red");
          warnings.put(warning);
        }
      }

      if (heuristics.has("avail")) {
        if (availMem < Utils.getMemoryQuantity(heuristics.get("avail"))) {
          JSONObject warning = new JSONObject();
          warning.put("avail", heuristics.get("avail"));
          warning.put("level", "red");
          warnings.put(warning);
        }
      }

      if (heuristics.has("cached")) {
        if (cached != null && cached != 0 &&
            cached * heuristics.getDouble("cached") < constant.getLong("threshold") / 1024) {
          JSONObject warning = new JSONObject();
          warning.put("cached", heuristics.get("cached"));
          warning.put("level", "red");
          warnings.put(warning);
        }
      }

      if (heuristics.has("avail2")) {
        if (memAvailable != null &&
            memAvailable < Utils.getMemoryQuantity(heuristics.get("avail2"))) {
          JSONObject warning = new JSONObject();
          warning.put("avail2", heuristics.get("avail2"));
          warning.put("level", "red");
          warnings.put(warning);
        }
      }

      JSONObject nativeAllocatedParams = heuristics.optJSONObject("nativeAllocated");
      long nativeAllocatedLimit = deviceLimit.optLong("nativeAllocated");
      if (nativeAllocatedParams != null && nativeAllocatedLimit > 0) {
        String level = null;
        if (Debug.getNativeHeapAllocatedSize()
            > nativeAllocatedLimit * nativeAllocatedParams.getDouble("red")) {
          level = "red";
        } else if (Debug.getNativeHeapAllocatedSize()
            > nativeAllocatedLimit * nativeAllocatedParams.getDouble("yellow")) {
          level = "yellow";
        }
        if (level != null) {
          JSONObject warning = new JSONObject();
          warning.put("nativeAllocated", heuristics.get("nativeAllocated"));
          warning.put("level", level);
          warnings.put(warning);
        }
      }

      JSONObject vmSizeParams = heuristics.optJSONObject("VmSize");
      long vmSizeLimit = deviceLimit.optLong("VmSize");
      if (vmSizeParams != null && vmSize != null && vmSizeLimit > 0) {
        String level = null;
        if (vmSize > vmSizeLimit * vmSizeParams.getDouble("red")) {
          level = "red";
        } else if (vmSize > vmSizeLimit * vmSizeParams.getDouble("yellow")) {
          level = "yellow";
        }
        if (level != null) {
          JSONObject warning = new JSONObject();
          warning.put("VmSize", heuristics.get("VmSize"));
          warning.put("level", level);
          warnings.put(warning);
        }
      }

      JSONObject oomScoreParams = heuristics.optJSONObject("oom_score");
      long oomScoreLimit = deviceLimit.optLong("oom_score");
      if (oomScoreParams != null && oomScoreLimit > 0) {
        String level = null;
        if (oomScore > oomScoreLimit * oomScoreParams.getDouble("red")) {
          level = "red";
        } else if (oomScore > oomScoreLimit * oomScoreParams.getDouble("yellow")) {
          level = "yellow";
        }
        if (level != null) {
          JSONObject warning = new JSONObject();
          warning.put("oom_score", heuristics.get("oom_score"));
          warning.put("level", level);
          warnings.put(warning);
        }
      }

      if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
        JSONObject summaryGraphicsParams = heuristics.optJSONObject("summary.graphics");
        long summaryGraphicsLimit = deviceLimit.optLong("summary.graphics");
        if (summaryGraphicsParams != null && summaryGraphicsLimit > 0) {
          String level = null;
          long summaryGraphics = metrics.getLong("summary.graphics");
          if (summaryGraphics > summaryGraphicsLimit * summaryGraphicsParams.getDouble("red")) {
            level = "red";
          } else if (summaryGraphics
              > summaryGraphicsLimit * summaryGraphicsParams.getDouble("yellow")) {
            level = "yellow";
          }
          if (level != null) {
            JSONObject warning = new JSONObject();
            warning.put("summary.graphics", heuristics.get("summary.graphics"));
            warning.put("level", level);
            warnings.put(warning);
          }
        }

        JSONObject summaryTotalPssParams = heuristics.optJSONObject("summary.total-pss");
        long summaryTotalPssLimit = deviceLimit.optLong("summary.total-pss");
        if (summaryTotalPssParams != null && summaryTotalPssLimit > 0) {
          String level = null;
          long summaryTotalPss = metrics.getLong("summary.total-pss");
          if (summaryTotalPss > summaryTotalPssLimit * summaryTotalPssParams.getDouble("red")) {
            level = "red";
          } else if (summaryTotalPss
              > summaryTotalPssLimit * summaryTotalPssParams.getDouble("yellow")) {
            level = "yellow";
          }
          if (level != null) {
            JSONObject warning = new JSONObject();
            warning.put("summary.total-pss", heuristics.get("summary.total-pss"));
            warning.put("level", level);
            warnings.put(warning);
          }
        }
      }

      JSONObject availMemParams = heuristics.optJSONObject("availMem");
      long availMemLimit = deviceLimit.optLong("availMem");
      if (availMemParams != null && availMemLimit > 0) {
        String level = null;
        if (availMem * availMemParams.getDouble("red") < availMemLimit) {
          level = "red";
        } else if (availMem * availMemParams.getDouble("yellow") < availMemLimit) {
          level = "yellow";
        }
        if (level != null) {
          JSONObject warning = new JSONObject();
          warning.put("availMem", heuristics.get("availMem"));
          warning.put("level", level);
          warnings.put(warning);
        }
      }

      JSONObject cachedParams = heuristics.optJSONObject("Cached");
      long cachedLimit = deviceLimit.optLong("Cached");
      if (cachedParams != null && cached != null && cachedLimit > 0) {
        String level = null;
        if (cached * cachedParams.getDouble("red") < cachedLimit) {
          level = "red";
        } else if (cached * cachedParams.getDouble("yellow") < cachedLimit) {
          level = "yellow";
        }
        if (level != null) {
          JSONObject warning = new JSONObject();
          warning.put("Cached", heuristics.get("Cached"));
          warning.put("level", level);
          warnings.put(warning);
        }
      }

      JSONObject memAvailableParams = heuristics.optJSONObject("MemAvailable");
      long memAvailableLimit = deviceLimit.optLong("MemAvailable");
      if (memAvailableParams != null && memAvailable != null && memAvailableLimit > 0) {
        String level = null;
        if (memAvailable * memAvailableParams.getDouble("red") < memAvailableLimit) {
          level = "red";
        } else if (memAvailable * memAvailableParams.getDouble("yellow") < memAvailableLimit) {
          level = "yellow";
        }
        if (level != null) {
          JSONObject warning = new JSONObject();
          warning.put("MemAvailable", heuristics.get("MemAvailable"));
          warning.put("level", level);
          warnings.put(warning);
        }
      }

      // Handler for device-based metrics.
      Iterator<String> it = heuristics.keys();
      while (it.hasNext()) {
        String key = it.next();
        JSONObject heuristic;
        try {
          heuristic = heuristics.getJSONObject(key);
        } catch (JSONException e) {
          break;
        }

        if (!metrics.has(key)) {
          continue;
        }

        if (!baseline.has(key)) {
          continue;
        }

        if (!deviceLimit.has(key)) {
          continue;
        }

        if (!deviceBaseline.has(key)) {
          continue;
        }

        long deviceLimitValue = deviceLimit.getLong(key);
        long deviceBaselineValue = deviceBaseline.getLong(key);
        boolean increasing = deviceLimitValue > deviceBaselineValue;

        // Fires warnings as metrics approach absolute values.
        // Example: "Active": {"fixed": {"red": "300M", "yellow": "400M"}}
        if (heuristic.has("fixed")) {
          JSONObject fixed = heuristic.getJSONObject("fixed");
          long metricValue = metrics.getLong(key);
          long red = Utils.getMemoryQuantity(fixed.get("red"));
          long yellow = Utils.getMemoryQuantity(fixed.get("yellow"));
          String level = null;
          if (increasing ? metricValue > red : metricValue < red) {
            level = "red";
          } else if (increasing ? metricValue > yellow : metricValue < yellow) {
            level = "yellow";
          }
          if (level != null) {
            JSONObject warning = new JSONObject();
            JSONObject trigger = new JSONObject();
            trigger.put("fixed", fixed);
            warning.put(key, trigger);
            warning.put("level", level);
            warnings.put(warning);
          }
        }

        // Fires warnings as metrics approach ratios of the device baseline.
        // Example: "availMem": {"baselineRatio": {"red": 0.30, "yellow": 0.40}}
        if (heuristic.has("baselineRatio")) {
          JSONObject baselineRatio = heuristic.getJSONObject("baselineRatio");
          long metricValue = metrics.getLong(key);
          long baselineValue = baseline.getLong(key);

          String level = null;
          if (increasing ? metricValue > baselineValue * baselineRatio.getDouble("red")
                         : metricValue < baselineValue * baselineRatio.getDouble("red")) {
            level = "red";
          } else if (increasing ? metricValue > baselineValue * baselineRatio.getDouble("yellow")
                                : metricValue < baselineValue * baselineRatio.getDouble("yellow")) {
            level = "yellow";
          }
          if (level != null) {
            JSONObject warning = new JSONObject();
            JSONObject trigger = new JSONObject();
            trigger.put("baselineRatio", baselineRatio);
            warning.put(key, trigger);
            warning.put("level", level);
            warnings.put(warning);
          }
        }

        // Fires warnings as baseline-relative metrics approach ratios of the device's baseline-
        // relative limit.
        // Example: "oom_score": {"deltaLimit": {"red": 0.85, "yellow": 0.75}}
        if (heuristic.has("deltaLimit")) {
          JSONObject deltaLimit = heuristic.getJSONObject("deltaLimit");
          long limitValue = deviceLimitValue - deviceBaselineValue;
          long metricValue = metrics.getLong(key) - baseline.getLong(key);

          String level = null;
          if (increasing ? metricValue > limitValue * deltaLimit.getDouble("red")
                         : metricValue < limitValue * deltaLimit.getDouble("red")) {
            level = "red";
          } else if (increasing ? metricValue > limitValue * deltaLimit.getDouble("yellow")
                                : metricValue < limitValue * deltaLimit.getDouble("yellow")) {
            level = "yellow";
          }
          if (level != null) {
            JSONObject warning = new JSONObject();
            JSONObject trigger = new JSONObject();
            trigger.put("deltaLimit", deltaLimit);
            warning.put(key, trigger);
            warning.put("level", level);
            warnings.put(warning);
          }
        }

        // Fires warnings as metrics approach ratios of the device's limit.
        // Example: "VmRSS": {"deltaLimit": {"red": 0.90, "yellow": 0.75}}
        if (heuristic.has("limit")) {
          JSONObject limit = heuristic.getJSONObject("limit");
          long metricValue = metrics.getLong(key);
          String level = null;
          if (increasing ? metricValue > deviceLimitValue * limit.getDouble("red")
                         : metricValue * limit.getDouble("red") < deviceLimitValue) {
            level = "red";
          } else if (increasing ? metricValue > deviceLimitValue * limit.getDouble("yellow")
                                : metricValue * limit.getDouble("yellow") < deviceLimitValue) {
            level = "yellow";
          }
          if (level != null) {
            JSONObject warning = new JSONObject();
            JSONObject trigger = new JSONObject();
            trigger.put("limit", limit);
            warning.put(key, trigger);
            warning.put("level", level);
            warnings.put(warning);
          }
        }
      }

      results.put("warnings", warnings);
    }

    if (deviceLimit.has("applicationAllocated")) {
      long applicationAllocated = deviceLimit.getLong("applicationAllocated");
      JSONObject predictions = new JSONObject();

      Iterator<String> it = metrics.keys();
      while (it.hasNext()) {
        String key = it.next();

        if (!PREDICTION_FIELDS.contains(key)) {
          continue;
        }

        if (!baseline.has(key)) {
          continue;
        }

        if (!deviceLimit.has(key)) {
          continue;
        }

        if (!deviceBaseline.has(key)) {
          continue;
        }

        long delta = metrics.getLong(key) - baseline.getLong(key);
        long deviceDelta = deviceLimit.getLong(key) - deviceBaseline.getLong(key);
        if (deviceDelta == 0) {
          continue;
        }

        float percentageEstimate = (float) delta / deviceDelta;
        predictions.put(key, (long) (applicationAllocated * percentageEstimate));
      }

      results.put("predictions", predictions);
    }
    return results;
  }
}
