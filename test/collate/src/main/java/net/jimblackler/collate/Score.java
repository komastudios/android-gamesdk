package net.jimblackler.collate;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectWriter;
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
import org.w3c.dom.Node;

public class Score {
  private static final boolean USE_DEVICE = false;

  public static void main(String[] args) throws IOException {
    go(USE_DEVICE);
  }

  static void go(boolean useDevice) throws IOException {
    ObjectWriter objectWriter = new ObjectMapper().writerWithDefaultPrettyPrinter();
    Map<String, Map<String, Result>> out = new HashMap<>();
    Map<String, Map<String, Object>> deviceInfos = new HashMap<>();
    AtomicReference<Path> directory = new AtomicReference<>();
    AtomicReference<List<Object>> tests = new AtomicReference<>();
    AtomicReference<String> historyId = new AtomicReference<>();
    AtomicReference<Timer> timer = new AtomicReference<>();
    Consumer<List<Object>> collect = result -> {
      if (timer.get() != null) {
        timer.get().cancel();
      }
      timer.set(new Timer());
      TimerTask task = new TimerTask() {
        @Override
        public void run() {
          List<Object> _tests = tests.get();
          if (_tests == null || _tests.isEmpty()) {
            return;
          }
          try {
            URI uri = writeReport(out, deviceInfos, directory.get(), tests.get());
            System.out.println(uri);
          } catch (IOException e) {
            throw new IllegalStateException(e);
          }
        }
      };
      timer.get().schedule(task, 1000 * 10);

      Map<String, Object> first = (Map<String, Object>) result.get(0);
      Map<String, Object> params = (Map<String, Object>) first.get("params");
      if (params == null) {
        System.out.println("No usable results. Data returned was:");
        try {
          System.out.println(objectWriter.writeValueAsString(first));
        } catch (JsonProcessingException e) {
          throw new IllegalStateException(e);
        }
        return;
      }
      Map<String, Object> deviceInfo = ReportUtils.getDeviceInfo(result);
      if (deviceInfo == null) {
        System.out.println("Could not find deviceInfo. Data returned was:");
        try {
          System.out.println(objectWriter.writeValueAsString(first));
        } catch (JsonProcessingException e) {
          throw new IllegalStateException(e);
        }
        return;
      }
      Map<String, Object> runParameters = (Map<String, Object>) deviceInfo.get("params");
      List<Object> coordinates = (List<Object>) params.get("coordinates");
      tests.set((List<Object>) params.get("tests"));

      assert deviceInfo.containsKey("build");
      Map<String, Object> build = (Map<String, Object>) deviceInfo.get("build");
      String id = build.toString();
      if (first.containsKey("extra")) {
        Map<String, Object> extra = (Map<String, Object>) first.get("extra");
        historyId.set((String) extra.get("historyId"));
        if (extra.containsKey("step")) {
          Map<String, Object> step = (Map<String, Object>) extra.get("step");
          List<Object> dimensions = (List<Object>) step.get("dimensionValue");
          id = dimensions.toString();
        }
      }
      deviceInfos.put(id, deviceInfo);

      long lowestTop = Long.MAX_VALUE;
      long largest = 0;
      boolean exited = false;
      boolean allocFailed = false;
      boolean serviceCrashed = false;

      List<Map<String, Object>> results = Lists.newArrayList();
      for (int idx2 = 0; idx2 != result.size(); idx2++) {
        Map<String, Object> map = (Map<String, Object>) result.get(idx2);
        if (ReportUtils.rowMetrics(map) != null) {
          results.add(map);
        }
      }
      results.sort(Comparator.comparingLong(ReportUtils::rowTime));
      for (Map<String, Object> row : results) {
        if (row.containsKey("exiting")) {
          exited = true;
        }
        if (row.containsKey("allocFailed") || row.containsKey("mmapAnonFailed")
            || row.containsKey("mmapFileFailed") || row.containsKey("criticalLogLines")
            || row.containsKey("failedToClear")) {
          allocFailed = true;
        }
        if (row.containsKey("serviceCrashed")) {
          serviceCrashed = true;
        }
        long score = 0;
        if (row.containsKey("testMetrics")) {
          Map<String, Object> testMetrics = (Map<String, Object>) row.get("testMetrics");
          for (Object o : testMetrics.values()) {
            score += ((Number) o).longValue();
          }
        }

        if (score > largest) {
          largest = score;
        }

        Map<String, Object> advice = (Map<String, Object>) row.get("advice");
        if (advice != null) {
          Iterable<Object> warnings = (Iterable<Object>) advice.get("warnings");
          if (warnings != null && !Boolean.TRUE.equals(row.get("paused"))) {
            for (Object o2 : warnings) {
              Map<String, Object> warning = (Map<String, Object>) o2;
              if (!"red".equals(warning.get("level"))) {
                continue;
              }
              long top = score;
              if (top < lowestTop) {
                lowestTop = top;
              }
              break;
            }
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
      List<Object> results1 = new ArrayList<>();
      results1.add(result);
      Map<String, Object> group = Utils.clone(runParameters);
      group.remove("advisorParameters");
      if (directory.get() == null) {
        String dirName = historyId.get();
        if (dirName == null) {
          dirName = (String) params.get("run");
        }
        directory.set(Path.of("reports").resolve(dirName));
        try {
          File outDir = directory.get().toFile();
          if (outDir.exists()) {
            // Empty the directory if it already exists.
            try (Stream<Path> files = Files.walk(directory.get())) {
              files.sorted(Comparator.reverseOrder()).map(Path::toFile).forEach(File::delete);
            }
          }
          outDir.mkdirs();
          Utils.copyFolder(Path.of("resources", "static"), directory.get().resolve("static"));
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
    URI uri = writeReport(out, deviceInfos, directory.get(), tests.get());
    System.out.println(uri);
  }

  private static URI writeReport(Map<String, Map<String, Result>> rows,
      Map<String, Map<String, Object>> deviceInfos, Path directory, List<Object> tests)
      throws IOException {
    StringBuilder body = new StringBuilder();
    // The vertical orders are the variations of the graphs. The vertical order refers to how the
    // dimensions will be ordered in the header, which is arranged like a tree. The first group is
    // at the top of the tree. The final group will appear as multiple leaves at the base of the
    // tree.
    List<List<Integer>> verticalOrders = getPermutations(tests.size());
    for (List<Integer> verticalOrder : verticalOrders) {
      if (!worthRendering(verticalOrder, tests)) {
        continue;
      }
      writeTable(body, rows, deviceInfos, tests, directory, verticalOrder);
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
   *
   * @param verticalOrder The vertical group order under consideration.
   * @param objects       The array of dimension arrays.
   * @return true if the permutation is the definitive version.
   */
  private static boolean worthRendering(Iterable<Integer> verticalOrder, List<Object> objects) {
    int original = 0;
    for (int idx : verticalOrder) {
      if (idx != original) {
        int n = ((Collection<Object>) objects.get(idx)).size();
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
   *
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
      Map<String, Map<String, Object>> deviceInfos, List<Object> tests, Path directory,
      List<Integer> verticalOrder) {
    int rowspan = tests.size() + 1;

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
      List<Object> test = (List<Object>) tests.get(idx);
      colspan /= test.size();
      body.append("<tr>");

      for (int repeat = 0; repeat != repeats; repeat++) {
        for (int param = 0; param < test.size(); param++) {
          Map<String, Object> _param = (Map<String, Object>) test.get(param);
          Node dataExplorer = DataExplorer.getDataExplorer(HtmlUtils.getDocument(), _param);
          body.append("<th colspan=" + colspan + " class='params'>")
              .append(HtmlUtils.toString(dataExplorer))
              .append("</th>");
        }
      }
      body.append("</tr>");
      repeats *= test.size();
    }
    body.append("</thead>");

    List<List<Object>> horizontalOrder = getHorizontalOrder(tests, verticalOrder);
    for (Map.Entry<String, Map<String, Result>> row : rows.entrySet()) {
      body.append("<tr>");

      String id = row.getKey();
      Map<String, Result> rows0 = row.getValue();
      Map<String, Object> deviceInfo = deviceInfos.get(id);
      Map<String, Object> build = (Map<String, Object>) deviceInfo.get("build");

      Map<String, Object> version = (Map<String, Object>) build.get("version");
      Map<String, Object> fields = (Map<String, Object>) build.get("fields");
      body.append("<td>")
          .append(fields.get("MANUFACTURER"))
          .append("</td>")
          .append("<td>")
          .append(fields.get("MODEL"))
          .append("</td>")
          .append("<td>")
          .append(fields.get("DEVICE"))
          .append("</td>")
          .append("<td>")
          .append(version.get("SDK_INT"))
          .append("</td>")
          .append("<td>")
          .append(version.get("RELEASE"))
          .append("</td>");

      Map<String, Float> maxScore = new HashMap<>();

      for (int idx = 0; idx != horizontalOrder.size(); idx++) {
        List<Object> coords = horizontalOrder.get(idx);
        Result result = rows0.get(coords.toString());
        if (result != null && result.isAcceptable()) {
          maxScore.merge(result.getGroup(), result.getScore(), Math::max);
        }
      }

      for (int idx = 0; idx != horizontalOrder.size(); idx++) {
        List<Object> coords = horizontalOrder.get(idx);
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
   *
   * @param tests         The test dimensions to reorder.
   * @param verticalOrder The new order to use.
   * @return The selections for the tests in an array, ordered left to right.
   */
  private static List<List<Object>> getHorizontalOrder(
      List<Object> tests, List<Integer> verticalOrder) {
    List<List<Object>> horizontalOrder = new ArrayList<>();
    List<Object> base = new ArrayList<>();
    for (int idx = 0; idx < tests.size(); idx++) {
      base.add(0);
    }

    while (true) {
      horizontalOrder.add(new ArrayList<>(base));
      boolean rolledOver = true;
      ListIterator<Integer> it = verticalOrder.listIterator(verticalOrder.size());
      while (it.hasPrevious()) {
        int idx = it.previous();
        Collection<Object> test = (Collection<Object>) tests.get(idx);
        int value = (int) base.get(idx);
        if (value == test.size() - 1) {
          base.set(idx, 0);
        } else {
          base.set(idx, value + 1);
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

  private static int getTotalVariations(List<Object> tests) {
    int total = 1;
    for (Object o : tests) {
      total *= ((Collection<Object>) o).size();
    }
    return total;
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
