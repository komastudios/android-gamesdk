package com.google.android.apps.internal.games.memoryadvice_common;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;

public class StreamUtils {
  /**
   * Loads all the text from an input string and returns the result as a string.
   *
   * @param inputStream The stream to read.
   * @return All of the text from the stream.
   * @throws IOException Thrown if a read error occurs.
   */
  public static String readStream(InputStream inputStream) throws IOException {
    return readStream(inputStream, null);
  }

  /**
   * Loads all the text from an input string and returns the result as a string.
   *
   * @param inputStream The stream to read.
   * @param out Optional stream to output the data as it is read.
   * @return All of the text from the stream.
   * @throws IOException Thrown if a read error occurs.
   */
  public static String readStream(InputStream inputStream, PrintStream out) throws IOException {
    try (InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
         BufferedReader reader = new BufferedReader(inputStreamReader)) {
      String newline = System.lineSeparator();
      StringBuilder output = new StringBuilder();
      String line;
      while ((line = reader.readLine()) != null) {
        if (output.length() > 0) {
          output.append(newline);
        }
        if (out != null) {
          out.println(line);
        }
        output.append(line);
      }
      return output.toString();
    }
  }
}
