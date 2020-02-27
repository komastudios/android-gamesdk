package net.jimblackler.collate;

import com.google.api.client.util.Lists;
import java.awt.Desktop;
import java.io.IOException;
import java.net.URI;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.function.Consumer;
import org.json.JSONArray;
import org.json.JSONObject;

public class Score {
  private static final boolean USE_DEVICE = false;
  private static final int DEVICE_SCENARIOS = 1;

  public static void main(String[] args) throws IOException, InterruptedException {
    Map<String, List<Result>> out = new HashMap<>();
    Map<String, JSONObject> builds = new HashMap<>();
    Path directory = Files.createTempDirectory("report-");
    final int[] variations = {0};
    Map<Integer, JSONObject> paramsMap = new HashMap<>();
    Consumer<JSONArray> collect =
        result -> {
          if (result.isEmpty()) {
            return;
          }
          JSONObject first = result.getJSONObject(0);
          JSONObject params = first.getJSONObject("params");

          JSONObject runParameters = Utils.flattenParams(params);
          JSONArray coordinates = params.getJSONArray("coordinates");
          JSONArray tests = params.getJSONArray("tests");
          int total = 1;
          int variationNumber = 0;
          for (int coordinateNumber = coordinates.length() - 1;
              coordinateNumber >= 0;
              coordinateNumber--) {
            int bound = tests.getJSONArray(coordinateNumber).length();
            int value = coordinates.getInt(coordinateNumber);
            variationNumber += value * total;
            total *= bound;
          }
          if (variations[0] == 0) {
            variations[0] = total;
          } else {
            assert variations[0] == total;
          }
          paramsMap.put(variationNumber, runParameters);

          assert first.has("build");
          JSONObject build = first.getJSONObject("build");
          String id = build.toString();
          if (first.has("extra")) {
            JSONObject extra = first.getJSONObject("extra");
            if (extra.has("dimensions")) {
              JSONObject dimensions = extra.getJSONObject("dimensions");
              id = dimensions.toString();
            }
          }
          builds.put(id, build);

          long lowestTop = Long.MAX_VALUE;
          long largest = 0;
          boolean exited = false;
          boolean allocFailed = false;
          boolean serviceCrashed = false;

          List<JSONObject> results = Lists.newArrayList();
          for (int idx2 = 0; idx2 != result.length(); idx2++) {
            JSONObject jsonObject = result.getJSONObject(idx2);
            if (jsonObject.has("time")) {
              results.add(jsonObject);
            }
          }
          results.sort(Comparator.comparingLong(o -> o.getLong("time")));
          for (JSONObject row : results) {
            if (row.has("exiting")) {
              exited = true;
            }
            if (row.has("allocFailed")) {
              allocFailed = true;
            }
            if (row.has("serviceCrashed")) {
              serviceCrashed = true;
            }
            if (!row.has("nativeAllocated")) {
              continue;
            }
            long score;
            if (runParameters.has("glTest")) {
              score = row.getLong("gl_allocated");
            } else {
              score = row.getLong("nativeAllocated");
            }

            if (score > largest) {
              largest = score;
            }

            if (row.has("trigger")) {
              long top = score;
              if (top < lowestTop) {
                lowestTop = top;
              }
            }
          }

          float score = (lowestTop == Long.MAX_VALUE ? (float) largest : (float) lowestTop)
              / (1024 * 1024);
          List<Result> results0;
          if (out.containsKey(id)) {
            results0 = out.get(id);
          } else {
            results0 = new ArrayList<>();
            while (results0.size() < variations[0]) {
              results0.add(null);
            }
            out.put(id, results0);
          }
          JSONArray results1 = new JSONArray();
          results1.put(result);
          while (results0.size() <= variations[0] - 1) {
            results0.add(null);
          }
          assert results0.get(variationNumber) == null;
          results0.set(
              variationNumber,
              new Result(
                  score,
                  Main.writeGraphs(directory, results1),
                  exited && !allocFailed,
                  serviceCrashed,
                  first.has("service") ? 1 : 0));
        };

    if (USE_DEVICE) {
      for (int scenario = 1; scenario <= DEVICE_SCENARIOS; scenario++) {
        Collector.deviceCollect(collect, scenario);
      }
    } else {
      Collector.cloudCollect(collect);
    }

    StringBuilder body = new StringBuilder();
    body.append("<table>")
        .append("<thead>")
        .append("<th>")
        .append("Manufacturer")
        .append("</th>")
        .append("<th>")
        .append("Model")
        .append("</th>")
        .append("<th>")
        .append("SDK")
        .append("</th>")
        .append("<th>")
        .append("Release")
        .append("</th>");
    for (int variation = 0; variation < variations[0]; variation++) {
      if (paramsMap.containsKey(variation)) {
        JSONObject params = Utils.flattenParams(paramsMap.get(variation));
        String paramString =
            params.toString().replace("{", "").replace("}", "")
                .replace("\"", "").replace(":true", "").replace(":", " ").replace("heuristics", "")
                .replace(",", " ");
        body.append("<th>").append(paramString).append("</th>");
      } else {
        body.append("<th>").append("</th>");
      }
    }
    body.append("</thead>");

    for (Map.Entry<String, List<Result>> row : out.entrySet()) {
      body.append("<tr>");

      String id = row.getKey();
      JSONObject build = builds.get(id);

      body.append("<td>")
          .append(build.getString("MANUFACTURER"))
          .append("</td>")
          .append("<td>")
          .append(build.getString("MODEL"))
          .append("</td>")
          .append("<td>")
          .append(build.getInt("SDK_INT"))
          .append("</td>")
          .append("<td>")
          .append(build.getString("RELEASE"))
          .append("</td>");

      float[] maxScore = {Float.MIN_VALUE, Float.MIN_VALUE};

      for (Result result : row.getValue()) {
        if (result != null && result.isAcceptable()) {
          int group = result.getGroup();
          maxScore[group] = Math.max(result.getScore(), maxScore[group]);
        }
      }

      for (Result result : row.getValue()) {
        if (result == null) {
          body.append("<td/>");
          continue;
        }
        Collection<String> classes = new ArrayList<>();
        int group = result.getGroup();
        if (result.isAcceptable()) {
          float max = maxScore[group];
          if (result.getScore() == max) {
            classes.add("best");
          } else if (result.getScore() > max * 0.90) {
            classes.add("good");
          }
        }
        if (!result.isAcceptable()) {
          classes.add("unacceptable");
        }
        if (result.isServiceCrashed()) {
          classes.add("serviceCrashed");
        }
        if (classes.isEmpty()) {
          body.append("<td>");
        } else {
          body.append(String.format("<td class='%s'>", String.join(" ", classes)));
        }

        URI uri = directory.toUri().relativize(result.getUri());
        body.append(String.format("<a href='%s'>%.1f</a>", uri, result.getScore())).append("</td>");
      }

      body.append("</tr>");
    }
    body.append("</table>");

    Utils.copy(directory, "report.css");
    Utils.copy(directory, "sorter.js");
    String content = Utils.fileToString("score.html");
    content = content.replace("<!--body-->", body);

    Path outputFile = directory.resolve("index.html");
    Files.writeString(outputFile, content);
    Desktop.getDesktop().browse(outputFile.toUri());
  }

  private static String capitalizeFirstLetter(String s) {
    return s.substring(0, 1).toUpperCase() + s.substring(1);
  }

  private static class Result {
    private final float score;
    private final URI uri;
    private final boolean acceptable;
    private final boolean serviceCrashed;
    private final int group;

    Result(float score, URI uri, boolean acceptable, boolean serviceCrashed, int group) {
      this.score = score;
      this.uri = uri;
      this.acceptable = acceptable;
      this.serviceCrashed = serviceCrashed;
      this.group = group;
    }

    float getScore() {
      return score;
    }

    URI getUri() {
      return uri;
    }

    boolean isAcceptable() {
      return acceptable;
    }

    boolean isServiceCrashed() {
      return serviceCrashed;
    }

    int getGroup() {
      return group;
    }
  }
}
