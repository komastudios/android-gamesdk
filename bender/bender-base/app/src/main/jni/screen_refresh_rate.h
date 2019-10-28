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
#pragma once
#include <jni.h>
#include <vector>

#define SCREEN_REFRESH_RATE 60

struct DisplayMode {
    int id;
    int width;
    int height;
    int screenRefreshRate;
};
int getScreenRefreshRate(JNIEnv *env, jobject activity);
// setRefreshRate must be called from the UI thread
void setScreenRefreshRateMainThread(JNIEnv *env, jobject activity, int refreshRate);
void setScreenRefreshRate(JNIEnv *env, jobject activity, int refreshRate);
