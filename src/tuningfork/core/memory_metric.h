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

#include "histogram.h"
#include "memory_record_type.h"
#include "memory_telemetry.h"
#include "metricdata.h"
#include "settings.h"

namespace tuningfork {

static const int32_t kBufferSize = 2 * 60;

static const Duration kMemoryMetricInterval = std::chrono::seconds(60);

struct InitialMemoryMetric {
    int64_t total_mem_, proportional_set_size_, swap_total_;

    InitialMemoryMetric(int64_t total_mem, int64_t proportional_set_size,
                        int64_t swap_total)
        : total_mem_(total_mem),
          proportional_set_size_(proportional_set_size),
          swap_total_(swap_total) {}
};

struct RealTimeMemoryMetric {
    int64_t avail_mem_, oom_score_;
    Duration time_since_process_start_;

    RealTimeMemoryMetric(int64_t avail_mem, int64_t oom_score,
                         Duration time_since_process_start)
        : avail_mem_(avail_mem),
          oom_score_(oom_score),
          time_since_process_start_(time_since_process_start) {}

    void UpdateValues(int64_t avail_mem, int64_t oom_score,
                      Duration time_since_process_start) {
        avail_mem_ = avail_mem;
        oom_score_ = oom_score;
        time_since_process_start_ = time_since_process_start;
    }
};

struct MemoryMetricData : public MetricData {
    MemoryMetricData(MetricId metric_id)
        : MetricData(MetricType()), metric_id_(metric_id) {}
    MetricId metric_id_;
    std::vector<RealTimeMemoryMetric> data_;
    int cyclical_buffer_location = 0;
    std::unique_ptr<InitialMemoryMetric> initial_memory_metric_;

    void Record(IMemInfoProvider *mem_info_provider,
                Duration time_since_process_start) {
        if (initial_memory_metric_.get() == nullptr) {
            mem_info_provider->UpdateMemInfo();
            initial_memory_metric_ = std::make_unique<InitialMemoryMetric>(
                mem_info_provider->GetTotalMem(), mem_info_provider->GetPss(),
                mem_info_provider->GetMemInfoSwapTotalBytes());
        }
        mem_info_provider->UpdateOomScore();
        if (data_.size() < kBufferSize) {
            RealTimeMemoryMetric metric(mem_info_provider->GetAvailMem(),
                                        mem_info_provider->GetMemInfoOomScore(),
                                        time_since_process_start);
            data_.push_back(metric);
        } else {
            data_[cyclical_buffer_location].UpdateValues(
                mem_info_provider->GetAvailMem(),
                mem_info_provider->GetMemInfoOomScore(),
                time_since_process_start);
            cyclical_buffer_location++;
            cyclical_buffer_location %= kBufferSize;
        }
    }
    virtual void Clear() override { data_.clear(); }
    virtual size_t Count() const override { return data_.size(); }
    static Metric::Type MetricType() { return Metric::Type::MEMORY; }
};

}  // namespace tuningfork
