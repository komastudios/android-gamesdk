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

// Load settings from assets/tuningfork/tuningfork_settings.bin.
// Ownership of @p settings is passed to the caller: call
//  TFSettings_Free to deallocate data stored in the struct.
// Returns TFERROR_OK and fills 'settings' if the file could be loaded.
// Returns TFERROR_NO_SETTINGS if the file was not found.
TFErrorCode TuningFork_findSettingsInApk(JNIEnv* env, jobject ctx, TFSettings* settings);

// Load fidelity params from assets/tuningfork/<filename>
// Ownership of @p fp is passed to the caller: call
//  CProtobufSerialization_Free to deallocate data stored in the struct.
TFErrorCode TuningFork_findFidelityParamsInApk(JNIEnv* env, jobject ctx,
                                               const char* filename,
                                               CProtobufSerialization* fp);

// Download fidelity parameters on a separate thread.
// A download thread is activated to retrieve fidelity params and retries are
//    performed until a download is successful or a timeout occurs.
// Downloaded params are stored locally and used in preference of default
//    params when the app is started in future.
// default_params is a protobuf serialization containing the fidelity params that
//  will be used if there is no download connection and there are no saved parameters.
// fidelity_params_callback is called with any downloaded params or with default /
//  saved params.
// initialTimeoutMs is the time to wait for an initial download. The fidelity_params_callback
//  will be called after this time with the default / saved params if no params
//  could be downloaded..
// ultimateTimeoutMs is the time after which to stop retrying the download.
void TuningFork_startFidelityParamDownloadThread(
                                      const CProtobufSerialization* default_params,
                                      ProtoCallback fidelity_params_callback,
                                      int initialTimeoutMs, int ultimateTimeoutMs);

// The TuningFork_init function will save fidelity params to a file
//  for use when a download connection is not available. With this function,
//  you can replace or delete any saved file. To delete the file, pass fps=NULL.
TFErrorCode TuningFork_saveOrDeleteFidelityParamsFile(JNIEnv* env, jobject context,
                                                      CProtobufSerialization* fps);

#ifdef __cplusplus
}
#endif
