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
import java.util.stream.Collectors;

class Utils {
  static String fileToString(String filename) throws IOException {
    return Files.readString(Paths.get("resources", filename));
  }

  static void copy(Path directory, String filename) throws IOException {
    Files.copy(Paths.get("resources", filename), directory.resolve(filename));
  }

  private static String readStream(InputStream inputStream) throws IOException {
    try (BufferedReader bufferedReader =
        new BufferedReader(new InputStreamReader(inputStream, StandardCharsets.UTF_8))) {
      return bufferedReader.lines().collect(Collectors.joining(System.lineSeparator()));
    }
  }

  static String execute(String... args) throws IOException {
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

  static JSONObject flattenParams(JSONObject params1) {
    JSONObject params = new JSONObject();
    try {
      JSONArray coordinates = params1.getJSONArray("coordinates");
      JSONArray tests = params1.getJSONArray("tests");

      for (int coordinateNumber = 0; coordinateNumber != coordinates.length(); coordinateNumber++) {
        JSONArray jsonArray = tests.getJSONArray(coordinateNumber);
        JSONObject jsonObject = jsonArray.getJSONObject(coordinates.getInt(coordinateNumber));
        Iterator<String> keys = jsonObject.keys();
        while (keys.hasNext()) {
          String key = keys.next();
          params.put(key, jsonObject.get(key));
        }
      }
    } catch (JSONException e) {
      e.printStackTrace();
      System.out.println(params1.toString());
    }
    return params;
  }
}
