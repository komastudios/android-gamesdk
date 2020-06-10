package net.jimblackler.istresser;

import static com.google.android.apps.internal.games.helperlibrary.Helper.getBuild;
import static net.jimblackler.istresser.ServiceCommunicationHelper.CRASHED_BEFORE;
import static net.jimblackler.istresser.ServiceCommunicationHelper.TOTAL_MEMORY_MB;
import static net.jimblackler.istresser.Utils.flattenParams;
import static net.jimblackler.istresser.Utils.getFileSize;
import static net.jimblackler.istresser.Utils.getMemoryQuantity;
import static com.google.android.apps.internal.games.helperlibrary.Utils.getMemoryInfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.getOomScore;
import static com.google.android.apps.internal.games.helperlibrary.Utils.lowMemoryCheck;
import static com.google.android.apps.internal.games.helperlibrary.Utils.processMeminfo;
import static com.google.android.apps.internal.games.helperlibrary.Utils.readFile;
import static com.google.android.apps.internal.games.helperlibrary.Utils.readStream;

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
import android.webkit.WebView;

import com.google.android.apps.internal.games.helperlibrary.Info;
import com.google.common.collect.Lists;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.Objects;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.atomic.AtomicBoolean;

import net.jimblackler.istresser.Heuristic.Indicator;
import org.apache.commons.io.FileUtils;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/** The main activity of the istresser app */
public class MainActivity extends AppCompatActivity {

  public static final int DELAY_AFTER_RELEASE = 1000;
  private static final String TAG = MainActivity.class.getSimpleName();

  // Set MAX_DURATION to zero to stop the app from self-exiting.
  private static final int MAX_DURATION = 1000 * 60 * 10;

  // Set LAUNCH_DURATION to a value in milliseconds to trigger the launch test.
  private static final int LAUNCH_DURATION = 0;
  private static final int RETURN_DURATION = LAUNCH_DURATION + 1000 * 20;
  private static final int BACKGROUND_MEMORY_PRESSURE_MB = 500;
  private static final int BACKGROUND_PRESSURE_PERIOD_SECONDS = 30;
  private static final int BYTES_IN_MEGABYTE = 1024 * 1024;
  private static final int MMAP_ANON_BLOCK_BYTES = 2 * BYTES_IN_MEGABYTE;
  private static final int MMAP_FILE_BLOCK_BYTES = 2 * BYTES_IN_MEGABYTE;
  private static final int MEMORY_TO_FREE_PER_CYCLE_MB = 500;

  private static final String MEMORY_BLOCKER = "MemoryBlockCommand";
  private static final String ALLOCATE_ACTION = "Allocate";
  private static final String FREE_ACTION = "Free";

  static {
    System.loadLibrary("native-lib");
  }

  private final Timer timer = new Timer();
  private final List<byte[]> data = Lists.newArrayList();
  private JSONObject deviceSettings;
  private long nativeAllocatedByTest;
  private long mmapAnonAllocatedByTest;
  private long mmapFileAllocatedByTest;
  private long recordNativeHeapAllocatedSize;
  private PrintStream resultsStream = System.out;
  private final Info info = new Info();
  private long latestAllocationTime = -1;
  private int releases;
  private long appSwitchTimerStart;
  private long lastLaunched;
  private int serviceTotalMb = 0;
  private ServiceCommunicationHelper serviceCommunicationHelper;
  private boolean isServiceCrashed = false;
  private long mallocBytesPerMillisecond;
  private long glAllocBytesPerMillisecond;
  private JSONObject params;
  private boolean yellowLightTesting = false;
  private long mmapAnonBytesPerMillisecond;
  private long mmapFileBytesPerMillisecond;
  private MmapFileGroup mmapFiles;
  private long maxConsumer;

  enum ServiceState {
    ALLOCATING_MEMORY,
    ALLOCATED,
    FREEING_MEMORY,
    DEALLOCATED,
  }

