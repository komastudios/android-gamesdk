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
#include "metricdata.h"
#include "settings.h"

namespace tuningfork {

// Base in the Session id mapping
static const uint64_t kMemoryMetricBase = 0x0100000000000000;

static const Duration kFastMemoryMetricInterval =
    std::chrono::milliseconds(100);

static const Duration kSlowMemoryMetricInterval =
    std::chrono::milliseconds(1000);

// Enum describing how the memory records were obtained.
enum MemoryRecordType {
    INVALID = 0,
    // From calls to android.os.Debug.getNativeHeapAllocatedSize
    ANDROID_DEBUG_NATIVE_HEAP,
    // From /proc/<PID>/oom_score file
    ANDROID_OOM_SCORE,
    // From /proc/meminfo and /proc/<PID>/status files
    ANDROID_MEMINFO_ACTIVE,
    ANDROID_MEMINFO_ACTIVEANON,
    ANDROID_MEMINFO_ACTIVEFILE,
    ANDROID_MEMINFO_ANONPAGES,
    ANDROID_MEMINFO_COMMITLIMIT,
    ANDROID_MEMINFO_HIGHTOTAL,
    ANDROID_MEMINFO_LOWTOTAL,
    ANDROID_MEMINFO_MEMAVAILABLE,
    ANDROID_MEMINFO_MEMFREE,
    ANDROID_MEMINFO_MEMTOTAL,
    ANDROID_MEMINFO_VMDATA,
    ANDROID_MEMINFO_VMRSS,
    ANDROID_MEMINFO_VMSIZE,
    END
};

struct MemoryMetric {
    MemoryMetric(MemoryRecordType memory_record_type, Duration period)
        : memory_record_type_(memory_record_type),
          period_ms_(
              std::chrono::duration_cast<std::chrono::milliseconds>(period)
                  .count()) {}
    MemoryRecordType memory_record_type_;
    uint32_t period_ms_;
};

struct MemoryData : public MetricData {
    MemoryData(MemoryMetric metric, const Settings::Histogram& settings)
        : MetricData(MetricDataType()),
          metric_(metric),
          histogram_(settings, false) {}
    MemoryMetric metric_;
    Histogram<uint64_t> histogram_;
    void Record(uint64_t value) { histogram_.Add(value); }
    virtual void Clear() override { histogram_.Clear(); }
    virtual size_t Count() const override { return histogram_.Count(); }
    static MetricData::Type MetricDataType() {
        return MetricData::Type::MEMORY;
    }
};

}  // namespace tuningfork