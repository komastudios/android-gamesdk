package com.google.androidgamesdk.gameinput;

import android.view.inputmethod.EditorInfo;

// Settings for InputConnection
public final class Settings {
  EditorInfo mEditorInfo;
  boolean mForwardKeyEvents;

  public Settings(EditorInfo editorInfo, boolean forwardKeyEvents) {
    mEditorInfo = editorInfo;
    mForwardKeyEvents = forwardKeyEvents;
  }
}
