package net.jimblackler.collate;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

class Utils {
  static String fileToString(String filename) throws IOException {
    return Files.readString(Paths.get("resources", filename));
  }

  static void copy(Path directory, String filename) throws IOException {
    Files.copy(Paths.get("resources", filename), directory.resolve(filename));
  }
}
