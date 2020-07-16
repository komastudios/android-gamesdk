package com.google.android.apps.internal.games.memoryadvice;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import java.io.IOException;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import static com.google.android.apps.internal.games.memoryadvice.Utils.readStream;

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
   */
  public MemoryAdvisor(Context context) {
    this(context, getDefaultParams(context.getAssets()));
  }

  private static JSONObject getDefaultParams(AssetManager assets) {
    JSONObject params;
    try {
      params = new JSONObject(readStream(assets.open("memoryadvice/default.json")));
    } catch (JSONException | IOException ex) {
      Log.e(TAG, "Problem getting default params", ex);
      params = new JSONObject();
    }
    return params;
  }

  /**
   * Create an Android memory advice fetcher.
   *
   * @param context The Android context to employ.
   * @param params The active configuration.
   */
  public MemoryAdvisor(Context context, JSONObject params) {
    super(context, params.optJSONObject("metrics"));
    this.params = params;
    deviceProfile = DeviceProfile.getDeviceProfile(context.getAssets(), params, baseline);
  }

  /**
   * Returns 'true' if there are any low memory warnings in the advice object.
   * @param advice The advice object returned by getAdvice().
   * @return if there are any low memory warnings in the advice object.
   */
  public static boolean anyWarnings(JSONObject advice) {
    JSONArray warnings = advice.optJSONArray("warnings");
    return warnings != null && warnings.length() > 0;
  }

  /**
   * Returns an estimate for the amount of memory that can safely be allocated,
   * in bytes.
   * @param advice The advice object returned by getAdvice().
   * @return an estimate for the amount of memory that can safely be allocated,
   * in bytes. 0 if no estimate is available.
   */
  public static long availabilityEstimate(JSONObject advice) {
    if (!advice.has("predictions")) {
      return 0;
    }
    try {
      long smallestEstimate = Long.MAX_VALUE;
      JSONObject predictions = advice.getJSONObject("predictions");
      Iterator<String> it = predictions.keys();
      if (!it.hasNext()) {
        return 0;
      }
      do {
        String key = it.next();
        smallestEstimate = Math.min(smallestEstimate, predictions.getLong(key));
      } while (it.hasNext());
      return smallestEstimate;
    } catch (JSONException ex) {
      Log.w(TAG, "Problem getting memory estimate", ex);
    }
    return 0;
  }

  /**
   * Return 'true' if there are any 'red' (critical) warnings in the advice object.
   * @param advice The advice object returned by getAdvice().
   * @return if there are any 'red' (critical) warnings in the advice object.
   */
  public static boolean anyRedWarnings(JSONObject advice) {
    JSONArray warnings = advice.optJSONArray("warnings");
    if (warnings == null) {
      return false;
    }

    for (int idx = 0; idx != warnings.length(); idx++) {
      JSONObject warning = warnings.optJSONObject(idx);
      if (warning != null && "red".equals(warning.optString("level"))) {
        return true;
      }
    }
    return false;
  }

  /**
   * The value the advisor returns when asked for memory pressure on the device through the
   * getSignal method. GREEN indicates it is safe to allocate further, YELLOW indicates further
   * allocation shouldn't happen, and RED indicates high memory pressure.
   */
  public JSONObject getAdvice() {
    long time = System.currentTimeMillis();
    JSONObject results = new JSONObject();

    try {
      JSONObject metricsParams = params.getJSONObject("metrics");
      JSONObject metrics = getMemoryMetrics(metricsParams.getJSONObject("variable"));
      results.put("metrics", metrics);
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

        if (heuristics.has("onTrim")) {
          if (metrics.optInt("onTrim") > 0) {
            JSONObject warning = new JSONObject();
            warning.put("onTrim", heuristics.get("onTrim"));
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

        if (warnings.length() > 0) {
          results.put("warnings", warnings);
        }
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

  /**
   * Fetch information about the device.
   * @param context The Android context.
   * @return Information about the device, in a JSONObject.
   */
  public JSONObject getDeviceInfo(Context context) {
    JSONObject deviceInfo = new JSONObject();
    try {
      deviceInfo.put("build", BuildInfo.getBuild(context));
      deviceInfo.put("baseline", baseline);
      deviceInfo.put("deviceProfile", deviceProfile);
      deviceInfo.put("params", params);
    } catch (JSONException ex) {
      Log.w(TAG, "Problem getting device info", ex);
    }
    return deviceInfo;
  }
}
