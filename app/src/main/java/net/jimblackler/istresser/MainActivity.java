package net.jimblackler.istresser;

import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Debug;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;

import com.google.common.collect.HashMultiset;
import com.google.common.collect.ImmutableList;
import com.google.common.collect.Lists;
import com.google.common.collect.Multiset;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Objects;
import java.util.Scanner;
import java.util.Set;
import java.util.Timer;
import java.util.TimerTask;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class MainActivity extends AppCompatActivity {

  private static final String TAG = MainActivity.class.getSimpleName();
  public static final int MAX_DURATION = 1000 * 60 * 10;

  static {
    System.loadLibrary("native-lib");
  }

  private final Timer timer = new Timer();

  private ArrayList<byte[]> data = Lists.newArrayList();

  private Multiset<Integer> onTrims = HashMultiset.create();
  private long nativeAllocatedByTest;
  private long recordNativeHeapAllocatedSize;
  private PrintStream resultsStream = System.out;
  private long startTime;
  private long allocationStartedAt = -1;
  private int scenario = 2;
  private List<Integer> pids;
  private ActivityManager activityManager;

  private static String memoryString(long bytes) {
    return String.format(Locale.getDefault(), "%.1f MB", (float) bytes / (1024 * 1024));
  }

  private static String execute(String... args) throws IOException {
    return readStream(new ProcessBuilder(args).start().getInputStream());
  }

  private static String readStream(InputStream inputStream) throws IOException {
    try (
        InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
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

  private static Map<String, Long> processMeminfo(String meminfoText) {
    Map<String, Long> output = new HashMap<>();
    Pattern pattern = Pattern.compile("([^:]+)[^\\d]*(\\d+).*\n");

    Matcher matcher = pattern.matcher(meminfoText);

    while (matcher.find()) {
      output.put(matcher.group(1), Long.parseLong(Objects.requireNonNull(matcher.group(2))));
    }
    return output;
  }

  private static List<Integer> parseList(String string) {
    Scanner scanner = new Scanner(string);
    List<Integer> list = new ArrayList<>();
    while (scanner.hasNextInt()) {
      list.add(scanner.nextInt());
    }
    return list;
  }

  private static int[] toIntArray(List<Integer> list) {
    int[] ints = new int[list.size()];
    for (int i = 0; i < ints.length; i++) {
      ints[i] = list.get(i);
    }
    return ints;
  }

  private static String readFile(String filename) throws IOException {
    return readStream(new FileInputStream(filename));
  }

  private static ActivityManager.MemoryInfo getMemoryInfo(ActivityManager activityManager) {
    ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
    activityManager.getMemoryInfo(memoryInfo);
    return memoryInfo;
  }

  @Override
  protected void onStart() {
    super.onStart();
    activityManager =
        (ActivityManager) Objects.requireNonNull(getSystemService(Context.ACTIVITY_SERVICE));

    try {
      JSONObject report = new JSONObject();

      PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(), 0);
      // Two methods of getting the app's pids.
      if (false) {
        this.pids = parseList(execute("pidof", packageInfo.packageName));
      } else {
        List<Integer> pids = new ArrayList<>();
        List<ActivityManager.RunningAppProcessInfo> runningAppProcesses =
            activityManager.getRunningAppProcesses();
        for (ActivityManager.RunningAppProcessInfo runningAppProcessInfo : runningAppProcesses) {
          pids.add(runningAppProcessInfo.pid);
        }
        this.pids = pids;
      }
      report.put("version", packageInfo.versionCode);
      Intent launchIntent = getIntent();
      if ("com.google.intent.action.TEST_LOOP".equals(launchIntent.getAction())) {
        Uri logFile = launchIntent.getData();
        if (logFile != null) {
          String logPath = logFile.getEncodedPath();
          if (logPath != null) {
            Log.i(TAG, "Log file " + logPath);
            try {
              resultsStream = new PrintStream(
                  Objects.requireNonNull(getContentResolver().openOutputStream(logFile)));
            } catch (FileNotFoundException e) {
              e.printStackTrace();
            }
          }
        }
        int scenario = launchIntent.getIntExtra("scenario", 0);
        this.scenario = scenario;
        report.put("scenario", scenario);
      }
      JSONObject build = new JSONObject();

      build.put("ID", Build.ID);
      build.put("DISPLAY", Build.DISPLAY);
      build.put("PRODUCT", Build.PRODUCT);
      build.put("DEVICE", Build.DEVICE);
      build.put("BOARD", Build.BOARD);
      build.put("MANUFACTURER", Build.MANUFACTURER);
      build.put("BRAND", Build.BRAND);
      build.put("MODEL", Build.MODEL);
      build.put("BOOTLOADER", Build.BOOTLOADER);
      build.put("HARDWARE", Build.HARDWARE);
      if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
        build.put("BASE_OS", Build.VERSION.BASE_OS);
      }
      build.put("CODENAME", Build.VERSION.CODENAME);
      build.put("INCREMENTAL", Build.VERSION.INCREMENTAL);
      build.put("RELEASE", Build.VERSION.RELEASE);
      build.put("SDK_INT", Build.VERSION.SDK_INT);
      if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
        build.put("PREVIEW_SDK_INT", Build.VERSION.PREVIEW_SDK_INT);
        build.put("SECURITY_PATCH", Build.VERSION.SECURITY_PATCH);
      }
      report.put("build", build);

      JSONObject constant = new JSONObject();
      ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activityManager);
      constant.put("totalMem", memoryInfo.totalMem);
      constant.put("threshold", memoryInfo.threshold);
      constant.put("memoryClass", activityManager.getMemoryClass() * 1024 * 1024);

      Map<String, Long> meminfo = processMeminfo(readFile("/proc/meminfo"));
      for (String key : ImmutableList.of("MemTotal", "CommitLimit")) {
        constant.put(key, meminfo.get(key));
      }
      report.put("constant", constant);

      resultsStream.println(report);
    } catch (JSONException | PackageManager.NameNotFoundException | IOException e) {
      e.printStackTrace();
    }

    startTime = System.currentTimeMillis();
    allocationStartedAt = startTime;
    timer.schedule(new TimerTask() {
      @Override
      public void run() {
        try {
          JSONObject report = standardInfo();
          final long _allocationStartedAt = allocationStartedAt;
          if (_allocationStartedAt != -1) {
            ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activityManager);
            if (scenarioGroup(1) && getOomScore() > 650) {
              releaseMemory();
            } else if (scenarioGroup(2) && memoryInfo.lowMemory) {
              releaseMemory();
            } else if (scenarioGroup(3) && !tryAlloc(1024 * 1024 * 32)) {
              releaseMemory();
            } else {
              int bytesPerMillisecond = 100 * 1024;
              long sinceStart = System.currentTimeMillis() - _allocationStartedAt;
              int owed = (int) ((sinceStart * bytesPerMillisecond) - nativeAllocatedByTest);
              if (owed > 0) {
                boolean succeeded = nativeConsume(owed);
                if (succeeded) {
                  nativeAllocatedByTest += owed;
                } else {
                  report.put("allocFailed", true);
                }
              }
            }
          }
          long timeRunning = System.currentTimeMillis() - startTime;

          if (timeRunning > MAX_DURATION) {
            try {
              report = standardInfo();
              report.put("exiting", true);
              resultsStream.println(report);
            } catch (JSONException e) {
              e.printStackTrace();
            }
          }

          resultsStream.println(report);

          if (timeRunning > MAX_DURATION) {
            resultsStream.close();
            finish();
          }
          updateInfo();
        } catch (JSONException e) {
          e.printStackTrace();
        }
      }
    }, 0, 1000 / 10);

  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
  }

  @Override
  protected void onDestroy() {
    try {
      JSONObject report = standardInfo();
      report.put("onDestroy", true);
      resultsStream.println(report);
    } catch (JSONException e) {
      e.printStackTrace();
    }
    super.onDestroy();
  }

  private void updateRecords() {
    long nativeHeapAllocatedSize = Debug.getNativeHeapAllocatedSize();
    if (nativeHeapAllocatedSize > recordNativeHeapAllocatedSize) {
      recordNativeHeapAllocatedSize = nativeHeapAllocatedSize;
    }
  }

  private void updateInfo() {
    runOnUiThread(() -> {
      updateRecords();
      TextView freeMemory = findViewById(R.id.freeMemory);
      freeMemory.setText(memoryString(Runtime.getRuntime().freeMemory()));

      TextView totalMemory = findViewById(R.id.totalMemory);
      totalMemory.setText(memoryString(Runtime.getRuntime().totalMemory()));

      TextView maxMemory = findViewById(R.id.maxMemory);
      maxMemory.setText(memoryString(Runtime.getRuntime().maxMemory()));

      TextView nativeHeap = findViewById(R.id.nativeHeap);
      nativeHeap.setText(memoryString(Debug.getNativeHeapSize()));

      TextView nativeAllocated = findViewById(R.id.nativeAllocated);
      long nativeHeapAllocatedSize = Debug.getNativeHeapAllocatedSize();
      nativeAllocated.setText(memoryString(nativeHeapAllocatedSize));

      TextView recordNativeAllocated = findViewById(R.id.recordNativeAllocated);
      recordNativeAllocated.setText(memoryString(recordNativeHeapAllocatedSize));

      TextView nativeAllocatedByTestTextView = findViewById(R.id.nativeAllocatedByTest);
      nativeAllocatedByTestTextView.setText(memoryString(nativeAllocatedByTest));

      TextView trimMemoryComplete = findViewById(R.id.trimMemoryComplete);
      trimMemoryComplete.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_COMPLETE)));

      TextView trimMemoryModerate = findViewById(R.id.trimMemoryModerate);
      trimMemoryModerate.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_MODERATE)));

      TextView trimMemoryBackground = findViewById(R.id.trimMemoryBackground);
      trimMemoryBackground.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_BACKGROUND)));

      TextView trimMemoryUiHidden = findViewById(R.id.trimMemoryUiHidden);
      trimMemoryUiHidden.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_UI_HIDDEN)));

      TextView trimMemoryRunningCritical = findViewById(R.id.trimMemoryRunningCritical);
      trimMemoryRunningCritical.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_RUNNING_CRITICAL)));

      TextView trimMemoryRunningLow = findViewById(R.id.trimMemoryRunningLow);
      trimMemoryRunningLow.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_RUNNING_LOW)));

      TextView trimMemoryRunningModerate = findViewById(R.id.trimMemoryRunningModerate);
      trimMemoryRunningModerate.setText(
          String.format(Locale.getDefault(), "%d", onTrims.count(TRIM_MEMORY_RUNNING_MODERATE)));
    });
  }

  void jvmConsume(int bytes) {
    try {
      byte[] array = new byte[bytes];
      for (int count = 0; count < array.length; count++) {
        array[count] = (byte) count;
      }
      data.add(array);
    } catch (OutOfMemoryError e) {
      e.printStackTrace();
    }
  }

  @Override
  public void onTrimMemory(int level) {

    try {
      JSONObject report = standardInfo();
      report.put("onTrimMemory", level);

      if (scenarioGroup(0)) {
        releaseMemory();
      }
      resultsStream.println(report);

      onTrims.add(level);

      updateInfo();
      super.onTrimMemory(level);
    } catch (JSONException e) {
      throw new RuntimeException(e);
    }
  }

  private boolean scenarioGroup(int i) {
    return ((scenario - 1 & 1 << i) != 0);
  }

  private void releaseMemory() {
    allocationStartedAt = -1;
    runAfterDelay(() -> {
      JSONObject report = null;
      try {
        report = standardInfo();
      } catch (JSONException e) {
        throw new RuntimeException(e);
      }
      resultsStream.println(report);
      if (nativeAllocatedByTest > 0) {
        nativeAllocatedByTest = 0;
        freeAll();
      }
      data.clear();
      System.gc();
      runAfterDelay(() -> {
        try {
          resultsStream.println(standardInfo());
          allocationStartedAt = System.currentTimeMillis();
        } catch (JSONException e) {
          throw new RuntimeException(e);
        }
      }, 1000);
    }, 1000);
  }

  private void runAfterDelay(Runnable runnable, int delay) {

    timer.schedule(
        new TimerTask() {
          @Override
          public void run() {
            runnable.run();
          }
        }, delay);
  }

  private JSONObject standardInfo() throws JSONException {
    updateRecords();
    JSONObject report = new JSONObject();
    report.put("time", System.currentTimeMillis() - this.startTime);
    report.put("nativeAllocated", Debug.getNativeHeapAllocatedSize());
    boolean paused = allocationStartedAt == -1;
    if (paused) {
      report.put("paused", paused);
    }

    ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activityManager);
    report.put("availMem", memoryInfo.availMem);
    if (memoryInfo.lowMemory) {
      report.put("lowMemory", memoryInfo.lowMemory);
    }
    report.put("nativeAllocatedByTest", nativeAllocatedByTest);

    if (this.pids != null && !this.pids.isEmpty()) {
      if (false) {
        // Getting this info can make the app unresponsive for seconds.
        // Most of the values are strongly correlated with other info we can get more cheaply
        // anyway.
        getProcessMemoryInfo(report);
      }
      report.put("oom_score", getOomScore());
    }
    return report;
  }

  private int getOomScore()  {
    try {
      return Integer.parseInt(readFile(("/proc/" + this.pids.get(0)) + "/oom_score"));
    } catch (IOException | NumberFormatException e) {
      return 0;
    }
  }

  private void getProcessMemoryInfo(JSONObject report) throws JSONException {
    ActivityManager activityManager =
        (ActivityManager) Objects.requireNonNull(getSystemService(Context.ACTIVITY_SERVICE));
    int totalPss = 0;
    int totalPrivateDirty = 0;
    int totalSharedDirty = 0;
    for (Debug.MemoryInfo memoryInfo2 :
        activityManager.getProcessMemoryInfo(toIntArray(this.pids))) {

      totalPss += memoryInfo2.getTotalPss();
      totalPrivateDirty += memoryInfo2.getTotalPrivateDirty();
      totalSharedDirty += memoryInfo2.getTotalSharedDirty();
    }

    report.put("totalPss", totalPss);
    report.put("totalPrivateDirty", totalPrivateDirty);
    report.put("totalSharedDirty", totalSharedDirty);
  }

  public native void freeAll();

  public native boolean nativeConsume(int bytes);

  public native boolean tryAlloc(int bytes);
}
