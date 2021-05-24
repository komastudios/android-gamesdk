package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice.Utils.processMemFormatFile;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.lessThan;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import java.util.HashMap;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class UtilsTest {
  static final int BYTES_IN_KILOBYTE = 1024;
  static final int BYTES_IN_MEGABYTE = BYTES_IN_KILOBYTE * 1024;
  static final int BYTES_IN_GIGABYTE = BYTES_IN_MEGABYTE * 1024;

  @Test
  public void testProcessMemFormatFile() {
    HashMap<String, Long> output = new HashMap<>();
    processMemFormatFile("/proc/meminfo", output::put);
    // There's no CTS definition of what should be in proc/meminfo but we can expect MemTotal to be
    // there and to have a value between 500K and 100G.
    assertThat(output.containsKey("MemTotal"), is(true));
    long memTotal = output.get("MemTotal");
    assertThat(memTotal, greaterThan(500L * BYTES_IN_KILOBYTE));
    assertThat(memTotal, lessThan(100L * BYTES_IN_GIGABYTE));
  }
}