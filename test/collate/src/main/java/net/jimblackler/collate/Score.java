package net.jimblackler.collate;

import com.google.api.client.util.Lists;
import java.awt.Desktop;
import java.io.File;
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
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Consumer;
import java.util.stream.Stream;
import org.json.JSONArray;
import org.json.JSONObject;

public class Score {
  private static final boolean USE_DEVICE = false;

  public static void main(String[] args) throws IOException {
    go(USE_DEVICE);
  }

  static void go(boolean useDevice) throws IOException {
    Map<String, Map<String, Result>> out = new HashMap<>();
    Map<String, JSONObject> builds = new HashMap<>();
    AtomicReference<Path> directory = new AtomicReference<>();
    AtomicReference<JSONArray> tests = new AtomicReference<>();
    AtomicReference<String> historyId = new AtomicReference<>();
    AtomicReference<Timer> timer = new AtomicReference<>();
    Consumer<JSONArray> collect = result -> {
      if (timer.get() != null) {
        timer.get().cancel();
      }
      timer.set(new Timer());
      TimerTask task = new TimerTask() {
        @Override
        public void run() {
          JSONArray _tests = tests.get();
          if (_tests == null || _tests.isEmpty()) {
            return;
          }
          try {
            writeReport(out, builds, directory.get(), tests.get());
          } catch (IOException e) {
            throw new IllegalStateException(e);
          }
        }
      };
      timer.get().schedule(task, 1000 * 10);

      JSONObject first = result.getJSONObject(0);
      if (!first.has("params")) {
        System.out.println("No usable results. Data returned was:");
        System.out.println(first.toString(2));
        return;
      }
      JSONObject params = first.getJSONObject("params");

      JSONObject runParameters = Utils.flattenParams(params);
      JSONArray coordinates = params.getJSONArray("coordinates");
      tests.set(params.getJSONArray("tests"));

      assert first.has("build");
      JSONObject build = first.getJSONObject("build");
      String id = build.toString();
      if (first.has("extra")) {
        JSONObject extra = first.getJSONObject("extra");
        historyId.set(extra.getString("historyId"));
        if (extra.has("step")) {
          JSONObject step = extra.getJSONObject("step");
          JSONArray dimensions = step.getJSONArray("dimensionValue");
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
        if (row.has("allocFailed") || row.has("mmapAnonFailed") || row.has("mmapFileFailed")
            || row.has("criticalLogLines") || row.has("failedToClear")) {
          allocFailed = true;
        }
        if (row.has("serviceCrashed")) {
          serviceCrashed = true;
        }
        long score = 0;
        if (row.has("testMetrics")) {
          JSONObject testMetrics = row.getJSONObject("testMetrics");
          for (String key : testMetrics.keySet()) {
            score += testMetrics.getLong(key);
          }
        }

        if (score > largest) {
          largest = score;
        }

        if (row.has("trigger") && !row.optBoolean("paused", false)) {
          long top = score;
          if (top < lowestTop) {
            lowestTop = top;
          }
        }
      }

      float score =
          (lowestTop == Long.MAX_VALUE ? (float) largest : (float) lowestTop) / (1024 * 1024);
      Map<String, Result> results0;
      if (out.containsKey(id)) {
        results0 = out.get(id);
      } else {
        results0 = new HashMap<>();
        out.put(id, results0);
      }
      JSONArray results1 = new JSONArray();
      results1.put(result);
      JSONObject group = new JSONObject(runParameters.toString());
      group.remove("heuristics");
      if (directory.get() == null) {
        String dirName = historyId.get();
        if (dirName == null) {
          dirName = params.getString("run");
        }
        directory.set(Path.of("reports").resolve(dirName));
        try {
          if (directory.get().toFile().exists()) {
            // Empty the directory if it already exists.
            try (Stream<Path> files = Files.walk(directory.get())) {
              files.sorted(Comparator.reverseOrder()).map(Path::toFile).forEach(File::delete);
            }
          }
          Files.createDirectory(directory.get());
        } catch (IOException e) {
          throw new IllegalStateException(e);
        }
      }
      results0.put(coordinates.toString(),
          new Result(score, Main.writeGraphs(directory.get(), results1), exited && !allocFailed,
              serviceCrashed, group.toString()));
    };

    if (useDevice) {
      Collector.deviceCollect("net.jimblackler.istresser", collect);
    } else {
      Collector.cloudCollect(null, collect);
    }

    if (timer.get() != null) {
      timer.get().cancel();
    }
    Desktop.getDesktop().browse(writeReport(out, builds, directory.get(), tests.get()));
  }

  private static URI writeReport(Map<String, Map<String, Result>> rows,
      Map<String, JSONObject> builds, Path directory, JSONArray tests) throws IOException {
    StringBuilder body = new StringBuilder();
    writeTable(body, rows, builds, tests, directory);

    Utils.copy(directory, "report.css");
    Utils.copy(directory, "sorter.js");
    String content = Utils.fileToString("score.html");
    content = content.replace("<!--body-->", body);

    Path outputFile = directory.resolve("index.html");
    Files.writeString(outputFile, content);
    return outputFile.toUri();
  }

  private static void writeTable(StringBuilder body, Map<String, Map<String, Result>> rows,
      Map<String, JSONObject> builds, JSONArray tests, Path directory) {
    int rowspan = tests.length() + 1;

    int colspan = getTotalVariations(tests);
    body.append("<table>")
        .append("<thead>")
        .append("<tr>")
        .append("<th rowspan=" + rowspan + " >")
        .append("Manufacturer")
        .append("</th>")
        .append("<th rowspan=" + rowspan + " >")
        .append("Model")
        .append("</th>")
        .append("<th rowspan=" + rowspan + " >")
        .append("Device")
        .append("</th>")
        .append("<th rowspan=" + rowspan + " >")
        .append("SDK")
        .append("</th>")
        .append("<th rowspan=" + rowspan + " >")
        .append("Release")
        .append("</th>")
        .append("</tr>");

    int repeats = 1;
    for (int idx = 0; idx < tests.length(); idx++) {
      JSONArray test = tests.getJSONArray(idx);
      colspan /= test.length();
      body.append("<tr>");

      if (false) {
        body.append("<th>")
            .append("</th>")
            .append("<th>")
            .append("</th>")
            .append("<th>")
            .append("</th>")
            .append("<th>")
            .append("</th>");
      }
      for (int repeat = 0; repeat != repeats; repeat++) {
        for (int param = 0; param < test.length(); param++) {
          JSONObject _param = test.getJSONObject(param);
          body.append("<th colspan=" + colspan + ">").append(toString(_param)).append("</th>");
        }
      }
      body.append("</tr>");
      repeats *= test.length();
    }
    body.append("</thead>");

    for (Map.Entry<String, Map<String, Result>> row : rows.entrySet()) {
      body.append("<tr>");

      String id = row.getKey();
      JSONObject build = builds.get(id);

      final JSONObject version = build.getJSONObject("version");
      body.append("<td>")
          .append(build.getString("MANUFACTURER"))
          .append("</td>")
          .append("<td>")
          .append(build.getString("MODEL"))
          .append("</td>")
          .append("<td>")
          .append(build.getString("DEVICE"))
          .append("</td>")
          .append("<td>")
          .append(version.getInt("SDK_INT"))
          .append("</td>")
          .append("<td>")
          .append(version.getString("RELEASE"))
          .append("</td>");

      Map<String, Float> maxScore = new HashMap<>();

      for (Result result : row.getValue().values()) {
        if (result != null && result.isAcceptable()) {
          String group = result.getGroup();
          if (maxScore.containsKey(group)) {
            maxScore.put(group, Math.max(result.getScore(), maxScore.get(group)));
          } else {
            maxScore.put(group, result.getScore());
          }
        }
      }

      for (Result result : row.getValue().values()) {
        if (result == null) {
          body.append("<td/>");
          continue;
        }
        Collection<String> classes = new ArrayList<>();
        String group = result.getGroup();
        if (result.isAcceptable()) {
          float max = maxScore.get(group);
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
  }

  private static int getTotalVariations(JSONArray tests) {
    int total = 1;
    for (int idx = 0; idx != tests.length(); idx++) {
      JSONArray test = tests.getJSONArray(idx);
      total *= test.length();
    }
    return total;
  }

  private static String toString(Object obj) {
    StringBuilder builder = new StringBuilder();
    if (obj instanceof JSONObject) {
      JSONObject obj2 = (JSONObject) obj;
      String divider = "";
      for (String key : obj2.keySet()) {
        builder.append(divider)
            .append("<span class='key'>")
            .append(key)
            .append("</span>")
            .append("<span class='value'>")
            .append(toString(obj2.get(key)))
            .append("</span>");
        divider = " ";
      }
    } else if (obj instanceof JSONArray) {
      JSONArray array = (JSONArray) obj;
      for (int idx = 0; idx < array.length(); idx++) {
        if (idx > 0) {
          builder.append(", ");
        }
        builder.append(toString(array.get(idx)));
      }
    } else {
      builder.append(obj);
    }
    return builder.toString();
  }

  private static class Result {
    private final float score;
    private final URI uri;
    private final boolean acceptable;
    private final boolean serviceCrashed;
    private final String group;

    Result(float score, URI uri, boolean acceptable, boolean serviceCrashed, String group) {
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

    String getGroup() {
      return group;
    }
  }
}
