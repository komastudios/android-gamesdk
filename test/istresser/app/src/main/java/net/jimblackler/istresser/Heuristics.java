package net.jimblackler.istresser;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.os.Debug.MemoryInfo;
import android.util.Log;
import com.google.common.primitives.Ints;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
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
    List<RunningAppProcessInfo> runningAppProcessInfos = activityManager.getRunningAppProcesses();
    if (runningAppProcessInfos != null) {
      for (RunningAppProcessInfo runningAppProcessInfo : runningAppProcessInfos) {
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
        int oom = Integer.parseInt(Utils.readFile(("/proc/" + pid) + "/oom_score"));
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
      String meminfoText = Utils.readFile(filename);
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
          String meminfoText = Utils.readFile(filename);
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
  static MemoryInfo[] getDebugMemoryInfo(ActivityManager activityManager) {
    return activityManager.getProcessMemoryInfo(Ints.toArray(getPids(activityManager)));
  }
}
