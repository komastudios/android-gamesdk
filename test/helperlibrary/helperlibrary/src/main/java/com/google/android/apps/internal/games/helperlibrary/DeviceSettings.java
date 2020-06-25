package com.google.android.apps.internal.games.helperlibrary;

import static com.google.android.apps.internal.games.helperlibrary.Utils.readStream;

import android.content.res.AssetManager;
import android.os.Build;
import android.util.Log;
import java.io.IOException;
import java.util.Iterator;
import org.json.JSONException;
import org.json.JSONObject;

public class DeviceSettings {
  /**
   * Return the first index where two strings differ.
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

  static JSONObject getDeviceSettings(AssetManager assets) {
    JSONObject settings = new JSONObject();
    try {
      JSONObject lookup = new JSONObject(readStream(assets.open("lookup.json")));
      int bestScore = -1;
      String best = null;
      Iterator<String> it = lookup.keys();
      while (it.hasNext()) {
        String key = it.next();
        int score = mismatchIndex(Build.FINGERPRINT, key);
        if (score > bestScore) {
          bestScore = score;
          best = key;
        }
      }
      settings.put("limits", lookup.getJSONObject(best));
      settings.put("matched", best);
      settings.put("fingerprint", Build.FINGERPRINT);
    } catch (JSONException | IOException e) {
      Log.w("Settings problem.", e);
    }
    return settings;
  }
}
