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

  /**
   * Loads all text from the specified file and returns the result as a string.
   *
   * @param path The path of the file to read.
   * @return All of the text from the file.
   * @throws IOException Thrown if a read error occurs.
   */
  static String readFile(Path path) throws IOException {
    return readFile(path.toString());
  }

  /**
   * Write the supplied string to the path provided.
   * @param path The path of a file to create or overwrite.
   * @param text The text to write.
   */
  static void writeString(Path path, String text) {
    try (PrintWriter out = new PrintWriter(path.toFile(), StandardCharsets.UTF_8.name())) {
      out.println(text);
    } catch (FileNotFoundException | UnsupportedEncodingException e) {
      throw new RuntimeException(e);
    }
  }
}
