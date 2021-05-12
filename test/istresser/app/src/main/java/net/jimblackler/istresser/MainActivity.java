package net.jimblackler.istresser;

import static com.google.android.apps.internal.games.memoryadvice.Utils.readFile;
import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getMemoryQuantity;
import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getOrDefault;
import static com.google.android.apps.internal.games.memoryadvice_common.StreamUtils.readStream;
import static com.google.android.apps.internal.games.memorytest.DurationUtils.getDuration;
import static com.google.android.apps.internal.games.memorytest.MapUtils.flattenParams;
import static net.jimblackler.istresser.ServiceCommunicationHelper.CRASHED_BEFORE;
import static net.jimblackler.istresser.ServiceCommunicationHelper.TOTAL_MEMORY_MB;

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
import com.google.android.apps.internal.games.memorytest.MemoryTest;
import com.google.android.apps.internal.games.memorytest.TestSurface;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Timer;
import java.util.TimerTask;

/**
 * The main activity of the istresser app
 */
public class MainActivity extends Activity {
  private static final String TAG = MainActivity.class.getSimpleName();
  private static final int BACKGROUND_MEMORY_PRESSURE_MB = 500;
  private static final int BACKGROUND_PRESSURE_PERIOD_SECONDS = 30;
  private static final int BYTES_IN_MEGABYTE = 1024 * 1024;
  private static final String MEMORY_BLOCKER = "MemoryBlockCommand";
  private static final String ALLOCATE_ACTION = "Allocate";
  private static final String FREE_ACTION = "Free";

  private final ObjectMapper objectMapper = new ObjectMapper();

  private PrintStream resultsStream = System.out;
  private MemoryAdvisor memoryAdvisor;
  private ServiceCommunicationHelper serviceCommunicationHelper;
  private Map<String, Object> params;
  private ServiceState serviceState;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

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
        Map<String, Object> report = new LinkedHashMap<>();
        report.put("time", System.currentTimeMillis());
        if (intent.getBooleanExtra(CRASHED_BEFORE, false)) {
          report.put("serviceCrashed", true);
        }
        Map<String, Object> testMetrics = new LinkedHashMap<>();
        testMetrics.put(
            "serviceTotalMemory", BYTES_IN_MEGABYTE * intent.getIntExtra(TOTAL_MEMORY_MB, 0));
        report.put("testMetrics", testMetrics);
        try {
          resultsStream.println(objectMapper.writeValueAsString(report));
        } catch (JsonProcessingException e) {
          throw new IllegalStateException(e);
        }

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

