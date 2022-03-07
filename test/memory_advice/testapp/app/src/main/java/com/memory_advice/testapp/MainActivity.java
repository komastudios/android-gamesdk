package com.memory_advice.testapp;

import android.text.method.ScrollingMovementMethod;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

  // Used to load the 'testapp' library on application startup.
  static {
    System.loadLibrary("native-lib");
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    TextView tv = findViewById(R.id.sample_text);
    tv.setMovementMethod(new ScrollingMovementMethod());
    tv.setText(runTests());
  }

  public native String runTests();
}