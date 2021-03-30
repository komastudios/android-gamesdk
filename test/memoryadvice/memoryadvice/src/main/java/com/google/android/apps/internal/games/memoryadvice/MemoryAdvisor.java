package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getMemoryQuantity;
import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getOrDefault;
import static com.google.android.apps.internal.games.memoryadvice_common.StreamUtils.readStream;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;
import com.google.gson.Gson;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

/**
 * Wrapper class for methods related to memory advice.
 */
public class MemoryAdvisor extends MemoryMonitor {
  private static final String TAG = MemoryMonitor.class.getSimpleName();
  private final Map<String, Object> deviceProfile;
  private final Map<String, Object> params;
  private final float predictedOomLimit;
  private Map<String, Object> onDeviceLimit;
  private Map<String, Object> onDeviceBaseline;
  private final Evaluator evaluator = new Evaluator();

  /**
   * Create an Android memory advice fetcher.
   *
   * @param context The Android context to employ.
   */
  public MemoryAdvisor(Context context, ReadyHandler readyHandler) {
    this(context, getDefaultParams(context.getAssets()), readyHandler);
  }

  /**
   * Create an Android memory advice fetcher.
   *
   * @param context The Android context to employ.
   * @param params  The active configuration.
   * @throws MemoryAdvisorException
   */
  public MemoryAdvisor(Context context, Map<String, Object> params) {
    this(context, params, null);
  }

  /**
   * Create an Android memory advice fetcher.
   *
   * @param context      The Android context to employ.
   * @param params       The active configuration; described by params.schema.json.
   * @param readyHandler A callback used when on device stress test is required.
   * @throws MemoryAdvisorException
   */
  public MemoryAdvisor(Context context, Map<String, Object> params, ReadyHandler readyHandler) {
    super(context, (Map<String, Object>) params.get("metrics"));
    this.params = params;
    ScheduledExecutorService scheduledExecutorService =
        Executors.newSingleThreadScheduledExecutor();

    if (getOrDefault(params, "predictOomLimit", false)) {
      Predictor predictor = new Predictor("/oom.tflite", "/oom_features.json");
      predictedOomLimit = predictor.predict(getDeviceInfo(context));
    } else {
      predictedOomLimit = -1;
    }

    if (getOrDefault(params, "onDeviceStressTest", false)) {
      deviceProfile = null;
      if (readyHandler == null) {
        throw new MemoryAdvisorException("Ready handler required for on device stress test");
      }
      new OnDeviceStressTester(context, params, new OnDeviceStressTester.Consumer() {
        public void progress(Map<String, Object> metrics) {
          readyHandler.stressTestProgress(metrics);
        }

        public void accept(
            Map<String, Object> baseline, Map<String, Object> limit, boolean timedOut) {
          onDeviceBaseline = baseline;
          onDeviceLimit = limit;
          scheduledExecutorService.schedule(
              () -> readyHandler.onComplete(timedOut), 1, TimeUnit.MILLISECONDS);
        }
      });
    } else {
      deviceProfile = DeviceProfile.getDeviceProfile(context.getAssets(), params, baseline);
      if (readyHandler != null) {
        scheduledExecutorService.schedule(
            () -> readyHandler.onComplete(false), 1, TimeUnit.MILLISECONDS);
      }
    }
  }

  /**
   * Get the library's default parameters. These can be selectively modified by the application and
   * passed back in to the constructor.
   *
   * @param assets The AssetManager used to fetch the default parameter file.
   * @return The default parameters as a JSON object.
   */
  public static Map<String, Object> getDefaultParams(AssetManager assets) {
    Map<String, Object> params;
    try {
      params = new Gson().fromJson(readStream(assets.open("memoryadvice/default.json")), Map.class);
    } catch (IOException ex) {
      Log.e(TAG, "Problem getting default params", ex);
      params = new LinkedHashMap<>();
    }
    return params;
  }

