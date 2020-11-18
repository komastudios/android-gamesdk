package com.google.android.apps.internal.games.memoryadvice;

import org.json.JSONObject;

public interface ReadyHandler {
  default void stressTestProgress(JSONObject metrics) {}
  void onComplete();
}
