package com.google.android.apps.internal.games.memorytest;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

/**
 * A helper class with static methods to help with Heuristics and file IO
 */
public class MapUtils {
  private static final String TAG = MapUtils.class.getSimpleName();

  /**
   * Selects the parameters for a run based on the 'tests' and 'coordinates' of the test
   * specification file.
   *
   * @param spec The test specification file.
   * @return The selected parameters.
   */
  public static Map<String, Object> flattenParams(Map<String, Object> spec) {
    Map<String, Object> params = new HashMap<>();

    List<Object> coordinates = (List<Object>) spec.get("coordinates");
    List<Object> tests = (List<Object>) spec.get("tests");

    for (int coordinateNumber = 0; coordinateNumber != coordinates.size(); coordinateNumber++) {
      List<Object> jsonArray = (List<Object>) tests.get(coordinateNumber);
      Map<String, Object> jsonObject = (Map<String, Object>) jsonArray.get(
          ((Number) coordinates.get(coordinateNumber)).intValue());
      merge(jsonObject, params);
    }
    return params;
  }

  /**
   * Creates a deep union of two maps. Arrays are concatenated and dictionaries are merged.
   *
   * @param in  The first map; read only.
   * @param out The second map and the object into which changes are written.
   */
  private static void merge(Map<String, Object> in, Map<String, Object> out) {
    for (Map.Entry<String, Object> entry : in.entrySet()) {
      String key = entry.getKey();
      Object inObject = entry.getValue();
      Object outObject = out.get(key);
      if (outObject == null) {
        out.put(key, clone(inObject));
        continue;
      }
      if (inObject instanceof List && outObject instanceof List) {
        ((Collection<Object>) outObject).addAll((Collection<?>) clone(inObject));
        continue;
      }
      if (inObject instanceof Map && outObject instanceof Map) {
        merge((Map<String, Object>) inObject, ((Map<String, Object>) outObject));
        continue;
      }
    }
  }

  /**
   * Return a deep-clone of a JSON-compatible object tree.
   * @param in The tree to clone.
   * @param <T> The cloned tree.
   */
  public static <T> T clone(T in) {
    if (in instanceof Map) {
      Map<String, Object> map = new LinkedHashMap<>();
      for (Map.Entry<String, Object> entry : ((Map<String, Object>) in).entrySet()) {
        map.put(entry.getKey(), clone(entry.getValue()));
      }
      return (T) map;
    }

    if (in instanceof List) {
      Collection<Object> list = new ArrayList<>();
      for (Object obj : (Iterable<Object>) in) {
        list.add(clone(obj));
      }
      return (T) list;
    }
    return in;
  }
}
