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

#include "memory_telemetry.h"

namespace tuningfork {

static const uint64_t HIST_START = 0;
static const uint64_t HIST_END = 16000000000L; // 16GB
static const uint32_t NUM_BUCKETS = 200;
static const uint32_t UPDATE_PERIOD_MS = 16;

#define NATIVE_HEAP_ALLOCATED_IX 0

MemoryTelemetry::MemoryTelemetry(IMemInfoProvider* meminfo_provider) : histograms_{
    MemoryHistogram { ANDROID_DEBUG_NATIVE_HEAP, // type
                      UPDATE_PERIOD_MS, // period_ms
                      Histogram<uint64_t> {0, 0, NUM_BUCKETS} // histogram
    }
}, meminfo_provider_(meminfo_provider)
{
    // We don't have any histograms if the memory provider is null.
    if (meminfo_provider_ == nullptr)
        histograms_.clear();
}

void MemoryTelemetry::Ping(SystemTimePoint t) {
    if (meminfo_provider_ != nullptr && meminfo_provider_->IsEnabled()) {
        auto dt = t - last_time_;
        if (dt > std::chrono::milliseconds(UPDATE_PERIOD_MS)) {
            auto memory = meminfo_provider_->GetNativeHeapAllocatedSize();
            histograms_[NATIVE_HEAP_ALLOCATED_IX].histogram.Add(memory);
            last_time_ = t;
        }
    }
}

uint64_t DefaultMemInfoProvider::GetNativeHeapAllocatedSize() {
    if (jni::IsValid()) {
        // Call android.os.Debug.getNativeHeapAllocatedSize()
        return android_debug_.getNativeHeapAllocatedSize();
    }
    return 0;
}

bool DefaultMemInfoProvider::IsEnabled() {
    return true;
}

} // namespace tuningfork
