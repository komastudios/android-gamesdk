package net.jimblackler.collate;

import static java.nio.file.StandardCopyOption.REPLACE_EXISTING;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Date;
import java.util.List;
import java.util.UUID;

import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONTokener;

public class Launcher {

  private static final Path HOME = Path.of(System.getProperty("user.home"));
  private static final Path GCLOUD_LOCATION = HOME.resolve("google-cloud-sdk/bin");
  private static final Path SDK_BASE = HOME.resolve("code/android-games-sdk");
  private static final Path ISTRESSER_BASE = SDK_BASE.resolve("gamesdk/test/istresser");
  private static final Path GRABBER_BASE = SDK_BASE.resolve("gamesdk/test/grabber");
  private static final Path GCLOUD_EXECUTABLE = GCLOUD_LOCATION.resolve("gcloud");
  private static final int MIN_VERSION = 26;
  private static final int MAX_DEVICES = Integer.MAX_VALUE;
  private static final String STANDARD_APK_PATH = "app/build/outputs/apk/debug/app-debug.apk";

  public static void main(String[] args) throws IOException {

    UUID uuid = UUID.randomUUID();
    Date date = new Date();
    Path iStresserPath = ISTRESSER_BASE.resolve(STANDARD_APK_PATH);
    Path grabberPath = GRABBER_BASE.resolve(STANDARD_APK_PATH);

    List<String> baseCommands = new ArrayList<>(Arrays.asList(
        GCLOUD_EXECUTABLE.toString(),
        "beta", "firebase", "test", "android", "run",
        "--app", iStresserPath.toString(),
        "--results-history-name", uuid.toString(),
        "--type", "game-loop",
        "--async",
        "--format", "json"));

    Collection<String> devices = getDevices();
    System.out.println(devices);
    baseCommands.addAll(devices);

    Path grabberCopy = null;

    JSONArray tests = new JSONArray(Utils.fileToString("tests.json"));
    int numberDimensions = tests.length();
    List<Integer> coordinates = new ArrayList<>();
    for (int dimension = 0; dimension < numberDimensions; dimension++) {
      coordinates.add(0);
    }
    while (true) {

      JSONObject paramsIn = new JSONObject();
      paramsIn.put("tests", tests);
      paramsIn.put("run", uuid.toString());
      paramsIn.put("started", date.getTime());
      paramsIn.put("coordinates", new JSONArray(coordinates));

      Path tempFile = Files.createTempFile("params", ".json");
      System.out.println(coordinates.toString());
      try (BufferedWriter writer = new BufferedWriter(new FileWriter(tempFile.toFile()))) {
        writer.write(paramsIn.toString(2));
      }

      List<String> commands = new ArrayList<>(baseCommands);
      commands.add("--other-files");
      commands.add("/sdcard/params.json=" + tempFile.toString());

      if (paramsIn.has("serviceBlocker")) {
        if (grabberCopy == null) {
          grabberCopy = Files.createTempFile("grabber", ".apk");
          Files.copy(grabberPath, grabberCopy, REPLACE_EXISTING);
        }
        commands.add("--additional-apks");
        commands.add(grabberCopy.toString());
      }

      String text = Utils.execute(commands.toArray(new String[0]));
      System.out.println(text);

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

  private static Collection<String> getDevices() throws IOException {
    Collection<String> devicesOut = new ArrayList<>();
    String[] args1 = {
      GCLOUD_EXECUTABLE.toString(),
      "beta", "firebase", "test", "android", "models", "list",
      "--format", "json"
    };

    String text = Utils.execute(args1);
    JSONTokener jsonTokener = new JSONTokener(text);
    JSONArray devices = new JSONArray(jsonTokener);
    int count = 0;
    for (int idx = 0; idx != devices.length(); idx++) {
      JSONObject device = devices.getJSONObject(idx);
      if (!device.getString("formFactor").equals("PHONE")) {
        continue;
      }
      if (!device.getString("form").equals("PHYSICAL")) {
        continue;
      }

      JSONArray supportedVersionIds = device.getJSONArray("supportedVersionIds");
      for (int idx2 = 0; idx2 != supportedVersionIds.length(); idx2++) {
        int versionId = supportedVersionIds.getInt(idx2);
        if (versionId < MIN_VERSION) {
          continue;
        }
        devicesOut.add("--device");
        devicesOut.add(String.format("model=%s,version=%d", device.getString("id"), versionId));
        count++;
        if (count >= MAX_DEVICES) {
          return devicesOut;
        }
      }
    }
    return devicesOut;
  }
}
