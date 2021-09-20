package net.jimblackler.collate;

import java.io.IOException;
import java.nio.file.FileSystems;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Tool to create tables from stats received from a live run on a local device.
 */
public class LocalInfo {
  public static void main(String[] args) throws ExecutionError {
    int pid = Integer.parseInt(Utils.execute("adb", "shell", "pidof", "net.jimblackler.istresser"));
    int[] c = {0};
    new Timer().scheduleAtFixedRate(new TimerTask() {
      @Override
      public void run() {
        try {
          {
            String data = Utils.execute("adb", "shell", "cat", "/d/kgsl/globals");
            FileUtils.writeString(FileSystems.getDefault().getPath(
                                      "out", String.format("kgsl_globals_%02d.txt", c[0])),
                data);
          }

          {
            String data = Utils.execute("adb", "shell", "cat", "/d/kgsl/proc/" + pid + "/mem");
            FileUtils.writeString(
                FileSystems.getDefault().getPath("out", String.format("kgsl_mem_%02d.txt", c[0])),
                data);
          }

          c[0]++;

        } catch (IOException e) {
          throw new RuntimeException(e);
        }
      }
    }, 1000, 1000);
  }
}
