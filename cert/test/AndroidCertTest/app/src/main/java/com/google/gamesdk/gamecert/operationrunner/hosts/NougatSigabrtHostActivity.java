/*
 * Copyright 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.google.gamesdk.gamecert.operationrunner.hosts;

import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.provider.MediaStore;
import android.util.Log;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * This host activity is particularly required for functional test F1.20. Upon onTrimMemory() call,
 * triggers the camera app via intent which, as it was documented by community threads, produced a
 * RenderThread crash on some, not-yet-patched Nougat devices.
 */
public class NougatSigabrtHostActivity extends GLSurfaceViewHostActivity {

    public static final String ID = "NougatSigabrtHostActivity";
    // Please, if changing this TAG, change it also in NougatSigabrtOperation so that both log
    // closely related messages under the same TAG filter. E.g.,
    // $ adb logcat -s NougatCrashTest:I
    private static final String TAG = "NougatCrashTest";

    private final Lock _lock = new ReentrantLock();

    private boolean cameraAlreadyStarted = false;

    /**
     * Triggers the camera up if memory is running low. Does it only once per test.
     */
    @Override
    public void onTrimMemory(int level) {
        super.onTrimMemory(level);

        _lock.lock();
        try {
            if (false == cameraAlreadyStarted) {
                cameraAlreadyStarted = true;
                startCamera();
            }
        } finally {
            _lock.unlock();
        }
    }

    /**
     * Starts the camera app via ACTION_IMAGE_CAPTURE intent. A few seconds later will issue a back
     * button press in order to finish the test. That's enough time for the crash (if happened at
     * all) to occur and get captured by NougatSigabrtOperation.
     */
    private void startCamera() {
        Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        if (takePictureIntent.resolveActivity(getPackageManager()) != null) {
            new Handler(Looper.getMainLooper()).postDelayed(() -> {
                stopDataGathererAndStressors();
                Log.i(TAG, "Data gatherer and stressors stopped");
            }, 5000);
            startActivity(takePictureIntent);
            Log.i(TAG, "Camera launched. Nougat crash, if at all, should happen next");
        }
    }

}
