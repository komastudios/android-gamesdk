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

#include "tuningfork/tuningfork_extra.h"
#include "tuningfork/protobuf_util.h"
#include "tuningfork_internal.h"
#include "tuningfork_utils.h"

#include <cinttypes>
#include <dlfcn.h>
#include <memory>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <thread>
#include <fstream>
#include <mutex>
#include <chrono>

#define LOG_TAG "TuningFork"
#include "Log.h"

#include <android/asset_manager_jni.h>
#include <jni.h>

namespace tuningfork {

ProtobufSerialization GetAssetAsSerialization(AAsset* asset) {
    ProtobufSerialization ser;
    uint64_t size = AAsset_getLength64(asset);
    ser.resize(size);
    memcpy(const_cast<uint8_t*>(ser.data()), AAsset_getBuffer(asset), size);
    return ser;
}

// Get the name of the tuning fork save file. Returns true if the directory
//  for the file exists and false on error.
bool GetSavedFileName(const JniCtx& jni, std::string& name) {

    // Create tuningfork/version folder if it doesn't exist
    std::stringstream tf_path_str;
    tf_path_str << file_utils::GetAppCacheDir(jni) << "/tuningfork";
    if (!file_utils::CheckAndCreateDir(tf_path_str.str())) {
        return false;
    }
    tf_path_str << "/V" << apk_utils::GetVersionCode(jni);
    if (!file_utils::CheckAndCreateDir(tf_path_str.str())) {
        return false;
    }
    tf_path_str << "/saved_fp.bin";
    name = tf_path_str.str();
    return true;
}

// Get a previously save fidelity param serialization.
bool GetSavedFidelityParams(const JniCtx& jni, ProtobufSerialization& params) {
    std::string save_filename;
    if (GetSavedFileName(jni, save_filename)) {
        CProtobufSerialization c_params;
        if (file_utils::LoadBytesFromFile(save_filename, &c_params)) {
            ALOGI("Loaded fps from %s (%u bytes)", save_filename.c_str(), c_params.size);
            params = ToProtobufSerialization(c_params);
            CProtobufSerialization_Free(&c_params);
            return true;
        }
        ALOGI("Couldn't load fps from %s", save_filename.c_str());
    }
    return false;
}

// Save fidelity params to the save file.
bool SaveFidelityParams(const JniCtx& jni, const ProtobufSerialization& params) {
    std::string save_filename;
    if (GetSavedFileName(jni, save_filename)) {
        std::ofstream save_file(save_filename, std::ios::binary);
        if (save_file.good()) {
            save_file.write(reinterpret_cast<const char*>(params.data()), params.size());
            ALOGI("Saved fps to %s (%zu bytes)", save_filename.c_str(), params.size());
            return true;
        }
        ALOGI("Couldn't save fps to %s", save_filename.c_str());
    }
    return false;
}

// Check if we have saved fidelity params.
bool SavedFidelityParamsFileExists(const JniCtx& jni) {
    std::string save_filename;
    if (GetSavedFileName(jni, save_filename)) {
        return file_utils::FileExists(save_filename);
    }
    return false;
}

static bool s_kill_thread = false;
static std::thread s_fp_thread;

// Download FPs on a separate thread
TFErrorCode StartFidelityParamDownloadThread(const ProtobufSerialization& default_params,
                                             ProtoCallback fidelity_params_callback,
                                             int initialTimeoutMs, int ultimateTimeoutMs) {
    if (fidelity_params_callback == nullptr) return TFERROR_BAD_PARAMETER;
    static std::mutex threadMutex;
    std::lock_guard<std::mutex> lock(threadMutex);
    if (s_fp_thread.joinable()) {
        ALOGW("Fidelity param download thread already started");
        return TFERROR_DOWNLOAD_THREAD_ALREADY_STARTED;
    }
    s_kill_thread = false;
    s_fp_thread = std::thread([=]() {
        ProtobufSerialization params;
        auto waitTime = std::chrono::milliseconds(initialTimeoutMs);
        bool first_time = true;
        while (!s_kill_thread) {
            auto startTime = std::chrono::steady_clock::now();
            auto err = GetFidelityParameters(default_params,
                                             params, waitTime.count());
            if (err==TFERROR_OK) {
                ALOGI("Got fidelity params from server");
                SaveFidelityParams(tuningfork::GetJniCtx(), params);
                CProtobufSerialization cpbs;
                ToCProtobufSerialization(params, cpbs);
                fidelity_params_callback(&cpbs);
                CProtobufSerialization_Free(&cpbs);
                break;
            } else {
                ALOGI("Could not get fidelity params from server : err = %d", err);
                if (first_time) {
                    CProtobufSerialization cpbs;
                    ToCProtobufSerialization(default_params, cpbs);
                    fidelity_params_callback(&cpbs);
                    CProtobufSerialization_Free(&cpbs);
                    first_time = false;
                }
                // Wait if the call returned earlier than expected
                auto dt = std::chrono::steady_clock::now() - startTime;
                if(waitTime>dt) std::this_thread::sleep_for(waitTime - dt);
                if (waitTime.count() > ultimateTimeoutMs) {
                    ALOGW("Not waiting any longer for fidelity params");
                    break;
                }
                waitTime *= 2; // back off
            }
        }
    });
    return TFERROR_OK;
}

// Load fidelity params from assets/tuningfork/<filename>
// Ownership of serializations is passed to the caller: call
//  CProtobufSerialization_Free to deallocate any memory.
TFErrorCode FindFidelityParamsInApk(const JniCtx& jni,
                                    const std::string& filename,
                                    ProtobufSerialization& fp) {
    std::stringstream full_filename;
    full_filename << "tuningfork/" << filename;
    AAsset* a = apk_utils::GetAsset(jni, full_filename.str().c_str());
    if (a==nullptr) {
        ALOGE("Can't find %s", full_filename.str().c_str());
        return TFERROR_NO_FIDELITY_PARAMS;
    }
    ALOGI("Using file %s for default params", full_filename.str().c_str());
    fp = GetAssetAsSerialization(a);
    AAsset_close(a);
    return TFERROR_OK;
}

std::unique_ptr<ProtobufSerialization> GetTrainingParams(const Settings& settings) {
    std::unique_ptr<ProtobufSerialization> training_params;
    auto cpbs = settings.c_settings.training_fidelity_params;
    if (cpbs != nullptr) {
        training_params = std::make_unique<ProtobufSerialization>(ToProtobufSerialization(*cpbs));
    }
    return training_params;
}

TFErrorCode GetDefaultsFromAPKAndDownloadFPs(const Settings& settings, const JniCtx& jni) {
    ProtobufSerialization default_params;
    // Use the saved params as default, if they exist
    if (SavedFidelityParamsFileExists(jni)) {
        ALOGI("Using saved default params");
        GetSavedFidelityParams(jni, default_params);
    } else {
        // Use the training mode params if they are present
        auto training_params = GetTrainingParams(settings);
        if (training_params.get() != nullptr) {
            default_params = *training_params;
        }
        else {
            // Try to get the parameters from file.
            if (settings.default_fidelity_parameters_filename.empty())
                return TFERROR_INVALID_DEFAULT_FIDELITY_PARAMS;
            auto err = FindFidelityParamsInApk(jni,
                                           settings.default_fidelity_parameters_filename.c_str(),
                                           default_params);
            if (err!=TFERROR_OK)
                return err;
        }
    }
    auto training_params = GetTrainingParams(settings);
    StartFidelityParamDownloadThread(default_params,
                                     settings.c_settings.fidelity_params_callback,
                                     settings.initial_request_timeout_ms,
                                     settings.ultimate_request_timeout_ms);
    return TFERROR_OK;
}

TFErrorCode KillDownloadThreads() {
    if (s_fp_thread.joinable()) {
        s_kill_thread = true;
        s_fp_thread.join();
        return TFERROR_OK;
    }
    return TFERROR_TUNINGFORK_NOT_INITIALIZED;
}

} // namespace tuningfork

