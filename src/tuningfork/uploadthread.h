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

#ifndef TUNINGFORK_UPLOADTHREAD_H
#define TUNINGFORK_UPLOADTHREAD_H

#include <thread>
#include <mutex>
#include <map>
#include <condition_variable>
#include "prong.h"

namespace tuningfork {

class UploadThread {
private:
    typedef void (*UploadCallback)(const CProtobufSerialization*);
    std::unique_ptr<std::thread> thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool do_quit_;
    const ProngCache *ready_;
    Backend *backend_;
    ProtobufSerialization current_fidelity_params_;
    UploadCallback upload_callback_;
public:
    UploadThread(Backend *backend);

    ~UploadThread();

    void Start();

    void Stop();

    void Run();

    // Returns true if we submitted, false if we are waiting for a previous submit to complete
    bool Submit(const ProngCache *prongs);

    void SetCurrentFidelityParams(const ProtobufSerialization &fp) {
        current_fidelity_params_ = fp;
    }

    void SetUploadCallback(UploadCallback upload_callback) {
        upload_callback_ = upload_callback;
    }

    friend class ClearcutSerializer;
};

} // namespace tuningfork {

#endif //TUNINGFORK_UPLOADTHREAD_H
