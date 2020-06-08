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

#include <utility>

#include "histogram.h"
#include "jni/jni_wrap.h"
#include "tuningfork_internal.h"

namespace tuningfork {

// Enum describing how the memory records were obtained.
enum MemoryRecordType {
    INVALID = 0,
    // From calls to android.os.Debug.getNativeHeapAllocatedSize
    ANDROID_DEBUG_NATIVE_HEAP,
    // From /proc/<PID>/oom_score file
    ANDROID_OOM_SCORE,
    // From /proc/meminfo and /proc/<PID>/status files
    ANDROID_PROC_MEM,
};

// Enum describing collected histograms.
enum HistogramContentsType {
    NATIVE_HEAP_ALLOCATED_IX = 0,
    MEMINFO_OOM_SCORE,
    MEMINFO_ACTIVE,
    MEMINFO_ACTIVEANON,
    MEMINFO_ACTIVEFILE,
    MEMINFO_ANONPAGES,
    MEMINFO_COMMITLIMIT,
    MEMINFO_HIGHTOTAL,
    MEMINFO_LOWTOTAL,
    MEMINFO_MEMAVAILABLE,
    MEMINFO_MEMFREE,
    MEMINFO_MEMTOTAL,
    MEMINFO_VMDATA,
    MEMINFO_VMRSS,
    MEMINFO_VMSIZE,
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
    SystemTimePoint last_time_slow_;
    IMemInfoProvider* meminfo_provider_;

   public:
    // Construct with default histogram settings.
    MemoryTelemetry(IMemInfoProvider* meminfo_provider);

    // Get a reference to the current histogram being filled.
    const std::vector<MemoryHistogram>& GetHistograms() const {
        return histograms_;
    }

    // Record memory usage, if enough time has passed since the previous tick
    // and the memory_provider is not null.
    void Ping(SystemTimePoint t);

    // Clear the histograms
    void Clear() {
        for (auto& h : histograms_) {
            h.histogram.Clear();
        }
    }
};

struct MemInfo {
    bool initialized = false;
    uint32_t pid;
    int oom_score;
    std::pair<uint64_t, bool> active;
    std::pair<uint64_t, bool> activeAnon;
    std::pair<uint64_t, bool> activeFile;
    std::pair<uint64_t, bool> anonPages;
    std::pair<uint64_t, bool> commitLimit;
    std::pair<uint64_t, bool> highTotal;
    std::pair<uint64_t, bool> lowTotal;
    std::pair<uint64_t, bool> memAvailable;
    std::pair<uint64_t, bool> memFree;
    std::pair<uint64_t, bool> memTotal;
    std::pair<uint64_t, bool> vmData;
    std::pair<uint64_t, bool> vmRss;
    std::pair<uint64_t, bool> vmSize;
};

class DefaultMemInfoProvider : public IMemInfoProvider {
    bool enabled_ = false;
    uint64_t device_memory_bytes = 0;
    jni::android::os::DebugClass android_debug_;

   protected:
    MemInfo memInfo;

   public:
    void updateMemInfo() override;
    uint64_t GetNativeHeapAllocatedSize() override;
    void SetEnabled(bool enable) override;
    bool GetEnabled() const override;
    void SetDeviceMemoryBytes(uint64_t bytesize) override;
    uint64_t GetDeviceMemoryBytes() const override;
    uint64_t GetMeminfoOomScore() const override;
    bool IsMeminfoActiveAvailable() const override;
    bool IsMeminfoActiveAnonAvailable() const override;
    bool IsMeminfoActiveFileAvailable() const override;
    bool IsMeminfoAnonPagesAvailable() const override;
    bool IsMeminfoCommitLimitAvailable() const override;
    bool IsMeminfoHighTotalAvailable() const override;
    bool IsMeminfoLowTotalAvailable() const override;
    bool IsMeminfoMemAvailableAvailable() const override;
    bool IsMeminfoMemFreeAvailable() const override;
    bool IsMeminfoMemTotalAvailable() const override;
    bool IsMeminfoVmDataAvailable() const override;
    bool IsMeminfoVmRssAvailable() const override;
    bool IsMeminfoVmSizeAvailable() const override;
    uint64_t GetMeminfoActiveBytes() const override;
    uint64_t GetMeminfoActiveAnonBytes() const override;
    uint64_t GetMeminfoActiveFileBytes() const override;
    uint64_t GetMeminfoAnonPagesBytes() const override;
    uint64_t GetMeminfoCommitLimitBytes() const override;
    uint64_t GetMeminfoHighTotalBytes() const override;
    uint64_t GetMeminfoLowTotalBytes() const override;
    uint64_t GetMeminfoMemAvailableBytes() const override;
    uint64_t GetMeminfoMemFreeBytes() const override;
    uint64_t GetMeminfoMemTotalBytes() const override;
    uint64_t GetMeminfoVmDataBytes() const override;
    uint64_t GetMeminfoVmRssBytes() const override;
    uint64_t GetMeminfoVmSizeBytes() const override;
};

}  // namespace tuningfork