  /**
   * Returns 'true' if there are any low memory warnings in the advice object.
   *
   * @param advice The advice object returned by getAdvice().
   * @return if there are any low memory warnings in the advice object.
   * @deprecated since 0.7. Use getMemoryState() instead.
   */
  @Deprecated
  public static boolean anyWarnings(Map<String, Object> advice) {
    Collection<Object> warnings = (Collection<Object>) advice.get("warnings");
    return warnings != null && !warnings.isEmpty();
  }

  /**
   * Returns an estimate for the amount of memory that can safely be allocated,
   * in bytes.
   *
   * @param advice The advice object returned by getAdvice().
   * @return an estimate for the amount of memory that can safely be allocated,
   * in bytes. 0 if no estimate is available.
   */
  public static long availabilityEstimate(Map<String, Object> advice) {
    if (!advice.containsKey("predictions")) {
      return 0;
    }

    long smallestEstimate = Long.MAX_VALUE;
    Map<String, Object> predictions = (Map<String, Object>) advice.get("predictions");
    Iterator<String> it = predictions.keySet().iterator();
    if (!it.hasNext()) {
      return 0;
    }
    do {
      String key = it.next();
      smallestEstimate = Math.min(smallestEstimate, ((Number) predictions.get(key)).longValue());
    } while (it.hasNext());
    return smallestEstimate;
  }

  /**
   * Return 'true' if there are any 'red' (critical) warnings in the advice object.
   *
   * @param advice The advice object returned by getAdvice().
   * @return if there are any 'red' (critical) warnings in the advice object.
   * @deprecated since 0.7. Use getMemoryState() instead.
   */
  @Deprecated
  public static boolean anyRedWarnings(Map<String, Object> advice) {
    List<Object> warnings = (List<Object>) advice.get("warnings");
    if (warnings == null) {
      return false;
    }

    for (int idx = 0; idx != warnings.size(); idx++) {
      Map<String, Object> warning = (Map<String, Object>) warnings.get(idx);
      if (warning != null && "red".equals(warning.get("level"))) {
        return true;
      }
    }
    return false;
  }

  /**
   * Get the memory state from an advice object returned by the Memory Advisor.
   *
   * @param advice The object to analyze for the memory state.
   * @return The current memory state.
   */
  public static MemoryState getMemoryState(Map<String, Object> advice) {
    if (getOrDefault(advice, "backgrounded", false)) {
      return MemoryState.BACKGROUNDED;
    }
    if (anyRedWarnings(advice)) {
      return MemoryState.CRITICAL;
    }
    if (anyWarnings(advice)) {
      return MemoryState.APPROACHING_LIMIT;
    }
    return MemoryState.OK;
  }

  /**
   * Find a Number in a JSON object, even when it is nested in sub-dictionaries in the object.
   *
   * @param object The object to search.
   * @param key    The key of the Number to find.
   * @return The value of the Number.
   */
  private static Number getValue(Map<String, Object> object, String key) {
    if (object.containsKey(key)) {
      return (Number) object.get(key);
    }
    Iterator<String> it = object.keySet().iterator();
    while (it.hasNext()) {
      Object value = object.get(it.next());
      if (value instanceof Map) {
        Number value1 = getValue((Map<String, Object>) value, key);
        if (value1 != null) {
          return value1;
        }
      }
    }
    return null;
  }

