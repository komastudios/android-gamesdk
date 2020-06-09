package net.jimblackler.istresser;

import android.util.Log;

import java.io.File;
import java.io.IOException;

import java.util.Iterator;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/** A helper class with static methods to help with Heuristics and file IO */
public class Utils {

  private static final String TAG = Utils.class.getSimpleName();

  /**
   * Gets the size in bytes of the indicated file.
   *
   * @param filename The name of the file to get the size for.
   * @return File size.
   * @throws IOException Thrown if a filesystem error occurs.
   */
  static long getFileSize(String filename) throws IOException {
    return new File(filename).length();
  }

  /**
   * Converts a memory quantity value in a JSON object to a number of bytes. If no value exists with
   * that key, zero is returned. If the value is a JSON number, it is interpreted as the number of
   * bytes. If the value is a JSON string, it is converted according to the specified unit. e.g.
   * "36K", "52.5 M", "9.1G". No unit is interpreted as bytes.
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
      Log.v(TAG, key + " is not a number.");
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

  /**
   * Return the first index where two strings differ.
   * @param a The first string to compare.
   * @param b The second string to compare.
   * @return The first index where the two strings have a different character, or either terminate.
   */
  static int mismatchIndex(CharSequence a, CharSequence b) {
    int index = 0;
    while (true) {
      if (index >= a.length() || index >= b.length()) {
        return index;
      }
      if (a.charAt(index) != b.charAt(index)) {
        return index;
      }
      index++;
    }
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
}
