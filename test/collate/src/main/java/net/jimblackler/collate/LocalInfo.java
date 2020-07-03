package net.jimblackler.collate;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Timer;
import java.util.TimerTask;

public class LocalInfo {
  public static void main(String[] args) throws IOException {
    int pid = Integer.parseInt(Utils.execute("adb", "shell", "pidof", "net.jimblackler.istresser"));
    int[] c = {0};
    new Timer().scheduleAtFixedRate(new TimerTask() {
      @Override
      public void run() {
        try {
          {
            String data = Utils.execute("adb", "shell", "cat", "/d/kgsl/globals");
            Files.writeString(Path.of("out", String.format("kgsl_globals_%02d.txt", c[0])), data);
          }

          {
            String data = Utils.execute("adb", "shell", "cat", "/d/kgsl/proc/" + pid + "/mem");
            Files.writeString(Path.of("out", String.format("kgsl_mem_%02d.txt", c[0])), data);
          }

          c[0]++;

        } catch (IOException e) {
          throw new RuntimeException(e);
        }
      }
    }, 1000, 1000);
  }
}
