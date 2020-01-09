package net.jimblackler.istresser;

import static net.jimblackler.istresser.Heuristic.Identifier.AVAIL;
import static net.jimblackler.istresser.Heuristic.Identifier.AVAIL2;
import static net.jimblackler.istresser.Heuristic.Identifier.CACHED;
import static net.jimblackler.istresser.Heuristic.Identifier.CL;
import static net.jimblackler.istresser.Heuristic.Identifier.LOW;
import static net.jimblackler.istresser.Heuristic.Identifier.OOM;
import static net.jimblackler.istresser.Heuristic.Identifier.TRIM;
import static net.jimblackler.istresser.Heuristic.Identifier.TRY;
import static net.jimblackler.istresser.ServiceCommunicationHelper.CRASHED_BEFORE;
import static net.jimblackler.istresser.ServiceCommunicationHelper.TOTAL_MEMORY_MB;

import android.app.ActivityManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Debug;
import android.os.Environment;
import android.provider.MediaStore;
import android.support.annotation.Nullable;
import android.support.v4.content.FileProvider;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import com.google.common.collect.ImmutableList;
import com.google.common.collect.Lists;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintStream;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Objects;
import java.util.Timer;
import java.util.TimerTask;
import net.jimblackler.istresser.Heuristic.Identifier;
import net.jimblackler.istresser.Heuristic.Indicator;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class MainActivity extends AppCompatActivity {

  /* Modify this variable when running the test locally */
  private static final int SCENARIO_NUMBER = 1;

  private static final String TAG = MainActivity.class.getSimpleName();

  // Set BYTES_PER_MILLISECOND to zero to disable malloc testing.
  public static final int BYTES_PER_MILLISECOND = 100 * 1024;

  // Set MAX_DURATION to zero to stop the app from self-exiting.
  private static final int MAX_DURATION = 1000 * 60 * 10;

  // Set LAUNCH_DURATION to a value in milliseconds to trigger the launch test.
  private static final int LAUNCH_DURATION = 0;
  private static final int RETURN_DURATION = LAUNCH_DURATION + 1000 * 20;
  private static final int MAX_SERVICE_MEMORY_MB = 500;
  private static final int BYTES_IN_MEGABYTE = 1024 * 1024;
  private static final boolean GL_TEST = true;

  private static final List<List<Identifier>> groups =
      ImmutableList.<List<Identifier>>builder()
          .add(ImmutableList.of())
          .add(ImmutableList.of(TRIM))
          .add(ImmutableList.of(OOM))
          .add(ImmutableList.of(LOW))
          .add(ImmutableList.of(TRY))
          .add(ImmutableList.of(CL))
          .add(ImmutableList.of(AVAIL))
          .add(ImmutableList.of(CACHED))
          .add(ImmutableList.of(AVAIL2))
          .add(ImmutableList.of(CL, AVAIL, AVAIL2))
          .add(ImmutableList.of(CL, AVAIL, AVAIL, LOW))
          .add(ImmutableList.of(TRIM, TRY, CL, AVAIL, AVAIL2))
          .add(ImmutableList.of(TRIM, LOW, TRY, CL, AVAIL, AVAIL2))
          .build();

  private static final ImmutableList<String> MEMINFO_FIELDS =
      ImmutableList.of("Cached", "MemFree", "MemAvailable", "SwapFree");

  static {
    System.loadLibrary("native-lib");
  }

  private final Timer timer = new Timer();
  private final List<byte[]> data = Lists.newArrayList();
  private long nativeAllocatedByTest;
  private long recordNativeHeapAllocatedSize;
  private PrintStream resultsStream = System.out;
  private long startTime;
  private long allocationStartedAt = -1;
  private int releases;
  private int scenario;
  private int groupNumber;
  private long appSwitchTimerStart;
  private long lastLaunched;
  private int serviceTotalMb = 0;
  private ServiceCommunicationHelper serviceCommunicationHelper;
  private boolean isServiceCrashed = false;

  enum ServiceState {
    ALLOCATING_MEMORY,
    ALLOCATED,
    FREEING_MEMORY,
    DEALLOCATED,
  }

  private ServiceState serviceState;

  private static String memoryString(long bytes) {
    return String.format(Locale.getDefault(), "%.1f MB", (float) bytes / (1024 * 1024));
  }

  private static String join(Iterable<String> strings, String separator) {
    StringBuilder stringBuilder = new StringBuilder();
    String useSeparator = "";
    for (String string : strings) {
      stringBuilder.append(useSeparator);
      stringBuilder.append(string);
      useSeparator = separator;
    }
    return stringBuilder.toString();
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    serviceCommunicationHelper = new ServiceCommunicationHelper(this);
    serviceState = ServiceState.DEALLOCATED;

    try {
      // Setting up broadcast receiver
      BroadcastReceiver receiver =
          new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
              isServiceCrashed = intent.getBooleanExtra(CRASHED_BEFORE, false);
              serviceTotalMb = intent.getIntExtra(TOTAL_MEMORY_MB, 0);
              switch (serviceState) {
                case ALLOCATING_MEMORY:
                  serviceState = ServiceState.ALLOCATED;

                  new Thread() {
                    @Override
                    public void run() {
                      try {
                        Thread.sleep(60 * 1000);
                      } catch (Exception e) {
                        e.printStackTrace();
                      }
                      serviceState = ServiceState.FREEING_MEMORY;
                      serviceCommunicationHelper.freeServerMemory();
                    }
                  }.start();

                  break;
                case FREEING_MEMORY:
                  serviceState = ServiceState.DEALLOCATED;

                  new Thread() {
                    @Override
                    public void run() {
                      try {
                        Thread.sleep(60 * 1000);
                      } catch (Exception e) {
                        e.printStackTrace();
                      }
                      serviceState = ServiceState.ALLOCATING_MEMORY;
                      serviceCommunicationHelper.allocateServerMemory(MAX_SERVICE_MEMORY_MB);
                    }
                  }.start();

                  break;
              }
            }
          };
      registerReceiver(receiver, new IntentFilter("com.google.gamesdk.grabber.RETURN"));

      JSONObject report = new JSONObject();

      if (!GL_TEST) {
        TestSurface testSurface = findViewById(R.id.glsurfaceView);
        testSurface.setVisibility(View.GONE);
      }

      PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(), 0);
      report.put("version", packageInfo.versionCode);
      Intent launchIntent = getIntent();
      if ("com.google.intent.action.TEST_LOOP".equals(launchIntent.getAction())) {
        Uri logFile = launchIntent.getData();
        if (logFile != null) {
          String logPath = logFile.getEncodedPath();
          if (logPath != null) {
            Log.i(TAG, "Log file " + logPath);
            try {
              resultsStream =
                  new PrintStream(
                      Objects.requireNonNull(getContentResolver().openOutputStream(logFile)));
            } catch (FileNotFoundException e) {
              throw new RuntimeException(e);
            }
          }
        }
        scenario = launchIntent.getIntExtra("scenario", 0);
      } else {
        scenario = SCENARIO_NUMBER;
      }

      groupNumber = (scenario - 1) % groups.size();
      if (scenario > groups.size() && scenario <= 2 * groups.size()) {
        serviceState = ServiceState.ALLOCATING_MEMORY;
        serviceTotalMb = 0;
        serviceCommunicationHelper.allocateServerMemory(MAX_SERVICE_MEMORY_MB);
      }

      report.put("scenario", scenario);

      JSONArray groupsOut = new JSONArray();
      for (Identifier identifier : groups.get(groupNumber)) {
        groupsOut.put(identifier.name());
      }
      TextView strategies = findViewById(R.id.strategies);
      strategies.setText(groupsOut.toString());

      report.put("groups", groupsOut);

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
      ActivityManager activityManager =
          (ActivityManager) Objects.requireNonNull(getSystemService((Context.ACTIVITY_SERVICE)));
      ActivityManager.MemoryInfo memoryInfo = Heuristics.getMemoryInfo(this);
      constant.put("totalMem", memoryInfo.totalMem);
      constant.put("threshold", memoryInfo.threshold);
      constant.put("memoryClass", activityManager.getMemoryClass() * 1024 * 1024);

      Long commitLimit = Heuristics.processMeminfo().get("CommitLimit");
      if (commitLimit != null) {
        constant.put("CommitLimit", commitLimit);
      }

      report.put("constant", constant);

      resultsStream.println(report);
    } catch (JSONException | PackageManager.NameNotFoundException e) {
      throw new RuntimeException(e);
    }

    startTime = System.currentTimeMillis();
    appSwitchTimerStart = System.currentTimeMillis();
    allocationStartedAt = startTime;
    timer.schedule(
        new TimerTask() {
          @Override
          public void run() {
            try {
              JSONObject report = standardInfo();
              long _allocationStartedAt = allocationStartedAt;
              if (_allocationStartedAt != -1) {
                boolean memoryReleased = false;
                for (Heuristic heuristic : Heuristic.availableHeuristics) {
                  if (groups.get(groupNumber).contains(heuristic.getIdentifier())
                      && heuristic.getSignal(MainActivity.this) == Indicator.RED) {
                    releaseMemory(report, heuristic.getIdentifier().toString());
                    memoryReleased = true;
                    break;
                  }
                }
                if (!memoryReleased) {
                  long sinceStart = System.currentTimeMillis() - _allocationStartedAt;
                  long target = sinceStart * BYTES_PER_MILLISECOND;
                  int owed = (int) (target - nativeAllocatedByTest);
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
              if (LAUNCH_DURATION != 0) {
                long appSwitchTimeRunning = System.currentTimeMillis() - appSwitchTimerStart;
                if (appSwitchTimeRunning > LAUNCH_DURATION && lastLaunched < LAUNCH_DURATION) {
                  lastLaunched = appSwitchTimeRunning;
                  Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
                  File file =
                      new File(
                          Environment.getExternalStoragePublicDirectory(
                                  Environment.DIRECTORY_PICTURES)
                              + File.separator
                              + "pic.jpg");
                  String authority = getApplicationContext().getPackageName() + ".provider";
                  Uri uriForFile = FileProvider.getUriForFile(MainActivity.this, authority, file);
                  intent.putExtra(MediaStore.EXTRA_OUTPUT, uriForFile);
                  startActivityForResult(intent, 1);
                }
                if (appSwitchTimeRunning > RETURN_DURATION && lastLaunched < RETURN_DURATION) {
                  lastLaunched = appSwitchTimeRunning;
                  finishActivity(1);
                  appSwitchTimerStart = System.currentTimeMillis();
                  lastLaunched = 0;
                }
              }
              if (timeRunning > MAX_DURATION) {
                try {
                  report = standardInfo();
                  report.put("exiting", true);
                  resultsStream.println(report);
                } catch (JSONException e) {
                  throw new RuntimeException(e);
                }
              }

              resultsStream.println(report);

              if (timeRunning > MAX_DURATION) {
                resultsStream.close();
                finish();
              }
              updateInfo();
            } catch (JSONException e) {
              throw new RuntimeException(e);
            }
          }
        },
        0,
        1000 / 10);
  }

  @Override
  protected void onDestroy() {
    try {
      JSONObject report = standardInfo();
      report.put("onDestroy", true);
      resultsStream.println(report);
    } catch (JSONException e) {
      throw new RuntimeException(e);
    }
    super.onDestroy();
  }

  @Override
  protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
    super.onActivityResult(requestCode, resultCode, data);
  }

  @Override
  protected void onPause() {
    super.onPause();
    try {
      JSONObject report = standardInfo();
      report.put("activityPaused", true);
      resultsStream.println(report);
    } catch (JSONException e) {
      throw new RuntimeException(e);
    }
  }

  @Override
  protected void onResume() {
    super.onResume();
    try {
      JSONObject report = standardInfo();
      report.put("activityPaused", false);
      resultsStream.println(report);
    } catch (JSONException e) {
      throw new RuntimeException(e);
    }
  }

  private void updateRecords() {
    long nativeHeapAllocatedSize = Debug.getNativeHeapAllocatedSize();
    if (nativeHeapAllocatedSize > recordNativeHeapAllocatedSize) {
      recordNativeHeapAllocatedSize = nativeHeapAllocatedSize;
    }
  }

  private void updateInfo() {
    runOnUiThread(
        () -> {
          updateRecords();

          TextView uptime = findViewById(R.id.uptime);
          float timeRunning = (float) (System.currentTimeMillis() - startTime) / 1000;
          uptime.setText(String.format(Locale.getDefault(), "%.2f", timeRunning));

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

          ActivityManager.MemoryInfo memoryInfo = Heuristics.getMemoryInfo(this);
          TextView availMemTextView = findViewById(R.id.availMem);
          availMemTextView.setText(memoryString(memoryInfo.availMem));

          TextView oomScoreTextView = findViewById(R.id.oomScore);
          oomScoreTextView.setText("" + Heuristics.getOomScore(this));

          TextView lowMemoryTextView = findViewById(R.id.lowMemory);
          lowMemoryTextView.setText(Boolean.valueOf(Heuristics.lowMemoryCheck(this)).toString());

          TextView trimMemoryComplete = findViewById(R.id.releases);
          trimMemoryComplete.setText(String.format(Locale.getDefault(), "%d", releases));
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
      throw new RuntimeException(e);
    }
  }

  @Override
  public void onTrimMemory(int level) {

    try {
      JSONObject report = standardInfo();
      report.put("onTrimMemory", level);

      if (groups.get(groupNumber).contains(TRIM)) {
        releaseMemory(report, TRIM.name());
      }
      resultsStream.println(report);

      updateInfo();
      super.onTrimMemory(level);
    } catch (JSONException e) {
      throw new RuntimeException(e);
    }
  }

  private void releaseMemory(JSONObject report, String trigger) {
    try {
      report.put("trigger", trigger);
    } catch (JSONException e) {
      throw new RuntimeException(e);
    }
    allocationStartedAt = -1;
    runAfterDelay(
        () -> {
          JSONObject report2;
          try {
            report2 = standardInfo();
          } catch (JSONException e) {
            throw new RuntimeException(e);
          }
          resultsStream.println(report2);
          if (nativeAllocatedByTest > 0) {
            nativeAllocatedByTest = 0;
            releases++;
            freeAll();
          }
          data.clear();
          if (GL_TEST) {
            TestSurface testSurface = findViewById(R.id.glsurfaceView);
            if (testSurface != null) {
              testSurface.queueEvent(
                  () -> {
                    TestRenderer renderer = testSurface.getRenderer();
                    renderer.release();
                  });
            }
          }
          runAfterDelay(
              () -> {
                try {
                  resultsStream.println(standardInfo());
                  allocationStartedAt = System.currentTimeMillis();
                } catch (JSONException e) {
                  throw new RuntimeException(e);
                }
              },
              1000);
        },
        1000);
  }

  private void runAfterDelay(Runnable runnable, int delay) {

    timer.schedule(
        new TimerTask() {
          @Override
          public void run() {
            runnable.run();
          }
        },
        delay);
  }

  private JSONObject standardInfo() throws JSONException {
    updateRecords();
    JSONObject report = new JSONObject();
    report.put("time", System.currentTimeMillis() - startTime);
    report.put("nativeAllocated", Debug.getNativeHeapAllocatedSize());
    boolean paused = allocationStartedAt == -1;
    if (paused) {
      report.put("paused", true);
    }

    ActivityManager.MemoryInfo memoryInfo = Heuristics.getMemoryInfo(this);
    report.put("availMem", memoryInfo.availMem);
    boolean lowMemory = Heuristics.lowMemoryCheck(this);
    if (lowMemory) {
      report.put("lowMemory", true);
    }
    if (isServiceCrashed) {
      report.put("serviceCrashed", true);
    }
    report.put("nativeAllocatedByTest", nativeAllocatedByTest);
    report.put("serviceTotalMemory", BYTES_IN_MEGABYTE * serviceTotalMb);
    report.put("oom_score", Heuristics.getOomScore(this));

    Map<String, Long> values = Heuristics.processMeminfo();
    for (Map.Entry<String, Long> pair : values.entrySet()) {
      String key = pair.getKey();
      if (MEMINFO_FIELDS.contains(key)) {
        report.put(key, pair.getValue());
      }
    }
    return report;
  }

  public static native void initGl();

  public static native void nativeDraw();

  public static native void release();

  public native void freeAll();

  public native boolean nativeConsume(int bytes);
}
