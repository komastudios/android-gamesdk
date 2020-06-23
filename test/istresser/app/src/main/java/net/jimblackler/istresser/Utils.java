package net.jimblackler.istresser;

import java.util.Iterator;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/** A helper class with static methods to help with Heuristics and file IO */
public class Utils {
  private static final String TAG = Utils.class.getSimpleName();

  /**
   * Converts a memory quantity value in an object to a number of bytes. If the value is a number,
   * it is interpreted as the number of bytes. If the value is a string, it is converted according
   * to the specified unit. e.g. "36K", "52.5 m", "9.1G". No unit is interpreted as bytes.
   *
   * @param object The object to extract from.
   * @return The equivalent number of bytes.
   */
  static long getMemoryQuantity(Object object) {
    if (object instanceof Number) {
      return ((Number) object).longValue();
    }

    if (object instanceof String) {
      String str = ((String) object).toUpperCase();
      int unitPosition = str.indexOf('K');
      long unitMultiplier = 1024;
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
    throw new IllegalArgumentException("Input to getMemoryQuantity neither string or number.");
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
