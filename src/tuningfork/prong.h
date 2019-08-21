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

#include "tuningfork_internal.h"
#include "histogram.h"

#include <inttypes.h>
#include <vector>
#include <map>
#include <string>
#include <memory>

namespace tuningfork {

typedef ProtobufSerialization SerializedAnnotation;

// A prong holds a histogram for a given annotation and instrument key
class Prong {
public:
    InstrumentationKey instrumentation_key_;
    SerializedAnnotation annotation_;
    Histogram histogram_;
    TimePoint last_time_;
    Duration duration_;

    Prong(InstrumentationKey instrumentation_key = 0,
          const SerializedAnnotation &annotation = {},
          const TFHistogram& histogram_settings = {})
        : instrumentation_key_(instrumentation_key), annotation_(annotation),
          histogram_(histogram_settings),
          last_time_(TimePoint::min()),
          duration_(Duration::zero()) {}

    void Tick(TimePoint t) {
        if (last_time_ != TimePoint::min())
            Trace(t - last_time_);
        last_time_ = t;
    }

    void Trace(Duration dt) {
        // The histogram stores millisecond values as doubles
        double dt_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count() / 1000000.0;
        histogram_.Add(dt_ms);
        duration_ += dt;
    }

    void Clear() {
        if (histogram_.Count() > 0)
            histogram_.Clear();
        last_time_ = TimePoint::min();
        duration_ = Duration::zero();
    }

    size_t Count() const {
        return histogram_.Count();
    }

    void SetInstrumentKey(InstrumentationKey key) {
        instrumentation_key_ = key;
    }

};

struct TimeInterval {
    std::chrono::system_clock::time_point start, end;
};

// Simple fixed-size cache
class ProngCache {
    std::vector<std::unique_ptr<Prong>> prongs_;
    int max_num_instrumentation_keys_;
    TimeInterval time_;
public:
    ProngCache(size_t size, int max_num_instrumentation_keys,
               const std::vector<TFHistogram>& histogram_settings,
               const std::function<SerializedAnnotation(uint64_t)>& seralizeId);

    Prong *Get(uint64_t compound_id);

    void Clear();

    void SetInstrumentKeys(const std::vector<InstrumentationKey>& instrument_keys);

    // Update times
    void Ping(std::chrono::system_clock::time_point t);

    TimeInterval time() const { return time_; }
    const std::vector<std::unique_ptr<Prong>>& prongs() const { return prongs_; }

};

} // namespace tuningfork {
