package com.google.android.apps.internal.games.memoryadvice_exampleclient;

import android.app.Activity;
import android.os.Bundle;
import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;
import com.google.android.apps.internal.games.memoryadvice.MemoryWatcher;
import org.json.JSONObject;

public class MainActivity extends Activity {
  private MemoryAdvisor memoryAdvisor;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    memoryAdvisor = new MemoryAdvisor(this, new Runnable() {
      @Override
      public void run() {
        // The budget for overhead introduced by the advisor and watcher.
        int maxMillisecondsPerSecond = 10;

        // The minimum time duration between iterations, in milliseconds.
        int minimumFrequency = 1000;

        MemoryWatcher memoryWatcher = new MemoryWatcher(
            memoryAdvisor, maxMillisecondsPerSecond, minimumFrequency, new MemoryWatcher.Client() {
              @Override
              public void newState(MemoryAdvisor.MemoryState state) {}
            });

        JSONObject deviceInfo = memoryAdvisor.getDeviceInfo(MainActivity.this);

        System.out.println(deviceInfo);

        JSONObject advice = memoryAdvisor.getAdvice();

        MemoryAdvisor.MemoryState memoryState = MemoryAdvisor.getMemoryState(advice);

        long availabilityEstimate = MemoryAdvisor.availabilityEstimate(advice);

        System.out.println(advice);
      }
    });
  }

  @Override
  public void onTrimMemory(int level) {
    memoryAdvisor.setOnTrim(level);
    super.onTrimMemory(level);
  }
}
