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

#include "metrics_provider.h"

#include <stdlib.h>
#include <unistd.h>

#include <chrono>
#include <fstream>
#include <iterator>
#include <map>
#include <regex>
#include <sstream>
#include <streambuf>
#include <utility>
#include <vector>

#include "jni/jni_wrap.h"

using namespace gamesdk::jni;

constexpr size_t BYTES_IN_KB = 1024;
constexpr int64_t BYTES_IN_MB = 1024 * 1024;
const std::regex MEMINFO_REGEX("([^:]+)[^\\d]*(\\d+).*\n");
const std::regex STATUS_REGEX("([a-zA-Z]+)[^\\d]*(\\d+) kB.*\n");

namespace memory_advice {
std::unique_ptr<android::app::ActivityManager>
    MetricsProvider::activity_manager_;

std::map<std::string, int64_t> MetricsProvider::GetMeminfoValues() {
    return GetMemoryValuesFromFile("/proc/meminfo", MEMINFO_REGEX);
}

std::map<std::string, int64_t> MetricsProvider::GetStatusValues() {
    std::stringstream ss_path;
    ss_path << "/proc/" << getpid() << "/status";
    return GetMemoryValuesFromFile(ss_path.str(), STATUS_REGEX);
}

std::map<std::string, int64_t> MetricsProvider::GetProcValues() {
    std::map<std::string, int64_t> proc_map;
    proc_map["oom_score"] = GetOomScore();
    return proc_map;
}

std::map<std::string, int64_t> MetricsProvider::GetActivityManagerValues() {
    std::map<std::string, int64_t> metrics_map;

    metrics_map["MemoryClass"] =
        activity_manager_->getMemoryClass() * BYTES_IN_MB;
    metrics_map["LargeMemoryClass"] =
        activity_manager_->getLargeMemoryClass() * BYTES_IN_MB;
    metrics_map["LowRamDevice"] = activity_manager_->isLowRamDevice();

    return metrics_map;
}

std::map<std::string, int64_t> MetricsProvider::GetActivityManagerMemoryInfo() {
    android::app::MemoryInfo memory_info;
    std::map<std::string, int64_t> metrics_map;
    activity_manager_->getMemoryInfo(memory_info.J());
    metrics_map["threshold"] = memory_info.threshold();
    metrics_map["availMem"] = memory_info.availMem();
    metrics_map["totalMem"] = memory_info.totalMem();
    metrics_map["lowMemory"] = memory_info.lowMemory();

    return metrics_map;
}

std::map<std::string, int64_t> MetricsProvider::GetDebugValues() {
    std::map<std::string, int64_t> metrics_map;
    metrics_map["nativeHeapAllocatedSize"] =
        android_debug_.getNativeHeapAllocatedSize();
    metrics_map["nativeHeapFreeSize"] = android_debug_.getNativeHeapFreeSize();
    metrics_map["nativeHeapSize"] = android_debug_.getNativeHeapSize();

    return metrics_map;
}

std::map<std::string, int64_t> MetricsProvider::GetMemoryValuesFromFile(
    const std::string &path, const std::regex &pattern) {
    std::ifstream file_stream(path);
    std::map<std::string, int64_t> metrics_map;
    if (!file_stream) {
        ALOGE("Could not open %s", path.c_str());
        return metrics_map;
    }

    std::string file((std::istreambuf_iterator<char>(file_stream)),
                     std::istreambuf_iterator<char>());
    std::smatch match;
    while (std::regex_search(file, match, pattern)) {
        metrics_map[match[1].str()] =
            strtoll(match[2].str().c_str(), nullptr, 10) * BYTES_IN_KB;
        file = match.suffix().str();
    }
    return metrics_map;
}

int64_t MetricsProvider::GetOomScore() {
    std::stringstream ss_path;
    ss_path << "/proc/" << getpid() << "/oom_score";
    std::ifstream oom_file(ss_path.str());
    if (!oom_file) {
        ALOGE_ONCE("Could not open %s", ss_path.str().c_str());
        return -1;
    } else {
        int64_t oom_score;
        oom_file >> oom_score;
        return oom_score;
    }
}

void MetricsProvider::InitActivityManager() {
    java::Object obj = AppContext().getSystemService(
        android::content::Context::ACTIVITY_SERVICE);
    activity_manager_ =
        std::make_unique<android::app::ActivityManager>(std::move(obj));
}
}  // namespace memory_advice
