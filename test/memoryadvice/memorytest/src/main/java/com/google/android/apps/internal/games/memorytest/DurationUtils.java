package com.google.android.apps.internal.games.memorytest;

/**
 * A helper class with static methods to help with Heuristics and file IO
 */
public class DurationUtils {
  private static final String TAG = DurationUtils.class.getSimpleName();

  /**
   * Converts a time duration in an object to a number of milliseconds. If the object is a number,
   * it is interpreted as the number of milliseconds. If the value is a string, it is converted
   * according to the specified unit. e.g. "30s", "1H". No unit is interpreted as milliseconds.
   *
   * @param object The object to extract from.
   * @return The equivalent number of milliseconds.
   */
  public static long getDuration(Object object) {
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
}
