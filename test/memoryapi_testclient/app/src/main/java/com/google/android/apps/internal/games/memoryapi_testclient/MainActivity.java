package com.google.android.apps.internal.games.memoryapi_testclient;

import android.app.Activity;
import android.os.Bundle;

import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;

import org.json.JSONObject;

public class MainActivity extends Activity {
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    MemoryAdvisor memoryAdvisor = new MemoryAdvisor(this);

    JSONObject deviceInfo = memoryAdvisor.getDeviceInfo();

    System.out.println(deviceInfo);

    JSONObject advice = memoryAdvisor.getAdvice();

    boolean criticalWarnings = MemoryAdvisor.anyRedWarnings(advice);

    boolean warnings = MemoryAdvisor.anyWarnings(advice);

    long availabiliyEstimate = MemoryAdvisor.availabilityEstimate(advice);

    System.out.println(advice);

  }
}
