package net.jimblackler.collate;

import static net.jimblackler.collate.CsvEscaper.escape;

import java.io.IOException;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

/**
 * Builds a table of data about devices from lab test results.
 */
public class DeviceTable {
  public static void main(String[] args) throws IOException {
    try (PrintWriter writer = new PrintWriter(Files.newBufferedWriter(Paths.get("devices.csv")))) {
      writer.print("Id");
      writer.print(",");
      writer.print("Versions");
      writer.print(",");
      writer.print("Manufacturer");
      writer.print(",");
      writer.print("Brand");
      writer.print(",");
      writer.print("Codename");
      writer.print(",");
      writer.print("Name");
      writer.print(",");
      writer.print("Form");
      writer.print(",");
      writer.print("FormFactor");
      writer.print(",");
      writer.print("ScreenDensity");
      writer.print(",");
      writer.print("ScreenX");
      writer.print(",");
      writer.print("ScreenY");
      writer.print(",");
      writer.print("SupportedAbis");
      writer.print(",");
      writer.print("Thumbnail");
      writer.println();

      DeviceFetcher.fetch(device -> {
        writer.print(device.get("id"));
        writer.print(",");
        writer.print(escape(versionIdsToString((List<Object>) device.get("supportedVersionIds"))));
        writer.print(",");
        writer.print(device.get("manufacturer"));
        writer.print(",");
        writer.print(device.get("brand"));
        writer.print(",");
        writer.print(device.get("codename"));
        writer.print(",");
        writer.print(escape((String) device.get("name")));
        writer.print(",");
        writer.print(device.get("form"));
        writer.print(",");
        writer.print(device.get("formFactor"));
        writer.print(",");
        writer.print(device.get("screenDensity"));
        writer.print(",");
        writer.print(device.get("screenX"));
        writer.print(",");
        writer.print(device.get("screenY"));
        writer.print(",");
        writer.print(escape(supportedAbisToString((List<Object>) device.get("supportedAbis"))));
        writer.print(",");
        writer.print(
            device.containsKey("thumbnailUrl") ? escape((String) device.get("thumbnailUrl")) : "");
        writer.println();
      });
    }
  }

  private static String supportedAbisToString(List<Object> supportedAbis) {
    StringBuilder sb = new StringBuilder();
    for (int idx = 0; idx != supportedAbis.size(); idx++) {
      if (idx != 0) {
        sb.append(",");
      }
      String items = (String) supportedAbis.get(idx);
      sb.append(items);
    }
    return sb.toString();
  }

  private static String versionIdsToString(List<Object> supportedVersionIds) {
    StringBuilder sb = new StringBuilder();
    for (int idx = 0; idx != supportedVersionIds.size(); idx++) {
      if (idx != 0) {
        sb.append(",");
      }
      sb.append(supportedVersionIds.get(idx));
    }
    return sb.toString();
  }
}
