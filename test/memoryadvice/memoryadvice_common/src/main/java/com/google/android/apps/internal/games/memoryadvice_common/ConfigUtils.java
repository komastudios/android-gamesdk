package com.google.android.apps.internal.games.memoryadvice_common;

import java.util.Map;

public class ConfigUtils {
  /**
   * Converts a memory quantity value in an object to a number of bytes. If the value is a number,
   * it is interpreted as the number of bytes. If the value is a string, it is converted according
   * to the specified unit. e.g. "36K", "52.5 m", "9.1G". No unit is interpreted as bytes.
   *
   * @param object The object to extract from.
   * @return The equivalent number of bytes.
   */
  public static long getMemoryQuantity(Object object) {
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
   * Return the value associated with the key in the given map. If the map does not define
   * the key, return the specified default value.
   * @param object The map to extract the value from.
   * @param key The key associated with the value.
   * @param defaultValue The value to return if the map does not specify the key.
   * @param <T> The parameter type.
   * @return The associated value, or the defaultValue if the object does not define the key.
   */
  public static <T> T getOrDefault(Map<String, Object> object, String key, T defaultValue) {
    if (object.containsKey(key)) {
      return (T) object.get(key);
    }
    return defaultValue;
  }
}