  private ServiceState serviceState;

  private static String memoryString(long bytes) {
    return String.format(Locale.getDefault(), "%.1f MB", (float) bytes / BYTES_IN_MEGABYTE);
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    initNative();

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
            throw new IllegalStateException(e);
          }
        }
      }
      try {
        params1 = new JSONObject(readFile("/sdcard/params.json"));
      } catch (IOException | JSONException e) {
        throw new IllegalStateException(e);
      }
    }

    if (params1 == null) {
      try {
        params1 = new JSONObject(readStream(getAssets().open("default.json")));
      } catch (IOException | JSONException e) {
        throw new IllegalStateException(e);
      }
    }

    params = flattenParams(params1);
    deviceSettings= getDeviceSettings();
    setContentView(R.layout.activity_main);
    serviceCommunicationHelper = new ServiceCommunicationHelper(this);
    serviceState = ServiceState.DEALLOCATED;
    ActivityManager activityManager =
        (ActivityManager) Objects.requireNonNull(getSystemService(Context.ACTIVITY_SERVICE));

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
                        if (e instanceof InterruptedException) {
                          Thread.currentThread().interrupt();
                        }
                        throw new IllegalStateException(e);
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
                        if (e instanceof InterruptedException) {
                          Thread.currentThread().interrupt();
                        }
                        throw new IllegalStateException(e);
                      }
                      serviceState = ServiceState.ALLOCATING_MEMORY;
                      serviceCommunicationHelper.allocateServerMemory(
                          BACKGROUND_MEMORY_PRESSURE_MB);
                    }
                  }.start();

                  break;
                default:
              }
            }
          };
      registerReceiver(receiver, new IntentFilter("com.google.gamesdk.grabber.RETURN"));

      JSONObject report = new JSONObject();
      report.put("params", params1);
      report.put("settings", deviceSettings);

      TestSurface testSurface = findViewById(R.id.glsurfaceView);
      testSurface.setVisibility(View.GONE);
      maxConsumer = getMemoryQuantity(params, "maxConsumer");
      testSurface.getRenderer().setMaxConsumerSize(maxConsumer);

      PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(), 0);
      report.put("version", packageInfo.versionCode);

      yellowLightTesting = params.optBoolean("yellowLightTesting");

      if (params.optBoolean("serviceBlocker")) {
        activateServiceBlocker();
      }

      if (params.optBoolean("firebaseBlocker")) {
        activateFirebaseBlocker();
      }

      mallocBytesPerMillisecond = getMemoryQuantity(params, "malloc");

      glAllocBytesPerMillisecond = getMemoryQuantity(params, "glTest");
      if (mallocBytesPerMillisecond == 0) {
        nativeConsume((int) Utils.getMemoryQuantity(params, "mallocFixed"));
      }

      if (glAllocBytesPerMillisecond > 0) {
        testSurface.setVisibility(View.VISIBLE);
      } else {
        long glFixed = getMemoryQuantity(params, "glFixed");
        if (glFixed > 0) {
          testSurface.setVisibility(View.VISIBLE);
          testSurface.getRenderer().setTarget(glFixed);
        }
      }

      mmapAnonBytesPerMillisecond = getMemoryQuantity(params, "mmapAnon");

      mmapFileBytesPerMillisecond = getMemoryQuantity(params, "mmapFile");
      if (mmapFileBytesPerMillisecond > 0) {
        String mmapPath = getApplicationContext().getCacheDir().toString();
        int mmapFileCount = params.getInt("mmapFileCount");
        long mmapFileSize = getMemoryQuantity(params, "mmapFileSize");
        mmapFiles = new MmapFileGroup(mmapPath, mmapFileCount, mmapFileSize);
      }

      report.put("build", getBuild());

      JSONObject constant = new JSONObject();
      ActivityManager.MemoryInfo memoryInfo = getMemoryInfo(activityManager);
      constant.put("totalMem", memoryInfo.totalMem);
      constant.put("threshold", memoryInfo.threshold);
      constant.put("memoryClass", activityManager.getMemoryClass() * BYTES_IN_MEGABYTE);

      Long commitLimit = processMeminfo().get("CommitLimit");
      if (commitLimit != null) {
        constant.put("CommitLimit", commitLimit);
      }

      report.put("constant", constant);

      resultsStream.println(report);
    } catch (IOException | JSONException | PackageManager.NameNotFoundException e) {
      throw new IllegalStateException(e);
    }

    appSwitchTimerStart = System.currentTimeMillis();
    latestAllocationTime = info.getStartTime();

    timer.schedule(
        new TimerTask() {
          @Override
          public void run() {
            try {
              JSONObject report = standardInfo();
              long sinceLastAllocation = System.currentTimeMillis() - latestAllocationTime;
              if (sinceLastAllocation > 0) {
                boolean shouldAllocate = true;
                Indicator[] maxMemoryPressureLevel = {Indicator.GREEN};
                String[] triggeringHeuristic = {null};

                Heuristic.checkHeuristics(activityManager, params, deviceSettings,
                    (heuristic, heuristicMemoryPressureLevel) -> {
                      if (heuristicMemoryPressureLevel == Indicator.GREEN) {
                        // GREEN heuristic does nothing here.
                        return;
                      }
                      if (maxMemoryPressureLevel[0] == Indicator.RED) {
                        // RED cannot be overwritten.
                        return;
                      }
                      triggeringHeuristic[0] = heuristic;
                      maxMemoryPressureLevel[0] = heuristicMemoryPressureLevel;
                  });

                if (yellowLightTesting) {
                  if (maxMemoryPressureLevel[0] == Indicator.RED) {
                    shouldAllocate = false;
                    freeMemory(MEMORY_TO_FREE_PER_CYCLE_MB * BYTES_IN_MEGABYTE);
                  } else if (maxMemoryPressureLevel[0] == Indicator.YELLOW) {
                    shouldAllocate = false;
                    // Allocating 0 MB
                  }
                  latestAllocationTime = System.currentTimeMillis();
                } else {
                  if (maxMemoryPressureLevel[0] == Indicator.RED) {
                    shouldAllocate = false;
                    releaseMemory(report, triggeringHeuristic[0]);
                  }
                }

                if (mallocBytesPerMillisecond > 0 && shouldAllocate) {
                  int owed = (int) (sinceLastAllocation * mallocBytesPerMillisecond);
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
                if (glAllocBytesPerMillisecond > 0 && shouldAllocate) {
                  long target = sinceLastAllocation * glAllocBytesPerMillisecond;
                  TestSurface testSurface = findViewById(R.id.glsurfaceView);
                  testSurface.getRenderer().setTarget(target);
                }

                if (mmapAnonBytesPerMillisecond > 0) {
                  long owed = sinceLastAllocation * mmapAnonBytesPerMillisecond;
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
                if (mmapFileBytesPerMillisecond > 0) {
                  long owed = sinceLastAllocation * mmapFileBytesPerMillisecond;
                  if (owed > MMAP_FILE_BLOCK_BYTES) {
                    MmapFileInfo file = mmapFiles.alloc(owed);
                    long allocated =
                        mmapFileConsume(file.getPath(), file.getAllocSize(), file.getOffset());
                    if (allocated == 0) {
                      report.put("mmapFileFailed", true);
                    } else {
                      mmapFileAllocatedByTest += allocated;
                      latestAllocationTime = System.currentTimeMillis();
                    }
                  }
                }
              }
              long timeRunning = System.currentTimeMillis() - info.getStartTime();
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
                  throw new IllegalStateException(e);
                }
              }

              resultsStream.println(report);

              if (timeRunning > MAX_DURATION) {
                resultsStream.close();
                finish();
              }
              updateInfo(report);
            } catch (JSONException e) {
              throw new IllegalStateException(e);
            }
          }
        },
        0,
        1000 / 10);
  }

  private JSONObject getDeviceSettings() {
    JSONObject settings = new JSONObject();
    try {
      JSONObject lookup = new JSONObject(readStream(getAssets().open("lookup.json")));
      int bestScore = -1;
      String best = null;
      Iterator<String> it = lookup.keys();
      while (it.hasNext()) {
        String key = it.next();
        int score = Utils.mismatchIndex(Build.FINGERPRINT, key);
        if (score > bestScore) {
          bestScore = score;
          best = key;
        }
      }
      settings = lookup.getJSONObject(best);
    } catch (JSONException | IOException e) {
      Log.w("Settings problem.", e);
    }
    return settings;
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
            if (e instanceof InterruptedException) {
              Thread.currentThread().interrupt();
            }
            throw new IllegalStateException(e);
          }
          Log.v(MEMORY_BLOCKER, FREE_ACTION);
          serviceTotalMb = 0;
          try {
            Thread.sleep(BACKGROUND_PRESSURE_PERIOD_SECONDS * 1000);
          } catch (Exception e) {
            if (e instanceof InterruptedException) {
              Thread.currentThread().interrupt();
            }
            throw new IllegalStateException(e);
          }
        }
      }
    }.start();
  }

  @Override
  protected void onDestroy() {
    try {
      JSONObject report = standardInfo();
      report.put("onDestroy", true);
      resultsStream.println(report);
    } catch (JSONException e) {
      throw new IllegalStateException(e);
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
      throw new IllegalStateException(e);
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
      throw new IllegalStateException(e);
    }
  }

  private void updateInfo(JSONObject metrics) {
    runOnUiThread(
        () -> {
          long nativeHeapAllocatedSize = Debug.getNativeHeapAllocatedSize();
          if (nativeHeapAllocatedSize > recordNativeHeapAllocatedSize) {
            recordNativeHeapAllocatedSize = nativeHeapAllocatedSize;
          }
          WebView webView = findViewById(R.id.webView);
          try {
            webView.loadData(metrics.toString(2) + System.lineSeparator() + params.toString(2),
                "text/plain; charset=utf-8", "UTF-8");
          } catch (JSONException e) {
            throw new IllegalStateException(e);
          }
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
      throw new IllegalStateException(e);
    }
  }

  @Override
  public void onTrimMemory(int level) {

    try {
      JSONObject report = standardInfo();
      report.put("onTrimMemory", level);

      if (params.has("heuristics")) {
        JSONObject heuristics = params.getJSONObject("heuristics");
        if (heuristics.has("trim")) {
          releaseMemory(report, "trim");
        }
      }
      resultsStream.println(report);

      updateInfo(report);
      super.onTrimMemory(level);
    } catch (JSONException e) {
      throw new IllegalStateException(e);
    }
  }

  private void releaseMemory(JSONObject report, String trigger) {
    try {
      report.put("trigger", trigger);
    } catch (JSONException e) {
      throw new IllegalStateException(e);
    }
    latestAllocationTime = -1;
    runAfterDelay(
        () -> {
          JSONObject report2;
          try {
            report2 = standardInfo();
          } catch (JSONException e) {
            throw new IllegalStateException(e);
          }
          resultsStream.println(report2);
          if (nativeAllocatedByTest > 0) {
            nativeAllocatedByTest = 0;
            releases++;
            freeAll();
          }
          mmapAnonFreeAll();
          mmapAnonAllocatedByTest = 0;
          data.clear();
          if (glAllocBytesPerMillisecond > 0) {
            TestSurface testSurface = findViewById(R.id.glsurfaceView);
            if (testSurface != null) {
              testSurface.queueEvent(
                  () -> {
                    TestRenderer renderer = testSurface.getRenderer();
                    renderer.release();
                  });
            }
          }
          runAfterDelay(new Runnable() {
            @Override
            public void run() {
              try {
                resultsStream.println(standardInfo());
                AtomicBoolean anyWarnings = new AtomicBoolean(false);
                ActivityManager activityManager =
                    (ActivityManager) Objects.requireNonNull(getSystemService(Context.ACTIVITY_SERVICE));

                Heuristic.checkHeuristics(activityManager, params, deviceSettings,
                    (heuristic, heuristicMemoryPressureLevel) -> {
                      if (heuristicMemoryPressureLevel != Indicator.GREEN) {
                        anyWarnings.set(true);
                      }
                    });

                if (anyWarnings.get()) {
                  runAfterDelay(this, DELAY_AFTER_RELEASE);
                } else {
                  latestAllocationTime = System.currentTimeMillis();
                }
              } catch (JSONException e) {
                throw new IllegalStateException(e);
              }
            }
          }, DELAY_AFTER_RELEASE);
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
    JSONObject report = info.getMemoryMetrics(this);
    boolean paused = latestAllocationTime == -1;
    if (paused) {
      report.put("paused", true);
    }

    if (isServiceCrashed) {
      report.put("serviceCrashed", true);
    }
    report.put("nativeAllocatedByTest", nativeAllocatedByTest);
    report.put("mmapAnonAllocatedByTest", mmapAnonAllocatedByTest);
    report.put("mmapFileAllocatedByTest", mmapFileAllocatedByTest);
    report.put("serviceTotalMemory", BYTES_IN_MEGABYTE * serviceTotalMb);

    TestSurface testSurface = findViewById(R.id.glsurfaceView);
    TestRenderer renderer = testSurface.getRenderer();
    report.put("gl_allocated", renderer.getAllocated());
    if (renderer.getFailed()) {
      report.put("allocFailed", true);
    }
    return report;
  }

  public static native void initNative();

  public static native boolean tryAlloc(int bytes);

  public static native void initGl();

  public static native int nativeDraw(int toAllocate);

  public static native void release();

  public native void freeAll();

  public native void freeMemory(int bytes);

  public native boolean nativeConsume(int bytes);

  public native void mmapAnonFreeAll();

  public native long mmapAnonConsume(long bytes);

  public native long mmapFileConsume(String path, long bytes, long offset);

  public static native boolean writeRandomFile(String path, long bytes);

  static class MmapFileInfo {
    private final long fileSize;
    private long allocSize;
    private final String path;
    private long offset;

    public MmapFileInfo(String path) throws IOException {
      this.path = path;
      fileSize = getFileSize(path);
      offset = 0;
    }

    public boolean alloc(long desiredSize) {
      offset += allocSize;
      long limit = fileSize - offset;
      allocSize = Math.min(limit, desiredSize);
      return allocSize > 0;
    }

    public void reset() {
      offset = 0;
      allocSize = 0;
    }

    public String getPath() {
      return path;
    }

    public long getOffset() {
      return offset;
    }

    public long getAllocSize() {
      return allocSize;
    }
  }

  static class MmapFileGroup {
    private final ArrayList<MmapFileInfo> files = new ArrayList<>();
    private int current;

    public MmapFileGroup(String mmapPath, int mmapFileCount, long mmapFileSize) throws IOException {
      int digits = Integer.toString(mmapFileCount - 1).length();
      FileUtils.cleanDirectory(new File(mmapPath));
      for (int i = 0; i < mmapFileCount; ++i) {
        String filename = mmapPath + "/" + String.format("test%0" + digits + "d.dat", i);
        writeRandomFile(filename, mmapFileSize);
        files.add(new MmapFileInfo(filename));
      }
    }

    public MmapFileInfo alloc(long desiredSize) {
      while (!files.get(current).alloc(desiredSize)) {
        current = (current + 1) % files.size();
        files.get(current).reset();
      }
      return files.get(current);
    }
  }
}
