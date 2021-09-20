package net.jimblackler.collate;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectWriter;
import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.nio.file.FileSystems;
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
  private static final boolean USE_DATASTORE = true;
  private static final String VERSION = "1.7r";

  public static void main(String[] args) throws IOException {
    go(USE_DEVICE, USE_DATASTORE);
  }

  static void go(boolean useDevice, boolean useDatastore) throws IOException {
    ObjectMapper objectMapper = new ObjectMapper();
    ObjectWriter objectWriter = objectMapper.writerWithDefaultPrettyPrinter();
    Map<String, Map<String, Result>> out = new HashMap<>();
    Map<String, Map<String, Object>> deviceInfos = new HashMap<>();
    AtomicReference<Path> directory = new AtomicReference<>();
    AtomicReference<List<List<Map<String, Object>>>> tests = new AtomicReference<>();
    AtomicReference<Timer> timer = new AtomicReference<>();

    if (useDatastore) {
      Collector.collectDataStoreResult(
          (results1, result)
              -> handleResult(result, out, deviceInfos, directory, tests, objectWriter,
                  Boolean.TRUE.equals(results1.get("automatic"))),
          VERSION);
    } else {
      Consumer<List<Map<String, Object>>> collect = result -> {
        if (timer.get() != null) {
          timer.get().cancel();
        }
        timer.set(new Timer());
        TimerTask task = new TimerTask() {
          @Override
          public void run() {
            List<List<Map<String, Object>>> _tests = tests.get();
            if (_tests == null || _tests.isEmpty()) {
              return;
            }
            try {
              URI uri = writeReport(out, deviceInfos, directory.get(), _tests);
              System.out.println(uri);
            } catch (IOException e) {
              throw new IllegalStateException(e);
            }
          }
        };
        timer.get().schedule(task, 1000 * 10);

        handleResult(result, out, deviceInfos, directory, tests, objectWriter, true);
      };

      if (useDevice) {
        Collector.deviceCollect("net.jimblackler.istresser", collect);
      } else {
        Collector.cloudCollect(null, collect);
      }
    }

    if (timer.get() != null) {
      timer.get().cancel();
    }
    URI uri = writeReport(out, deviceInfos, directory.get(), tests.get());
    System.out.println(uri);
  }

  private static void handleResult(List<Map<String, Object>> result,
      Map<String, Map<String, Result>> out, Map<String, Map<String, Object>> deviceInfos,
      AtomicReference<Path> directory, AtomicReference<List<List<Map<String, Object>>>> tests,
      ObjectWriter objectWriter, boolean automatic) {
    Map<String, Object> deviceInfo = ReportUtils.getDeviceInfo(result);
    if (deviceInfo == null) {
      System.out.println("Could not find deviceInfo.");
      return;
    }
    deviceInfo.put("automatic", automatic);
    Map<String, Object> first = result.get(0);
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
    List<Number> coordinates = (List<Number>) params.get("coordinates");
    tests.set((List<List<Map<String, Object>>>) params.get("tests"));

    assert deviceInfo.containsKey("build");
    Map<String, Object> build = (Map<String, Object>) deviceInfo.get("build");
    String key = coordinates.toString();
    String id = build.toString();

    Map<String, Object> extra = (Map<String, Object>) first.get("extra");
    if (extra != null) {
      Map<String, Object> step = (Map<String, Object>) extra.get("step");
      if (step != null) {
        id = step.get("dimensionValue").toString();
      }
    }

    while (out.containsKey(id) && out.get(id).containsKey(key)) {
      id += "2";
    }

    if (directory.get() == null) {
      String dirName = extra == null ? (String) params.get("run") : (String) extra.get("historyId");
      directory.set(FileSystems.getDefault().getPath("reports").resolve(dirName));
      try {
        File outDir = directory.get().toFile();
        if (outDir.exists()) {
          // Empty the directory if it already exists.
          try (Stream<Path> files = Files.walk(directory.get())) {
            files.sorted(Comparator.reverseOrder()).map(Path::toFile).forEach(File::delete);
          }
        }
        outDir.mkdirs();
        Utils.copyFolder(FileSystems.getDefault().getPath("resources", "static"),
            directory.get().resolve("static"));
      } catch (IOException e) {
        throw new IllegalStateException(e);
      }
    }

    deviceInfos.put(id, deviceInfo);

    long lowestTop = Long.MAX_VALUE;
    long largest = 0;
    boolean exited = false;
    boolean allocFailed = false;
    boolean failedToClear = false;
    boolean serviceCrashed = false;

    for (Object o : result) {
      Map<String, Object> row = (Map<String, Object>) o;
      exited |= Boolean.TRUE.equals(row.get("exiting"));

      allocFailed |= Boolean.TRUE.equals(row.get("allocFailed"));
      allocFailed |= Boolean.TRUE.equals(row.get("mmapAnonFailed"));
      allocFailed |= Boolean.TRUE.equals(row.get("mmapFileFailed"));
      allocFailed |= row.containsKey("criticalLogLines");

      failedToClear |= Boolean.TRUE.equals(row.get("failedToClear"));

      serviceCrashed |= Boolean.TRUE.equals(row.get("serviceCrashed"));
      long score = 0;
      Map<String, Object> testMetrics = (Map<String, Object>) row.get("testMetrics");
      if (testMetrics != null) {
        for (Object o2 : testMetrics.values()) {
          score += ((Number) o2).longValue();
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
    Map<String, Result> results0 = out.computeIfAbsent(id, k -> new HashMap<>());
    Map<String, Object> group = Utils.flattenParams(coordinates, tests.get());
    group.remove("advisorParameters");
    URI uri = Main.writeGraphs(directory.get(), result);
    System.out.println(uri);
    results0.put(key,
        new Result(
            score, uri, exited && !allocFailed, failedToClear, serviceCrashed, group.toString()));
  }

  private static URI writeReport(Map<String, Map<String, Result>> rows,
      Map<String, Map<String, Object>> deviceInfos, Path directory,
      List<List<Map<String, Object>>> tests) throws IOException {
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
    FileUtils.writeString(outputFile, content);
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
  private static boolean worthRendering(
      Iterable<Integer> verticalOrder, List<List<Map<String, Object>>> objects) {
    int original = 0;
    for (int idx : verticalOrder) {
      if (idx != original) {
        int n = objects.get(idx).size();
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
   * @param size The permutation.
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
      Map<String, Map<String, Object>> deviceInfos, List<List<Map<String, Object>>> tests,
      Path directory, List<Integer> verticalOrder) {
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
        .append("<th rowspan=" + rowspan + " >")
        .append("totalMem")
        .append("</th>")
        .append("<th rowspan=" + rowspan + " >")
        .append("Automatic")
        .append("</th>")
        .append("</tr>");

    int repeats = 1;
    for (int idx : verticalOrder) {
      Collection<Map<String, Object>> test = tests.get(idx);
      colspan /= test.size();
      body.append("<tr>");

      for (int repeat = 0; repeat != repeats; repeat++) {
        for (Map<String, Object> _param : test) {
          Node dataExplorer = DataExplorer.getDataExplorer(HtmlUtils.getDocument(), _param);
          body.append("<th colspan=" + colspan + " class='params'>")
              .append(HtmlUtils.toString(dataExplorer))
              .append("</th>");
        }
      }
      body.append("</tr>");
      repeats *= test.size();
    }

    List<List<Object>> horizontalOrder = getHorizontalOrder(tests, verticalOrder);
    body.append("<tr>");
    body.append("<td colspan=7/>");
    for (List<Object> coords : horizontalOrder) {
      int total = 0;
      int acceptable = 0;
      float validScore = 0;
      for (Map.Entry<String, Map<String, Result>> row : rows.entrySet()) {
        Map<String, Result> rows0 = row.getValue();
        Result result = rows0.get(coords.toString());
        if (result == null) {
          continue;
        }
        total++;
        if (result.isAcceptable()) {
          acceptable++;
          validScore += result.getScore();
        }
      }
      body.append("<td>");
      if (total > 0) {
        body.append(acceptable * 100 / total).append("%").append(" ");
      }

      if (acceptable > 0) {
        body.append((int) validScore / acceptable).append("</td>");
      }
    }
    body.append("</tr>");
    body.append("</thead>");

    for (Map.Entry<String, Map<String, Result>> row : rows.entrySet()) {
      body.append("<tr>");

      String id = row.getKey();
      Map<String, Result> rows0 = row.getValue();
      Map<String, Object> deviceInfo = deviceInfos.get(id);
      Map<String, Object> build = (Map<String, Object>) deviceInfo.get("build");

      Map<String, Object> version = (Map<String, Object>) build.get("version");
      Map<String, Object> fields = (Map<String, Object>) build.get("fields");
      Map<String, Object> baseline = (Map<String, Object>) deviceInfo.get("baseline");
      Map<String, Object> baselineMemoryInfo = (Map<String, Object>) baseline.get("MemoryInfo");
      long totalMem = ((Number) baselineMemoryInfo.get("totalMem")).longValue();
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
          .append("</td>")
          .append("<td>")
          .append(totalMem / (1024 * 1024))
          .append("</td>")
          .append("<td>")
          .append(Boolean.TRUE.equals(deviceInfo.get("automatic")) ? "auto" : "")
          .append("</td>");

      Map<String, Float> maxScore = new HashMap<>();

      for (List<Object> coords : horizontalOrder) {
        Result result = rows0.get(coords.toString());
        if (result != null && result.isAcceptable()) {
          maxScore.merge(result.getGroup(), result.getScore(), Math::max);
        }
      }

      for (List<Object> coords : horizontalOrder) {
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
        if (result.isFailedToClear()) {
          classes.add("failedToClear");
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
      List<List<Map<String, Object>>> tests, List<Integer> verticalOrder) {
    List<List<Object>> horizontalOrder = new ArrayList<>();
    List<Object> base = new ArrayList<>();
    while (base.size() < tests.size()) {
      base.add(0);
    }

    while (true) {
      horizontalOrder.add(new ArrayList<>(base));
      boolean rolledOver = true;
      ListIterator<Integer> it = verticalOrder.listIterator(verticalOrder.size());
      while (it.hasPrevious()) {
        int idx = it.previous();
        List<Map<String, Object>> test = tests.get(idx);
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

  private static int getTotalVariations(Iterable<List<Map<String, Object>>> tests) {
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
    private final boolean failedToClear;
    private final boolean serviceCrashed;
    private final String group;

    Result(float score, URI uri, boolean acceptable, boolean failedToClear, boolean serviceCrashed,
        String group) {
      this.score = score;
      this.uri = uri;
      this.acceptable = acceptable;
      this.failedToClear = failedToClear;
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

    boolean isFailedToClear() {
      return failedToClear;
    }

    boolean isServiceCrashed() {
      return serviceCrashed;
    }

    String getGroup() {
      return group;
    }
  }
}
