package com.google.android.apps.internal.games.memoryadvice;

import org.json.JSONException;
import org.json.JSONObject;

class JsonUtils {
  /**
   * Performs a simple path lookup on a JSONObject.
   * @param path A path separated by forward slashes, and beginning with a forward slash.
   * @param object The object found in the object at the supplied path.
   */
  public static Object getFromPath(String path, JSONObject object) throws JSONException {
    path = path.substring(1);
    int slash = path.indexOf("/");

    if (slash == -1) {
      return object.get(path);
    }
    return getFromPath(path.substring(slash), (JSONObject) object.get(path.substring(0, slash)));
  }
}
