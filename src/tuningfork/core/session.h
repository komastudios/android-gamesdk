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

#include "prong.h"

namespace tuningfork {

// A recording session which stores histograms and time-series.
// These are double-buffered inside tuning fork.
class Session {
    std::vector<std::unique_ptr<Prong>> prongs_;
    int max_num_instrumentation_keys_;
    Prong::TimeInterval time_;
    MemoryTelemetry memory_telemetry_;

   public:
    Session(size_t size, int max_num_instrumentation_keys,
            const std::vector<Settings::Histogram>& histogram_settings,
            const std::function<SerializedAnnotation(uint64_t)>& serializeId,
            const std::function<bool(uint64_t)>& is_loading_id,
            IMemInfoProvider* meminfo_provider);

    Prong* Get(uint64_t compound_id) const;

    void Clear();

    void SetInstrumentKeys(
        const std::vector<InstrumentationKey>& instrument_keys);

    // Update times
    void Ping(SystemTimePoint t);

    Prong::TimeInterval time() const { return time_; }
    const std::vector<std::unique_ptr<Prong>>& prongs() const {
        return prongs_;
    }

    const MemoryTelemetry& GetMemoryTelemetry() const {
        return memory_telemetry_;
    }
};

}  // namespace tuningfork
