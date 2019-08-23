/*
 * Copyright 2018 The Android Open Source Project
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

#include <thread>
#include <mutex>
#include <map>
#include <condition_variable>
#include "prong.h"

namespace tuningfork {

class Runnable {
protected:
    std::unique_ptr<std::thread> thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool do_quit_;
    int wait_time_ms_;
public:
    Runnable(int wait_time_ms) : wait_time_ms_(wait_time_ms) {}
    virtual void Start();
    virtual void Run();
    virtual void Stop();
    virtual void DoWork() = 0;
};

class UltimateUploader;

class UploadThread : protected Runnable {
private:
    const ProngCache *ready_;
    bool upload_;
    Backend *backend_;
    ProtobufSerialization current_fidelity_params_;
    UploadCallback upload_callback_;
    ExtraUploadInfo extra_info_;
    std::unique_ptr<UltimateUploader> ultimate_uploader_;
    const TFCache* persister_;
 public:
    UploadThread(Backend *backend, const ExtraUploadInfo& extraInfo);
    ~UploadThread();

    void InitialChecks(ProngCache& prongs,
                       IdProvider& id_provider,
                       const TFCache* persister);

    void Start() override;
    void Stop() override;
    void DoWork() override;

    // Returns true if we submitted, false if we are waiting for a previous submit to complete
    // If upload is false, the cache is serialized and saved, not uploaded.
    bool Submit(const ProngCache *prongs, bool upload);

    void SetCurrentFidelityParams(const ProtobufSerialization &fp,
                                  const std::string& experiment_id) {
        current_fidelity_params_ = fp;
        extra_info_.experiment_id = experiment_id;
    }

    void SetUploadCallback(UploadCallback upload_callback) {
        upload_callback_ = upload_callback;
    }

    static ExtraUploadInfo GetExtraUploadInfo(JNIEnv* env, jobject context);

 private:
    void UpdateGLVersion();

};

} // namespace tuningfork {
