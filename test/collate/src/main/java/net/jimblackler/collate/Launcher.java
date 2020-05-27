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

  private static final int MIN_VERSION = 26;
  private static final int MAX_DEVICES = Integer.MAX_VALUE;
  private static final String STANDARD_APK_PATH = "app/build/outputs/apk/debug/app-debug.apk";

  public static void main(String[] args) throws IOException {

    UUID uuid = UUID.randomUUID();
    Date date = new Date();

    Path grabberPath = Config.GRABBER_BASE.resolve(STANDARD_APK_PATH);

    List<String> baseCommands = new ArrayList<>(Arrays.asList(
        Config.GCLOUD_EXECUTABLE.toString(),
        "beta", "firebase", "test", "android", "run",
        "--results-history-name", uuid.toString(),
        "--type", "game-loop",
        "--async",
        "--format", "json"));

    Collection<String> devices = getDevices();
    System.out.println(devices);

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

      JSONObject flattened = Utils.flattenParams(paramsIn);
      commands.add("--app=" +
          Path.of(flattened.getString("apk_base"), flattened.getString("apk_name")));

      if (flattened.has("firebase")) {
        JSONObject firebase = flattened.getJSONObject("firebase");
        for (String key : firebase.keySet()) {
          commands.add("--" + key + "=" + firebase.get(key));
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
      Config.GCLOUD_EXECUTABLE.toString(),
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
        devicesOut.add(
            String.format("--device=model=%s,version=%d", device.getString("id"), versionId));
        count++;
        if (count >= MAX_DEVICES) {
          return devicesOut;
        }
      }
    }
    return devicesOut;
  }
}
