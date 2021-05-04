package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice.Utils.processMemFormatFile;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.greaterThan;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import java.util.HashMap;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class UtilsTest {
  @Test
  public void testProcessMemFormatFile() {
    HashMap<String, Long> output = new HashMap<>();
    processMemFormatFile("/proc/meminfo", output::put);
    // There's no CTS definition of what should be in proc/meminfo but we can expect MemTotal to be
    // there and to have a positive value.
    assertThat(output.get("MemTotal"), greaterThan(0L));
  }
}