package net.jimblackler.collate;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.SortedMap;
import java.util.TreeMap;

/**
 * Creates training data for the machine learning tool.
 */
public class CreateTrainingData {
  public static void main(String[] args) throws IOException {
    Gson gson = new GsonBuilder().setPrettyPrinting().create();
    List<Object> dataOut = new ArrayList<>();
    Random random = new Random();

    File[] directories = new File("reports").listFiles(File::isDirectory);
    for (File directory : directories) {
      Collector.cloudCollect(directory.getName(), results -> {
        if (results.isEmpty()) {
          return;
        }
        Map<String, Object> first = (Map<String, Object>) results.get(0);

        if (!first.containsKey("params")) {
          System.out.println("No usable results. Data returned was:");
          System.out.println(gson.toJson(results));
          return;
        }
        Map<String, Object> deviceInfo = ReportUtils.getDeviceInfo(results);
        if (deviceInfo == null) {
          return;
        }
        Map<String, Object> flattened =
            Utils.flattenParams((Map<String, Object>) first.get("params"));
        Map<String, Object> advisorParameters =
            (Map<String, Object>) flattened.get("advisorParameters");
        if (advisorParameters != null) {
          Map<String, Object> heuristics =
              (Map<String, Object>) advisorParameters.get("heuristics");
          if (heuristics != null && !heuristics.isEmpty()) {
            // Runs with heuristics are not useful.
            return;
          }
        }

        if (!flattened.containsKey("malloc") && !flattened.containsKey("glTest")) {
          // Only interested in these kinds of stress tests.
          return;
        }

        Map<String, Object> dataOut2 = new LinkedHashMap<>();
        dataOut2.put("build", deviceInfo.get("build"));
        dataOut2.put("baseline", deviceInfo.get("baseline"));

        int seen = 0;
        Map<String, Object> sample = null;

        SortedMap<Long, Map<String, Object>> sorted = new TreeMap<>();
        for (Object o : results) {
          Map<String, Object> row = (Map<String, Object>) o;
          sorted.put(ReportUtils.rowTime(row), row);
        }

        Map<String, Object> baseline = null;
        Map<String, Object> limit = null;

        long firstTime = Long.MAX_VALUE;
        for (Map.Entry<Long, Map<String, Object>> pair : sorted.entrySet()) {
          long time = pair.getKey();
          Map<String, Object> row = pair.getValue();
          Map<String, Object> metrics = ReportUtils.rowMetrics(row);
          if (metrics == null) {
            continue;
          }

          // Limit is the latest reading.
          limit = row;

          if (Boolean.TRUE.equals(row.get("activityPaused"))) {
            // The run was interrupted and is unusable.
            return;
          }
          if (Boolean.TRUE.equals(row.get("allocFailed"))
              || Boolean.TRUE.equals(row.get("mmapAnonFailed"))
              || Boolean.TRUE.equals(row.get("mmapFileFailed"))
              || row.containsKey("criticalLogLines")) {
            break;
          }

          if (firstTime == Long.MAX_VALUE) {
            firstTime = time;
          }

          if (metrics.containsKey("constant") && baseline == null) {
            // Baseline is the earliest reading with 'constant'.
            baseline = row;
          }

          seen++;
          if (random.nextInt(seen) == 0) {
            sample = row;
          }
        }
        if (limit == null) {
          return;
        }
        if (sample == null) {
          return;
        }

        long finalTime = ReportUtils.rowTime(limit);
        dataOut2.put("limit", limit);
        dataOut2.put("firstTime", firstTime);
        dataOut2.put("finalTime", finalTime);
        dataOut2.put("sample", ReportUtils.rowMetrics(sample));
        if (sample.containsKey("testMetrics")) {
          dataOut2.put("sampleTestMetrics", sample.get("testMetrics"));
        }
        dataOut.add(dataOut2);
      });
    }

    Files.write(Paths.get("realtime.json"), gson.toJson(dataOut).getBytes(StandardCharsets.UTF_8));
  }
}
