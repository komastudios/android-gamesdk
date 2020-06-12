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

#include "backend.h"
#include "runnable.h"
#include "session.h"

namespace tuningfork {

class UploadThread : public Runnable {
   private:
    const Session* ready_;
    bool upload_;
    IBackend* backend_;
    TuningFork_UploadCallback upload_callback_;
    const TuningFork_Cache* persister_;

   public:
    UploadThread(IBackend* backend);
    ~UploadThread();

    void InitialChecks(Session& session, IdProvider& id_provider,
                       const TuningFork_Cache* persister);

    void Start() override;
    Duration DoWork() override;

    // Returns true if we submitted, false if we are waiting for a previous
    // submit to complete If upload is false, the cache is serialized and saved,
    // not uploaded.
    bool Submit(const Session* session, bool upload);

    void SetUploadCallback(TuningFork_UploadCallback upload_callback) {
        upload_callback_ = upload_callback;
    }
};

}  // namespace tuningfork
