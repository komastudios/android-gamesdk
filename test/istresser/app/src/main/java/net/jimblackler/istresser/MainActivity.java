package net.jimblackler.istresser;

import static com.google.android.apps.internal.games.memoryadvice.Utils.readFile;
import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getMemoryQuantity;
import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getOrDefault;
import static com.google.android.apps.internal.games.memoryadvice_common.StreamUtils.readStream;
import static net.jimblackler.istresser.ServiceCommunicationHelper.CRASHED_BEFORE;
import static net.jimblackler.istresser.ServiceCommunicationHelper.TOTAL_MEMORY_MB;
import static net.jimblackler.istresser.Utils.flattenParams;
import static net.jimblackler.istresser.Utils.getDuration;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.webkit.WebView;
import androidx.annotation.Nullable;
import androidx.core.content.FileProvider;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;
import com.google.android.apps.internal.games.memoryadvice.MemoryWatcher;
import com.google.common.collect.Lists;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.atomic.AtomicReference;
import org.apache.commons.io.FileUtils;

/**
 * The main activity of the istresser app
 */
public class MainActivity extends Activity {
  private static final String TAG = MainActivity.class.getSimpleName();
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

  private final ObjectMapper objectMapper = new ObjectMapper();

  private final List<byte[]> data = Lists.newArrayList();
  private PrintStream resultsStream = System.out;
  private MemoryAdvisor memoryAdvisor;
  private int serviceTotalMb;
  private ServiceCommunicationHelper serviceCommunicationHelper;
  private boolean isServiceCrashed;
  private Map<String, Object> params;
  private MmapFileGroup mmapFiles;
  private ServiceState serviceState;

  public static native void initNative();

  public static native void initGl();

  public static native void initMMap();

  public static native int nativeDraw(int toAllocate);

  public static native void release();

  public static native long vkAlloc(long size);

  public static native void vkRelease();

  public static native boolean writeRandomFile(String path, long bytes);

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    initNative();
    initMMap();

    Intent launchIntent = getIntent();

    Map<String, Object> params1 = null;
    if ("com.google.intent.action.TEST_LOOP".equals(launchIntent.getAction())) {
      // Parameters for an automatic run have been set via Firebase Test Loop.
      Uri logFile = launchIntent.getData();
      if (logFile != null) {
        String logPath = logFile.getEncodedPath();
        if (logPath != null) {
          Log.i(TAG, "Log file " + logPath);
          try {
            resultsStream = new PrintStream(
                Objects.requireNonNull(getContentResolver().openOutputStream(logFile)));
          } catch (FileNotFoundException e) {
            throw new IllegalStateException(e);
          }
        }
      }
      try {
        params1 = objectMapper.readValue(readFile("/sdcard/params.json"), Map.class);
      } catch (IOException e) {
        throw new IllegalStateException(e);
      }
    }

    if (launchIntent.hasExtra("Params")) {
      // Parameters for an automatic run have been set via Intent.
      try {
        params1 = objectMapper.readValue(
            Objects.requireNonNull(launchIntent.getStringExtra("Params")), Map.class);
      } catch (JsonProcessingException e) {
        throw new IllegalStateException(e);
      }
    }

    if (params1 == null) {
      // Must be manually launched. Get the parameters from the local default.json.
      try {
        params1 = objectMapper.readValue(readStream(getAssets().open("default.json")), Map.class);
      } catch (IOException e) {
        throw new IllegalStateException(e);
      }
    }
    if (resultsStream == System.out) {
      // This run is not running on Firebase. Determine the filename.
      List<Object> coordinates = (List<Object>) params1.get("coordinates");
      StringBuilder coordsString = new StringBuilder();
      for (int coordinateNumber = 0; coordinateNumber != coordinates.size(); coordinateNumber++) {
        coordsString.append("_").append(coordinates.get(coordinateNumber));
      }
      String fileName = getExternalFilesDir(null) + "/results" + coordsString + ".json";
      if (!new File(fileName).exists()) {
        // We deliberately do not overwrite the result for the case where Android automatically
        // relaunches the intent following a crash.
        try {
          resultsStream = new PrintStream(fileName);
        } catch (FileNotFoundException e) {
          throw new IllegalStateException(e);
        }
      }
    }

