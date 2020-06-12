
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

#include "session.h"

namespace tuningfork {

void FrameTimeData::Tick(TimePoint t) {
    if (last_time_ != TimePoint::min()) Record(t - last_time_);
    last_time_ = t;
}

void FrameTimeData::Record(Duration dt) {
    // The histogram stores millisecond values as doubles
    histogram_.Add(
        double(
            std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count()) /
        1000000);
    duration_ += dt;
}

void FrameTimeData::Clear() {
    last_time_ = TimePoint::min();
    histogram_.Clear();
}

void LoadingTimeData::Record(Duration dt) {
    // The histogram stores millisecond values as doubles
    histogram_.Add(
        double(
            std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count()) /
        1000000);
    duration_ += dt;
}

FrameTimeData* Session::CreateFrameTimeHistogram(
    const FrameTimeMetric& metric, uint64_t id,
    const Settings::Histogram& settings) {
    if (metric_data_.size() >= max_histogram_size_) return nullptr;
    metric_data_[id] = std::make_unique<FrameTimeData>(metric, settings);
    return reinterpret_cast<FrameTimeData*>(metric_data_[id].get());
}

LoadingTimeData* Session::CreateLoadingTimeSeries(
    const LoadingTimeMetric& metric, uint64_t id) {
    if (metric_data_.size() >= max_histogram_size_) return nullptr;
    metric_data_[id] = std::make_unique<LoadingTimeData>(metric);
    return reinterpret_cast<LoadingTimeData*>(metric_data_[id].get());
}

MemoryData* Session::CreateMemoryHistogram(
    const MemoryMetric& metric, uint64_t id,
    const Settings::Histogram& settings) {
    if (metric_data_.size() >= max_histogram_size_) return nullptr;
    metric_data_[id] = std::make_unique<MemoryData>(metric, settings);
    return reinterpret_cast<MemoryData*>(metric_data_[id].get());
}

void Session::ClearData() {
    for (auto& p : metric_data_) {
        if (p.second.get() != nullptr) p.second->Clear();
    }
    time_.start = SystemTimePoint();
    time_.end = SystemTimePoint();
}

void Session::Ping(SystemTimePoint t) {
    if (time_.start == SystemTimePoint()) {
        time_.start = t;
    }
    time_.end = t;
}

void Session::Clear() {
    metric_data_.clear();
    time_.start = SystemTimePoint();
    time_.end = SystemTimePoint();
}

}  // namespace tuningfork
