package net.jimblackler.collate;

import java.io.IOException;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import org.json.JSONObject;

public class WaterlineTable {
  public static void main(String[] args) throws IOException {
    JSONObject out = new JSONObject(Files.readString(Path.of("lookup.json")));
    List<String> baselineMetrics = new ArrayList<>();
    {
      Iterator<String> it = out.keys();
      while (it.hasNext()) {
        String key = it.next();
        JSONObject limits = out.getJSONObject(key);
        JSONObject baseline = limits.getJSONObject("baseline");
        load(baselineMetrics, baseline);
        JSONObject constant = baseline.getJSONObject("constant");
        load(baselineMetrics, constant);
      }
    }
    Collections.sort(baselineMetrics);

    List<String> limitMetrics = new ArrayList<>();
    {
      Iterator<String> it = out.keys();
      while (it.hasNext()) {
        String key = it.next();
        JSONObject limits = out.getJSONObject(key);
        JSONObject limit = limits.getJSONObject("limit");

        load(limitMetrics, limit);
      }
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
        Iterator<String> it = out.keys();
        while (it.hasNext()) {
          String key = it.next();
          JSONObject limits = out.getJSONObject(key);
          JSONObject limit = limits.getJSONObject("limit");
          JSONObject baseline = limits.getJSONObject("baseline");
          JSONObject constant = baseline.getJSONObject("constant");

          writer.print(key);

          for (String metric : baselineMetrics) {
            writer.print(",");
            if (baseline.has(metric)) {
              writer.print(baseline.getLong(metric));
            } else if (constant.has(metric)) {
              writer.print(constant.getLong(metric));
            }
          }

          for (String metric : limitMetrics) {
            writer.print(",");
            if (limit.has(metric)) {
              Object object = limit.get(metric);
              if (object instanceof Number) {
                writer.print(limit.getLong(metric));
              }
            }
          }

          writer.println();
        }
      }
    }
  }

  private static void load(List<String> allMetrics, JSONObject metrics) {
    Iterator<String> it = metrics.keys();
    while (it.hasNext()) {
      String metric = it.next();
      if (allMetrics.contains(metric)) {
        continue;
      }
      Object object = metrics.get(metric);
      if (object instanceof Number) {
        allMetrics.add(metric);
      }
    }
  }
}
