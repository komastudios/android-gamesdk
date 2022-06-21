package com.google.jnitestapp;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.support.test.InstrumentationRegistry;
import android.support.test.runner.AndroidJUnit4;
import android.util.Log;

import org.junit.Test;
import org.junit.runner.RunWith;

import com.google.common.io.BaseEncoding;

import java.security.MessageDigest;

import static org.junit.Assert.*;

/**
 * Instrumented test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class TuningForkJNIInstrumentedTest {
    // Used to load app's native library on application startup.
    static {
        System.loadLibrary("JNITestApp");
    }
    static String TAG = "TuningForkJNIInstrumentedTest";

    @Test
    public void useAppContext() {
        // Context of the app under test.
        Context appContext = InstrumentationRegistry.getTargetContext();

        try {
            PackageInfo pi = appContext.getPackageManager().getPackageInfo(appContext.getPackageName(),
                    PackageManager.GET_SIGNATURES);
            Signature[] sigs = pi.signatures;
            assertEquals(sigs.length, 1);
            MessageDigest md = MessageDigest.getInstance("SHA1");
            byte[] sig = sigs[0].toByteArray();
            String hex_sig = BaseEncoding.base16().lowerCase().encode(md.digest(sig));
            String errors = NativeTest.run(appContext, sig, hex_sig);
            assertEquals("", errors);
        }
        catch (Exception e) {
            Log.i(TAG, "useAppContext: " + e.getLocalizedMessage());
        }

    }
}
