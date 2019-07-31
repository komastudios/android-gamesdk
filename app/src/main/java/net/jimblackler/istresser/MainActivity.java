package net.jimblackler.istresser;

import android.os.Bundle;
import android.os.Debug;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

import java.util.Locale;

public class MainActivity extends AppCompatActivity {

  private static String memoryString(long bytes) {
    return String.format(Locale.getDefault(), "%.1f MB", (float) bytes / (1024 * 1024));
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    TextView freeMemory = findViewById(R.id.freeMemory);
    freeMemory.setText(memoryString(Runtime.getRuntime().freeMemory()));

    TextView totalMemory = findViewById(R.id.totalMemory);
    totalMemory.setText(memoryString(Runtime.getRuntime().totalMemory()));

    TextView maxMemory = findViewById(R.id.maxMemory);
    maxMemory.setText(memoryString(Runtime.getRuntime().maxMemory()));

    TextView nativeHeap = findViewById(R.id.nativeHeap);
    nativeHeap.setText(memoryString(Debug.getNativeHeapSize()));

    TextView nativeAllocated = findViewById(R.id.nativeAllocated);
    nativeAllocated.setText(memoryString(Debug.getNativeHeapAllocatedSize()));
  }
}
