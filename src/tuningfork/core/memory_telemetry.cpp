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

#include <unistd.h>

#include <fstream>
#include <iterator>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#define LOG_TAG "TuningFork"
#include "Log.h"

namespace tuningfork {

static const uint64_t HIST_START = 0;
static const uint64_t DEFAULT_HIST_END = 10000000000L;  // 10GB
static const uint32_t NUM_BUCKETS = 200;
static const uint32_t UPDATE_PERIOD_MS = 16;
static const uint32_t SLOW_UPDATE_PERIOD_MS = 15000;

constexpr size_t BYTES_IN_KB = 1024;

using memInfoMap = std::unordered_map<std::string, size_t>;

#define MEMORY_HISTOGRAM(type, period)                          \
    MemoryHistogram {                                           \
        type, period, Histogram<uint64_t> {                     \
            HIST_START,                                         \
                (meminfo_provider != nullptr                    \
                     ? meminfo_provider->GetDeviceMemoryBytes() \
                     : DEFAULT_HIST_END),                       \
                NUM_BUCKETS                                     \
        }                                                       \
    }

MemoryTelemetry::MemoryTelemetry(IMemInfoProvider *meminfo_provider)
    : histograms_{MEMORY_HISTOGRAM(ANDROID_DEBUG_NATIVE_HEAP, UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_OOM_SCORE, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS),
                  MEMORY_HISTOGRAM(ANDROID_PROC_MEM, SLOW_UPDATE_PERIOD_MS)},
      meminfo_provider_(meminfo_provider) {
    // We don't have any histograms if the memory provider is null.
    if (meminfo_provider_ == nullptr) histograms_.clear();
}

#define UPDATE_OPT_HIST(label, metric)                       \
    if (meminfo_provider_->IsMeminfo##metric##Available()) { \
        histograms_[label].histogram.Add(                    \
            meminfo_provider_->GetMeminfo##metric##Bytes()); \
    }

void MemoryTelemetry::Ping(SystemTimePoint t) {
    if (meminfo_provider_ != nullptr && meminfo_provider_->GetEnabled()) {
        auto dt = t - last_time_;
        if (dt > std::chrono::milliseconds(UPDATE_PERIOD_MS)) {
            histograms_[NATIVE_HEAP_ALLOCATED_IX].histogram.Add(
                meminfo_provider_->GetNativeHeapAllocatedSize());

            last_time_ = t;
        }
        dt = t - last_time_slow_;
        if (dt > std::chrono::milliseconds(SLOW_UPDATE_PERIOD_MS)) {
            meminfo_provider_->UpdateMemInfo();
            histograms_[MEMINFO_OOM_SCORE].histogram.Add(
                meminfo_provider_->GetMeminfoOomScore());

            UPDATE_OPT_HIST(MEMINFO_ACTIVE, Active);
            UPDATE_OPT_HIST(MEMINFO_ACTIVEANON, ActiveAnon);
            UPDATE_OPT_HIST(MEMINFO_ACTIVEFILE, ActiveFile);
            UPDATE_OPT_HIST(MEMINFO_ANONPAGES, AnonPages);
            UPDATE_OPT_HIST(MEMINFO_COMMITLIMIT, CommitLimit);
            UPDATE_OPT_HIST(MEMINFO_HIGHTOTAL, HighTotal);
            UPDATE_OPT_HIST(MEMINFO_LOWTOTAL, LowTotal);
            UPDATE_OPT_HIST(MEMINFO_MEMAVAILABLE, MemAvailable);
            UPDATE_OPT_HIST(MEMINFO_MEMFREE, MemFree);
            UPDATE_OPT_HIST(MEMINFO_MEMTOTAL, MemTotal);
            UPDATE_OPT_HIST(MEMINFO_VMDATA, VmData);
            UPDATE_OPT_HIST(MEMINFO_VMRSS, VmRss);
            UPDATE_OPT_HIST(MEMINFO_VMSIZE, VmSize);

            last_time_slow_ = t;
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

// Add entries to the given memInfoMap structure, or update the entries if the
// new value is higher.
static void getMemInfoFromFile(memInfoMap &data, std::string path) {
    std::ifstream file_stream(path);
    if (!file_stream) {
        ALOGE("Could not open %s", path.c_str());
        return;
    }
    for (std::string line; std::getline(file_stream, line);) {
        std::istringstream ss(line);
        std::vector<std::string> split(std::istream_iterator<std::string>{ss},
                                       std::istream_iterator<std::string>());
        if (split.size() == 3 && split[2] == "kB") {
            std::string &key = split[0];
            // Remove colon at end of first word
            key.pop_back();
            size_t value = std::stoul(split[1]) * BYTES_IN_KB;
            if (data.find(key) == data.end() || data[key] < value) {
                data[split[0]] = value;
            }
        }
    }
}

static std::pair<uint64_t, bool> getValueMemInfo(memInfoMap data,
                                                 std::string key) {
    if (data.count(key)) {
        return std::make_pair(data[key], true);
    } else {
        return std::make_pair(0, false);
    }
}

void DefaultMemInfoProvider::UpdateMemInfo() {
    std::stringstream ss_path;
    memInfoMap data;

    getMemInfoFromFile(data, "/proc/meminfo");
    ss_path << "/proc/" << memInfo.pid << "/status";
    getMemInfoFromFile(data, ss_path.str());

    memInfo.active = getValueMemInfo(data, "Active");
    memInfo.activeAnon = getValueMemInfo(data, "Active(anon)");
    memInfo.activeFile = getValueMemInfo(data, "Active(file)");
    memInfo.anonPages = getValueMemInfo(data, "AnonPages");
    memInfo.commitLimit = getValueMemInfo(data, "CommitLimit");
    memInfo.highTotal = getValueMemInfo(data, "HighTotal");
    memInfo.lowTotal = getValueMemInfo(data, "LowTotal");
    memInfo.memAvailable = getValueMemInfo(data, "MemAvailable");
    memInfo.memFree = getValueMemInfo(data, "MemFree");
    memInfo.memTotal = getValueMemInfo(data, "MemTotal");
    memInfo.vmData = getValueMemInfo(data, "VmData");
    memInfo.vmRss = getValueMemInfo(data, "VmRSS");
    memInfo.vmSize = getValueMemInfo(data, "VmSize");

    ss_path.clear();
    ss_path.str(std::string());
    ss_path << "/proc/" << memInfo.pid << "/oom_score";
    std::ifstream oom_file(ss_path.str());
    if (!oom_file) {
        ALOGE("Could not open %s", ss_path.str().c_str());
    } else {
        oom_file >> memInfo.oom_score;
        if (!oom_file) {
            ALOGE("Bad conversion in %s", ss_path.str().c_str());
        }
    }
}

void DefaultMemInfoProvider::SetEnabled(bool enabled) {
    enabled_ = enabled;

    if (enabled && !memInfo.initialized) {
        memInfo.initialized = true;
        memInfo.pid = (uint32_t)getpid();
    }
}

bool DefaultMemInfoProvider::GetEnabled() const { return enabled_; }

void DefaultMemInfoProvider::SetDeviceMemoryBytes(uint64_t bytesize) {
    device_memory_bytes = bytesize;
}

uint64_t DefaultMemInfoProvider::GetDeviceMemoryBytes() const {
    return device_memory_bytes;
}

uint64_t DefaultMemInfoProvider::GetMeminfoOomScore() const {
    return memInfo.oom_score;
}

bool DefaultMemInfoProvider::IsMeminfoActiveAvailable() const {
    return memInfo.active.second;
}

bool DefaultMemInfoProvider::IsMeminfoActiveAnonAvailable() const {
    return memInfo.activeAnon.second;
}

bool DefaultMemInfoProvider::IsMeminfoActiveFileAvailable() const {
    return memInfo.activeFile.second;
}

bool DefaultMemInfoProvider::IsMeminfoAnonPagesAvailable() const {
    return memInfo.anonPages.second;
}

bool DefaultMemInfoProvider::IsMeminfoCommitLimitAvailable() const {
    return memInfo.commitLimit.second;
}

bool DefaultMemInfoProvider::IsMeminfoHighTotalAvailable() const {
    return memInfo.highTotal.second;
}

bool DefaultMemInfoProvider::IsMeminfoLowTotalAvailable() const {
    return memInfo.lowTotal.second;
}

bool DefaultMemInfoProvider::IsMeminfoMemAvailableAvailable() const {
    return memInfo.memAvailable.second;
}

bool DefaultMemInfoProvider::IsMeminfoMemFreeAvailable() const {
    return memInfo.memFree.second;
}

bool DefaultMemInfoProvider::IsMeminfoMemTotalAvailable() const {
    return memInfo.memTotal.second;
}

bool DefaultMemInfoProvider::IsMeminfoVmDataAvailable() const {
    return memInfo.vmData.second;
}

bool DefaultMemInfoProvider::IsMeminfoVmRssAvailable() const {
    return memInfo.vmRss.second;
}

bool DefaultMemInfoProvider::IsMeminfoVmSizeAvailable() const {
    return memInfo.vmSize.second;
}

uint64_t DefaultMemInfoProvider::GetMeminfoActiveBytes() const {
    return memInfo.active.first;
}

uint64_t DefaultMemInfoProvider::GetMeminfoActiveAnonBytes() const {
    return memInfo.activeAnon.first;
}

uint64_t DefaultMemInfoProvider::GetMeminfoActiveFileBytes() const {
    return memInfo.activeFile.first;
}

uint64_t DefaultMemInfoProvider::GetMeminfoAnonPagesBytes() const {
    return memInfo.anonPages.first;
}

uint64_t DefaultMemInfoProvider::GetMeminfoCommitLimitBytes() const {
    return memInfo.commitLimit.first;
}

uint64_t DefaultMemInfoProvider::GetMeminfoHighTotalBytes() const {
    return memInfo.highTotal.first;
}

uint64_t DefaultMemInfoProvider::GetMeminfoLowTotalBytes() const {
    return memInfo.lowTotal.first;
}

uint64_t DefaultMemInfoProvider::GetMeminfoMemAvailableBytes() const {
    return memInfo.memAvailable.first;
}

uint64_t DefaultMemInfoProvider::GetMeminfoMemFreeBytes() const {
    return memInfo.memFree.first;
}

uint64_t DefaultMemInfoProvider::GetMeminfoMemTotalBytes() const {
    return memInfo.memTotal.first;
}

uint64_t DefaultMemInfoProvider::GetMeminfoVmDataBytes() const {
    return memInfo.vmData.first;
}

uint64_t DefaultMemInfoProvider::GetMeminfoVmRssBytes() const {
    return memInfo.vmRss.first;
}

uint64_t DefaultMemInfoProvider::GetMeminfoVmSizeBytes() const {
    return memInfo.vmSize.first;
}

}  // namespace tuningfork
