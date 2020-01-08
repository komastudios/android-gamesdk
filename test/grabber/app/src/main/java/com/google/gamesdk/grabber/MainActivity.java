package com.google.gamesdk.grabber;

import static com.google.gamesdk.grabber.MemoryPressureService.ACTION_TYPE;
import static com.google.gamesdk.grabber.MemoryPressureService.ALLOCATE_MEMORY_ACTION;
import static com.google.gamesdk.grabber.MemoryPressureService.TOTAL_MEMORY_MB;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;

public class MainActivity extends Activity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    Button button = findViewById(R.id.service_button);
    EditText editText = findViewById(R.id.editText);
    Context context = this;
    button.setOnClickListener(
        v -> {
          Intent intent = new Intent(context, MemoryPressureService.class);
          intent.putExtra(ACTION_TYPE, ALLOCATE_MEMORY_ACTION);
          intent.putExtra(TOTAL_MEMORY_MB, Integer.valueOf(editText.getText().toString()));
          context.startService(intent);
        });
  }
}
