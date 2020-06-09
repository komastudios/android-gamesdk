package net.jimblackler.collate;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Iterator;

/**
 * Class containing various utility methods used by tools in the package.
 */
class Utils {
  static String fileToString(String filename) throws IOException {
    return Files.readString(Paths.get("resources", filename));
  }

  static void copy(Path directory, String filename) throws IOException {
    Files.copy(Paths.get("resources", filename), directory.resolve(filename));
  }

  private static String readStream(InputStream inputStream) throws IOException {
    StringBuilder stringBuilder = new StringBuilder();
    try (BufferedReader bufferedReader =
        new BufferedReader(new InputStreamReader(inputStream, StandardCharsets.UTF_8))) {
      String separator = "";
      while (true) {
        String line = bufferedReader.readLine();
        if (line == null) {
          return stringBuilder.toString();
        }
        System.out.println(line);
        stringBuilder.append(separator);
        separator = System.lineSeparator();
        stringBuilder.append(line);
      }
    }
  }

  static String execute(String... args) throws IOException {
    System.out.println(String.join(" ", args));
    Process process = new ProcessBuilder(args).start();

    String input = readStream(process.getInputStream());
    String error = readStream(process.getErrorStream());
    try {
      process.waitFor();
    } catch (InterruptedException e) {
      throw new RuntimeException(e);
    }
    int exit = process.exitValue();
    if (exit != 0) {
      throw new IOException(error);
    }
    return input;
  }

  /**
   * Selects the parameters for a run based on the 'tests' and 'coordinates' of the test
   * specification file.
   * @param spec The test specification file.
   * @return The selected parameters.
   */
  static JSONObject flattenParams(JSONObject spec) {
    JSONObject params = new JSONObject();
    try {
      JSONArray coordinates = spec.getJSONArray("coordinates");
      JSONArray tests = spec.getJSONArray("tests");

      for (int coordinateNumber = 0; coordinateNumber != coordinates.length(); coordinateNumber++) {
        JSONArray jsonArray = tests.getJSONArray(coordinateNumber);
        JSONObject jsonObject = jsonArray.getJSONObject(coordinates.getInt(coordinateNumber));
        merge(jsonObject, params);
      }
    } catch (JSONException e) {
      throw new IllegalStateException(e);
    }
    return params;
  }

  /**
   * Creates a deep union of two JSON objects. Arrays are concatenated and dictionaries are merged.
   * @param in The first JSON object; read only.
   * @param out The second JSON object and the object into which changes are written.
   * @throws JSONException In the case of a JSON handling error.
   */
  private static void merge(JSONObject in, JSONObject out) throws JSONException {
    in = new JSONObject(in.toString());  // Clone to allow original (freely mutable) copies.
    Iterator<String> keys = in.keys();
    while (keys.hasNext()) {
      String key = keys.next();
      Object inObject = in.get(key);
      if (out.has(key)) {
        Object outObject = out.get(key);
        if (inObject instanceof JSONArray && outObject instanceof JSONArray) {
          JSONArray inArray = (JSONArray) inObject;
          JSONArray outArray = (JSONArray) outObject;
          for (int idx = 0; idx != inArray.length(); idx++) {
            outArray.put(inArray.get(idx));
          }
          continue;
        }
        if (inObject instanceof JSONObject && outObject instanceof JSONObject) {
          merge((JSONObject) inObject, (JSONObject) outObject);
          continue;
        }
      }
      out.put(key, inObject);
    }
  }

  static String getProjectId() throws IOException {
    return execute(Config.GCLOUD_EXECUTABLE.toString(), "config", "get-value", "project");
  }
}