  /**
   * The value the advisor returns when asked for memory pressure on the device through the
   * getSignal method. GREEN indicates it is safe to allocate further, YELLOW indicates further
   * allocation shouldn't happen, and RED indicates high memory pressure.
   */
  public Map<String, Object> getAdvice() throws MemoryAdvisorException {
    long time = System.currentTimeMillis();
    Map<String, Object> results = new LinkedHashMap<>();

    Map<String, Object> metricsParams = (Map<String, Object>) params.get("metrics");
    Map<String, Object> metrics =
        getMemoryMetrics((Map<String, Object>) metricsParams.get("variable"));
    results.put("metrics", metrics);
    Map<String, Object> deviceBaseline;
    Map<String, Object> deviceLimit;
    if (deviceProfile != null) {
      Map<String, Object> limits = (Map<String, Object>) deviceProfile.get("limits");
      deviceLimit = (Map<String, Object>) limits.get("limit");
      deviceBaseline = (Map<String, Object>) limits.get("baseline");
    } else if (onDeviceLimit != null) {
      deviceBaseline = onDeviceBaseline;
      deviceLimit = onDeviceLimit;
    } else {
      throw new MemoryAdvisorException("Methods called before Advisor was ready");
    }

    if (params.containsKey("heuristics")) {
      List<Object> warnings = new ArrayList<>();
      Map<String, Object> heuristics = (Map<String, Object>) params.get("heuristics");
      if (heuristics.containsKey("try")) {
        if (!TryAllocTester.tryAlloc((int) getMemoryQuantity(heuristics.get("try")))) {
          Map<String, Object> warning = new LinkedHashMap<>();
          warning.put("try", heuristics.get("try"));
          warning.put("level", "red");
          warnings.add(warning);
        }
      }

      if (heuristics.containsKey("lowMemory")) {
        Map<String, Object> memoryInfo = (Map<String, Object>) metrics.get("MemoryInfo");
        if (memoryInfo != null && getOrDefault(memoryInfo, "lowMemory", false)) {
          Map<String, Object> warning = new LinkedHashMap<>();
          warning.put("lowMemory", heuristics.get("lowMemory"));
          warning.put("level", "red");
          warnings.add(warning);
        }
      }

      if (heuristics.containsKey("mapTester")) {
        if (getOrDefault(metrics, "mapTester", false)) {
          Map<String, Object> warning = new LinkedHashMap<>();
          warning.put("mapTester", heuristics.get("mapTester"));
          warning.put("level", "red");
          warnings.add(warning);
        }
      }

      if (heuristics.containsKey("canaryProcessTester")
          && metrics.containsKey("canaryProcessTester")) {
        Map<String, Object> warning = new LinkedHashMap<>();
        warning.put("canaryProcessTester", heuristics.get("canaryProcessTester"));
        warning.put("level", "red");
        warnings.add(warning);
      }

      if (heuristics.containsKey("onTrim") && getOrDefault(metrics, "onTrim", 0L) > 0) {
        Map<String, Object> warning = new LinkedHashMap<>();
        warning.put("onTrim", heuristics.get("onTrim"));
        warning.put("level", "red");
        warnings.add(warning);
      }

      if (heuristics.containsKey("formulas")) {
        Map<String, Object> allFormulas = (Map<String, Object>) heuristics.get("formulas");
        for (Map.Entry<String, Object> entry : allFormulas.entrySet()) {
          List<String> formulas = (List<String>) entry.getValue();
          for (int idx = 0; idx != formulas.size(); idx++) {
            String formula = formulas.get(idx);
            try {
              if (evaluator.evaluate(formula, key1 -> {
                    if ("predictedOomLimit".equals(key1)) {
                      return (double) predictedOomLimit;
                    }
                    Map<String, Object> dictionary;
                    if (key1.startsWith("baseline.")) {
                      key1 = key1.substring("baseline.".length());
                      dictionary = baseline;
                    } else {
                      dictionary = metrics;
                    }
                    Number value = getValue(dictionary, key1);
                    if (value == null) {
                      throw new LookupException(key1 + " not defined");
                    }
                    return value.doubleValue();
                  })) {
                Map<String, Object> warning = new LinkedHashMap<>();
                warning.put("formula", formula);
                warning.put("level", entry.getKey());
                warnings.add(warning);
              }
            } catch (LookupException ex) {
              Log.w(TAG, ex);
            }
          }
        }
      }

      // Handler for device-based metrics.
      for (Map.Entry<String, Object> entry : heuristics.entrySet()) {
        String key = entry.getKey();
        Object value = entry.getValue();
        if (!(value instanceof Map)) {
          continue;
        }
        Map<String, Object> heuristic = (Map<String, Object>) value;
        Number _metricValue = getValue(metrics, key);
        if (_metricValue == null) {
          continue;
        }
        long metricValue = _metricValue.longValue();

        Number _deviceLimitValue = getValue(deviceLimit, key);
        if (_deviceLimitValue == null) {
          continue;
        }
        long deviceLimitValue = _deviceLimitValue.longValue();

        Number _deviceBaselineValue = getValue(deviceBaseline, key);
        if (_deviceBaselineValue == null) {
          continue;
        }
        long deviceBaselineValue = _deviceBaselineValue.longValue();

        Number _baselineValue = getValue(baseline, key);
        if (_baselineValue == null) {
          continue;
        }
        long baselineValue = _baselineValue.longValue();

        boolean increasing = deviceLimitValue > deviceBaselineValue;

        // Fires warnings as metrics approach absolute values.
        // Example: "Active": {"fixed": {"red": "300M", "yellow": "400M"}}
        if (heuristic.containsKey("fixed")) {
          Map<String, Object> fixed = (Map<String, Object>) heuristic.get("fixed");
          long red = getMemoryQuantity(fixed.get("red"));
          long yellow = getMemoryQuantity(fixed.get("yellow"));
          String level = null;
          if (increasing ? metricValue > red : metricValue < red) {
            level = "red";
          } else if (increasing ? metricValue > yellow : metricValue < yellow) {
            level = "yellow";
          }
          if (level != null) {
            Map<String, Object> warning = new LinkedHashMap<>();
            Map<String, Object> trigger = new LinkedHashMap<>();
            trigger.put("fixed", fixed);
            warning.put(key, trigger);
            warning.put("level", level);
            warnings.add(warning);
          }
        }

        // Fires warnings as metrics approach ratios of the device baseline.
        // Example: "availMem": {"baselineRatio": {"red": 0.30, "yellow": 0.40}}
        if (heuristic.containsKey("baselineRatio")) {
          Map<String, Object> baselineRatio = (Map<String, Object>) heuristic.get("baselineRatio");

          String level = null;
          double baselineRed = ((Number) baselineRatio.get("red")).doubleValue();
          if (increasing ? metricValue > baselineValue * baselineRed
                         : metricValue < baselineValue * baselineRed) {
            level = "red";
          } else {
            double baselineYellow = ((Number) baselineRatio.get("yellow")).doubleValue();
            if (increasing ? metricValue > baselineValue * baselineYellow
                           : metricValue < baselineValue * baselineYellow) {
              level = "yellow";
            }
          }
          if (level != null) {
            Map<String, Object> warning = new LinkedHashMap<>();
            Map<String, Object> trigger = new LinkedHashMap<>();
            trigger.put("baselineRatio", baselineRatio);
            warning.put(key, trigger);
            warning.put("level", level);
            warnings.add(warning);
          }
        }

        // Fires warnings as baseline-relative metrics approach ratios of the device's baseline-
        // relative limit.
        // Example: "oom_score": {"deltaLimit": {"red": 0.85, "yellow": 0.75}}
        if (heuristic.containsKey("deltaLimit")) {
          Map<String, Object> deltaLimit = (Map<String, Object>) heuristic.get("deltaLimit");
          long limitValue = deviceLimitValue - deviceBaselineValue;
          long relativeValue = metricValue - baselineValue;
          String level = null;
          double deltaLimitRed = ((Number) deltaLimit.get("red")).doubleValue();
          if (increasing ? relativeValue > limitValue * deltaLimitRed
                         : relativeValue < limitValue * deltaLimitRed) {
            level = "red";
          } else {
            double deltaLimitYellow = ((Number) deltaLimit.get("yellow")).doubleValue();
            if (increasing ? relativeValue > limitValue * deltaLimitYellow
                           : relativeValue < limitValue * deltaLimitYellow) {
              level = "yellow";
            }
          }
          if (level != null) {
            Map<String, Object> warning = new LinkedHashMap<>();
            Map<String, Object> trigger = new LinkedHashMap<>();
            trigger.put("deltaLimit", deltaLimit);
            warning.put(key, trigger);
            warning.put("level", level);
            warnings.add(warning);
          }
        }

        // Fires warnings as metrics approach ratios of the device's limit.
        // Example: "VmRSS": {"deltaLimit": {"red": 0.90, "yellow": 0.75}}
        if (heuristic.containsKey("limit")) {
          Map<String, Object> limit = (Map<String, Object>) heuristic.get("limit");
          String level = null;
          double redLimit = ((Number) limit.get("red")).doubleValue();
          if (increasing ? metricValue > deviceLimitValue * redLimit
                         : metricValue * redLimit < deviceLimitValue) {
            level = "red";
          } else {
            double yellowLimit = ((Number) limit.get("yellow")).doubleValue();
            if (increasing ? metricValue > deviceLimitValue * yellowLimit
                           : metricValue * yellowLimit < deviceLimitValue) {
              level = "yellow";
            }
          }
          if (level != null) {
            Map<String, Object> warning = new LinkedHashMap<>();
            Map<String, Object> trigger = new LinkedHashMap<>();
            trigger.put("limit", limit);
            warning.put(key, trigger);
            warning.put("level", level);
            warnings.add(warning);
          }
        }
      }

      if (!warnings.isEmpty()) {
        results.put("warnings", warnings);
      }
    }

    if (deviceLimit.containsKey("stressed") && params.containsKey("predictions")) {
      Map<String, Object> stressed = (Map<String, Object>) deviceLimit.get("stressed");
      if (stressed.containsKey("applicationAllocated")) {
        long applicationAllocated = ((Number) stressed.get("applicationAllocated")).longValue();
        Map<String, Object> predictions = new LinkedHashMap<>();
        Map<String, Object> predictionsParams = (Map<String, Object>) params.get("predictions");

        for (String key : predictionsParams.keySet()) {
          Number metricValue = getValue(metrics, key);
          if (metricValue == null) {
            continue;
          }

          Number deviceLimitValue = getValue(deviceLimit, key);
          if (deviceLimitValue == null) {
            continue;
          }

          Number deviceBaselineValue = getValue(deviceBaseline, key);
          if (deviceBaselineValue == null) {
            continue;
          }

          Number baselineValue = getValue(baseline, key);
          if (baselineValue == null) {
            continue;
          }

          long delta = metricValue.longValue() - baselineValue.longValue();
          long deviceDelta = deviceLimitValue.longValue() - deviceBaselineValue.longValue();
          if (deviceDelta == 0) {
            continue;
          }

          float percentageEstimate = (float) delta / deviceDelta;
          predictions.put(key, (long) (applicationAllocated * (1.0f - percentageEstimate)));
        }

        results.put("predictions", predictions);
        Map<String, Object> meta = new LinkedHashMap<>();
        meta.put("duration", System.currentTimeMillis() - time);
        results.put("meta", meta);
      }
    }
    return results;
  }

