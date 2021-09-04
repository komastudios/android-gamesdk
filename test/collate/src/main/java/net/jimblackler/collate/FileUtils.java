package net.jimblackler.collate;

import com.google.android.apps.internal.games.memoryadvice_common.StreamUtils;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;

class FileUtils {
  /**
   * Loads all text from the specified file and returns the result as a string.
   *
   * @param filename The name of the file to read.
   * @return All of the text from the file.
   * @throws IOException Thrown if a read error occurs.
   */
  static String readFile(String filename) throws IOException {
    try (FileInputStream inputStream = new FileInputStream(filename)) {
      return StreamUtils.readStream(inputStream);
    }
  }

  static String readFile(Path path) throws IOException {
    return readFile(path.toString());
  }

  static void writeString(Path p, String data) {
    try (PrintWriter out = new PrintWriter(p.toFile(), StandardCharsets.UTF_8.name())) {
      out.println(data);
    } catch (FileNotFoundException | UnsupportedEncodingException e) {
      throw new RuntimeException(e);
    }
  }
}
