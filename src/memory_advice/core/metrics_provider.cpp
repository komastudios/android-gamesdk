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
#include <sstream>
#include <utility>
#include <vector>

constexpr size_t BYTES_IN_KB = 1024;

namespace memory_advice {

// Get a list of memory metrics stored in /proc/meminfo
std::map<std::string, int64_t> MetricsProvider::GetMeminfoValues() {
    return GetMemoryValuesFromFile("/proc/meminfo");
}

// Get a list of memory metrics stored in /proc/{pid}/status
std::map<std::string, int64_t> MetricsProvider::GetStatusValues() {
    std::stringstream ss_path;
    ss_path << "/proc/" << getpid() << "/status";
    return GetMemoryValuesFromFile(ss_path.str());
}

// Get a list of various memory metrics stored in /proc/{pid} folder.
std::map<std::string, int64_t> MetricsProvider::GetProcValues() {
    std::map<std::string, int64_t> proc_map;
    proc_map["oom_score"] = GetOomScore();
    return proc_map;
}

// Reads the given file and dumps the memory values within as a map
std::map<std::string, int64_t> MetricsProvider::GetMemoryValuesFromFile(
    const std::string &path) {
    std::ifstream file_stream(path);
    std::map<std::string, int64_t> metrics_map;
    if (!file_stream) {
        ALOGE("Could not open %s", path.c_str());
        return metrics_map;
    }
    std::string line;
    while (std::getline(file_stream, line)) {
        std::istringstream ss(line);
        std::vector<std::string> split(std::istream_iterator<std::string>{ss},
                                       std::istream_iterator<std::string>());
        if (split.size() == 3 && split[2] == "kB") {
            std::string &key = split[0];
            // Remove colon at end of first word
            key.pop_back();
            int64_t value =
                strtoll(split[1].c_str(), nullptr, 10) * BYTES_IN_KB;
            metrics_map[key] = value;
        }
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
}  // namespace memory_advice