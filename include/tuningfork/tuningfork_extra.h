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

#include "tuningfork.h"

#ifdef __cplusplus
extern "C" {
#endif

bool TuningFork_findSettingsInAPK(JNIEnv* env, jobject activity, CProtobufSerialization* settings_ser);

// Initialize tuning fork and automatically inject tracers into Swappy if it is available.
// If Swappy is not available or could not be initialized, the function will return false.
// When using Swappy, there will be ? default tick points added and the histogram settings need to be
// coordinated with your swap rate (TODO: more detail).
// If you know where the shared library in which Swappy will be, you can pass it in libraryName (e.g. "libgamesdk.so").
// If libraryName is NULL or TuningFork cannot find Swappy in the library, it will look in the current module and then
// try the following libraries: [] (TODO: add)
bool TuningFork_initWithSwappy(const CProtobufSerialization* settings, JNIEnv* env, jobject activity,
                               const char* libraryName, void (*annotation_callback)());

// This function will be called on a separate thread every time TuningFork performs an upload.
void TuningFork_setUploadCallback(void(*cbk)(const CProtobufSerialization*));

#ifdef __cplusplus
}
#endif
