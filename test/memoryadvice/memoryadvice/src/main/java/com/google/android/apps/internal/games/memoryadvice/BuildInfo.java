package com.google.android.apps.internal.games.memoryadvice;

import android.os.Build;
import android.util.Log;
import java.lang.reflect.Field;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * A class to provide information about the current Android build to an application in JSON
 * format.
 */
class BuildInfo {
  private static final String TAG = BuildInfo.class.getSimpleName();
  /**
   * Copies the Android build data into a JSON object.
   * @return A JSONObject containing the Android build data.
   */
  static JSONObject getBuild() {
    JSONObject build = new JSONObject();
    try {
      build.put("fields", getStaticFields(Build.class));
      build.put("version", getStaticFields(Build.VERSION.class));
    } catch (JSONException ex) {
      Log.w(TAG, "Problem getting build data", ex);
    }
    return build;
  }

  /**
   * Use reflection to copy all the static fields from the specified class into a JSON object.
   * @param aClass The class to copy static fields from.
   * @return the fields in a JSONObject.
   */
  private static JSONObject getStaticFields(Class<?> aClass) throws JSONException {
    JSONObject object = new JSONObject();
    for (Field field : aClass.getFields()) {
      if (!java.lang.reflect.Modifier.isStatic(field.getModifiers())) {
        continue;
      }
      try {
        object.put(field.getName(), JSONObject.wrap(field.get(null)));
      } catch (IllegalAccessException e) {
        // Silent by design.
      }
    }
    return object;
  }
}
