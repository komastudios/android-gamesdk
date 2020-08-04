package net.jimblackler.istresser;

import android.util.Log;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;

/**
 * A long-running service that sends every new log line to a client.
 */
public class LogMonitor {
  private static final String TAG = LogMonitor.class.getSimpleName();

  interface LogReaderClient {
    void receive(String line);
  }

  /**
   * Create a new log monitor.
   * @param client The client to send every new log line to on a best-effort basis.
   */
  public LogMonitor(LogReaderClient client) {
    startLog(client);
  }

  private static void startLog(LogReaderClient client) {
    new Thread(() -> {
      try (InputStream inputStream = Runtime.getRuntime().exec("logcat").getInputStream();
           BufferedReader bufferedReader =
               new BufferedReader(new InputStreamReader(inputStream, StandardCharsets.UTF_8))) {
        // Initially all log lines already present on the log are discarded. The class is only for
        // monitoring future log activity.
        while (bufferedReader.ready()) {
          bufferedReader.readLine();
        }

        while (true) {
          try {
            String line = bufferedReader.readLine();
            if (line == null) {
              // Null is returned when and only when the stream terminates.
              // logcat is supposed to be long-running (unless -d is used) but has been seen to
              // terminate on some devices. If this happens we simply restart it; ASAP to reduce
              // the number of lost log lines.
              Log.w(TAG, "Restarting log monitor");
              startLog(client);
              return;
            } else {
              client.receive(line);
            }
          } catch (IOException e) {
            Log.w(TAG, "Error reading a log line", e);
          }
        }
      } catch (IOException e) {
        Log.w(TAG, "Log monitor could not access logcat", e);
      }
    }).start();
  }
}
