package com.google.android.apps.internal.games.memoryadvice;

import android.content.Context;
import android.content.pm.FeatureInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;

/**
 * A class to provide information about the current Android build to an application in JSON
 * format.
 */
class BuildInfo {
  private static final String TAG = BuildInfo.class.getSimpleName();

  /**
   * Copies the Android build data into a JSON object.
   *
   * @param context The Android context.
   * @return A map containing the Android build data.
   */
  static Map<String, Object> getBuild(Context context) {
    Map<String, Object> build = new LinkedHashMap<>();
    build.put("fields", getStaticFields(Build.class));
    build.put("version", getStaticFields(Build.VERSION.class));
    build.put("features", getFeatures(context));
    build.put("library", getStaticFields(BuildConfig.class));
    build.put("system", getSystem());
    return build;
  }

  private static Map<String, Object> getSystem() {
    Map<String, Object> system = new LinkedHashMap<>();
    Properties properties = System.getProperties();
    for (String propertyName : properties.stringPropertyNames()) {
      system.put(propertyName, properties.getProperty(propertyName));
    }
    return system;
  }

  /**
   * Use reflection to copy all the static fields from the specified class into a JSON object.
   *
   * @param aClass The class to copy static fields from.
   * @return the fields in a map.
   */
  private static Map<String, Object> getStaticFields(Class<?> aClass) {
    Map<String, Object> object = new LinkedHashMap<>();
    for (Field field : aClass.getFields()) {
      if (!java.lang.reflect.Modifier.isStatic(field.getModifiers())) {
        continue;
      }
      try {
        object.put(field.getName(), field.get(null));
      } catch (IllegalAccessException e) {
        // Silent by design.
      }
    }
    return object;
  }

  /**
   * Convert the system features into a map.
   *
   * @param context The current context.
   * @return The system features in a map.
   */
  private static Map<String, Object> getFeatures(Context context) {
    Map<String, Object> features = new LinkedHashMap<>();
    PackageManager packageManager = context.getPackageManager();

    Map<String, Object> named = new LinkedHashMap<>();
    List<Object> unnamed = new ArrayList<>();

    for (FeatureInfo featureInfo : packageManager.getSystemAvailableFeatures()) {
      Map<String, Object> feature = new LinkedHashMap<>();
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
        unnamed.add(feature);
      } else {
        named.put(featureInfo.name, feature);
      }
    }
    features.put("named", named);
    features.put("unnamed", unnamed);

    return features;
  }
}