    Map<String, Object> report = new LinkedHashMap<>();
    report.put("activityPaused", false);
    report.put("time", System.currentTimeMillis());
    try {
      resultsStream.println(objectMapper.writeValueAsString(report));
    } catch (JsonProcessingException e) {
      throw new IllegalStateException(e);
    }
  }

  @Override
  protected void onPause() {
    super.onPause();

    Map<String, Object> report = new LinkedHashMap<>();
    report.put("activityPaused", true);
    report.put("time", System.currentTimeMillis());
    try {
      resultsStream.println(objectMapper.writeValueAsString(report));
    } catch (JsonProcessingException e) {
      throw new IllegalStateException(e);
    }
  }

  @Override
  protected void onDestroy() {
    Map<String, Object> report = new LinkedHashMap<>();
    report.put("onDestroy", true);
    report.put("time", System.currentTimeMillis());
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
    new LogMonitor(line -> {
      if (line.contains("Out of memory")) {
        Map<String, Object> report = new LinkedHashMap<>();
        Collection<Object> lines = new ArrayList<>();
        lines.add(line);
        report.put("criticalLogLines", lines);
        report.put("time", System.currentTimeMillis());
        try {
          resultsStream.println(objectMapper.writeValueAsString(report));
        } catch (JsonProcessingException e) {
          throw new IllegalStateException(e);
        }
      }
    });

    Map<String, Object> report = new LinkedHashMap<>();
    report.put("deviceInfo", memoryAdvisor.getDeviceInfo(this));
    try {
      resultsStream.println(objectMapper.writeValueAsString(report));
    } catch (JsonProcessingException e) {
      throw new IllegalStateException(e);
    }

    Map<String, Object> switchTest = (Map<String, Object>) params.get("switchTest");
    if (switchTest != null) {
      scheduleAppSwitch(switchTest);
    }

    Number maxMillisecondsPerSecond = (Number) params.get("maxMillisecondsPerSecond");
    Number minimumFrequency = (Number) params.get("minimumFrequency");
    Number maximumFrequency = (Number) params.get("maximumFrequency");

    String paramsString;
    try {
      paramsString = objectMapper.writeValueAsString(params);
    } catch (JsonProcessingException e) {
      throw new IllegalStateException(e);
    }

    WebView webView = findViewById(R.id.webView);

    new MemoryWatcher(memoryAdvisor,
        maxMillisecondsPerSecond == null ? 1000 : maxMillisecondsPerSecond.longValue(),
        minimumFrequency == null ? 200 : minimumFrequency.longValue(),
        maximumFrequency == null ? 2000 : maximumFrequency.longValue(),
        new MemoryTest(this, memoryAdvisor, findViewById(R.id.glsurfaceView), params, report0 -> {
          String reportString;
          try {
            reportString = objectMapper.writeValueAsString(report0);
          } catch (JsonProcessingException e) {
            throw new IllegalStateException(e);
          }
          resultsStream.println(reportString);
          if (Boolean.TRUE.equals(report0.get("exiting"))) {
            resultsStream.close();
            finish();
          } else {
            runOnUiThread(()
                              -> webView.loadData(reportString + System.lineSeparator()
                                      + System.lineSeparator() + paramsString,
                                  "text/plain; charset=utf-8", "UTF-8"));
          }
        }));
  }

  private void scheduleAppSwitch(Map<String, Object> switchTest) {
    new Timer().schedule(new TimerTask() {
      @Override
      public void run() {
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        File file =
            new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES)
                + File.separator + "pic.jpg");
        String authority = getApplicationContext().getPackageName() + ".provider";
        Uri uriForFile = FileProvider.getUriForFile(MainActivity.this, authority, file);
        intent.putExtra(MediaStore.EXTRA_OUTPUT, uriForFile);
        startActivityForResult(intent, 1);
        new Timer().schedule(new TimerTask() {
          @Override
          public void run() {
            finishActivity(1);
            scheduleAppSwitch(switchTest);
          }
        }, getDuration(getOrDefault(switchTest, "returnDuration", "60S")));
      }
    }, getDuration(getOrDefault(switchTest, "launchDuration", "30S")));
  }

  private void activateServiceBlocker() {
    serviceState = ServiceState.ALLOCATING_MEMORY;
    serviceCommunicationHelper.allocateServerMemory(BACKGROUND_MEMORY_PRESSURE_MB);
  }

  private void activateFirebaseBlocker() {
    new Thread() {
      @Override
      public void run() {
        Log.v(MEMORY_BLOCKER, FREE_ACTION);
        while (true) {
          Log.v(MEMORY_BLOCKER, ALLOCATE_ACTION + " " + BACKGROUND_MEMORY_PRESSURE_MB);
          {
            Map<String, Object> report = new LinkedHashMap<>();
            report.put("time", System.currentTimeMillis());
            Map<String, Object> testMetrics = new LinkedHashMap<>();
            testMetrics.put(
                "serviceTotalMemory", BYTES_IN_MEGABYTE * BACKGROUND_MEMORY_PRESSURE_MB);
            report.put("testMetrics", testMetrics);
            try {
              resultsStream.println(objectMapper.writeValueAsString(report));
            } catch (JsonProcessingException e) {
              throw new IllegalStateException(e);
            }
          }
          try {
            Thread.sleep(BACKGROUND_PRESSURE_PERIOD_SECONDS * 1000);
          } catch (Exception e) {
            if (e instanceof InterruptedException) {
              Thread.currentThread().interrupt();
            }
            throw new IllegalStateException(e);
          }
          Log.v(MEMORY_BLOCKER, FREE_ACTION);
          {
            Map<String, Object> report = new LinkedHashMap<>();
            report.put("time", System.currentTimeMillis());
            Map<String, Object> testMetrics = new LinkedHashMap<>();
            testMetrics.put("serviceTotalMemory", 0);
            report.put("testMetrics", testMetrics);
            try {
              resultsStream.println(objectMapper.writeValueAsString(report));
            } catch (JsonProcessingException e) {
              throw new IllegalStateException(e);
            }
          }
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

  enum ServiceState {
    ALLOCATING_MEMORY,
    ALLOCATED,
    FREEING_MEMORY,
    DEALLOCATED,
  }
}
