package net.jimblackler.collate;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import java.io.IOException;
import java.io.PrintWriter;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Map;

/**
 * Tool to create a table of maximum values from tests.
 */
public class WaterlineTable {
  public static void main(String[] args) throws IOException {
    Gson gson = new GsonBuilder().setPrettyPrinting().create();
    Map<String, Object> out = gson.fromJson(
        FileUtils.readFile(FileSystems.getDefault().getPath("lookup.json")), Map.class);
    List<String> baselineMetrics = new ArrayList<>();

    for (Object o : out.values()) {
      Map<String, Object> limits = (Map<String, Object>) o;
      Map<String, Object> baseline = (Map<String, Object>) limits.get("baseline");
      load(baselineMetrics, baseline);

      Map<String, Object> constant = (Map<String, Object>) baseline.get("constant");
      if (constant != null) {
        load(baselineMetrics, constant);
      }
    }

    Collections.sort(baselineMetrics);

    List<String> limitMetrics = new ArrayList<>();

    for (Object o : out.values()) {
      Map<String, Object> limits = (Map<String, Object>) o;
      Map<String, Object> limit = (Map<String, Object>) limits.get("limit");
      load(limitMetrics, limit);
    }

    Collections.sort(limitMetrics);

    try (
        PrintWriter writer = new PrintWriter(Files.newBufferedWriter(Paths.get("waterline.csv")))) {
      writer.print("Device");

      for (String metric : baselineMetrics) {
        writer.print(",");
        writer.print(metric + " baseline");
      }

      for (String metric : limitMetrics) {
        writer.print(",");
        writer.print(metric + " limit");
      }
      writer.println();

      {
        for (Map.Entry<String, Object> entry : out.entrySet()) {
          Map<String, Object> limits = (Map<String, Object>) entry.getValue();
          Map<String, Object> limit = (Map<String, Object>) limits.get("limit");
          Map<String, Object> baseline = (Map<String, Object>) limits.get("baseline");
          Map<String, Object> constant = (Map<String, Object>) baseline.get("constant");

          writer.print(entry.getKey());

          for (String metric : baselineMetrics) {
            writer.print(",");
            Long value = find(baseline, metric);
            if (value == null && constant != null) {
              value = find(constant, metric);
            }
            if (value != null) {
              writer.print(value);
            }
          }

          for (String metric : limitMetrics) {
            writer.print(",");
            Long value = find(limit, metric);
            if (value != null) {
              writer.print(value);
            }
          }

          writer.println();
        }
      }
    }
  }

  private static Long find(Map<String, Object> metrics, String metric) {
    for (Object o : metrics.values()) {
      if (!(o instanceof Map)) {
        continue;
      }
      Map<String, Object> group = (Map<String, Object>) o;
      Number number = (Number) group.get(metric);
      if (number != null) {
        return number.longValue();
      }
    }
    return null;
  }

  private static void load(Collection<String> allMetrics, Map<String, Object> metrics) {
    for (Object o : metrics.values()) {
      if (!(o instanceof Map)) {
        continue;
      }
      Map<String, Object> group = (Map<String, Object>) o;
      for (Map.Entry<String, Object> entry : group.entrySet()) {
        String metric = entry.getKey();
        if (allMetrics.contains(metric)) {
          continue;
        }
        Object object = entry.getValue();
        if (object instanceof Number) {
          allMetrics.add(metric);
        }
      }
    }
  }
}
