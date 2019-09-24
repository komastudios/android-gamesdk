package com.google.jnitestapp;

import android.content.Context;
import android.content.pm.PackageInfo;

public class NativeTest {
    public static native String run(Context ctx, byte[] sig, String sig_hex);
}
