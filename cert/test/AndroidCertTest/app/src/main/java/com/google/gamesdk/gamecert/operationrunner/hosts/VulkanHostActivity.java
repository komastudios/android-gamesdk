/*
 * Copyright 2019 The Android Open Source Project
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

import android.content.Context;
import android.content.Intent;

import android.os.Build.VERSION;
import android.os.Build.VERSION_CODES;
import com.google.gamesdk.R;
import com.google.gamesdk.gamecert.operationrunner.transport.Configuration;

public class VulkanHostActivity extends BaseHostActivity {

    public static final String ID = "VulkanHostActivity";
    public static final String TAG = "VulkanHostActivity";

    public static Intent createIntent(Context ctx,
                                      Configuration.StressTest stressTest) {
        Intent i = new Intent(ctx, VulkanHostActivity.class);
        i.putExtra(STRESS_TEST_JSON, stressTest.toJson());
        return i;
    }

    public static boolean isSupported() {
        return (VERSION.SDK_INT >= VERSION_CODES.N);
    }


    @Override
    protected int getContentViewResource() {
        // TODO (dagum): rename the "swappy" part to something more neutral, so
        //               Vulkan doesn't look like "borrowing" something that
        //               doesn't belong exclusively to SwappyGLHostActivity.
        return R.layout.activity_swappy_surface;
    }

}
