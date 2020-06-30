package com.google.android.apps.internal.games.helperlibrary;

import android.content.Context;
import android.util.Log;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * Wrapper class for methods related to memory advice.
 */
public class MemoryAdvisor extends MemoryMonitor {
  private static final String TAG = MemoryMonitor.class.getSimpleName();
  private static final List<String> PREDICTION_FIELDS = Collections.singletonList("oom_score");
  private final JSONObject deviceProfile;
  private final JSONObject params;

  /**
   * Create an Android memory advice fetcher.
   *
   * @param context The Android context to employ.
   * @param params The active configuration.
   */
  public MemoryAdvisor(Context context, JSONObject params) {
    super(context, params.optBoolean("fetchDebug"));
    this.params = params;
    deviceProfile = DeviceProfile.getDeviceProfile(context.getAssets());
  }

  /**
   * The value the advisor returns when asked for memory pressure on the device through the
   * getSignal method. GREEN indicates it is safe to allocate further, YELLOW indicates further
   * allocation shouldn't happen, and RED indicates high memory pressure.
   */
  public JSONObject getAdvice(JSONObject metrics) {
    long time = System.currentTimeMillis();
    JSONObject results = new JSONObject();

    try {
      JSONObject baseline = getBaseline();
      JSONObject limits = deviceProfile.getJSONObject("limits");
      JSONObject deviceLimit = limits.getJSONObject("limit");
      JSONObject deviceBaseline = limits.getJSONObject("baseline");

      if (params.has("heuristics")) {
        JSONArray warnings = new JSONArray();
        JSONObject heuristics = params.getJSONObject("heuristics");
        if (heuristics.has("try")) {
          if (!TryAllocTester.tryAlloc((int) Utils.getMemoryQuantity(heuristics.get("try")))) {
            JSONObject warning = new JSONObject();
            warning.put("try", heuristics.get("try"));
            warning.put("level", "red");
            warnings.put(warning);
          }
        }

        if (heuristics.has("lowMemory")) {
          if (metrics.optBoolean("lowMemory")) {
            JSONObject warning = new JSONObject();
            warning.put("lowMemory", heuristics.get("lowMemory"));
            warning.put("level", "red");
            warnings.put(warning);
          }
        }

        if (heuristics.has("mapTester")) {
          if (metrics.optBoolean("mapTester")) {
            JSONObject warning = new JSONObject();
            warning.put("mapTester", heuristics.get("mapTester"));
            warning.put("level", "red");
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
            } else if (increasing
                    ? metricValue > baselineValue * baselineRatio.getDouble("yellow")
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
          predictions.put(key, (long) (applicationAllocated * (1.0f - percentageEstimate)));
        }

        results.put("predictions", predictions);
        JSONObject meta = new JSONObject();
        meta.put("duration", System.currentTimeMillis() - time);
        results.put("meta", meta);
      }
    } catch (JSONException ex) {
      Log.w(TAG, "Problem performing memory analysis", ex);
    }
    return results;
  }

  public JSONObject getDeviceProfile() {
    return deviceProfile;
  }
}
