package net.jimblackler.istresser;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.os.Debug;
import android.os.Debug.MemoryInfo;
import android.util.Log;
import com.google.common.primitives.Ints;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Heuristics {

  private static final String TAG = Heuristics.class.getSimpleName();

  /**
   * Return a collection of the process identifiers associated with the running app.
   *
   * @param activityManager The ActivityManager.
   * @return The collection of PIDs.
   */
  private static Collection<Integer> getPids(ActivityManager activityManager) {
    Collection<Integer> pids = new ArrayList<>();
    for (RunningAppProcessInfo runningAppProcessInfo : activityManager.getRunningAppProcesses()) {
      pids.add(runningAppProcessInfo.pid);
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
   * Loads all the text from an input string and returns the result as a string.
   *
   * @param inputStream The stream to read.
   * @return All of the text from the stream.
   * @throws IOException Thrown if a read error occurs.
   */
  private static String readStream(InputStream inputStream) throws IOException {
    try (InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
        BufferedReader reader = new BufferedReader(inputStreamReader)) {
      String newline = System.getProperty("line.separator");
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
  private static String readFile(String filename) throws IOException {
    return readStream(new FileInputStream(filename));
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
   * Returns 'true' if the OOM score for the activity is above a level that requires memory to be
   * freed to prevent the application from being killed.
   *
   * @param activityManager The ActivityManager.
   * @return The status value.
   */
  static boolean oomCheck(ActivityManager activityManager) {
    return getOomScore(activityManager) > 650;
  }

  /**
   * Returns the process memory info for the application.
   *
   * @param activityManager The ActivityManager.
   * @return The process memory info.
   */
  static MemoryInfo[] getDebugMemoryInfo(ActivityManager activityManager) {
    return activityManager.getProcessMemoryInfo(Ints.toArray(getPids(activityManager)));
  }

  /**
   * Returns 'true' if the commit limit is above a level that requires memory to be freed to prevent
   * the application from being killed.
   */
  static boolean commitLimitCheck() {
    Long value = processMeminfo().get("CommitLimit");
    if (value == null) {
      return false;
    }
    return Debug.getNativeHeapAllocatedSize() / 1024 > value;
  }

  /**
   * Returns 'true' if the availMem value is below a level that requires memory to be freed to
   * prevent the application from being killed.
   *
   * @param activityManager The ActivityManager.
   * @return The status value.
   */
  static boolean availMemCheck(ActivityManager activityManager) {
    ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activityManager);
    return memoryInfo.availMem < memoryInfo.threshold * 2;
  }

  /**
   * Returns 'true' if the cached value is at a level that requires memory to be freed to prevent
   * the application from being killed.
   *
   * @param activityManager The ActivityManager.
   * @return The status value.
   */
  static boolean cachedCheck(ActivityManager activityManager) {
    Long value = processMeminfo().get("Cached");
    if (value == null || value == 0) {
      return false;
    }
    ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activityManager);
    return value < memoryInfo.threshold / 1024;
  }

  /**
   * Returns 'true' if the MemAvailable value is at a level that requires memory to be freed to
   * prevent the application from being killed.
   *
   * @param activityManager The ActivityManager.
   * @return The status value.
   */
  static boolean memAvailableCheck(ActivityManager activityManager) {
    Long value = processMeminfo().get("MemAvailable");
    if (value == null || value == 0) {
      return false;
    }
    ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activityManager);
    return value < memoryInfo.threshold * 2 / 1024;
  }
}
