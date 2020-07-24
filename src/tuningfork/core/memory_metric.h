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
#include "metricdata.h"
#include "settings.h"

namespace tuningfork {

static const Duration kFastMemoryMetricInterval =
    std::chrono::milliseconds(100);

static const Duration kSlowMemoryMetricInterval =
    std::chrono::milliseconds(1000);

struct MemoryMetric {
    MemoryMetric() = default;
    MemoryMetric(MemoryRecordType memory_record_type, Duration period)
        : memory_record_type_(memory_record_type),
          period_ms_(
              std::chrono::duration_cast<std::chrono::milliseconds>(period)
                  .count()) {}
    MemoryRecordType memory_record_type_;
    uint32_t period_ms_;
};

struct MemoryMetricData : public MetricData {
    MemoryMetricData(MetricId metric_id, const Settings::Histogram& settings)
        : MetricData(MetricType()),
          metric_id_(metric_id),
          histogram_(settings, false) {}
    MetricId metric_id_;
    Histogram<uint64_t> histogram_;
    void Record(uint64_t value) { histogram_.Add(value); }
    virtual void Clear() override { histogram_.Clear(); }
    virtual size_t Count() const override { return histogram_.Count(); }
    static Metric::Type MetricType() { return Metric::Type::MEMORY; }
};

}  // namespace tuningfork
