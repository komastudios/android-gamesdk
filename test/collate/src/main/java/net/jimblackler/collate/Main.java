package net.jimblackler.collate;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectWriter;
import java.awt.Desktop;
import java.io.IOException;
import java.net.URI;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

class Main {
  public static void main(String[] args) throws IOException, InterruptedException {
    Path directory = Files.createTempDirectory("report-");
    List<Object> collect = new ArrayList<>();
    Collector.deviceCollect("net.jimblackler.istresser", collect::add);
    Desktop.getDesktop().browse(writeGraphs(directory, collect));
  }

  static URI writeGraphs(Path directory, List<Object> results) {
    ObjectWriter objectWriter = new ObjectMapper().writerWithDefaultPrettyPrinter();
    try {
      Path outputFile;
      if (results.size() == 1) {
        List<Object> result = (List<Object>) results.get(0);
        Map<String, Object> first = (Map<String, Object>) result.get(0);
        Map<String, Object> params = (Map<String, Object>) first.get("params");
        Map<String, Object> deviceInfo = ReportUtils.getDeviceInfo(result);
        Map<String, Object> build = (Map<String, Object>) deviceInfo.get("build");
        Map<String, Object> fields = (Map<String, Object>) build.get("fields");
        int count = 0;
        while (true) {
          String coordinates = params.get("coordinates").toString();
          String name = fields.get("DEVICE") + (count > 0 ? "_" + count : "")
              + coordinates.replace("[", "_").replace(",", "-").replace("]", "").replace(" ", "")
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
      // noinspection HardcodedFileSeparator
      content = content.replace("[/*data*/]", objectWriter.writeValueAsString(results));
      Files.writeString(outputFile, content);
      return outputFile.toUri();
    } catch (IOException ex) {
      throw new RuntimeException(ex);
    }
  }
}
