package net.jimblackler.collate;

import java.awt.Desktop;
import java.io.IOException;
import java.net.URI;
import java.nio.file.Files;
import java.nio.file.Path;
import org.json.JSONArray;
import org.json.JSONObject;

class Main {
  private static final int SCENARIO = 1;

  public static void main(String[] args) throws IOException, InterruptedException {
    Path directory = Files.createTempDirectory("report-");
    JSONArray collect = new JSONArray();
    Collector.deviceCollect(collect::put, SCENARIO);
    Desktop.getDesktop().browse(writeGraphs(directory, collect));
  }

  static URI writeGraphs(Path directory, JSONArray results) {
    try {
      Path outputFile;
      if (results.length() == 1) {
        JSONArray result = results.getJSONArray(0);
        JSONObject first = result.getJSONObject(0);
        JSONObject build = first.getJSONObject("build");
        int count = 0;
        while (true) {
          String name =
              build.getString("ID")
                  + "_"
                  + build.getString("RELEASE")
                  + "_"
                  + first.getInt("scenario")
                  + (count > 0 ? "_" + count : "")
                  + ".html";
          outputFile = directory.resolve(name);
          if (!Files.exists(outputFile)) {
            break;
          }
          count++;
        }
      } else {
        outputFile = Files.createTempFile(directory, "report-", ".html");
      }
      String content = Utils.fileToString("main.html");
      //noinspection HardcodedFileSeparator
      content = content.replace("[/*data*/]", results.toString());
      //noinspection HardcodedFileSeparator
      content = content.replace("/*css*/", Utils.fileToString("report.css"));
      Files.writeString(outputFile, content);
      return outputFile.toUri();
    } catch (IOException ex) {
      throw new RuntimeException(ex);
    }
  }
}
