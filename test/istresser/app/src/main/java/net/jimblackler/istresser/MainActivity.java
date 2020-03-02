package net.jimblackler.istresser;

import static net.jimblackler.istresser.Heuristic.Identifier.TRIM;
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
import android.os.Build.VERSION;
import android.os.Build.VERSION_CODES;
import android.os.Bundle;
import android.os.Debug;
import android.os.Debug.MemoryInfo;
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
import java.io.IOException;
import java.io.PrintStream;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Objects;
import java.util.Timer;
import java.util.TimerTask;
import java.util.TreeSet;
import net.jimblackler.istresser.Heuristic.Identifier;
import net.jimblackler.istresser.Heuristic.Indicator;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class MainActivity extends AppCompatActivity {

  private static final String TAG = MainActivity.class.getSimpleName();

  public static final int BYTES_PER_MILLISECOND = 100 * 1024;

  // Set MAX_DURATION to zero to stop the app from self-exiting.
  private static final int MAX_DURATION = 1000 * 60 * 10;

  // Set LAUNCH_DURATION to a value in milliseconds to trigger the launch test.
  private static final int LAUNCH_DURATION = 0;
  private static final int RETURN_DURATION = LAUNCH_DURATION + 1000 * 20;
  private static final int BACKGROUND_MEMORY_PRESSURE_MB = 500;
  private static final int BACKGROUND_PRESSURE_PERIOD_SECONDS = 30;
  private static final int BYTES_IN_MEGABYTE = 1024 * 1024;
  private static final int MEMORY_TO_FREE_PER_CYCLE_MB = 500;
  private static final int MMAP_ANON_BLOCK_BYTES = 2 * BYTES_IN_MEGABYTE;
  private static final int MMAP_FILE_BLOCK_BYTES = 2 * BYTES_IN_MEGABYTE;

  private static final String MEMORY_BLOCKER = "MemoryBlockCommand";
  private static final String ALLOCATE_ACTION = "Allocate";
  private static final String FREE_ACTION = "Free";

  private final Collection<Identifier> activeGroups = new TreeSet<>();

  private static final ImmutableList<String> MEMINFO_FIELDS =
      ImmutableList.of("Cached", "MemFree", "MemAvailable", "SwapFree");

  private static final ImmutableList<String> STATUS_FIELDS =
      ImmutableList.of("VmSize", "VmRSS", "VmData");

  private static final String[] SUMMARY_FIELDS = {
    "summary.graphics", "summary.native-heap", "summary.total-pss"
  };

  static {
    System.loadLibrary("native-lib");
  }

  private final Timer timer = new Timer();
  private final List<byte[]> data = Lists.newArrayList();
  private long nativeAllocatedByTest;
  private long mmapAnonAllocatedByTest;
  private long mmapFileAllocatedByTest;
  private long recordNativeHeapAllocatedSize;
  private PrintStream resultsStream = System.out;
  private long startTime;
  private long latestAllocationTime = -1;
  private int releases;
  private long appSwitchTimerStart;
  private long lastLaunched;
  private int serviceTotalMb = 0;
  private ServiceCommunicationHelper serviceCommunicationHelper;
  private boolean isServiceCrashed = false;
  private int mallocKbPerMillisecond;
  private int glAllocKbPerMillisecond;
  private List<Heuristic> activeHeuristics = new ArrayList<>();
  private boolean yellowLightTesting = false;
  private int mmapAnonKbPerMillisecond;
  private int mmapFileKbPerMillisecond;
  private MmapFileGroup mmapFiles;

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

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    Intent launchIntent = getIntent();

    JSONObject params1 = null;
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
      try {
        params1 = new JSONObject(Utils.readFile("/sdcard/params.json"));
      } catch (IOException | JSONException e) {
        throw new RuntimeException(e);
      }
    }

    if (params1 == null) {
      try {
        params1 = new JSONObject(Utils.readStream(getAssets().open("default.json")));
      } catch (IOException | JSONException e2) {
        throw new RuntimeException(e2);
      }
    }

    JSONObject params = flattenParams(params1);
    setContentView(R.layout.activity_main);
    serviceCommunicationHelper = new ServiceCommunicationHelper(this);
    serviceState = ServiceState.DEALLOCATED;
    ActivityManager activityManager =
        (ActivityManager) Objects.requireNonNull(getSystemService((Context.ACTIVITY_SERVICE)));

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
                        Thread.sleep(BACKGROUND_PRESSURE_PERIOD_SECONDS * 1000);
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
                        Thread.sleep(BACKGROUND_PRESSURE_PERIOD_SECONDS * 1000);
                      } catch (Exception e) {
                        e.printStackTrace();
                      }
                      serviceState = ServiceState.ALLOCATING_MEMORY;
                      serviceCommunicationHelper.allocateServerMemory(
                          BACKGROUND_MEMORY_PRESSURE_MB);
                    }
                  }.start();

                  break;
              }
            }
          };
      registerReceiver(receiver, new IntentFilter("com.google.gamesdk.grabber.RETURN"));

      JSONObject report = new JSONObject();
      report.put("params", params1);

      TestSurface testSurface = findViewById(R.id.glsurfaceView);
      testSurface.setVisibility(View.GONE);

      PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(), 0);
      report.put("version", packageInfo.versionCode);

      yellowLightTesting = params.optBoolean("yellowLightTesting");

      if (params.optBoolean("serviceBlocker")) {
        activateServiceBlocker();
      }

      if (params.optBoolean("firebaseBlocker")) {
        activateFirebaseBlocker();
      }

      mallocKbPerMillisecond = params.optInt("malloc");

      glAllocKbPerMillisecond = params.optInt("glTest");

      if (glAllocKbPerMillisecond > 0) {
        testSurface.setVisibility(View.VISIBLE);
      }

      mmapAnonKbPerMillisecond = params.optInt("mmapAnon");

      mmapFileKbPerMillisecond = params.optInt("mmapFile", -1);
      if (mmapFileKbPerMillisecond > 0) {
        // Other possible directories:
        // * getExternalCacheDir()
        // * getExternalFilesDir(null)
        // * getFilesDir()
        String mmapPath = getApplicationContext().getNoBackupFilesDir().toString();
        JSONArray mmapFileNames = params.getJSONArray("mmapFileNames");
        mmapFiles = new MmapFileGroup(mmapPath, mmapFileNames);
      }

      if (params.has("heuristics")) {
        JSONArray heuristics = params.getJSONArray("heuristics");
        for (int idx = 0; idx != heuristics.length(); idx++) {
          try {
            activeGroups.add(Enum.valueOf(Identifier.class, heuristics.getString(idx)));
          } catch (IllegalArgumentException e) {
            Log.e(TAG, "Enum error", e);
          }
        }
      }

      TextView strategies = findViewById(R.id.strategies);
      strategies.setText(activeGroups.toString());

