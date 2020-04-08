package com.google.android.apps.internal.games.helperlibrary;

import android.os.Build;

import org.json.JSONException;
import org.json.JSONObject;

import java.lang.reflect.Field;

/** A class to provide information about the current Android build to an application in JSON
 * format. */
public class Helper {

  /**
   * Copies the Android build data into a JSON object.
   * @return A JSONObject containing the Android build data.
   */
  public static JSONObject getBuild() throws JSONException {
    JSONObject build = new JSONObject();
    getStaticFields(build, Build.class);
    getStaticFields(build, Build.VERSION.class);
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
