package com.google.android.apps.internal.games.memoryadvice;

import java.util.Map;

class JsonUtils {
  /**
   * Performs a simple path lookup on a map.
   * @param path A path separated by forward slashes, and beginning with a forward slash.
   * @param object The object found in the object at the supplied path.
   */
  public static <T> T getFromPath(String path, Map<String, Object> object)
      throws MissingPathException {
    try {
      path = path.substring(1);
      int slash = path.indexOf('/');

      if (slash == -1) {
        if (!object.containsKey(path)) {
          throw new MissingPathException();
        }
        return (T) object.get(path);
      }
      String nextPart = path.substring(0, slash);
      Map<String, Object> nextObject = (Map<String, Object>) object.get(nextPart);
      if (nextObject == null) {
        throw new MissingPathException();
      }

      return getFromPath(path.substring(slash), nextObject);
    } catch (MissingPathException ex) {
      throw new MissingPathException("Missing path: " + path);
    }
  }
}
