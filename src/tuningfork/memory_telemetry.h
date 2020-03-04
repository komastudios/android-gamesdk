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

#include "tuningfork_internal.h"
#include "histogram.h"
#include "jni_wrap.h"

namespace tuningfork {

// Enum describing how the memory records were obtained.
enum MemoryRecordType {
    INVALID = 0,
    // From calls to android.os.Debug.getNativeHeapAllocatedSize
    ANDROID_DEBUG_NATIVE_HEAP = 1
};

struct MemoryHistogram {
    // The type of memory record.
    MemoryRecordType type;
    // How often a memory record was taken in milliseconds.
    uint32_t period_ms;

    Histogram<uint64_t> histogram;
};

class MemoryTelemetry {
    std::vector<MemoryHistogram> histograms_;
    SystemTimePoint last_time_;
    IMemInfoProvider* meminfo_provider_;
  public:
    // Construct with default histogram settings.
    MemoryTelemetry(IMemInfoProvider* meminfo_provider);

    // Get a reference to the current histogram being filled.
    const std::vector<MemoryHistogram>& GetHistograms() const { return histograms_; }

    // Record memory usage, if enough time has passed since the previous tick and the
    // memory_provider is not null.
    void Ping(SystemTimePoint t);

    // Clear the histograms
    void Clear() {
        for(auto& h: histograms_) {
            h.histogram.Clear();
        }
    }
};

class DefaultMemInfoProvider : public IMemInfoProvider {
    bool enabled_ = true;
    uint64_t device_memory_bytes = 0;
    jni::android::os::DebugClass android_debug_;
  public:
    uint64_t GetNativeHeapAllocatedSize() override;
    void SetEnabled(bool enable) override;
    bool GetEnabled() const override;
    void SetDeviceMemoryBytes(uint64_t bytesize) override;
    uint64_t GetDeviceMemoryBytes() const override;
};

}
