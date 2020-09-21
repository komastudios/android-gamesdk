package com.google.android.apps.internal.games.memoryadvice;

import android.util.Log;
import com.google.android.apps.internal.games.memoryadvice_common.StreamUtils;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/** A helper class with static methods to help with Heuristics and file IO */
public class Utils {
  private static final String TAG = Utils.class.getSimpleName();
  private static final Pattern MEMINFO_REGEX = Pattern.compile("([^:]+)[^\\d]*(\\d+).*\n");
  private static final Pattern PROC_REGEX = Pattern.compile("([a-zA-Z]+)[^\\d]*(\\d+) kB.*\n");

  /**
   * Loads all text from the specified file and returns the result as a string.
   *
   * @param filename The name of the file to read.
   * @return All of the text from the file.
   * @throws IOException Thrown if a read error occurs.
   */
  public static String readFile(String filename) throws IOException {
    return StreamUtils.readStream(new FileInputStream(filename));
  }

  /**
   * Returns the OOM score associated with a process.
   *
   * @return The OOM score, or -1 if the score cannot be obtained.
   * @param pid The process ID (pid).
   */
  static int getOomScore(int pid) {
    try {
      return Integer.parseInt(readFile(("/proc/" + pid) + "/oom_score"));
    } catch (IOException | NumberFormatException e) {
      return -1;
    }
  }

  /**
   * Returns a dictionary of values extracted from the /proc/meminfo file.
   *
   * @return A dictionary of values, in bytes.
   */
  static Map<String, Long> processMeminfo() {
    Map<String, Long> output = new HashMap<>();

    String filename = "/proc/meminfo";
    try {
      String meminfoText = readFile(filename);
      Matcher matcher = MEMINFO_REGEX.matcher(meminfoText);
      while (matcher.find()) {
        output.put(
            matcher.group(1), Long.parseLong(Objects.requireNonNull(matcher.group(2))) * 1024);
      }
    } catch (IOException e) {
      Log.w(TAG, "Failed to read " + filename);
    }
    return output;
  }

  /**
   * Return a dictionary of values extracted from a processes' /proc/../status
   * files.
   *
   * @param pid The process ID (pid).
   * @return A dictionary of values, in bytes.
   */
  static Map<String, Long> processStatus(int pid) {
    Map<String, Long> output = new HashMap<>();
    String filename = "/proc/" + pid + "/status";
    try {
      String meminfoText = readFile(filename);
      Matcher matcher = PROC_REGEX.matcher(meminfoText);
      while (matcher.find()) {
        String key = matcher.group(1);
        long value = Long.parseLong(Objects.requireNonNull(matcher.group(2)));
        output.put(key, value * 1024);
      }
    } catch (IOException e) {
      Log.w(TAG, "Failed to read " + filename);
    }
    return output;
  }
}
