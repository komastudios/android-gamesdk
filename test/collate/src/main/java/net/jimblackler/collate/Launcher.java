package net.jimblackler.collate;

import static java.lang.System.currentTimeMillis;
import static java.nio.file.StandardCopyOption.REPLACE_EXISTING;

import com.google.common.collect.ImmutableSet;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.OffsetDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Date;
import java.util.List;
import java.util.concurrent.atomic.AtomicReference;
import org.json.JSONArray;
import org.json.JSONObject;

public class Launcher {
  private static final boolean USE_DEVICE = false;
  private static final int MIN_VERSION = 26;
  private static final boolean USE_WHITELIST = false;
  private static final int MAX_DEVICES = Integer.MAX_VALUE;
  private static final String STANDARD_APK_PATH = "app/build/outputs/apk/debug/app-debug.apk";
  private static final long EXTRA_TIMEOUT = 1000 * 10;  // Allow extra time to start test on device.
  private static final ImmutableSet<String> whitelist = ImmutableSet.of("602SO", "A0001",
      "A1N_sprout", "ASUS_X00T_3", "ASUS_Z01H_1", "FRT", "G8142", "H8416", "HWCOR", "HWMHA",
      "HWMHA", "OnePlus5T", "OnePlus6T", "SC-02K", "a9y18qltechn", "addison", "albus", "aljeter_n",
      "athene_f", "aura", "blueline", "capricorn", "cheryl", "crownqlteue", "cruiserltesq",
      "deen_sprout", "flame", "g3", "hammerhead", "harpia", "hero2qlteatt", "hero2qltevzw",
      "heroqltetfnvzw", "hlte", "htc_m8", "hwALE-H", "j1acevelteub", "james", "jeter", "lithium",
      "lucye", "lv0", "m0", "mlv1", "poseidonlteatt", "potter", "shamu", "victara");

  public static void main(String[] args) throws IOException {
    if (USE_DEVICE) {
      deviceLaunch();
    } else {
      cloudLaunch();
    }
    Score.go(USE_DEVICE);
  }

  private static void deviceLaunch() throws IOException {
    AtomicReference<String> pack = new AtomicReference<>();  // 'package' is a Java keyword.
    AtomicReference<Path> installed = new AtomicReference<>();
    doLaunch((id, apkPath, paramsIn) -> {
      JSONObject flattened = Utils.flattenParams(paramsIn);
      pack.set(flattened.getString("package"));

      // Kill the app as a precaution against confusing an old run with the new run.
      try {
        int pid = getPid(pack.get());
        Utils.execute("adb", "shell", "kill", String.valueOf(pid));
        while (true) {
          getPid(pack.get());  // Wait for the process to actually go away.
        }
      } catch (IOException ex) {
        // Ignored by design.
      }

      if (!apkPath.equals(installed.getAndSet(apkPath))) {
        // Uninstall any previous versions of the app the first time.
        try {
          Utils.execute("adb", "uninstall", pack.get());
        } catch (IOException e) {
          // Ignored by design. App might not be installed.
        }
        Utils.execute("adb", "install", apkPath.toString());
      }

      long timeout = Utils.getDuration(Utils.getOrDefault(flattened, "timeout", "10m"));
      Utils.execute("adb", "shell", "am", "start", "-S", "-W", "-n",
          pack + "/" + pack + ".MainActivity", "--es", "\"Params\"",
          jsonToShellParameter(paramsIn));

      // Wait for process to end or for the run to time out.
      int pid = 0;
      long started = currentTimeMillis();
      while (true) {
        try {
          if (currentTimeMillis() > started + timeout + EXTRA_TIMEOUT) {
            break;
          }
          int newPid = getPid(pack.get());
          if (newPid != pid) {
            if (pid == 0) {
              pid = newPid;
              started = currentTimeMillis();  // Restart timeout timer.
            } else {
              break;  // pid has changed - something went wrong.
            }
          }

        } catch (IOException ex) {
          // Process doesn't exist.
          if (pid != 0) {
            // Process has been killed.
            break;
          }
        }
      }
    });
  }

  /**
   * Get the PID of a running Android app.
   * @param pack The package name of the app.
   * @return The PID.
   */
  private static int getPid(String pack) throws IOException {
    return Integer.parseInt(Utils.executeSilent("adb", "shell", "pidof", pack));
  }

  /**
   * Encode a JSON string for shell command line use.
   * @param json JSON string to encode.
   * @return The encoded string.
   */
  private static String jsonToShellParameter(JSONObject json) {
    return "\"" + json.toString().replace("\"", "\\\"") + "\"";
  }