  /**
   * Fetch information about the device.
   *
   * @param context The Android context.
   * @return Information about the device, in a map.
   */
  public Map<String, Object> getDeviceInfo(Context context) {
    Map<String, Object> deviceInfo = new LinkedHashMap<>();
    deviceInfo.put("build", build);
    deviceInfo.put("baseline", baseline);
    if (deviceProfile != null) {
      deviceInfo.put("deviceProfile", deviceProfile);
    }
    if (onDeviceBaseline != null) {
      deviceInfo.put("deviceBaseline", onDeviceBaseline);
    }
    if (onDeviceLimit != null) {
      deviceInfo.put("deviceLimit", onDeviceLimit);
    }
    deviceInfo.put("params", params);
    if (predictedOomLimit != -1) {
      deviceInfo.put("predictedOomLimit", predictedOomLimit);
    }
    return deviceInfo;
  }

  /**
   * Advice passed from the memory advisor to the application about the state of memory.
   */
  public enum MemoryState {
    /**
     * The memory state cannot be determined.
     */
    UNKNOWN,

    /**
     * The application can safely allocate significant memory.
     */
    OK,

    /**
     * The application should not allocate significant memory.
     */
    APPROACHING_LIMIT,

    /**
     * The application should free memory as soon as possible, until the memory state changes.
     */
    CRITICAL,

    /**
     * The application is backgrounded. The library does not make advice in this state.
     */
    BACKGROUNDED
  }
}
