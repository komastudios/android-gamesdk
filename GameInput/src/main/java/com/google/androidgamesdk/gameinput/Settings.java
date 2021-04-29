package com.google.androidgamesdk.gameinput;

import android.view.inputmethod.EditorInfo;

// Settings for InputConnection
public final class Settings {
    private final EditorInfo mEditorInfo;

    public Settings(EditorInfo editorInfo) {
        mEditorInfo = editorInfo;
    }

    public final EditorInfo getEditorInfo() {
        return mEditorInfo;
    }
}
