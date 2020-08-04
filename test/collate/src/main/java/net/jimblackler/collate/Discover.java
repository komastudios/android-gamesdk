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
      JSONObject deviceInfo = first.getJSONObject("deviceInfo");
      JSONObject flattened = deviceInfo.getJSONObject("params");
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
        JSONObject metrics = ReportUtils.rowMetrics(row);
        if (metrics == null) {
          continue;
        }
        long time = ReportUtils.rowTime(row);
        if (row.has("allocFailed") || row.has("mmapAnonFailed") || row.has("mmapFileFailed")
            || row.has("criticalLogLines")) {
          if (firstFailed == null || time < ReportUtils.rowTime(firstFailed)) {
            firstFailed = row;
          }
        }
        if (firstFailed != null && time >= ReportUtils.rowTime(firstFailed)) {
          continue;
        }
        if (metrics.has("constant")) {
          if (baseline == null || time < baseline.getJSONObject("meta").getLong("time")) {
            // Baseline is the earliest reading with 'constant'.
            baseline = row;
          }
        }
        if (limit == null || time > ReportUtils.rowTime(limit)) {
          // Limit is the latest reading.
          limit = row;
        }
      }
      if (limit != null) {
        String fingerprint =
            deviceInfo.getJSONObject("build").getJSONObject("fields").getString("FINGERPRINT");
        JSONObject baselineCurrent = deviceInfo.getJSONObject("baseline");
        JSONObject limitCurrent = ReportUtils.rowMetrics(limit);

        JSONObject testMetrics = limit.getJSONObject("testMetrics");
        long total = 0;
        for (String key : testMetrics.keySet()) {
          total += testMetrics.getLong(key);
        }
        JSONObject stressedGroup = new JSONObject();
        stressedGroup.put("applicationAllocated", total);
        limitCurrent.put("stressed", stressedGroup);

        if (recordResults.has(fingerprint)) {
          // Multiple results are combined.
          // We take the metric result with the smallest ('worst') delta in every case.
          JSONObject recordResult;
          recordResult = recordResults.getJSONObject(fingerprint);
          JSONObject limitPrevious = recordResult.getJSONObject("limit");
          JSONObject baselinePrevious = recordResult.getJSONObject("baseline");
          for (String groupName : limitPrevious.keySet()) {
            try {
              JSONObject limitPreviousGroup = limitPrevious.optJSONObject(groupName);
              if (limitPreviousGroup == null) {
                continue;
              }
              JSONObject limitCurrentGroup = limitCurrent.getJSONObject(groupName);
              JSONObject baselineCurrentGroup = baselineCurrent.optJSONObject(groupName);
              if (baselineCurrentGroup == null) {
                continue;
              }
              JSONObject baselinePreviousGroup = baselinePrevious.getJSONObject(groupName);
              for (String key : limitPreviousGroup.keySet()) {
                if (!(limitPreviousGroup.get(key) instanceof Number)) {
                  continue;
                }
                if (!baselinePreviousGroup.has(key)) {
                  if (limitCurrentGroup.getLong(key) < limitPreviousGroup.getLong(key)) {
                    limitPreviousGroup.put(key, limitCurrentGroup.getLong(key));
                  }
                  continue;
                }
                if (!(baselinePreviousGroup.get(key) instanceof Number)) {
                  continue;
                }
                long previous =
                    limitPreviousGroup.getLong(key) - baselinePreviousGroup.getLong(key);
                long baselineCurrentValue = baselineCurrentGroup.getLong(key);
                long limitCurrentValue = limitCurrentGroup.getLong(key);
                long current = limitCurrentValue - baselineCurrentValue;
                if (Math.abs(current) < Math.abs(previous)) {
                  baselinePreviousGroup.put(key, baselineCurrentValue);
                  limitPreviousGroup.put(key, limitCurrentValue);
                }
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
