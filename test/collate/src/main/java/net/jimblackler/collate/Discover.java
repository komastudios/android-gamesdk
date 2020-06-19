package net.jimblackler.collate;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * A tool to analyze the recent run of iStresser in order to extract the record values of certain
 * memory metrics for all devices in the run.
 */
public class Discover {
  public static void main(String[] args) throws IOException {
    JSONObject recordResults = new JSONObject();

    Collector.cloudCollect(null, result -> {
      if (result.isEmpty()) {
        return;
      }
      JSONObject first = result.getJSONObject(0);

      if (!first.has("params")) {
        System.out.println("No usable results. Data returned was:");
        System.out.println(result.toString(2));
        return;
      }
      JSONObject params = first.getJSONObject("params");
      JSONObject flattened = Utils.flattenParams(params);
      if (flattened.has("heuristics") && !flattened.getJSONObject("heuristics").isEmpty()) {
        // Runs with heuristics are not useful.
        return;
      }
      if (!flattened.has("malloc") && !flattened.has("glTest")) {
        // Only interested in these kinds of stress tests.
        return;
      }
      JSONObject baseline = null;
      JSONObject limit = null;
      JSONObject firstFailed = null;
      for (int idx2 = 0; idx2 != result.length(); idx2++) {
        JSONObject row = result.getJSONObject(idx2);
        if (row.optBoolean("activityPaused")) {
          // The run was interrupted and is unusable.
          limit = null;
          break;
        }
        if (!row.has("time")) {
          continue;
        }
        long time = row.getLong("time");
        if (row.has("allocFailed") || row.has("mmapAnonFailed") || row.has("mmapFileFailed")
            || row.has("criticalLogLines")) {
          if (firstFailed == null || time < firstFailed.getLong("time")) {
            firstFailed = row;
          }
        }
        if (firstFailed != null && time >= firstFailed.getLong("time")) {
          continue;
        }
        if (!row.has("metrics")) {
          continue;
        }
        if (baseline == null || time < baseline.getLong("time")) {
          // Baseline is the earliest reading.
          baseline = row;
        }
        if (limit == null || time > limit.getLong("time")) {
          // Limit is the latest reading.
          limit = row;
        }
      }
      if (limit != null) {
        String fingerprint = first.getJSONObject("build").getString("FINGERPRINT");
        JSONObject baselineCurrent = baseline.getJSONObject("metrics");
        JSONObject limitCurrent = limit.getJSONObject("metrics");

        JSONObject testMetrics = limit.getJSONObject("testMetrics");
        long total = 0;
        for (String key : testMetrics.keySet()) {
          total += testMetrics.getLong(key);
        }
        limitCurrent.put("applicationAllocated", total);

        if (recordResults.has(fingerprint)) {
          // Multiple results are combined.
          // We take the metric result with the smallest ('worst') delta in every case.
          JSONObject recordResult;
          recordResult = recordResults.getJSONObject(fingerprint);
          JSONObject limitPrevious = recordResult.getJSONObject("limit");
          JSONObject baselinePrevious = recordResult.getJSONObject("baseline");
          for (String key : limitPrevious.keySet()) {
            try {
              if (!(limitPrevious.get(key) instanceof Number)) {
                continue;
              }
              if (!baselinePrevious.has(key)) {
                if (limitCurrent.getLong(key) < limitPrevious.getLong(key)) {
                  limitPrevious.put(key, limitCurrent.getLong(key));
                }
                continue;
              }
              if (!(baselinePrevious.get(key) instanceof Number)) {
                continue;
              }
              long previous = limitPrevious.getLong(key) - baselinePrevious.getLong(key);
              long baselineCurrentValue = baselineCurrent.getLong(key);
              long limitCurrentValue = limitCurrent.getLong(key);
              long current = limitCurrentValue - baselineCurrentValue;
              if (Math.abs(current) < Math.abs(previous)) {
                baselinePrevious.put(key, baselineCurrentValue);
                limitPrevious.put(key, limitCurrentValue);
              }
            } catch (JSONException e) {
              throw new IllegalStateException(e);
            }
          }
        } else {
          JSONObject recordResult = new JSONObject();
          recordResult.put("baseline", baselineCurrent);
          recordResult.put("limit", limitCurrent);
          recordResults.put(fingerprint, recordResult);
        }
      }
    });

    Files.write(Paths.get("lookup.json"), recordResults.toString(2).getBytes());
  }
}
