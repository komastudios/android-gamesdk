package net.jimblackler.collate;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 * A tool to analyze the recent run of iStresser in order to extract the record values of certain
 * memory metrics for all devices in the run.
 */
public class Discover {
  public static void main(String[] args) throws IOException {
    ObjectWriter objectWriter = new ObjectMapper().writerWithDefaultPrettyPrinter();
    Map<String, Object> recordResults = new LinkedHashMap<>();

    Collector.cloudCollect(null, result -> {
      if (result.isEmpty()) {
        return;
      }
      Map<String, Object> first = (Map<String, Object>) result.get(0);

      if (!first.containsKey("params")) {
        System.out.println("No usable results. Data returned was:");
        try {
          System.out.println(objectWriter.writeValueAsString(result));
        } catch (JsonProcessingException e) {
          throw new IllegalStateException(e);
        }
        return;
      }
      Map<String, Object> deviceInfo = ReportUtils.getDeviceInfo(result);
      Map<String, Object> flattened =
          Utils.flattenParams((Map<String, Object>) first.get("params"));
      if (flattened.containsKey("advisorParameters")) {
        Map<String, Object> advisorParameters =
            (Map<String, Object>) flattened.get("advisorParameters");
        if (advisorParameters.containsKey("heuristics")
            && !((Map<String, Object>) advisorParameters.get("heuristics")).isEmpty()) {
          // Runs with heuristics are not useful.
          return;
        }
      }
      if (!flattened.containsKey("malloc") && !flattened.containsKey("glTest")) {
        // Only interested in these kinds of stress tests.
        return;
      }
      Map<String, Object> baseline = null;
      Map<String, Object> limit = null;
      Map<String, Object> firstFailed = null;
      for (int idx2 = 0; idx2 != result.size(); idx2++) {
        Map<String, Object> row = (Map<String, Object>) result.get(idx2);
        if (Boolean.TRUE.equals(row.get("activityPaused"))) {
          // The run was interrupted and is unusable.
          limit = null;
          break;
        }
        Map<String, Object> metrics = ReportUtils.rowMetrics(row);
        if (metrics == null) {
          continue;
        }
        long time = ReportUtils.rowTime(row);
        if (row.containsKey("allocFailed") || row.containsKey("mmapAnonFailed")
            || row.containsKey("mmapFileFailed") || row.containsKey("criticalLogLines")) {
          if (firstFailed == null || time < ReportUtils.rowTime(firstFailed)) {
            firstFailed = row;
          }
        }
        if (firstFailed != null && time >= ReportUtils.rowTime(firstFailed)) {
          continue;
        }
        if (metrics.containsKey("constant")) {
          if (baseline == null
              || time < ((Number) ((Map<String, Object>) baseline.get("meta")).get("time"))
                            .longValue()) {
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
            (String) ((Map<String, Object>) ((Map<String, Object>) deviceInfo.get("build"))
                          .get("fields"))
                .get("FINGERPRINT");
        Map<String, Object> baselineCurrent = (Map<String, Object>) deviceInfo.get("baseline");
        Map<String, Object> limitCurrent = ReportUtils.rowMetrics(limit);

        Map<String, Object> testMetrics = (Map<String, Object>) limit.get("testMetrics");
        long total = 0;
        for (Object o : testMetrics.values()) {
          total += ((Number) o).longValue();
        }
        Map<String, Object> stressedGroup = new LinkedHashMap<>();
        stressedGroup.put("applicationAllocated", total);
        limitCurrent.put("stressed", stressedGroup);

        if (recordResults.containsKey(fingerprint)) {
          // Multiple results are combined.
          // We take the metric result with the smallest ('worst') delta in every case.
          Map<String, Object> recordResult = (Map<String, Object>) recordResults.get(fingerprint);
          Map<String, Object> limitPrevious = (Map<String, Object>) recordResult.get("limit");
          Map<String, Object> baselinePrevious = (Map<String, Object>) recordResult.get("baseline");
          for (Map.Entry<String, Object> e : limitPrevious.entrySet()) {
            String groupName = e.getKey();
            Object value = e.getValue();
            if (!(value instanceof Map)) {
              continue;
            }
            Map<String, Object> limitPreviousGroup = (Map<String, Object>) value;
            Map<String, Object> limitCurrentGroup =
                (Map<String, Object>) limitCurrent.get(groupName);
            Map<String, Object> baselineCurrentGroup =
                (Map<String, Object>) baselineCurrent.get(groupName);
            if (baselineCurrentGroup == null) {
              continue;
            }
            Map<String, Object> baselinePreviousGroup =
                (Map<String, Object>) baselinePrevious.get(groupName);
            for (Map.Entry<String, Object> entry : limitPreviousGroup.entrySet()) {
              String key = entry.getKey();
              if (!(entry.getValue() instanceof Number)) {
                continue;
              }
              if (!baselinePreviousGroup.containsKey(key)) {
                if (((Number) limitCurrentGroup.get(key)).longValue()
                    < ((Number) limitPreviousGroup.get(key)).longValue()) {
                  limitPreviousGroup.put(key, limitCurrentGroup.get(key));
                }
                continue;
              }
              if (!(baselinePreviousGroup.get(key) instanceof Number)) {
                continue;
              }
              long previous = ((Number) limitPreviousGroup.get(key)).longValue()
                  - ((Number) baselinePreviousGroup.get(key)).longValue();
              long baselineCurrentValue = ((Number) baselineCurrentGroup.get(key)).longValue();
              long limitCurrentValue = ((Number) limitCurrentGroup.get(key)).longValue();
              long current = limitCurrentValue - baselineCurrentValue;
              if (Math.abs(current) < Math.abs(previous)) {
                baselinePreviousGroup.put(key, baselineCurrentValue);
                limitPreviousGroup.put(key, limitCurrentValue);
              }
            }
          }
        } else {
          Map<String, Object> recordResult = new LinkedHashMap<>();
          recordResult.put("baseline", baselineCurrent);
          recordResult.put("limit", limitCurrent);
          recordResults.put(fingerprint, recordResult);
        }
      }
    });

    Files.write(Paths.get("lookup.json"), objectWriter.writeValueAsBytes(recordResults));
  }
}
