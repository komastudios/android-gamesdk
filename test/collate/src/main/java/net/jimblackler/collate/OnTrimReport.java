package net.jimblackler.collate;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeSet;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.Collectors;

/**
 * A tool to analyze the recent run of iStresser in order to extract the record values of certain
 * memory metrics for all devices in the run.
 */
public class OnTrimReport {
  public static void main(String[] args) throws IOException {
    Gson gson = new GsonBuilder().setPrettyPrinting().create();
    Collection<Integer> trimLevels = new TreeSet<>();
    trimLevels.add(80);
    trimLevels.add(60);
    trimLevels.add(40);
    trimLevels.add(20);
    trimLevels.add(15);
    trimLevels.add(10);
    trimLevels.add(5);

    Map<Integer, List<Float>> fireTimes = new HashMap<>();
    trimLevels.forEach(trimLevel -> fireTimes.put(trimLevel, new ArrayList<>()));
    AtomicInteger totalResults = new AtomicInteger();
    Collector.cloudCollect(null, results -> {
      if (results.isEmpty()) {
        return;
      }
      Map<String, Object> first = (Map<String, Object>) results.get(0);

      if (!first.containsKey("params")) {
        System.out.println("No usable results. Data returned was:");
        System.out.println(gson.toJson(results));
        return;
      }

      Map<String, Object> flattened =
          Utils.flattenParams((Map<String, Object>) first.get("params"));
      Map<String, Object> advisorParameters =
          (Map<String, Object>) flattened.get("advisorParameters");
      if (advisorParameters != null) {
        Map<String, Object> heuristics = (Map<String, Object>) advisorParameters.get("heuristics");
        if (heuristics != null && !heuristics.isEmpty()) {
          // Runs with heuristics are not useful.
          return;
        }
      }
      if (!flattened.containsKey("malloc") && !flattened.containsKey("glTest")
          && !flattened.containsKey("vkTest")) {
        // Only interested in these kinds of stress tests.
        return;
      }

      Map<Integer, Map<String, Object>> firstFired = new HashMap<>();
      Map<String, Object> finalRow = null;
      for (Object o : results) {
        Map<String, Object> row = (Map<String, Object>) o;
        if (Boolean.TRUE.equals(row.get("activityPaused"))) {
          // The run was interrupted and is unusable.
          finalRow = null;
          break;
        }

        Map<String, Object> metrics = ReportUtils.rowMetrics(row);
        if (metrics == null) {
          continue;
        }

        if (Boolean.TRUE.equals(row.get("allocFailed"))
            || Boolean.TRUE.equals(row.get("mmapAnonFailed"))
            || Boolean.TRUE.equals(row.get("mmapFileFailed"))
            || row.containsKey("criticalLogLines")) {
          break;
        }
        finalRow = row;
        Number onTrim1 = (Number) metrics.get("onTrim");
        if (onTrim1 != null) {
          int onTrim = onTrim1.intValue();
          if (!trimLevels.contains(onTrim)) {
            throw new RuntimeException("Missing " + onTrim);
          }
          if (!firstFired.containsKey(onTrim)) {
            firstFired.put(onTrim, row);
          }
        }
      }
      if (finalRow != null) {
        totalResults.getAndIncrement();
        long firstTime = ReportUtils.rowTime(first);
        long finalTime = ReportUtils.rowTime(finalRow);
        trimLevels.forEach(trimLevel -> {
          if (firstFired.containsKey(trimLevel)) {
            long fireTime = ReportUtils.rowTime(firstFired.get(trimLevel));
            float relative = (float) (fireTime - firstTime) / (finalTime - firstTime);
            fireTimes.get(trimLevel).add(relative);
          }
        });
      }
    });

    System.out.println("Trim Level, Total, Fired, Times");
    trimLevels.forEach(trimLevel -> {
      System.out.print(trimLevel + ",");
      System.out.print(totalResults + ",");
      List<Float> times = fireTimes.get(trimLevel);
      System.out.print(times.size() + ",");
      System.out.println(times.stream().map(Object::toString).collect(Collectors.joining(",")));
    });
  }
}
