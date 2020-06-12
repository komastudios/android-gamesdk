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

#include <unordered_map>

#include "frametime_metric.h"
#include "histogram.h"
#include "loadingtime_metric.h"
#include "memory_metric.h"

namespace tuningfork {

struct TimeInterval {
    std::chrono::system_clock::time_point start, end;
};

// A recording session which stores histograms and time-series.
// These are double-buffered inside tuning fork.
class Session {
   public:
    Session(int max_histograms) : max_histogram_size_(max_histograms) {}

    // Get functions return nullptr if the id has not been created.
    template <typename T>
    T* GetData(uint64_t id) const {
        auto it = metric_data_.find(id);
        if (it == metric_data_.end()) return nullptr;
        if (it->second->type == T::MetricDataType())
            return reinterpret_cast<T*>(it->second.get());
        else
            return nullptr;
    }

    // Create an associated object, unless capacity has been reached, in which
    // case nullptr is returned.
    FrameTimeData* CreateFrameTimeHistogram(
        const FrameTimeMetric& metric, uint64_t id,
        const Settings::Histogram& settings);

    LoadingTimeData* CreateLoadingTimeSeries(const LoadingTimeMetric& metric,
                                             uint64_t id);

    // Create an associated object, unless capacity has been reached, in which
    // case nullptr is returned.
    MemoryData* CreateMemoryHistogram(const MemoryMetric& metric, uint64_t id,
                                      const Settings::Histogram& settings);

    // Clear the data in each created histogram or time series.
    void ClearData();

    // Remove all histograms and time-series.
    void Clear();

    template <typename T>
    std::vector<std::pair<uint64_t, const T*>> NonEmptyHistograms() const {
        std::vector<std::pair<uint64_t, const T*>> ret;
        for (const auto& t : metric_data_) {
            if (t.second->Count() > 0) {
                if (t.second->type == T::MetricDataType())
                    ret.push_back(
                        {t.first, reinterpret_cast<T*>(t.second.get())});
            }
        }
        return ret;
    }

    // Update times
    void Ping(SystemTimePoint t);

    TimeInterval time() const { return time_; }

    void SetInstrumentationKeys(const std::vector<InstrumentationKey>& ikeys) {
        instrumentation_keys_ = ikeys;
    }
    InstrumentationKey GetInstrumentationKey(uint32_t index) const {
        if (index < instrumentation_keys_.size())
            return instrumentation_keys_[index];
        else
            return 0;
    }

   private:
    int max_histogram_size_;
    TimeInterval time_;
    std::unordered_map<uint64_t, std::unique_ptr<MetricData>> metric_data_;
    std::vector<InstrumentationKey> instrumentation_keys_;
};

}  // namespace tuningfork
