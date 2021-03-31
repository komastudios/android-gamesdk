package net.jimblackler.collate;

import static net.jimblackler.collate.Utils.getOrDefault;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.Collectors;

/**
 * A tool to analyze the recent run of iStresser in order to extract the record values of certain
 * memory metrics for all devices in the run.
 */
public class CanaryFiredReport {
  private static final Mode mode = Mode.FORMULA;

  public static void main(String[] args) throws IOException {
    Gson gson = new GsonBuilder().setPrettyPrinting().create();
    List<Float> fireTimes = new ArrayList<>();

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

      Map<String, Object> fired = null;
      Map<String, Object> finalRow = null;
      Map<String, Object> baselineMetrics = null;
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
        if (baselineMetrics == null) {
          baselineMetrics = metrics;
        }

        if (Boolean.TRUE.equals(row.get("allocFailed"))
            || Boolean.TRUE.equals(row.get("mmapAnonFailed"))
            || Boolean.TRUE.equals(row.get("mmapFileFailed"))
            || row.containsKey("criticalLogLines")) {
          break;
        }
        finalRow = row;
        if (fired == null) {
          switch (mode) {
            case MAP:
              if (Boolean.TRUE.equals(metrics.get("mapTester"))) {
                fired = row;
              }
              break;
            case PROCESS:
              if ("red".equals(metrics.get("canaryProcessTester"))) {
                fired = row;
              }
              break;
            case LOW_MEMORY:
              Map<String, Object> memoryInfo = (Map<String, Object>) metrics.get("MemoryInfo");
              if (memoryInfo != null) {
                if (getOrDefault(memoryInfo, "lowMemory", false)) {
                  fired = row;
                }
              }
              break;
            case FORMULA:
              Map<String, Object> status = (Map<String, Object>) metrics.get("status");
              if (status != null) {
                Number vmRSS1 = (Number) status.get("VmRSS");
                long vmRSS = vmRSS1 == null ? 0 : vmRSS1.longValue();
                Map<String, Object> baselineMemoryInfo =
                    (Map<String, Object>) baselineMetrics.get("MemoryInfo");
                if (baselineMemoryInfo != null) {
                  Number availMem = (Number) baselineMemoryInfo.get("availMem");
                  long baselineAvailMem = availMem == null ? 0 : availMem.longValue();
                  if (vmRSS > baselineAvailMem) {
                    fired = row;
                  }
                }
              }
          }
        }
      }
      if (finalRow != null) {
        totalResults.getAndIncrement();
        long firstTime = ReportUtils.rowTime(first);
        long finalTime = ReportUtils.rowTime(finalRow);

        if (fired != null) {
          long fireTime = ReportUtils.rowTime(fired);
          float relative = (float) (fireTime - firstTime) / (finalTime - firstTime);
          fireTimes.add(relative);
        }
      }
    });

    System.out.println("Total, Fired, Didn't Fire, Times");
    System.out.print(totalResults + ",");
    System.out.print(fireTimes.size() + ",");
    System.out.print(totalResults.get() - fireTimes.size() + ",");
    System.out.println(fireTimes.stream().map(Object::toString).collect(Collectors.joining(",")));
  }

  private enum Mode { MAP, PROCESS, LOW_MEMORY, FORMULA }
}
