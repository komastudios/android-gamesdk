package net.jimblackler.istresser;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.security.InvalidParameterException;
import org.json.JSONException;
import org.json.JSONObject;

public class Utils {

  /**
   * Loads all the text from an input string and returns the result as a string.
   *
   * @param inputStream The stream to read.
   * @return All of the text from the stream.
   * @throws IOException Thrown if a read error occurs.
   */
  static String readStream(InputStream inputStream) throws IOException {
    try (InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
        BufferedReader reader = new BufferedReader(inputStreamReader)) {
      String newline = System.getProperty("line.separator");
      StringBuilder output = new StringBuilder();
      String line;
      while ((line = reader.readLine()) != null) {
        if (output.length() > 0) {
          output.append(newline);
        }
        output.append(line);
      }
      return output.toString();
    }
  }

  /**
   * Loads all text from the specified file and returns the result as a string.
   *
   * @param filename The name of the file to read.
   * @return All of the text from the file.
   * @throws IOException Thrown if a read error occurs.
   */
  static String readFile(String filename) throws IOException {
    return readStream(new FileInputStream(filename));
  }

  static long getFileSize(String filename) throws IOException {
    return new File(filename).length();
  }

  /**
   * Converts a memory quantity value in a JSON object to a number of bytes.
   * If no value exists with that key, zero is returned.
   * If the value is a JSON number, it is interpreted as the number of bytes.
   * If the value is a JSON string, it is converted according to the specified unit.
   * e.g. "36K", "52.5 M", "9.1G". No unit is interpreted as bytes.
   *
   * @param jsonObject The JSON object to extract from.
   * @param key The name of the key.
   * @return The equivalent number of bytes.
   */
  static long getMemoryQuantity(JSONObject jsonObject, String key) {
    if (!jsonObject.has(key)) {
      return 0;
    }
    try {
      return jsonObject.getLong(key);
    } catch (JSONException e) {
      // Ignored by design.
    }

    String str;
    try {
      str = jsonObject.getString(key);
    } catch (JSONException e) {
      throw new NumberFormatException(e.toString());
    }

    int unitPosition = str.indexOf('K');
    int unitMultiplier = 1024;
    if (unitPosition == -1) {
      unitPosition = str.indexOf('M');
      unitMultiplier *= 1024;
      if (unitPosition == -1) {
        unitPosition = str.indexOf('G');
        unitMultiplier *= 1024;
        if (unitPosition == -1) {
          unitMultiplier = 1;
        }
      }
    }
    float value = Float.parseFloat(str.substring(0, unitPosition));
    return (long) (value * unitMultiplier);
  }
}
