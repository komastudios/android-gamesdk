package net.jimblackler.collate;

import org.json.JSONObject;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;

public class Discover {

  public static void main(String[] args) throws IOException {
    Map<String, Long> lowestResults = new HashMap<>();
    Collector.cloudCollect(result -> {
      if (result.isEmpty()) {
        return;
      }
      JSONObject first = result.getJSONObject(0);

      JSONObject params = first.getJSONObject("params");
      JSONObject params1 = Utils.flattenParams(params);

      if (!params1.toString().equals("{\"malloc\":30}")) {
        return;
      }

      String fingerprint = first.getJSONObject("build").getString("FINGERPRINT");

      long maxAllocated = 0;
      for (int idx2 = 0; idx2 != result.length(); idx2++) {
        JSONObject row = result.getJSONObject(idx2);
        if (row.has("allocFailed")) {
          break;
        }
        if (!row.has("nativeAllocated")) {
          continue;
        }
        long nativeAllocated = row.getLong("nativeAllocated");
        if (nativeAllocated > maxAllocated) {
          maxAllocated = nativeAllocated;
        }
      }

      if (lowestResults.containsKey(fingerprint)) {
        if (maxAllocated < lowestResults.get(fingerprint)) {
          lowestResults.put(fingerprint, maxAllocated);
        }
      } else {
        lowestResults.put(fingerprint, maxAllocated);
      }

    });
    JSONObject table = new JSONObject();
    for (Map.Entry<String, Long> entry : lowestResults.entrySet()) {
      JSONObject settings = new JSONObject();
      settings.put("maxAllocated", entry.getValue());
      table.put(entry.getKey(), settings);
    }
    Files.write(Paths.get("out.json"), table.toString(2).getBytes());
  }
}
