package net.jimblackler.collate;

import java.io.IOException;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.function.Consumer;

/**
 * Generates a report about how long it took to gather various metric groups.
 */
public class Timing {
  private static final boolean USE_DEVICE = false;

  public static void main(String[] args) throws IOException {
    go(USE_DEVICE);
  }

  static void go(boolean useDevice) throws IOException {
    Set<String> allGroups = new HashSet<>();
    Map<String, Map<String, Long>> totalsByFingerprint = new HashMap<>();
    Map<String, Map<String, Long>> countsByFingerprint = new HashMap<>();

    Consumer<List<Map<String, Object>>> collect = results -> {
      Map<String, Object> params = (Map<String, Object>) results.get(0).get("params");
      if (params == null) {
        return;
      }
      Map<String, Long> totals = new HashMap<>();
      Map<String, Long> counts = new HashMap<>();
      boolean allZeroes = true;

      for (Object o : (Iterable<Object>) params.get("coordinates")) {
        int coordinate = ((Number) o).intValue();
        if (coordinate != 0) {
          allZeroes = false;
        }
      }
      if (!allZeroes) {
        return;
      }

      for (Object o : results) {
        Map<String, Object> row = (Map<String, Object>) o;
        if (!row.containsKey("metrics")) {
          continue;
        }
        Map<String, Object> metrics = (Map<String, Object>) row.get("metrics");

        Map<String, Object> memInfo = (Map<String, Object>) metrics.get("memInfo");
        if (memInfo != null) {
          register("memInfo", memInfo, totals, counts, allGroups);
        }

        Map<String, Object> debug = (Map<String, Object>) metrics.get("debug");
        if (debug != null) {
          register("debug", debug, totals, counts, allGroups);
        }

        Map<String, Object> memoryInfo = (Map<String, Object>) metrics.get("MemoryInfo");
        if (memInfo != null) {
          register("MemoryInfo", memoryInfo, totals, counts, allGroups);
        }

        Map<String, Object> proc = (Map<String, Object>) metrics.get("proc");
        if (proc != null) {
          register("proc", proc, totals, counts, allGroups);
        }

        Map<String, Object> status = (Map<String, Object>) metrics.get("status");
        if (status != null) {
          register("status", status, totals, counts, allGroups);
        }

        Map<String, Object> summary = (Map<String, Object>) metrics.get("summary");
        if (summary != null) {
          register("summary", summary, totals, counts, allGroups);
        }

        Map<String, Object> activityManager = (Map<String, Object>) metrics.get("activityManager");
        if (metrics.containsKey("activityManager")) {
          register("activityManager", activityManager, totals, counts, allGroups);
        }
      }

      Map<String, Object> deviceInfo = ReportUtils.getDeviceInfo(results);
      if (deviceInfo == null) {
        return;
      }
      Map<String, Object> deviceProfile = (Map<String, Object>) deviceInfo.get("deviceProfile");
      String fingerprint = (String) deviceProfile.get("fingerprint");
      totalsByFingerprint.put(fingerprint, totals);
      countsByFingerprint.put(fingerprint, counts);
    };

    if (useDevice) {
      Collector.deviceCollect("net.jimblackler.istresser", collect);
    } else {
      Collector.cloudCollect(null, collect);
    }

    List<String> allGroupsSorted = new ArrayList<>(allGroups);
    allGroupsSorted.sort(String::compareTo);

    try (PrintWriter writer = new PrintWriter(Files.newBufferedWriter(Paths.get("timing.csv")))) {
      {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append("Fingerprint");
        for (String group : allGroupsSorted) {
          stringBuilder.append(",");
          stringBuilder.append(group);
        }
        writer.println(stringBuilder);
      }
      for (Map.Entry<String, Map<String, Long>> entry : totalsByFingerprint.entrySet()) {
        String fingerprint = entry.getKey();
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(fingerprint);
        Map<String, Long> totalsByGroup = entry.getValue();
        Map<String, Long> countsByGroup = countsByFingerprint.get(fingerprint);
        for (String group : allGroupsSorted) {
          stringBuilder.append(",");
          Number v0 = totalsByGroup.get(group);
          if (v0 != null) {
            Number v = countsByGroup.get(group);
            if (v != null && v.longValue() > 0) {
              stringBuilder.append(v0.doubleValue() / (v.longValue() * 1000000));
            }
          }
        }
        writer.println(stringBuilder);
      }
    }
  }

  private static void register(String name, Map<String, Object> group, Map<String, Long> totals,
      Map<String, Long> counts, Set<String> allGroups) {
    Map<String, Object> meta = (Map<String, Object>) group.get("_meta");
    allGroups.add(name);
    if (!totals.containsKey(name)) {
      totals.put(name, 0L);
    }
    totals.put(name, totals.get(name) + ((Number) meta.get("duration")).longValue());

    if (!counts.containsKey(name)) {
      counts.put(name, 0L);
    }
    counts.put(name, counts.get(name) + 1);
  }
}
