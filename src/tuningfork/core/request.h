/*
 * Copyright 2020 The Android Open Source Project
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

#include "core/common.h"
#include "core/extra_upload_info.h"

namespace tuningfork {

class Request {
  protected:
    const ExtraUploadInfo& info_;
    std::string base_url_;
    std::string api_key_;
    Duration timeout_;
  public:
    Request(const ExtraUploadInfo& info, std::string base_url, std::string api_key,
            Duration timeout) : info_(info), base_url_(base_url), api_key_(api_key),
                                  timeout_(timeout) {}
    virtual ~Request() {}
    std::string GetURL(std::string rpcname) const;
    const ExtraUploadInfo& Info() const { return info_; }
    virtual TuningFork_ErrorCode Send(
        const std::string& rpc_name,
        const std::string& request,
        int& response_code,
        std::string& response_body);
};

} // namespace tuningfork
