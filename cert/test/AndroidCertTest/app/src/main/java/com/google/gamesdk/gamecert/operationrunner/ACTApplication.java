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

package com.google.gamesdk.gamecert.operationrunner;

import android.app.Application;
import android.content.SharedPreferences;

public class ACTApplication extends Application {

    private static ACTApplication _instance = null;
    private static final String PREFS_KEY_IS_RUNNING_TEST = "MainActivity.isRunningTest";

    public static ACTApplication getInstance() {
        return _instance;
    }

    @Override
    public void onCreate() {
        _instance = this;
        super.onCreate();
    }

    public void setIsRunningTest(boolean isRunningTest) {
        getPreferences().edit().putBoolean(PREFS_KEY_IS_RUNNING_TEST, isRunningTest).commit();
    }

    public boolean isRunningTest() {
        return getPreferences().getBoolean(PREFS_KEY_IS_RUNNING_TEST, false);
    }

    private SharedPreferences getPreferences() {
        return getSharedPreferences(getClass().getSimpleName(), MODE_PRIVATE);
    }

}
