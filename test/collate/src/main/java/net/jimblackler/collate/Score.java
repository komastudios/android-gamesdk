package net.jimblackler.collate;

import com.google.api.client.util.Lists;
import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Set;
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
            URI uri = writeReport(out, builds, directory.get(), tests.get());
            System.out.println(uri);
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
      JSONObject deviceInfo = first.getJSONObject("deviceInfo");
      JSONObject runParameters = deviceInfo.getJSONObject("params");
      JSONArray coordinates = params.getJSONArray("coordinates");
      tests.set(params.getJSONArray("tests"));

      assert deviceInfo.has("build");
      JSONObject build = deviceInfo.getJSONObject("build");
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
        if (ReportUtils.rowMetrics(jsonObject) != null) {
          results.add(jsonObject);
        }
      }
      results.sort(Comparator.comparingLong(ReportUtils::rowTime));
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
          Utils.copy(directory.get(), "static/report.js");
          Utils.copy(directory.get(), "report.css");
        } catch (IOException e) {
          throw new IllegalStateException(e);
        }
      }
      URI uri = Main.writeGraphs(directory.get(), results1);
      System.out.println(uri);
      results0.put(coordinates.toString(),
          new Result(score, uri, exited && !allocFailed, serviceCrashed, group.toString()));
    };

    if (useDevice) {
      Collector.deviceCollect("net.jimblackler.istresser", collect);
    } else {
      Collector.cloudCollect(null, collect);
    }

    if (timer.get() != null) {
      timer.get().cancel();
    }
    URI uri = writeReport(out, builds, directory.get(), tests.get());
    System.out.println(uri);
  }

  private static URI writeReport(Map<String, Map<String, Result>> rows,
      Map<String, JSONObject> builds, Path directory, JSONArray tests) throws IOException {
    StringBuilder body = new StringBuilder();
    // The vertical orders are the variations of the graphs. The vertical order refers to how the
    // dimensions will be ordered in the header, which is arranged like a tree. The first group is
    // at the top of the tree. The final group will appear as multiple leaves at the base of the
    // tree.
    List<List<Integer>> verticalOrders = getPermutations(tests.length());
    for (List<Integer> verticalOrder : verticalOrders) {
      if (!worthRendering(verticalOrder, tests)) {
        continue;
      }
      writeTable(body, rows, builds, tests, directory, verticalOrder);
      body.append("</br>");
    }

    String content = Utils.fileToString("score.html");
    content = content.replace("<!--body-->", body);

    Path outputFile = directory.resolve("index.html");
    Files.writeString(outputFile, content);
    return outputFile.toUri();
  }

  /**
   * Determines if a vertical group order will have an effective duplicate or not.
   * In short, there is no point repositioning the dimensions of length 1, since
   * that will only result in duplicates of the actual groupings.
   * @param verticalOrder The vertical group order under consideration.
   * @param objects The array of dimension arrays.
   * @return true if the permutation is the definitive version.
   */
  private static boolean worthRendering(Iterable<Integer> verticalOrder, JSONArray objects) {
    int original = 0;
    for (int idx : verticalOrder) {
      if (idx != original) {
        int n = objects.getJSONArray(idx).length();
        if (n <= 1) {
          return false;
        }
      }
      original++;
    }
    return true;
  }

  /**
   * Get a list of all possible orderings of a given permutation.
   * For example: '2' would return 1,2; 2,1
   * @param size
   * @return The list of possible orderings.
   */
  private static List<List<Integer>> getPermutations(int size) {
    List<List<Integer>> out = new ArrayList<>();
    Set<Integer> used = new HashSet<>();
    List<Integer> base = new ArrayList<>();
    getPermutations(base, used, size, out);
    return out;
  }

  private static void getPermutations(
      List<Integer> base, Set<Integer> used, int size, List<List<Integer>> out) {
    if (used.size() == size) {
      out.add(new ArrayList<>(base));
      return;
    }
    for (int i = 0; i != size; i++) {
      if (used.contains(i)) {
        continue;
      }
      used.add(i);
      base.add(i);

      getPermutations(base, used, size, out);

      base.remove(base.size() - 1);
      used.remove(i);
    }
  }

  private static void writeTable(StringBuilder body, Map<String, Map<String, Result>> rows,
      Map<String, JSONObject> builds, JSONArray tests, Path directory,
      List<Integer> verticalOrder) {
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
    for (int idx : verticalOrder) {
      JSONArray test = tests.getJSONArray(idx);
      colspan /= test.length();
      body.append("<tr>");

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

    List<JSONArray> horizontalOrder = getHorizontalOrder(tests, verticalOrder);
    for (Map.Entry<String, Map<String, Result>> row : rows.entrySet()) {
      body.append("<tr>");

      String id = row.getKey();
      Map<String, Result> rows0 = row.getValue();
      JSONObject build = builds.get(id);

      JSONObject version = build.getJSONObject("version");
      JSONObject fields = build.getJSONObject("fields");
      body.append("<td>")
          .append(fields.getString("MANUFACTURER"))
          .append("</td>")
          .append("<td>")
          .append(fields.getString("MODEL"))
          .append("</td>")
          .append("<td>")
          .append(fields.getString("DEVICE"))
          .append("</td>")
          .append("<td>")
          .append(version.getInt("SDK_INT"))
          .append("</td>")
          .append("<td>")
          .append(version.getString("RELEASE"))
          .append("</td>");

      Map<String, Float> maxScore = new HashMap<>();

      for (int idx = 0; idx != horizontalOrder.size(); idx++) {
        JSONArray coords = horizontalOrder.get(idx);
        Result result = rows0.get(coords.toString());
        if (result != null && result.isAcceptable()) {
          String group = result.getGroup();
          if (maxScore.containsKey(group)) {
            maxScore.put(group, Math.max(result.getScore(), maxScore.get(group)));
          } else {
            maxScore.put(group, result.getScore());
          }
        }
      }

      for (int idx = 0; idx != horizontalOrder.size(); idx++) {
        JSONArray coords = horizontalOrder.get(idx);
        Result result = rows0.get(coords.toString());
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

  /**
   * Use the vertical order to determine the dimension selections for every test in the table,
   * arranged from left to right.
   * @param tests The test dimensions to reorder.
   * @param verticalOrder The new order to use.
   * @return The selections for the tests in an array, ordered left to right.
   */
  private static List<JSONArray> getHorizontalOrder(JSONArray tests, List<Integer> verticalOrder) {
    List<JSONArray> horizontalOrder = new ArrayList<>();
    JSONArray base = new JSONArray();
    for (int idx = 0; idx < tests.length(); idx++) {
      base.put(0);
    }

    while (true) {
      horizontalOrder.add(new JSONArray(base.toString()));
      boolean rolledOver = true;
      ListIterator<Integer> it = verticalOrder.listIterator(verticalOrder.size());
      while (it.hasPrevious()) {
        int idx = it.previous();
        JSONArray test = tests.getJSONArray(idx);
        int value = base.getInt(idx);
        if (value == test.length() - 1) {
          base.put(idx, 0);
        } else {
          base.put(idx, value + 1);
          rolledOver = false;
          break;
        }
      }
      if (rolledOver) {
        break;
      }
    }
    return horizontalOrder;
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
