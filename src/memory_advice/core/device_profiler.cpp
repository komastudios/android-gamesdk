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

#include "device_profiler.h"

#include <stdlib.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <map>
#include <sstream>
#include <streambuf>
#include <string>

#include "Log.h"
#include "jni/jni_wrap.h"
#include "memory_advice/memory_advice.h"

#define LOG_TAG "MemoryAdvice:DeviceProfiler"

namespace memory_advice {

extern const char* lookup_string;

using namespace json11;

MemoryAdvice_ErrorCode DeviceProfiler::Init() {
    std::string err;
    lookup_table_ = std::make_unique<Json::object>(
        Json::parse(lookup_string, err).object_items());
    if (!err.empty()) {
        ALOGE("Error while parsing lookup table: %s", err.c_str());
        return MEMORYADVICE_ERROR_LOOKUP_TABLE_INVALID;
    }

    fingerprint_ = gamesdk::jni::android::os::Build::FINGERPRINT().C();

    return MEMORYADVICE_ERROR_OK;
}

Json::object DeviceProfiler::GetDeviceProfile() const {
    Json::object profile;
    // TODO(b/209602631): implement match by baseline
    std::string best = MatchByFingerprint();
    profile["limits"] = lookup_table_->at(best);
    profile["matched"] = best;
    profile["fingerprint"] = fingerprint_;

    return profile;
}

std::string DeviceProfiler::MatchByFingerprint() const {
    int best_score = -1;
    std::string best;

    for (auto& it : *lookup_table_) {
        int mismatch_index = std::mismatch(fingerprint_.begin(),
                                           fingerprint_.end(), it.first.begin())
                                 .first -
                             fingerprint_.begin();
        if (mismatch_index > best_score) {
            best_score = mismatch_index;
            best = it.first;
        }
    }
    return best;
}

}  // namespace memory_advice