  private static void cloudLaunch() throws IOException {
    Path grabberPath = Config.GRABBER_BASE.resolve(STANDARD_APK_PATH);

    List<String> baseCommands = new ArrayList<>(Arrays.asList(Config.GCLOUD_EXECUTABLE.toString(),
        "beta", "firebase", "test", "android", "run", "--type", "game-loop", "--async",
        "--scenario-numbers", "1", "--format", "json"));
    Collection<String> devices = new ArrayList<>();

    DeviceFetcher.fetch(device -> {
      if (!USE_WHITELIST && devices.size() >= MAX_DEVICES) {
        return;
      }
      if (!device.getString("formFactor").equals("PHONE")) {
        return;
      }
      if (!device.getString("form").equals("PHYSICAL")) {
        return;
      }
      JSONArray supportedVersionIds = device.getJSONArray("supportedVersionIds");
      for (int idx2 = 0; idx2 != supportedVersionIds.length(); idx2++) {
        int versionId = supportedVersionIds.getInt(idx2);
        if (versionId < MIN_VERSION) {
          continue;
        }
        String id = device.getString("id");
        if (USE_WHITELIST && !whitelist.contains(id)) {
          continue;
        }
        devices.add(String.format("--device=model=%s,version=%d", id, versionId));
      }
    });

    Path[] grabberCopy = {null};

    doLaunch((id, apkPath, paramsIn) -> {
      Path tempFile = Files.createTempFile("params", ".json");
      try (BufferedWriter writer = new BufferedWriter(new FileWriter(tempFile.toFile()))) {
        writer.write(paramsIn.toString(2));
      }

      List<String> commands = new ArrayList<>(baseCommands);
      commands.add("--other-files");
      commands.add("/sdcard/params.json=" + tempFile);
      commands.add("--results-history-name=" + id);
      commands.add("--app=" + apkPath);

      JSONObject flattened = Utils.flattenParams(paramsIn);

      if (flattened.has("firebase")) {
        JSONObject firebase = flattened.getJSONObject("firebase");
        for (String key : firebase.keySet()) {
          Object value = firebase.get(key);
          if (value.equals(Boolean.TRUE)) {
            commands.add("--" + key);
          } else {
            commands.add("--" + key + "=" + value);
          }
        }
      }

      if (flattened.has("orientation")) {
        for (String device : devices) {
          commands.add(device + ",orientation=" + flattened.get("orientation"));
        }
      } else {
        commands.addAll(devices);
      }

      if (paramsIn.has("serviceBlocker")) {
        if (grabberCopy[0] == null) {
          grabberCopy[0] = Files.createTempFile("grabber", ".apk");
          Files.copy(grabberPath, grabberCopy[0], REPLACE_EXISTING);
        }
        commands.add("--additional-apks");
        commands.add(grabberCopy[0].toString());
      }

      String text = Utils.execute(commands.toArray(new String[0]));
      System.out.println(text);
    });
  }

  private static void doLaunch(LaunchClient client) throws IOException {
    OffsetDateTime now = OffsetDateTime.now();
    DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd_HH:mm:ss");
    String id = formatter.format(now);
    Date date = new Date();

    JSONArray tests = new JSONArray(Utils.fileToString("tests.json"));
    int numberDimensions = tests.length();
    List<Integer> coordinates = new ArrayList<>();
    for (int dimension = 0; dimension < numberDimensions; dimension++) {
      coordinates.add(0);
    }
    while (true) {
      JSONObject paramsIn = new JSONObject();
      paramsIn.put("tests", tests);
      paramsIn.put("run", id);
      paramsIn.put("started", date.getTime());
      paramsIn.put("coordinates", new JSONArray(coordinates));

      JSONObject flattened = Utils.flattenParams(paramsIn);

      Path apkPath = Path.of(flattened.getString("apk_base"), flattened.getString("apk_name"));
      if (!apkPath.toFile().exists()) {
        throw new IllegalStateException(apkPath + " missing");
      }
      System.out.println(coordinates);

      client.launch(id, apkPath, paramsIn);

      int coordinateNumber = numberDimensions - 1;
      while (true) {
        int e = coordinates.get(coordinateNumber) + 1;
        if (e >= tests.getJSONArray(coordinateNumber).length()) {
          coordinates.set(coordinateNumber, 0);
          coordinateNumber--;
          if (coordinateNumber < 0) {
            break;
          }
        } else {
          coordinates.set(coordinateNumber, e);
          break;
        }
      }

      if (coordinateNumber < 0) {
        break;
      }
    }
  }

  private interface LaunchClient {
    void launch(String id, Path apkPath, JSONObject flattened) throws IOException;
  }
}
