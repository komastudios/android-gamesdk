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

import static android.view.KeyEvent.ACTION_DOWN;
import static android.view.KeyEvent.KEYCODE_BACK;

import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.provider.MediaStore;
import android.util.Log;
import android.view.KeyEvent;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * This host activity is particularly required for functional test F1.20. Upon onTrimMemory() call,
 * triggers the camera app via intent which, as it was documented by community threads, produced a
 * RenderThread crash on some, not-yet-patched Nougat devices.
 */
public class NougatSigabrtHostActivity extends GLSurfaceViewHostActivity {

    public static final String ID = "NougatSigabrtHostActivity";
    private static final String TAG = NougatSigabrtHostActivity.class.getSimpleName();

    private static final int REQUEST_IMAGE_CAPTURE = 1;
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
                startCameraActivityBriefly();
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
    private void startCameraActivityBriefly() {
        Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        if (takePictureIntent.resolveActivity(getPackageManager()) != null) {
            new Handler(Looper.getMainLooper()).postDelayed(() -> {
                Log.i("NougatSigabrtOperation", "Back simulation");
                onBackPressed();
                dispatchKeyEvent(new KeyEvent(ACTION_DOWN, KEYCODE_BACK));
            }, 5000);

            startActivityForResult(takePictureIntent, REQUEST_IMAGE_CAPTURE);
        }
    }

}
