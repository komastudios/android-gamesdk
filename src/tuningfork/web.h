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

#include "tuningfork_internal.h"
#include "jni_helper.h"

namespace tuningfork {

const uint64_t HISTOGRAMS_PAUSED = 0;
const uint64_t HISTOGRAMS_UPLOADING = 1;

class WebRequest {
    JniCtx jni_;
    std::string uri_;
    std::string api_key_;
    Duration timeout_;
public:
    WebRequest(const JniCtx& jni, const std::string& uri,
               const std::string& api_key, Duration timeout);
    WebRequest(const WebRequest&);
    WebRequest(WebRequest&& rq) = delete;
    WebRequest& operator=(const WebRequest& rq) = delete;
    TFErrorCode Send(const std::string& request_json,
                     int& response_code, std::string& response_body);
};

} // namespace tuningfork
