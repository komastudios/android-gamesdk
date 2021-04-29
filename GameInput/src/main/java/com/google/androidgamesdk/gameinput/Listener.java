package com.google.androidgamesdk.gameinput;

// Listener for text, selection and composing region changes.
public interface Listener {
  void stateChanged(State newState, boolean dismissed);
}
