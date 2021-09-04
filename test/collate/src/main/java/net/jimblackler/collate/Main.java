package net.jimblackler.collate;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectWriter;
import java.awt.Desktop;
import java.io.IOException;
import java.net.URI;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.Map;

class Main {
  public static void main(String[] args) throws IOException {
    Path directory = Files.createTempDirectory("report-");

    Collector.deviceCollect("net.jimblackler.istresser", result -> {
      try {
        Desktop.getDesktop().browse(writeGraphs(directory, result));
      } catch (IOException e) {
        throw new IllegalStateException(e);
      }
    });
  }

  static URI writeGraphs(Path directory, List<Map<String, Object>> result) {
    ObjectWriter objectWriter = new ObjectMapper().writerWithDefaultPrettyPrinter();
    try {
      Path outputFile;
      Map<String, Object> params = (Map<String, Object>) result.get(0).get("params");
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
      String content = Utils.fileToString("main.html");
      // noinspection HardcodedFileSeparator
      content = content.replace("[/*data*/]", objectWriter.writeValueAsString(result));
      FileUtils.writeString(outputFile, content);
      return outputFile.toUri();
    } catch (IOException ex) {
      throw new RuntimeException(ex);
    }
  }
}