    params = flattenParams(params1);
    serviceCommunicationHelper = new ServiceCommunicationHelper(this);
    serviceState = ServiceState.DEALLOCATED;

    // Setting up broadcast receiver
    BroadcastReceiver receiver = new BroadcastReceiver() {
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
                serviceCommunicationHelper.allocateServerMemory(BACKGROUND_MEMORY_PRESSURE_MB);
              }
            }.start();

            break;
          default:
        }
      }
    };
    registerReceiver(receiver, new IntentFilter("com.google.gamesdk.grabber.RETURN"));

    Map<String, Object> report = new LinkedHashMap<>();
    report.put("time", System.currentTimeMillis());
    report.put("params", params1);

    TestSurface testSurface = findViewById(R.id.glsurfaceView);
    testSurface.setVisibility(View.GONE);
    long maxConsumer = getMemoryQuantity(getOrDefault(params, "maxConsumer", "2048K"));
    testSurface.getRenderer().setMaxConsumerSize(maxConsumer);

    PackageInfo packageInfo = null;
    try {
      packageInfo = getPackageManager().getPackageInfo(getPackageName(), 0);
      report.put("version", packageInfo.versionCode);
    } catch (PackageManager.NameNotFoundException e) {
      Log.i(TAG, "Could not get package information", e);
    }

    if (Boolean.TRUE.equals(params.get("serviceBlocker"))) {
      activateServiceBlocker();
    }

    if (Boolean.TRUE.equals(params.get("firebaseBlocker"))) {
      activateFirebaseBlocker();
    }

    nativeConsume(getMemoryQuantity(getOrDefault(params, "mallocFixed", 0)));

    if (params.containsKey("glTest")) {
      testSurface.setVisibility(View.VISIBLE);
    } else {
      long glFixed = getMemoryQuantity(getOrDefault(params, "glFixed", 0));
      if (glFixed > 0) {
        testSurface.setVisibility(View.VISIBLE);
        testSurface.getRenderer().setTarget(glFixed);
      }
    }

    if (params.containsKey("mmapFile")) {
      String mmapPath = getApplicationContext().getCacheDir().toString();
      int mmapFileCount = ((Number) getOrDefault(params, "mmapFileCount", 10)).intValue();
      long mmapFileSize = getMemoryQuantity(getOrDefault(params, "mmapFileSize", "4K"));
      try {
        mmapFiles = new MmapFileGroup(mmapPath, mmapFileCount, mmapFileSize);
      } catch (IOException e) {
        throw new IllegalStateException(e);
      }
    }

    try {
      resultsStream.println(objectMapper.writeValueAsString(report));
    } catch (JsonProcessingException e) {
      throw new IllegalStateException(e);
    }
    memoryAdvisor = new MemoryAdvisor(this, (Map<String, Object>) params.get("advisorParameters"),
        (timedOut) -> runOnUiThread(this::startTest));
  }

  @Override
  protected void onResume() {
    super.onResume();

    Map<String, Object> report = standardInfo();
    report.put("activityPaused", false);
    report.put("metrics", memoryAdvisor.getMemoryMetrics());
    try {
      resultsStream.println(objectMapper.writeValueAsString(report));
    } catch (JsonProcessingException e) {
      throw new IllegalStateException(e);
    }
  }

  @Override
  protected void onPause() {
    super.onPause();

    Map<String, Object> report = standardInfo();
    report.put("activityPaused", true);
    report.put("metrics", memoryAdvisor.getMemoryMetrics());
    try {
      resultsStream.println(objectMapper.writeValueAsString(report));
    } catch (JsonProcessingException e) {
      throw new IllegalStateException(e);
    }
  }

  @Override
  protected void onDestroy() {
    Map<String, Object> report = standardInfo();
    report.put("onDestroy", true);
    report.put("metrics", memoryAdvisor.getMemoryMetrics());
    try {
      resultsStream.println(objectMapper.writeValueAsString(report));
    } catch (JsonProcessingException e) {
      throw new IllegalStateException(e);
    }
    super.onDestroy();
  }

  @Override
  public void onTrimMemory(int level) {
    memoryAdvisor.setOnTrim(level);
    super.onTrimMemory(level);
  }

  @Override
  protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
    super.onActivityResult(requestCode, resultCode, data);
  }

  void startTest() {
    AtomicReference<List<Object>> criticalLogLines = new AtomicReference<>();

    new LogMonitor(line -> {
      if (line.contains("Out of memory")) {
        if (criticalLogLines.get() == null) {
          criticalLogLines.set(new ArrayList<>());
        }
        criticalLogLines.get().add(line);
      }
    });

    Map<String, Object> report = new LinkedHashMap<>();
    report.put("deviceInfo", memoryAdvisor.getDeviceInfo(this));
    try {
      resultsStream.println(objectMapper.writeValueAsString(report));
    } catch (JsonProcessingException e) {
      throw new IllegalStateException(e);
    }

    long timeout = getDuration(getOrDefault(params, "timeout", "10m"));
    Number maxMillisecondsPerSecond = (Number) params.get("maxMillisecondsPerSecond");
    Number minimumFrequency = (Number) params.get("minimumFrequency");
    Number maximumFrequency = (Number) params.get("maximumFrequency");

    new MemoryWatcher(memoryAdvisor,
        maxMillisecondsPerSecond == null ? 1000 : maxMillisecondsPerSecond.longValue(),
        minimumFrequency == null ? 200 : minimumFrequency.longValue(),
        maximumFrequency == null ? 2000 : maximumFrequency.longValue(), new MemoryWatcher.Client() {
          private final boolean yellowLightTesting =
              Boolean.TRUE.equals(params.get("yellowLightTesting"));
          private final long mallocBytesPerMillisecond =
              getMemoryQuantity(getOrDefault(params, "malloc", 0));
          private final long glAllocBytesPerMillisecond =
              getMemoryQuantity(getOrDefault(params, "glTest", 0));
          private final long vkAllocBytesPerMillisecond =
              getMemoryQuantity(getOrDefault(params, "vkTest", 0));
          private final long mmapFileBytesPerMillisecond =
              getMemoryQuantity(getOrDefault(params, "mmapFile", 0));
          private final long mmapAnonBytesPerMillisecond =
              getMemoryQuantity(getOrDefault(params, "mmapAnon", 0));

          private final long testStartTime = System.currentTimeMillis();
          private long allocationStartedTime = System.currentTimeMillis();
          private long appSwitchTimerStart = System.currentTimeMillis();
          private long lastLaunched;

          private long nativeAllocatedByTest;
          private long vkAllocatedByTest;
          private long mmapAnonAllocatedByTest;
          private long mmapFileAllocatedByTest;

          private final Timer timer = new Timer();
          private final TestSurface testSurface = findViewById(R.id.glsurfaceView);

          @Override
          public void newState(MemoryAdvisor.MemoryState state) {
            Log.i(TAG, "New memory state: " + state.name());
          }

          private void runAfterDelay(Runnable runnable, long delay) {
            timer.schedule(new TimerTask() {
              @Override
              public void run() {
                runnable.run();
              }
            }, delay);
          }

          @Override
          public void receiveAdvice(Map<String, Object> advice) {
            Map<String, Object> report = standardInfo();
            report.put("advice", advice);
            if (criticalLogLines.get() != null) {
              report.put("criticalLogLines", criticalLogLines.get());
              criticalLogLines.set(null);
            }
            if (allocationStartedTime == -1) {
              report.put("paused", true);
            } else {
              long sinceAllocationStarted = System.currentTimeMillis() - allocationStartedTime;
              if (sinceAllocationStarted > 0) {
                boolean shouldAllocate = true;
                switch (MemoryAdvisor.getMemoryState(advice)) {
                  case APPROACHING_LIMIT:
                    if (yellowLightTesting) {
                      shouldAllocate = false;
                    }
                    break;
                  case CRITICAL:
                    shouldAllocate = false;
                    if (yellowLightTesting) {
                      freeMemory(MEMORY_TO_FREE_PER_CYCLE_MB * BYTES_IN_MEGABYTE);
                    } else {
                      // Allocating 0 MB
                      releaseMemory();
                    }
                    break;
                }
                if (shouldAllocate) {
                  if (mallocBytesPerMillisecond > 0) {
                    long owed =
                        sinceAllocationStarted * mallocBytesPerMillisecond - nativeAllocatedByTest;
                    if (owed > 0) {
                      boolean succeeded = nativeConsume(owed);
                      if (succeeded) {
                        nativeAllocatedByTest += owed;
                      } else {
                        report.put("allocFailed", true);
                      }
                    }
                  }
                  if (glAllocBytesPerMillisecond > 0) {
                    long target = sinceAllocationStarted * glAllocBytesPerMillisecond;
                    testSurface.getRenderer().setTarget(target);
                  }

                  if (vkAllocBytesPerMillisecond > 0) {
                    long owed =
                        sinceAllocationStarted * vkAllocBytesPerMillisecond - vkAllocatedByTest;
                    if (owed > 0) {
                      long allocated = vkAlloc(owed);
                      if (allocated >= owed) {
                        vkAllocatedByTest += owed;
                      } else {
                        report.put("allocFailed", true);
                      }
                    }
                  }

                  if (mmapAnonBytesPerMillisecond > 0) {
                    long owed = sinceAllocationStarted * mmapAnonBytesPerMillisecond
                        - mmapAnonAllocatedByTest;
                    if (owed > MMAP_ANON_BLOCK_BYTES) {
                      long allocated = mmapAnonConsume(owed);
                      if (allocated != 0) {
                        mmapAnonAllocatedByTest += allocated;
                      } else {
                        report.put("mmapAnonFailed", true);
                      }
                    }
                  }
                  if (mmapFileBytesPerMillisecond > 0) {
                    long owed = sinceAllocationStarted * mmapFileBytesPerMillisecond
                        - mmapFileAllocatedByTest;
                    if (owed > MMAP_FILE_BLOCK_BYTES) {
                      MmapFileInfo file = mmapFiles.alloc(owed);
                      long allocated =
                          mmapFileConsume(file.getPath(), file.getAllocSize(), file.getOffset());
                      if (allocated == 0) {
                        report.put("mmapFileFailed", true);
                      } else {
                        mmapFileAllocatedByTest += allocated;
                      }
                    }
                  }
                }
              }
            }
            long timeRunning = System.currentTimeMillis() - testStartTime;
            Map<String, Object> switchTest = (Map<String, Object>) params.get("switchTest");
            if (switchTest != null && Boolean.TRUE.equals(switchTest.get("enabled"))) {
              long launchDuration = getDuration(getOrDefault(switchTest, "launchDuration", "30S"));
              long returnDuration = getDuration(getOrDefault(switchTest, "returnDuration", "60S"));
              long appSwitchTimeRunning = System.currentTimeMillis() - appSwitchTimerStart;
              if (appSwitchTimeRunning > launchDuration && lastLaunched < launchDuration) {
                lastLaunched = appSwitchTimeRunning;
                Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
                File file = new File(
                    Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES)
                    + File.separator + "pic.jpg");
                String authority = getApplicationContext().getPackageName() + ".provider";
                Uri uriForFile = FileProvider.getUriForFile(MainActivity.this, authority, file);
                intent.putExtra(MediaStore.EXTRA_OUTPUT, uriForFile);
                startActivityForResult(intent, 1);
              }
              if (appSwitchTimeRunning > returnDuration && lastLaunched < returnDuration) {
                lastLaunched = appSwitchTimeRunning;
                finishActivity(1);
                appSwitchTimerStart = System.currentTimeMillis();
                lastLaunched = 0;
              }
            }
            if (timeRunning > timeout) {
              report.put("exiting", true);
            }

            if (!report.containsKey("advice")) {  // 'advice' already includes metrics.
              report.put("metrics", memoryAdvisor.getMemoryMetrics());
            }

            Map<String, Object> testMetrics = new LinkedHashMap<>();
            if (vkAllocatedByTest > 0) {
              testMetrics.put("vkAllocatedByTest", vkAllocatedByTest);
            }
            if (nativeAllocatedByTest > 0) {
              testMetrics.put("nativeAllocatedByTest", nativeAllocatedByTest);
            }
            if (mmapAnonAllocatedByTest > 0) {
              testMetrics.put("mmapAnonAllocatedByTest", mmapAnonAllocatedByTest);
            }
            if (mmapAnonAllocatedByTest > 0) {
              testMetrics.put("mmapFileAllocatedByTest", mmapFileAllocatedByTest);
            }

            TestRenderer renderer = testSurface.getRenderer();
            long glAllocated = renderer.getAllocated();
            if (glAllocated > 0) {
              testMetrics.put("gl_allocated", glAllocated);
            }
            if (renderer.getFailed()) {
              report.put("allocFailed", true);
            }
            report.put("testMetrics", testMetrics);

            try {
              resultsStream.println(objectMapper.writeValueAsString(report));
            } catch (JsonProcessingException e) {
              throw new IllegalStateException(e);
            }

            if (timeRunning > timeout) {
              resultsStream.close();
              finish();
            }
            runOnUiThread(() -> {
              WebView webView = findViewById(R.id.webView);
              try {
                webView.loadData(objectMapper.writeValueAsString(report) + System.lineSeparator()
                        + objectMapper.writeValueAsString(params),
                    "text/plain; charset=utf-8", "UTF-8");
              } catch (JsonProcessingException e) {
                throw new IllegalStateException(e);
              }
            });
          }
          private void releaseMemory() {
            allocationStartedTime = -1;
            long delayBeforeRelease = getDuration(getOrDefault(params, "delayBeforeRelease", "1s"));
            long delayAfterRelease = getDuration(getOrDefault(params, "delayAfterRelease", "1s"));
            runAfterDelay(() -> {
              Map<String, Object> report2 = standardInfo();
              report2.put("paused", true);
              report2.put("metrics", memoryAdvisor.getMemoryMetrics());

              try {
                resultsStream.println(objectMapper.writeValueAsString(report2));
              } catch (JsonProcessingException e) {
                throw new IllegalStateException(e);
              }
              if (nativeAllocatedByTest > 0) {
                nativeAllocatedByTest = 0;
                freeAll();
              }
              mmapAnonFreeAll();
              mmapAnonAllocatedByTest = 0;
              data.clear();
              if (glAllocBytesPerMillisecond > 0) {
                TestSurface testSurface = findViewById(R.id.glsurfaceView);
                if (testSurface != null) {
                  testSurface.queueEvent(() -> {
                    TestRenderer renderer = testSurface.getRenderer();
                    renderer.release();
                  });
                }
              }
              if (vkAllocBytesPerMillisecond > 0) {
                vkAllocatedByTest = 0;
                vkRelease();
              }

              runAfterDelay(new Runnable() {
                @Override
                public void run() {
                  Map<String, Object> report = standardInfo();
                  Map<String, Object> advice = memoryAdvisor.getAdvice();
                  report.put("advice", advice);
                  if (MemoryAdvisor.anyWarnings(advice)) {
                    report.put("failedToClear", true);
                    report.put("paused", true);
                    runAfterDelay(this, delayAfterRelease);
                  } else {
                    allocationStartedTime = System.currentTimeMillis();
                  }
                  try {
                    resultsStream.println(objectMapper.writeValueAsString(report));
                  } catch (JsonProcessingException e) {
                    throw new IllegalStateException(e);
                  }
                }
              }, delayAfterRelease);
            }, delayBeforeRelease);
          }
        });
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

  private Map<String, Object> standardInfo() {
    Map<String, Object> report = new LinkedHashMap<>();

    if (isServiceCrashed) {
      report.put("serviceCrashed", true);
    }

    Map<String, Object> testMetrics = new LinkedHashMap<>();
    if (serviceTotalMb > 0) {
      testMetrics.put("serviceTotalMemory", BYTES_IN_MEGABYTE * serviceTotalMb);
    }

    return report;
  }

  public native void freeAll();

  public native void freeMemory(int bytes);

  public native boolean nativeConsume(long bytes);

  public native void mmapAnonFreeAll();

  public native long mmapAnonConsume(long bytes);

  public native long mmapFileConsume(String path, long bytes, long offset);

  enum ServiceState {
    ALLOCATING_MEMORY,
    ALLOCATED,
    FREEING_MEMORY,
    DEALLOCATED,
  }

  static class MmapFileInfo {
    private final long fileSize;
    private final String path;
    private long allocSize;
    private long offset;

    public MmapFileInfo(String path) {
      this.path = path;
      fileSize = new File(path).length();
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
