package com.google.sample.tunnel;

import com.google.androidgamesdk.GameActivity;

public class EndlessTunnelActivity extends GameActivity {
    // Some code to load our native library:
    static {
        // Load the STL first to workaround issues on old Android versions:
        // "if your app targets a version of Android earlier than Android 4.3 (Android API level 18),
        // and you use libc++_shared.so, you must load the shared library before any other
        // library that depends on it."
        // See https://developer.android.com/ndk/guides/cpp-support#shared_runtimes
        System.loadLibrary("c++_shared");

        // Load the game library:
        System.loadLibrary("game");
    }
}
