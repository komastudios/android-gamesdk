package com.google.android.apps.internal.games.memoryadvice;

import java.util.Map;

class JsonUtils {
  /**
   * Performs a simple path lookup on a map.
   * @param path A path separated by forward slashes, and beginning with a forward slash.
   * @param object The object found in the object at the supplied path.
   */
  public static <T> T getFromPath(String path, Map<String, Object> object) {
    path = path.substring(1);
    int slash = path.indexOf('/');

    if (slash == -1) {
      return (T) object.get(path);
    }
    return getFromPath(
        path.substring(slash), (Map<String, Object>) object.get(path.substring(0, slash)));
  }
}
