package com.google.android.apps.internal.games.memoryadvice;

import android.content.Context;
import android.content.pm.FeatureInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.util.Log;
import java.lang.reflect.Field;
import org.json.JSONArray;
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
   * @param context The Android context.
   * @return A JSONObject containing the Android build data.
   */
  static JSONObject getBuild(Context context) {
    JSONObject build = new JSONObject();
    try {
      getStaticFields(build, Build.class);
      JSONObject version = new JSONObject();
      getStaticFields(version, Build.VERSION.class);
      build.put("version", version);
      build.put("features", getFeatures(context));
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

  /**
   * Convert the system features into a JSONObject.
   * @param context The current context.
   * @return The system features in a JSONObject.
   */
  private static JSONObject getFeatures(Context context) {
    JSONObject features = new JSONObject();
    PackageManager packageManager = context.getPackageManager();
    try {
      JSONObject named = new JSONObject();
      JSONArray unnamed = new JSONArray();

      for (FeatureInfo featureInfo : packageManager.getSystemAvailableFeatures()) {
        JSONObject feature = new JSONObject();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N && featureInfo.version != 0) {
          feature.put("version", featureInfo.version);
        }
        if (featureInfo.flags != 0) {
          feature.put("flags", featureInfo.flags);
        }
        if (featureInfo.reqGlEsVersion != FeatureInfo.GL_ES_VERSION_UNDEFINED) {
          feature.put("reqGlEsVersion", featureInfo.reqGlEsVersion);
        }
        if (featureInfo.name == null) {
          unnamed.put(feature);
        } else {
          named.put(featureInfo.name, feature);
        }
      }
      features.put("named", named);
      features.put("unnamed", unnamed);
    } catch (JSONException ex) {
      Log.w(TAG, "Problem getting features", ex);
    }
    return features;
  }
}
