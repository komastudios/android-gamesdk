package net.jimblackler.istresser;

import android.app.ActivityManager;
import android.os.Debug;
import android.util.Log;

import com.google.common.primitives.Ints;

import static com.google.common.base.StandardSystemProperty.LINE_SEPARATOR;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.json.JSONException;
import org.json.JSONObject;

/** A helper class with static methods to help with Heuristics and file IO */
public class Utils {

  private static final String TAG = Utils.class.getSimpleName();

  private Utils() {}

  /**
   * Loads all the text from an input string and returns the result as a string.
   *
   * @param inputStream The stream to read.
   * @return All of the text from the stream.
   * @throws IOException Thrown if a read error occurs.
   */
  static String readStream(InputStream inputStream) throws IOException {
    try (InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
         BufferedReader reader = new BufferedReader(inputStreamReader)) {
      String newline = LINE_SEPARATOR.value();
      StringBuilder output = new StringBuilder();
      String line;
      while ((line = reader.readLine()) != null) {
        if (output.length() > 0) {
          output.append(newline);
        }
        output.append(line);
      }
      return output.toString();
    }
  }

  /**
   * Loads all text from the specified file and returns the result as a string.
   *
   * @param filename The name of the file to read.
   * @return All of the text from the file.
   * @throws IOException Thrown if a read error occurs.
   */
  static String readFile(String filename) throws IOException {
    return readStream(new FileInputStream(filename));
  }

  /**
   * Gets the size in bytes of the indicated file.
   *
   * @param filename The name of the file to get the size for.
   * @return File size.
   * @throws IOException Thrown if a filesystem error occurs.
   */
  static long getFileSize(String filename) throws IOException {
    return new File(filename).length();
  }

  /**
   * Return a collection of the process identifiers associated with the running app.
   *
   * @param activityManager The ActivityManager.
   * @return The collection of PIDs.
   */
  private static Collection<Integer> getPids(ActivityManager activityManager) {
    Collection<Integer> pids = new ArrayList<>();
    List<ActivityManager.RunningAppProcessInfo> runningAppProcessInfos =
        activityManager.getRunningAppProcesses();
    if (runningAppProcessInfos != null) {
      for (ActivityManager.RunningAppProcessInfo runningAppProcessInfo : runningAppProcessInfos) {
        pids.add(runningAppProcessInfo.pid);
      }
    }
    return pids;
  }

  /**
   * Returns the OOM score associated with the main application process. As multiple processes can
   * be associated with an app the largest OOM score of all process is selected.
   *
   * @param activityManager The ActivityManager.
   * @return The OOM score.
   */
  static int getOomScore(ActivityManager activityManager) {
    try {
      int largestOom = 0;
      for (int pid : getPids(activityManager)) {
        int oom = Integer.parseInt(readFile(("/proc/" + pid) + "/oom_score"));
        largestOom = Math.max(largestOom, oom);
      }
      return largestOom;
    } catch (IOException | NumberFormatException e) {
      return 0;
    }
  }

  /**
   * Returns the Activity Manager's MemoryInfo object.
   *
   * @param activityManager The ActivityManager.
   * @return The MemoryInfo object.
   */
  static ActivityManager.MemoryInfo getMemoryInfo(ActivityManager activityManager) {
    ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
    activityManager.getMemoryInfo(memoryInfo);
    return memoryInfo;
  }

  /**
   * Returns a dictionary of values extracted from the /proc/meminfo file.
   *
   * @return A dictionary of values.
   */
  static Map<String, Long> processMeminfo() {
    Map<String, Long> output = new HashMap<>();

    String filename = "/proc/meminfo";
    try {
      String meminfoText = readFile(filename);
      Pattern pattern = Pattern.compile("([^:]+)[^\\d]*(\\d+).*\n");
      Matcher matcher = pattern.matcher(meminfoText);
      while (matcher.find()) {
        output.put(matcher.group(1), Long.parseLong(Objects.requireNonNull(matcher.group(2))));
      }
    } catch (IOException e) {
      Log.w(TAG, "Failed to read " + filename);
    }
    return output;
  }

  /**
   * Return a dictionary of values extracted from the application processes' /proc/../status files.
   * Where multiple processes are found, for each case the highest value is selected.
   *
   * @param activityManager The ActivityManager.
   * @return A dictionary of values.
   */
  static Map<String, Long> processStatus(ActivityManager activityManager) {
    Map<String, Long> output = new HashMap<>();
    //
    {
      for (int pid : getPids(activityManager)) {
        String filename = "/proc/" + pid + "/status";
        try {
          String meminfoText = readFile(filename);
          Pattern pattern = Pattern.compile("([a-zA-Z]+)[^\\d]*(\\d+) kB.*\n");
          Matcher matcher = pattern.matcher(meminfoText);
          while (matcher.find()) {
            String key = matcher.group(1);
            long value = Long.parseLong(Objects.requireNonNull(matcher.group(2)));
            Long existingValue = output.get(key);
            if (existingValue == null || existingValue < value) {
              output.put(key, value);
            }
          }
        } catch (IOException e) {
          Log.w(TAG, "Failed to read " + filename);
        }
      }
    }
    return output;
  }

  /**
   * Returns 'true' if the memory info 'low memory' value is currently true.
   *
   * @param activityManager The ActivityManager.
   * @return The low memory status.
   */
  static boolean lowMemoryCheck(ActivityManager activityManager) {
    return getMemoryInfo(activityManager).lowMemory;
  }

  /**
   * Returns the process memory info for the application.
   *
   * @param activityManager The ActivityManager.
   * @return The process memory info.
   */
  static Debug.MemoryInfo[] getDebugMemoryInfo(ActivityManager activityManager) {
    return activityManager.getProcessMemoryInfo(Ints.toArray(getPids(activityManager)));
  }

  /**
   * Converts a memory quantity value in a JSON object to a number of bytes. If no value exists with
   * that key, zero is returned. If the value is a JSON number, it is interpreted as the number of
   * bytes. If the value is a JSON string, it is converted according to the specified unit. e.g.
   * "36K", "52.5 M", "9.1G". No unit is interpreted as bytes.
   *
   * @param jsonObject The JSON object to extract from.
   * @param key The name of the key.
   * @return The equivalent number of bytes.
   */
  static long getMemoryQuantity(JSONObject jsonObject, String key) {
    if (!jsonObject.has(key)) {
      return 0;
    }
    try {
      return jsonObject.getLong(key);
    } catch (JSONException e) {
      Log.v(TAG, key + " is not a number.");
    }

    String str;
    try {
      str = jsonObject.getString(key);
    } catch (JSONException e) {
      throw new NumberFormatException(e.toString());
    }

    int unitPosition = str.indexOf('K');
    int unitMultiplier = 1024;
    if (unitPosition == -1) {
      unitPosition = str.indexOf('M');
      unitMultiplier *= 1024;
      if (unitPosition == -1) {
        unitPosition = str.indexOf('G');
        unitMultiplier *= 1024;
        if (unitPosition == -1) {
          unitMultiplier = 1;
        }
      }
    }
    float value = Float.parseFloat(str.substring(0, unitPosition));
    return (long) (value * unitMultiplier);
  }
}
