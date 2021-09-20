package com.google.android.apps.internal.games.memoryadvice;

import android.util.Log;
import com.google.android.apps.internal.games.memoryadvice_common.StreamUtils;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * A helper class with static methods to help with Heuristics and file IO
 */
public class Utils {
  private static final String TAG = Utils.class.getSimpleName();
  private static final Pattern MEMORY_REGEX = Pattern.compile("([a-zA-Z()_]+)[^\\d]*(\\d+) kB.*\n");

  /**
   * Loads all text from the specified file and returns the result as a string.
   *
   * @param filename The name of the file to read.
   * @return All of the text from the file.
   * @throws IOException Thrown if a read error occurs.
   */
  public static String readFile(String filename) throws IOException {
    try (FileInputStream inputStream = new FileInputStream(filename)) {
      return StreamUtils.readStream(inputStream);
    }
  }

  /**
   * Returns the OOM score associated with a process.
   *
   * @param pid The process ID (pid).
   * @return The OOM score, or -1 if the score cannot be obtained.
   */
  static int getOomScore(int pid) {
    try {
      return Integer.parseInt(readFile(("/proc/" + pid) + "/oom_score"));
    } catch (IOException | NumberFormatException e) {
      return -1;
    }
  }

  /**
   * Extracts values from a Linux memory summary format file, converting kB values to bytes.
   *
   * @param filename The file to read.
   * @param client The client to receive values.
   */
  static void processMemFormatFile(String filename, MemFormatClient client) {
    try {
      String meminfoText = readFile(filename);
      Matcher matcher = MEMORY_REGEX.matcher(meminfoText);
      while (matcher.find()) {
        client.receive(
            matcher.group(1), Long.parseLong(Objects.requireNonNull(matcher.group(2))) * 1024);
      }
    } catch (IOException e) {
      Log.w(TAG, "Failed to read " + filename);
    }
  }

  /**
   * A receiver of Linux memory file data.
   */
  interface MemFormatClient {
    void receive(String key, long bytes);
  }
}
