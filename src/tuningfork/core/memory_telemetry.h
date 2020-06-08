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
    void UpdateMemInfo() override;
    uint64_t GetNativeHeapAllocatedSize() override;
    void SetEnabled(bool enable) override;
    bool GetEnabled() const override;
    void SetDeviceMemoryBytes(uint64_t bytesize) override;
    uint64_t GetDeviceMemoryBytes() const override;
    uint64_t GetMemInfoOomScore() const override;
    bool IsMemInfoActiveAvailable() const override;
    bool IsMemInfoActiveAnonAvailable() const override;
    bool IsMemInfoActiveFileAvailable() const override;
    bool IsMemInfoAnonPagesAvailable() const override;
    bool IsMemInfoCommitLimitAvailable() const override;
    bool IsMemInfoHighTotalAvailable() const override;
    bool IsMemInfoLowTotalAvailable() const override;
    bool IsMemInfoMemAvailableAvailable() const override;
    bool IsMemInfoMemFreeAvailable() const override;
    bool IsMemInfoMemTotalAvailable() const override;
    bool IsMemInfoVmDataAvailable() const override;
    bool IsMemInfoVmRssAvailable() const override;
    bool IsMemInfoVmSizeAvailable() const override;
    uint64_t GetMemInfoActiveBytes() const override;
    uint64_t GetMemInfoActiveAnonBytes() const override;
    uint64_t GetMemInfoActiveFileBytes() const override;
    uint64_t GetMemInfoAnonPagesBytes() const override;
    uint64_t GetMemInfoCommitLimitBytes() const override;
    uint64_t GetMemInfoHighTotalBytes() const override;
    uint64_t GetMemInfoLowTotalBytes() const override;
    uint64_t GetMemInfoMemAvailableBytes() const override;
    uint64_t GetMemInfoMemFreeBytes() const override;
    uint64_t GetMemInfoMemTotalBytes() const override;
    uint64_t GetMemInfoVmDataBytes() const override;
    uint64_t GetMemInfoVmRssBytes() const override;
    uint64_t GetMemInfoVmSizeBytes() const override;
};

}  // namespace tuningfork
