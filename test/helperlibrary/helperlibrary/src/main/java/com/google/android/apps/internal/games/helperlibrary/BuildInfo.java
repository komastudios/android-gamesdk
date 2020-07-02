package com.google.android.apps.internal.games.helperlibrary;

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
      getStaticFields(build, Build.class);
      JSONObject version = new JSONObject();
      getStaticFields(version, Build.VERSION.class);
      build.put("version", version);
    } catch (JSONException ex) {
      Log.w(TAG, "Problem getting build data", ex);
    }
    return build;
  }

  /**
   * Use reflection to copy all the static fields from the specified class into a JSON object.
   * @param object The JSONObject to receive the data.
   * @param aClass The class to copy static fields from.
   */
  private static void getStaticFields(JSONObject object, Class<?> aClass) throws JSONException {
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
  }
}
