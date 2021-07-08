package com.google.android.apps.internal.games.memoryadvice;

import android.os.Build;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.SortedSet;
import java.util.TreeSet;

class DeviceProfile {
  /**
   * Return the first index where two strings differ.
   *
   * @param a The first string to compare.
   * @param b The second string to compare.
   * @return The first index where the two strings have a different character, or either terminate.
   */
  private static int mismatchIndex(CharSequence a, CharSequence b) {
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
   * This method finds the device with the most similar fingerprint string.
   *
   * @param lookup The lookup table.
   * @return The selected device.
   */
  private static String matchByFingerprint(Map<String, Object> lookup) {
    int bestScore = -1;
    String best = null;
    for (String key : lookup.keySet()) {
      int score = mismatchIndex(Build.FINGERPRINT, key);
      if (score > bestScore) {
        bestScore = score;
        best = key;
      }
    }
    return best;
  }

  /**
   * this method finds the device with the most similar baseline metrics.
   *
   * @param lookup   The lookup table.
   * @param baseline The current device metrics baseline.
   * @return The selected device.
   */
  private static String matchByBaseline(Map<String, Object> lookup, Map<String, Object> baseline) {
    Map<String, SortedSet<Long>> baselineValuesTable = buildBaselineValuesTable(lookup);

    float bestScore = Float.MAX_VALUE;
    String best = null;

    for (Map.Entry<String, Object> entry : lookup.entrySet()) {
      String key = entry.getKey();
      Map<String, Object> limits = (Map<String, Object>) entry.getValue();
      Map<String, Object> prospectBaseline = (Map<String, Object>) limits.get("baseline");

      float totalScore = 0;
      int totalUnion = 0;
      for (Map.Entry<String, Object> e : prospectBaseline.entrySet()) {
        Map<String, Object> prospectBaselineGroup = (Map<String, Object>) e.getValue();
        Map<String, Object> baselineGroup = (Map<String, Object>) baseline.get(e.getKey());
        if (baselineGroup == null) {
          break;
        }
        for (Map.Entry<String, Object> entry1 : prospectBaselineGroup.entrySet()) {
          String metric = entry1.getKey();
          Number baselineMetric = (Number) baselineGroup.get(metric);
          if (baselineMetric == null) {
            continue;
          }
          totalUnion++;
          SortedSet<Long> values = baselineValuesTable.get(metric);
          int prospectPosition =
              getPositionInList(values, ((Number) entry1.getValue()).longValue());
          int ownPosition = getPositionInList(values, baselineMetric.longValue());
          float score = (float) Math.abs(prospectPosition - ownPosition) / values.size();
          totalScore += score;
        }
      }

      if (totalUnion > 0) {
        totalScore /= totalUnion;
      }

      if (totalScore < bestScore) {
        bestScore = totalScore;
        best = key;
      }
    }
    return best;
  }

  /**
   * Finds the position of the first value exceeding the supplied value, in a sorted list.
   *
   * @param values The sorted list.
   * @param value  The value to find the position of.
   * @return the position of the first value exceeding the supplied value.
   */
  private static int getPositionInList(Iterable<Long> values, long value) {
    Iterator<Long> it = values.iterator();
    int count = 0;
    while (it.hasNext()) {
      if (it.next() > value) {
        break;
      }
      count++;
    }
    return count;
  }

  /**
   * For each metric in the lookup table, get a sorted list of values as seen for devices' baseline
   * values.
   *
   * @param lookup The device lookup table.
   * @return A dictionary of sorted values indexed by metric name.
   */
  private static Map<String, SortedSet<Long>> buildBaselineValuesTable(Map<String, Object> lookup) {
    Map<String, SortedSet<Long>> table = new HashMap<>();
    for (Object o : lookup.values()) {
      Map<String, Object> limits = (Map<String, Object>) o;
      Map<String, Object> prospectBaseline = (Map<String, Object>) limits.get("baseline");
      if (prospectBaseline == null) {
        continue;
      }
      for (Object o1 : prospectBaseline.values()) {
        Map<String, Object> group = (Map<String, Object>) o1;
        for (Map.Entry<String, Object> entry : group.entrySet()) {
          String metric = entry.getKey();
          SortedSet<Long> metricValues = table.get(metric);
          if (metricValues == null) {
            metricValues = new TreeSet<>();
            table.put(metric, metricValues);
          }
          metricValues.add(((Number) entry.getValue()).longValue());
        }
      }
    }
    return table;
  }
}
