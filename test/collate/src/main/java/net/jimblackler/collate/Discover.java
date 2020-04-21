package net.jimblackler.collate;

import com.google.common.collect.ImmutableSet;
import com.google.common.collect.Sets;
import org.json.JSONArray;
import org.json.JSONObject;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Discover {

  public static void main(String[] args) throws IOException {
    JSONObject recordResults = new JSONObject();
    ImmutableSet<String> increasing =
        ImmutableSet.of("VmSize", "oom_score", "nativeAllocated", "summary.graphics",
            "summary.total-pss");
    ImmutableSet<String> decreasing = ImmutableSet.of("availMem", "Cached", "MemAvailable");

    Collector.cloudCollect(result -> {
      if (result.isEmpty()) {
        return;
      }
      JSONObject first = result.getJSONObject(0);

      JSONObject params = first.getJSONObject("params");
      JSONArray coordinates = params.getJSONArray("coordinates");

      for (int coordinateNumber = 0; coordinateNumber != coordinates.length(); coordinateNumber++) {
        if (coordinates.getInt(coordinateNumber) != 0) {
          return;
        }
      }

      String fingerprint = first.getJSONObject("build").getString("FINGERPRINT");
      JSONObject recordResults0;
      if (recordResults.has(fingerprint)) {
        recordResults0 = recordResults.getJSONObject(fingerprint);
      } else {
        recordResults0 = new JSONObject();
        recordResults.put(fingerprint, recordResults0);
      }
      for (int idx2 = 0; idx2 != result.length(); idx2++) {
        JSONObject row = result.getJSONObject(idx2);
        if (row.has("allocFailed") || row.has("mmapAnonFailed") || row.has("mmapFileFailed")) {
          break;
        }
        for (String key : Sets.union(increasing, decreasing)) {
          if (!row.has(key)) {
            continue;
          }
          long value = row.getLong(key);

          if (recordResults0.has(key)) {
            long existing = recordResults0.getLong(key);
            if (increasing.contains(key) ? value > existing : value < existing) {
              recordResults0.put(key, value);
            }
          } else {
            recordResults0.put(key, value);
          }
        }
      }
    });

    Files.write(Paths.get("out.json"), recordResults.toString(2).getBytes());
  }
}
