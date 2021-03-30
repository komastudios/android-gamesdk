package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice_common.ConfigUtils.getOrDefault;
import static com.google.android.apps.internal.games.memoryadvice_common.StreamUtils.readStream;

import android.content.res.AssetManager;
import android.os.Build;
import android.util.Log;
import com.google.gson.Gson;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
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
   * Selects the device profile from all available.
   *
   * @param assets   The assert manager used to load files.
   * @param params   Configuration parameters (can affect selection method).
   * @param baseline This device's baseline metrics (can be used to aid selection).
   * @return The selected device profile, plus metadata.
   */
  static Map<String, Object> getDeviceProfile(
      AssetManager assets, Map<String, Object> params, Map<String, Object> baseline) {
    Map<String, Object> profile = new LinkedHashMap<>();

    Map<String, Object> lookup = null;
    try {
      lookup = new Gson().fromJson(readStream(assets.open("memoryadvice/lookup.json")), Map.class);

      String matchStrategy = getOrDefault(params, "matchStrategy", "fingerprint");
      String best;
      if ("fingerprint".equals(matchStrategy)) {
        best = matchByFingerprint(lookup);
      } else if ("baseline".equals(matchStrategy)) {
        best = matchByBaseline(lookup, baseline);
      } else {
        throw new IllegalStateException("Unknown match strategy " + matchStrategy);
      }
      profile.put("limits", lookup.get(best));
      profile.put("matched", best);
      profile.put("fingerprint", Build.FINGERPRINT);
    } catch (IOException e) {
      Log.w("Profile problem.", e);
    }
    return profile;
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
      for (String groupName : prospectBaseline.keySet()) {
        if (!baseline.containsKey(groupName)) {
          continue;
        }
        Map<String, Object> prospectBaselineGroup =
            (Map<String, Object>) prospectBaseline.get(groupName);
        Map<String, Object> baselineGroup = (Map<String, Object>) baseline.get(groupName);
        for (String metric : prospectBaselineGroup.keySet()) {
          if (!baselineGroup.containsKey(metric)) {
            continue;
          }
          totalUnion++;
          SortedSet<Long> values = baselineValuesTable.get(metric);
          int prospectPosition =
              getPositionInList(values, ((Number) prospectBaselineGroup.get(metric)).longValue());
          int ownPosition =
              getPositionInList(values, ((Number) baselineGroup.get(metric)).longValue());
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
    for (String device : lookup.keySet()) {
      Map<String, Object> limits = (Map<String, Object>) lookup.get(device);
      Map<String, Object> prospectBaseline = (Map<String, Object>) limits.get("baseline");
      for (String groupName : prospectBaseline.keySet()) {
        Map<String, Object> group = (Map<String, Object>) prospectBaseline.get(groupName);
        for (String metric : group.keySet()) {
          if (!table.containsKey(metric)) {
            table.put(metric, new TreeSet<Long>());
          }
          table.get(metric).add(((Number) group.get(metric)).longValue());
        }
      }
    }
    return table;
  }
}