//      report.put("groups", groupsOut);
//      report.put("mallocTest", mallocTestActive);
//      report.put("glTest", glTestActive);
//      report.put("mmapAnonTest", mmapAnonTestActive);
//      report.put("mmapFileTest", mmapFileTestActive);

      JSONObject build = new JSONObject();
      getStaticFields(build, Build.class);
      getStaticFields(build, Build.VERSION.class);
      report.put("build", build);

      JSONObject constant = new JSONObject();
      ActivityManager.MemoryInfo memoryInfo = Heuristics.getMemoryInfo(activityManager);
      constant.put("totalMem", memoryInfo.totalMem);
      constant.put("threshold", memoryInfo.threshold);
      constant.put("memoryClass", activityManager.getMemoryClass() * 1024 * 1024);

      Long commitLimit = Heuristics.processMeminfo().get("CommitLimit");
      if (commitLimit != null) {
        constant.put("CommitLimit", commitLimit);
      }

      report.put("constant", constant);

      resultsStream.println(report);
    } catch (IOException | JSONException | PackageManager.NameNotFoundException e) {
      throw new RuntimeException(e);
    }

    startTime = System.currentTimeMillis();
    appSwitchTimerStart = System.currentTimeMillis();
    latestAllocationTime = startTime;
    for (Heuristic heuristic : Heuristic.availableHeuristics) {
      if (activeGroups.contains((heuristic.getIdentifier()))) {
        activeHeuristics.add(heuristic);
      }
    }
    timer.schedule(
        new TimerTask() {
          @Override
          public void run() {
            try {
              JSONObject report = standardInfo();
              long _latestAllocationTime = latestAllocationTime;
              if (_latestAllocationTime != -1) {
                boolean shouldAllocate = true;
                Indicator maxMemoryPressureLevel = Indicator.GREEN;
                Identifier triggeringHeuristic = null;
                for (Heuristic heuristic : activeHeuristics) {
                  Indicator heuristicMemoryPressureLevel = heuristic.getSignal(activityManager);
                  if (heuristicMemoryPressureLevel == Indicator.RED) {
                    maxMemoryPressureLevel = Indicator.RED;
                    triggeringHeuristic = heuristic.getIdentifier();
                    break;
                  } else if (heuristicMemoryPressureLevel == Indicator.YELLOW) {
                    maxMemoryPressureLevel = Indicator.YELLOW;
                    triggeringHeuristic = heuristic.getIdentifier();
                  }
                }
                if (yellowLightTesting) {
                  if (maxMemoryPressureLevel == Indicator.RED) {
                    shouldAllocate = false;
                    freeMemory(MEMORY_TO_FREE_PER_CYCLE_MB * BYTES_IN_MEGABYTE);
                    latestAllocationTime = System.currentTimeMillis();
                  } else if (maxMemoryPressureLevel == Indicator.YELLOW) {
                    shouldAllocate = false;
                    // Allocating 0 MB
                    latestAllocationTime = System.currentTimeMillis();
                  }
                } else {
                  if (maxMemoryPressureLevel == Indicator.RED) {
                    shouldAllocate = false;
                    releaseMemory(report, triggeringHeuristic.toString());
                  }
                }

                if (mallocKbPerMillisecond > 0 && shouldAllocate) {
                  long sinceLastAllocation = System.currentTimeMillis() - _latestAllocationTime;
                  int owed = (int) (sinceLastAllocation * mallocKbPerMillisecond * 1024);
                  if (owed > 0) {
                    boolean succeeded = nativeConsume(owed);
                    if (succeeded) {
                      nativeAllocatedByTest += owed;
                      latestAllocationTime = System.currentTimeMillis();
                    } else {
                      report.put("allocFailed", true);
                    }
                  }
                }
                if (glAllocKbPerMillisecond > 0 && shouldAllocate) {
                  long sinceLastAllocation = System.currentTimeMillis() - _latestAllocationTime;
                  long target = sinceLastAllocation * glAllocKbPerMillisecond * 1024;
                  TestSurface testSurface = findViewById(R.id.glsurfaceView);
                  testSurface.getRenderer().setTarget(target);
                }

                if (mmapAnonKbPerMillisecond > 0) {
                  long sinceLastAllocation = System.currentTimeMillis() - _latestAllocationTime;
                  int owed = (int) (sinceLastAllocation * mmapAnonKbPerMillisecond * 1024);
                  if (owed > MMAP_ANON_BLOCK_BYTES) {
                    long allocated = mmapAnonConsume(owed);
                    if (allocated != 0) {
                      mmapAnonAllocatedByTest += allocated;
                      latestAllocationTime = System.currentTimeMillis();
                    } else {
                      report.put("mmapAnonFailed", true);
                    }
                  }
                }
                if (mmapFileKbPerMillisecond > 0) {
                  long sinceLastAllocation = System.currentTimeMillis() - _latestAllocationTime;
                  int owed = (int) (sinceLastAllocation * mmapFileKbPerMillisecond * 1024);
                  if (owed > MMAP_FILE_BLOCK_BYTES) {
                    MmapFileInfo file = mmapFiles.alloc(owed);
                    long allocated = mmapFileConsume(file.path, file.alloc_size, file.offset);
                    if (allocated != 0) {
                      mmapFileAllocatedByTest += allocated;
                      latestAllocationTime = System.currentTimeMillis();
                    } else {
                      report.put("mmapFileFailed", true);
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

  private static JSONObject flattenParams(JSONObject params1) {
    JSONObject params = new JSONObject();
    try {
      JSONArray coordinates = params1.getJSONArray("coordinates");
      JSONArray tests = params1.getJSONArray("tests");

      for (int coordinateNumber = 0; coordinateNumber != coordinates.length(); coordinateNumber++) {
        JSONArray jsonArray = tests.getJSONArray(coordinateNumber);
        JSONObject jsonObject = jsonArray.getJSONObject(coordinates.getInt(coordinateNumber));
        Iterator<String> keys = jsonObject.keys();
        while (keys.hasNext()) {
          String key = keys.next();
          params.put(key, jsonObject.get(key));
        }
      }
    } catch (JSONException e) {
      throw new RuntimeException(e);
    }
    return params;
  }

  private void activateServiceBlocker() {
    serviceState = ServiceState.ALLOCATING_MEMORY;
    serviceTotalMb = 0;
    serviceCommunicationHelper.allocateServerMemory(BACKGROUND_MEMORY_PRESSURE_MB);
  }

  private void activateFirebaseBlocker() {
    new Thread() {
      @Override
      public void run() {
        Log.v(MEMORY_BLOCKER, FREE_ACTION);
        while (true) {
          Log.v(MEMORY_BLOCKER, ALLOCATE_ACTION + " " + BACKGROUND_MEMORY_PRESSURE_MB);
          serviceTotalMb = BACKGROUND_MEMORY_PRESSURE_MB;
          try {
            Thread.sleep(BACKGROUND_PRESSURE_PERIOD_SECONDS * 1000);
          } catch (Exception e) {
            e.printStackTrace();
          }
          Log.v(MEMORY_BLOCKER, FREE_ACTION);
          serviceTotalMb = 0;
          try {
            Thread.sleep(BACKGROUND_PRESSURE_PERIOD_SECONDS * 1000);
          } catch (Exception e) {
            e.printStackTrace();
          }
        }
      }
    }.start();
  }

  private static void getStaticFields(JSONObject object, Class<?> aClass) throws JSONException {

    for (Field field : aClass.getFields()) {
      if (!java.lang.reflect.Modifier.isStatic(field.getModifiers())) {
        continue;
      }
      try {
        object.put(field.getName(), JSONObject.wrap(field.get(null)));
      } catch (IllegalAccessException e) {
        // Silent by design.
      }
    }
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

          ActivityManager activityManager =
              (ActivityManager) Objects.requireNonNull(this.getSystemService((ACTIVITY_SERVICE)));

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

          TextView mmapAnonAllocatedByTestTextView = findViewById(R.id.mmapAnonAllocatedByTest);
          mmapAnonAllocatedByTestTextView.setText(memoryString(mmapAnonAllocatedByTest));

          TextView mmapFileAllocatedByTestTextView = findViewById(R.id.mmapFileAllocatedByTest);
          mmapFileAllocatedByTestTextView.setText(memoryString(mmapFileAllocatedByTest));

          ActivityManager.MemoryInfo memoryInfo = Heuristics.getMemoryInfo(activityManager);
          TextView availMemTextView = findViewById(R.id.availMem);
          availMemTextView.setText(memoryString(memoryInfo.availMem));

          TextView oomScoreTextView = findViewById(R.id.oomScore);
          oomScoreTextView.setText("" + Heuristics.getOomScore(activityManager));

          TextView lowMemoryTextView = findViewById(R.id.lowMemory);
          lowMemoryTextView.setText(
              Boolean.valueOf(Heuristics.lowMemoryCheck(activityManager)).toString());

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

      if (activeGroups.contains(TRIM)) {
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
    latestAllocationTime = -1;
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
          if (glAllocKbPerMillisecond > 0) {
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
                  latestAllocationTime = System.currentTimeMillis();
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
    boolean paused = latestAllocationTime == -1;
    if (paused) {
      report.put("paused", true);
    }

    ActivityManager activityManager =
        (ActivityManager) Objects.requireNonNull(getSystemService((ACTIVITY_SERVICE)));
    ActivityManager.MemoryInfo memoryInfo = Heuristics.getMemoryInfo(activityManager);
    report.put("availMem", memoryInfo.availMem);
    boolean lowMemory = Heuristics.lowMemoryCheck(activityManager);
    if (lowMemory) {
      report.put("lowMemory", true);
    }
    if (isServiceCrashed) {
      report.put("serviceCrashed", true);
    }
    report.put("nativeAllocatedByTest", nativeAllocatedByTest);
    report.put("mmapAnonAllocatedByTest", mmapAnonAllocatedByTest);
    report.put("serviceTotalMemory", BYTES_IN_MEGABYTE * serviceTotalMb);
    report.put("oom_score", Heuristics.getOomScore(activityManager));

    TestSurface testSurface = findViewById(R.id.glsurfaceView);
    TestRenderer renderer = testSurface.getRenderer();
    report.put("gl_allocated", renderer.getAllocated());

    if (VERSION.SDK_INT >= VERSION_CODES.M) {
      MemoryInfo debugMemoryInfo = Heuristics.getDebugMemoryInfo(activityManager)[0];
      for (String key : SUMMARY_FIELDS) {
        report.put(key, debugMemoryInfo.getMemoryStat(key));
      }
    }

    Map<String, Long> values = Heuristics.processMeminfo();
    for (Map.Entry<String, Long> pair : values.entrySet()) {
      String key = pair.getKey();
      if (MEMINFO_FIELDS.contains(key)) {
        report.put(key, pair.getValue());
      }
    }

    for (Map.Entry<String, Long> pair : Heuristics.processStatus(activityManager).entrySet()) {
      String key = pair.getKey();
      if (STATUS_FIELDS.contains(key)) {
        report.put(key, pair.getValue());
      }
    }
    return report;
  }

  public static native void initGl();

  public static native int nativeDraw(long toAllocate);

  public static native void release();

  public native void freeAll();

  public native void freeMemory(int bytes);

  public native boolean nativeConsume(int bytes);

  public native long mmapAnonConsume(int bytes);

  public native long mmapFileConsume(String path, int bytes, int offset);

  static class MmapFileInfo {
    private long file_size;
    private int alloc_size;
    private String path;
    private int offset;

    public MmapFileInfo(String path) throws IOException {
      this.path = path;
      file_size = Utils.getFileSize(path);
      offset = 0;
    }

    public boolean alloc(int desiredSize) {
      offset += alloc_size;
      int limit = (int) (file_size - offset);
      alloc_size = desiredSize > limit ? limit : desiredSize;
      return alloc_size > 0;
    }

    public void reset() {
      offset = 0;
      alloc_size = 0;
    }

    public String getPath() { return path; }

    public int getOffset() { return offset; }

    public int getAllocSize() { return alloc_size; }
  }

  static class MmapFileGroup {
    private ArrayList<MmapFileInfo> files = new ArrayList<>();
    private int current;

    public MmapFileGroup(String mmapPath, JSONArray mmapFileNames)
        throws JSONException, IOException {
      for (int i = 0; i < mmapFileNames.length(); ++i) {
        files.add(new MmapFileInfo(mmapPath + "/" + mmapFileNames.get(i)));
      }
    }

    public MmapFileInfo alloc(int desiredSize) {
      while (!files.get(current).alloc(desiredSize)) {
        current = (current + 1) % files.size();
        files.get(current).reset();
      }
      return files.get(current);
    }
  }
}
