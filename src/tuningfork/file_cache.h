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

#include "tuningfork/tuningfork.h"
#include "tuningfork_internal.h"

#include <string>
#include <mutex>

// Implementation of a TFCache that persists to local storage
namespace tuningfork {

class FileCache {
    std::string path_;
    TFCache c_cache_;
    std::mutex mutex_;
  public:
    FileCache(const std::string& path);
    const TFCache* GetCCache() const { return &c_cache_;}

    TFErrorCode Get(uint64_t key, CProtobufSerialization* value);
    TFErrorCode Set(uint64_t key, const CProtobufSerialization* value);
    TFErrorCode Remove(uint64_t key);

    TFErrorCode Clear();
};

} // namespace tuningfork
