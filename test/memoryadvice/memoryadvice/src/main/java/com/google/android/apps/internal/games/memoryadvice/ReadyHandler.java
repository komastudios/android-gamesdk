package com.google.android.apps.internal.games.memoryadvice;

import java.util.Map;

public interface ReadyHandler {
  default void stressTestProgress(Map<String, Object> metrics) {}
  void onComplete(boolean timedOut);
}
