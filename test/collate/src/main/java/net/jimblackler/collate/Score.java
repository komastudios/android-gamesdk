package net.jimblackler.collate;

import com.google.api.client.util.Lists;
import com.google.common.collect.ImmutableList;
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
  public static final float MEMORY_FREE_THRESHOLD = 0.9f;

  private static final List<List<String>> baseGroups =
      ImmutableList.<List<String>>builder()
          .add(ImmutableList.of(""))
          .add(ImmutableList.of("trim"))
          .add(ImmutableList.of("oom"))
          .add(ImmutableList.of("low"))
          .add(ImmutableList.of("try"))
          .add(ImmutableList.of("cl"))
          .add(ImmutableList.of("avail"))
          .add(ImmutableList.of("cached"))
          .add(ImmutableList.of("avail2"))
          .add(ImmutableList.of("cl", "avail", "avail2"))
          .add(ImmutableList.of("cl", "avail", "avail2", "low"))
          .add(ImmutableList.of("trim", "try", "cl", "avail", "avail2"))
          .add(ImmutableList.of("trim", "low", "try", "cl", "avail", "avail2"))
          .build();

  private static final List<List<String>> groups =
      ImmutableList.<List<String>>builder().addAll(baseGroups).addAll(baseGroups).build();

  public static void main(String[] args) throws IOException, InterruptedException {
    Map<String, List<Result>> out = new HashMap<>();
    Map<String, JSONObject> builds = new HashMap<>();
    Path directory = Files.createTempDirectory("report-");
    Consumer<JSONArray> collect =
        result -> {
          if (result.isEmpty()) {
            return;
          }
          JSONObject first = result.getJSONObject(0);
          if (!first.has("scenario")) {
            System.out.println("No scenario");
            return;
          }

          int scenario = first.getInt("scenario");
          if (scenario > groups.size()) {
            return;
          }
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

          long previousNativeAllocated = -1;
          long previousNativeAllocatedTime = -1;
          long lowestTop = Long.MAX_VALUE;
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
            long nativeAllocated = row.getLong("nativeAllocated");
            long time = row.getLong("time");
            assert time >= previousNativeAllocatedTime;
            if (previousNativeAllocated != -1) {
              if (nativeAllocated < previousNativeAllocated * MEMORY_FREE_THRESHOLD) {
                long top = previousNativeAllocated;
                if (top < lowestTop) {
                  lowestTop = top;
                }
              }
            }
            previousNativeAllocated = nativeAllocated;
            previousNativeAllocatedTime = time;
          }
          if (lowestTop == Long.MAX_VALUE) {
            lowestTop = previousNativeAllocated;
          }
          float score = (float) lowestTop / (1024 * 1024);
          List<Result> results0;
          if (out.containsKey(id)) {
            results0 = out.get(id);
          } else {
            results0 = new ArrayList<>();
            while (results0.size() < groups.size()) {
              results0.add(null);
            }
            out.put(id, results0);
          }
          JSONArray results1 = new JSONArray();
          results1.put(result);
          assert results0.get(scenario - 1) == null;
          results0.set(
              scenario - 1,
              new Result(
                  score,
                  Main.writeGraphs(directory, results1),
                  exited && !allocFailed,
                  serviceCrashed));
        };

    if (USE_DEVICE) {
      for (int scenario = 1; scenario <= groups.size(); scenario++) {
        Collector.deviceCollect(collect, scenario);
      }
    } else {
      Collector.cloudCollect(collect);
    }

    StringBuilder body = new StringBuilder();
    body.append("<table>")
        .append("<thead>")
        .append("<th>")
        .append("Device")
        .append("</th>")
        .append("<th>")
        .append("Android")
        .append("</th>");
    for (int scenario = 1; scenario != groups.size() + 1; scenario++) {
      body.append("<th>").append(String.join("<br/>", groups.get(scenario - 1))).append("</th>");
    }
    body.append("</thead>");

    for (Map.Entry<String, List<Result>> row : out.entrySet()) {
      body.append("<tr>");

      String id = row.getKey();
      JSONObject build = builds.get(id);

      body.append("<td>")
          .append(capitalizeFirstLetter(build.getString("MANUFACTURER")))
          .append(" ")
          .append(build.getString("MODEL"))
          .append("</td>")
          .append("<td>")
          .append(build.getString("RELEASE"))
          .append("</td>");

      float maxScore = Float.MIN_VALUE;
      for (Result result : row.getValue()) {
        if (result != null && result.isAcceptable()) {
          maxScore = Math.max(result.getScore(), maxScore);
        }
      }

      for (Result result : row.getValue()) {
        if (result == null) {
          body.append("<td/>");
          continue;
        }
        Collection<String> classes = new ArrayList<>();
        if (result.getScore() == maxScore && result.isAcceptable()) {
          classes.add("best");
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
        body.append(String.format("<a href='%s'>%.0f</a>", uri, result.getScore())).append("</td>");
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

    Result(float score, URI uri, boolean acceptable, boolean serviceCrashed) {
      this.score = score;
      this.uri = uri;
      this.acceptable = acceptable;
      this.serviceCrashed = serviceCrashed;
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
  }
}
