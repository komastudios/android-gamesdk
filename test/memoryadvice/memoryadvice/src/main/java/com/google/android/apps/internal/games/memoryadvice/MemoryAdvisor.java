package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getMemoryQuantity;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;
import com.fasterxml.jackson.databind.ObjectMapper;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

/**
 * Wrapper class for methods related to memory advice.
 */
public class MemoryAdvisor {
  private static final String TAG = MemoryMonitor.class.getSimpleName();
  private static final long BYTES_IN_KILOBYTE = 1024;
  private static final long BYTES_IN_MEGABYTE = BYTES_IN_KILOBYTE * 1024;
  private static final long BYTES_IN_GIGABYTE = BYTES_IN_MEGABYTE * 1024;

  private final Map<String, Object> params;
  private final MemoryMonitor memoryMonitor;
  private final Map<String, Object> build;
  private Map<String, Object> baseline;
  private final Evaluator evaluator = new Evaluator();
  private Predictor realtimePredictor;
  private Predictor availablePredictor;

  /**
   * Create an Android memory advice fetcher.
   *
   * @param context The Android context to employ.
   */
  public MemoryAdvisor(Context context) {
    this(context, getDefaultParams(context.getAssets()));
  }

  /**
   * Create an Android memory advice fetcher.
   *
   * @param context      The Android context to employ.
   * @param params       The active configuration; described by advisorParameters.schema.json.
   * @throws MemoryAdvisorException
   */
  public MemoryAdvisor(Context context, Map<String, Object> params) {
    Map<String, Object> metrics = (Map<String, Object>) params.get("metrics");
    memoryMonitor = new MemoryMonitor(context, metrics);
    this.params = params;
    build = BuildInfo.getBuild(context);

    Map<String, Object> baselineSpec = (Map<String, Object>) metrics.get("baseline");
    baseline = new LinkedHashMap<>();
    if (baselineSpec == null) {
      baseline = null;
    } else {
      baseline = memoryMonitor.getMemoryMetrics(baselineSpec);
      Map<String, Object> constant = (Map<String, Object>) metrics.get("constant");
      if (constant != null) {
        baseline.put("constant", memoryMonitor.getMemoryMetrics(constant));
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
  private static Map<String, Object> getDefaultParams(AssetManager assets) {
    Map<String, Object> params;
    try {
      params = new ObjectMapper().reader().readValue(
          assets.open("memoryadvice/default.json"), Map.class);
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
    Map<String, Object> metrics = (Map<String, Object>) advice.get("metrics");
    Object predictedUsage = metrics.get("predictedAvailable");
    if (predictedUsage instanceof Number) {
      return ((Number) predictedUsage).longValue();
    }
    return 0L;
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
    if (Boolean.TRUE.equals(advice.get("backgrounded"))) {
      return MemoryState.BACKGROUNDED;
    }
    Collection<Map<String, Object>> warnings =
        (Collection<Map<String, Object>>) advice.get("warnings");
    if (warnings == null || warnings.isEmpty()) {
      return MemoryState.OK;
    }
    for (Map<String, Object> warning : warnings) {
      if (warning != null && "red".equals(warning.get("level"))) {
        return MemoryState.CRITICAL;
      }
    }
    return MemoryState.APPROACHING_LIMIT;
  }

  /**
   * Find a Number in a tree of Java objects, even when it is nested in sub-dictionaries in the
   * object.
   *
   * @param object The object to search.
   * @param key    The key of the Number to find.
   * @return The value of the Number.
   */
  private static Number getValue(Map<String, Object> object, String key) {
    if (object.containsKey(key)) {
      return (Number) object.get(key);
    }
    for (Object value : object.values()) {
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
    Map<String, Object> results = new LinkedHashMap<>();

    Map<String, Object> metricsParams = (Map<String, Object>) params.get("metrics");

    Map<String, Object> metricsSpec = (Map<String, Object>) metricsParams.get("variable");
    Map<String, Object> metrics = memoryMonitor.getMemoryMetrics(metricsSpec);
    boolean recordTimings = Boolean.TRUE.equals(metricsSpec.get("timings"));
    if (Boolean.TRUE.equals(metricsSpec.get("predictRealtime"))) {
      if (realtimePredictor == null) {
        realtimePredictor = new Predictor("/realtime.tflite", "/realtime_features.json");
      }

      long time1 = System.nanoTime();
      Map<String, Object> data = new LinkedHashMap<>();
      data.put("baseline", baseline);
      data.put("build", build);
      data.put("sample", metrics);
      try {
        metrics.put("predictedUsage", realtimePredictor.predict(data));
      } catch (MissingPathException e) {
        throw new MemoryAdvisorException(e);
      }

      if (recordTimings) {
        Map<String, Object> meta1 = new LinkedHashMap<>();
        meta1.put("duration", System.nanoTime() - time1);
        metrics.put("_predictedUsageMeta", meta1);
      }
    }

    if (Boolean.TRUE.equals(metricsSpec.get("availableRealtime"))) {
      if (availablePredictor == null) {
        availablePredictor = new Predictor("/available.tflite", "/available_features.json");
      }

      long time1 = System.nanoTime();
      Map<String, Object> data = new LinkedHashMap<>();
      data.put("baseline", baseline);
      data.put("build", build);
      data.put("sample", metrics);

      try {
        metrics.put(
            "predictedAvailable", (long) (BYTES_IN_GIGABYTE * availablePredictor.predict(data)));
      } catch (MissingPathException e) {
        throw new MemoryAdvisorException(e);
      }

      if (recordTimings) {
        Map<String, Object> meta1 = new LinkedHashMap<>();
        meta1.put("duration", System.nanoTime() - time1);
        metrics.put("_predictedAvailableMeta", meta1);
      }
    }

    results.put("metrics", metrics);

    Map<String, Object> heuristics = (Map<String, Object>) params.get("heuristics");
    if (heuristics != null) {
      Collection<Object> warnings = new ArrayList<>();
      Object try1 = heuristics.get("try");
      if (try1 != null && !TryAllocTester.tryAlloc((int) getMemoryQuantity(try1))) {
        Map<String, Object> warning = new LinkedHashMap<>();
        warning.put("try", try1);
        warning.put("level", "red");
        warnings.add(warning);
      }

      if (metrics != null) {
        Object lowMemory = heuristics.get("lowMemory");
        if (lowMemory != null) {
          Map<String, Object> memoryInfo = (Map<String, Object>) metrics.get("MemoryInfo");
          if (memoryInfo != null && Boolean.TRUE.equals(memoryInfo.get("lowMemory"))) {
            Map<String, Object> warning = new LinkedHashMap<>();
            warning.put("lowMemory", lowMemory);
            warning.put("level", "red");
            warnings.add(warning);
          }
        }

        Object mapTester = heuristics.get("mapTester");
        if (mapTester != null && Boolean.TRUE.equals(metrics.get("mapTester"))) {
          Map<String, Object> warning = new LinkedHashMap<>();
          warning.put("mapTester", mapTester);
          warning.put("level", "red");
          warnings.add(warning);
        }

        Object canaryProcessTester = heuristics.get("canaryProcessTester");
        if (canaryProcessTester != null && metrics.containsKey("canaryProcessTester")) {
          Map<String, Object> warning = new LinkedHashMap<>();
          warning.put("canaryProcessTester", canaryProcessTester);
          warning.put("level", "red");
          warnings.add(warning);
        }

        Object onTrim = heuristics.get("onTrim");
        Number onTrim1 = (Number) metrics.get("onTrim");
        if (onTrim != null && onTrim1 != null && onTrim1.longValue() > 0) {
          Map<String, Object> warning = new LinkedHashMap<>();
          warning.put("onTrim", onTrim);
          warning.put("level", "red");
          warnings.add(warning);
        }
      }

      Map<String, Object> allFormulas = (Map<String, Object>) heuristics.get("formulas");
      if (allFormulas != null) {
        for (Map.Entry<String, Object> entry : allFormulas.entrySet()) {
          List<String> formulas = (List<String>) entry.getValue();
          for (int idx = 0; idx != formulas.size(); idx++) {
            String formula = formulas.get(idx);
            try {
              if (evaluator.evaluate(formula, key1 -> {
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

      if (!warnings.isEmpty()) {
        results.put("warnings", warnings);
      }
    }

    return results;
  }

  public void setOnTrim(int level) {
    memoryMonitor.setOnTrim(level);
  }

  /**
   * Fetch information about the device.
   *
   * @return Information about the device, in a map.
   */
  public Map<String, Object> getDeviceInfo() {
    Map<String, Object> deviceInfo = new LinkedHashMap<>();
    deviceInfo.put("build", build);
    deviceInfo.put("baseline", baseline);
    deviceInfo.put("params", params);
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
