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

#include "prong.h"

#define LOG_TAG "TuningFork"
#include "Log.h"

#include <string>
#include <sstream>

namespace tuningfork {

// Allocate all the prongs up front
ProngCache::ProngCache(size_t size, int max_num_instrumentation_keys,
                       const std::vector<TFHistogram> &histogram_settings,
                       const std::function<SerializedAnnotation(uint64_t)> &seralizeId)
    : prongs_(size), max_num_instrumentation_keys_(max_num_instrumentation_keys) {
    // Allocate all the prongs
    InstrumentationKey ikey = 0;
    for (int i = 0; i < size; ++i) {
        auto &p = prongs_[i];
        SerializedAnnotation annotation = seralizeId(i);
        auto& h = histogram_settings[ikey<histogram_settings.size()?ikey:0];
        p = std::make_unique<Prong>(ikey, annotation, h);
        ++ikey;
        if (ikey >= max_num_instrumentation_keys)
            ikey = 0;
    }
}

Prong *ProngCache::Get(uint64_t compound_id) {
    if (compound_id >= prongs_.size()) {
        ALOGW("You have overrun the number of histograms (are your "
              "Settings correct?)");
        return nullptr;
    }
    return prongs_[compound_id].get();
}

void ProngCache::Clear() {
    for (auto &p: prongs_) {
        p->Clear();
    }
    time_.start = SystemTimePoint();
    time_.end = SystemTimePoint();
}

void ProngCache::SetInstrumentKeys(const std::vector<InstrumentationKey>& instrument_keys) {
    auto nAnnotations = prongs_.size()/max_num_instrumentation_keys_;
    for (int j=0; j<nAnnotations; ++j) {
        for (int i=0; i<instrument_keys.size(); ++i) {
            auto k = instrument_keys[i];
            auto& p = prongs_[i + j*max_num_instrumentation_keys_];
            p->SetInstrumentKey(k);
        }
    }
}

void ProngCache::Ping(std::chrono::system_clock::time_point t) {
    if(time_.start==std::chrono::system_clock::time_point()) {
        time_.start = t;
    }
    time_.end = t;
}

}
