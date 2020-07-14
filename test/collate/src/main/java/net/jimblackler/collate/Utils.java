package net.jimblackler.collate;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.FileAlreadyExistsException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Iterator;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * Class containing various utility methods used by tools in the package.
 */
class Utils {
  private static final PrintStream NULL_PRINT_STREAM = new PrintStream(new OutputStream() {
    @Override
    public void write(int b) {}
  });

  static String fileToString(String filename) throws IOException {
    return Files.readString(Paths.get("resources", filename));
  }

  static void copy(Path directory, String filename) throws IOException {
    Path fullPath = directory.resolve(filename);
    File file = fullPath.toFile();
    if (file.exists()) {
      file.delete();
    }
    try {
      Files.createDirectory(fullPath.getParent());
    } catch (FileAlreadyExistsException ex) {
      // Intentionally ignored.
    }
    Files.copy(Paths.get("resources", filename), fullPath);
  }

  private static String readStream(InputStream inputStream, PrintStream out) throws IOException {
    StringBuilder stringBuilder = new StringBuilder();
    try (BufferedReader bufferedReader =
             new BufferedReader(new InputStreamReader(inputStream, StandardCharsets.UTF_8))) {
      String separator = "";
      while (true) {
        String line = bufferedReader.readLine();
        if (line == null) {
          return stringBuilder.toString();
        }
        out.println(line);
        stringBuilder.append(separator);
        separator = System.lineSeparator();
        stringBuilder.append(line);
      }
    }
  }

  static String executeSilent(String... args) throws IOException {
    return execute(NULL_PRINT_STREAM, args);
  }

  static String execute(String... args) throws IOException {
    return execute(System.out, args);
  }

  static String execute(PrintStream out, String... args) throws IOException {
    out.println(String.join(" ", args));
    int sleepFor = 0;
    for (int attempt = 0; attempt != 10; attempt++) {
      Process process = new ProcessBuilder(args).start();

      String input = readStream(process.getInputStream(), out);
      String error = readStream(process.getErrorStream(), out);
      try {
        process.waitFor();
      } catch (InterruptedException e) {
        throw new RuntimeException(e);
      }
      int exit = process.exitValue();
      if (exit != 0) {
        if (error.contains("Broken pipe") || error.contains("Can't find service")) {
          sleepFor += 10;
          out.println(error);
          out.print("Retrying in " + sleepFor + " seconds... ");
          try {
            Thread.sleep(sleepFor * 1000L);
          } catch (InterruptedException e) {
            // Intentionally ignored.
          }
          out.println("done");
          continue;
        }
        throw new IOException(error);
      }
      return input;
    }
    throw new IOException("Maximum attempts reached");
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

  /**
   * Converts a time duration in an object to a number of milliseconds. If the object is a number,
   * it is interpreted as the number of milliseconds. If the value is a string, it is converted
   * according to the specified unit. e.g. "30s", "1H". No unit is interpreted as milliseconds.
   *
   * @param object The object to extract from.
   * @return The equivalent number of milliseconds.
   */
  static long getDuration(Object object) {
    if (object instanceof Number) {
      return ((Number) object).longValue();
    }

    if (object instanceof String) {
      String str = ((String) object).toUpperCase();
      int unitPosition = str.indexOf('S');
      int unitMultiplier = 1000;
      if (unitPosition == -1) {
        unitPosition = str.indexOf('M');
        unitMultiplier *= 60;
        if (unitPosition == -1) {
          unitPosition = str.indexOf('H');
          unitMultiplier *= 60;
          if (unitPosition == -1) {
            unitMultiplier = 1;
          }
        }
      }
      float value = Float.parseFloat(str.substring(0, unitPosition));
      return (long) (value * unitMultiplier);
    }
    throw new IllegalArgumentException("Input to getDuration neither string or number.");
  }

  /**
   * Return the value associated with the key in the given object. If the object does not define
   * the key, return the specified default value.
   * @param object The object to extract the value from.
   * @param key The key associated with the value.
   * @param defaultValue The value to return if the object does not specify the key.
   * @return The associated value, or the defaultValue if the object does not define the key.
   */
  static Object getOrDefault(JSONObject object, String key, Object defaultValue) {
    try {
      return object.get(key);
    } catch (JSONException e) {
      return defaultValue;
    }
  }
}
