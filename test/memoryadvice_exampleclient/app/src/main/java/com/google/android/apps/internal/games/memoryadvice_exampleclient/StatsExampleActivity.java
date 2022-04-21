package com.google.android.apps.internal.games.memoryadvice_exampleclient;

import android.app.Activity;
import android.os.Bundle;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectReader;
import com.fasterxml.jackson.databind.ObjectWriter;
import com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

public class StatsExampleActivity extends Activity {
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    try {
      ObjectMapper objectMapper = new ObjectMapper();
      ObjectReader objectReader = objectMapper.reader();
      ObjectWriter objectWriter = objectMapper.writer();

      Map<String, Object> params;
      params = objectReader.readValue(
          StatsExampleActivity.class.getResourceAsStream("/params.json"), Map.class);

      PrintStream printStream = System.out;
      MemoryAdvisor memoryAdvisor = new MemoryAdvisor(this, params);

      Map<String, Object> deviceInfo = memoryAdvisor.getDeviceInfo();
      printStream.println(objectWriter.writeValueAsString(deviceInfo));

      new Timer().schedule(new TimerTask() {
        @Override
        public void run() {
          Map<String, Object> advice = memoryAdvisor.getAdvice();
          try {
            printStream.println(objectWriter.writeValueAsString(advice));
          } catch (JsonProcessingException e) {
            throw new RuntimeException(e);
          }
        }
      }, 500, 500);
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}