extern "C" {

using namespace tuningfork;

// Download FPs on a separate thread
TFErrorCode TuningFork_startFidelityParamDownloadThread(
                                      const CProtobufSerialization* c_default_params,
                                      ProtoCallback fidelity_params_callback,
                                      int initialTimeoutMs, int ultimateTimeoutMs) {
    if (c_default_params==nullptr) return TFERROR_BAD_PARAMETER;
    if (fidelity_params_callback==nullptr) return TFERROR_BAD_PARAMETER;
    const Settings* settings = GetSettings();
    if (settings == nullptr) return TFERROR_TUNINGFORK_NOT_INITIALIZED;
    return StartFidelityParamDownloadThread(ToProtobufSerialization(*c_default_params),
                                            fidelity_params_callback,
                                            initialTimeoutMs, ultimateTimeoutMs);
}

// Load fidelity params from assets/tuningfork/<filename>
// Ownership of serializations is passed to the caller: call
//  CProtobufSerialization_Free to deallocate any memory.
TFErrorCode TuningFork_findFidelityParamsInApk(JNIEnv* env, jobject context,
                                               const char* filename,
                                               CProtobufSerialization* c_fps) {
    if (c_fps==nullptr) return TFERROR_BAD_PARAMETER;
    JniCtx jni(env, context);
    ProtobufSerialization fps;
    auto err = FindFidelityParamsInApk(jni, filename, fps);
    if (err != TFERROR_OK) return err;
    ToCProtobufSerialization(fps, *c_fps);
    return TFERROR_OK;
}

TFErrorCode TuningFork_setUploadCallback(UploadCallback cbk) {
    return tuningfork::SetUploadCallback(cbk);
}

TFErrorCode TuningFork_saveOrDeleteFidelityParamsFile(JNIEnv* env, jobject context, CProtobufSerialization* fps) {
    JniCtx jni(env, context);
    if(fps) {
        if (SaveFidelityParams(jni, ToProtobufSerialization(*fps)))
            return TFERROR_OK;
    } else {
        std::string save_filename;
        if (GetSavedFileName(jni, save_filename)) {
            if (file_utils::DeleteFile(save_filename))
                return TFERROR_OK;
        }
    }
    return TFERROR_COULDNT_SAVE_OR_DELETE_FPS;
}

} // extern "C"
