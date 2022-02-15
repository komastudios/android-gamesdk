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

#include <map>
#include <memory>
#include <regex>
#include <string>

#include "jni/jni_wrap.h"
#include "json11/json11.hpp"

#define LOG_TAG "MemoryAdvice"
#include "Log.h"

using namespace gamesdk::jni;

namespace memory_advice {

using namespace json11;

/**
 * @brief Provides memory info from various metrics
 */
class MetricsProvider {
   public:
    typedef Json::object (MetricsProvider::*MetricsFunction)();
    /** @brief A map matching metrics category names to their functions */
    std::map<std::string, MetricsFunction> metrics_categories_ = {
        {"meminfo", &MetricsProvider::GetMeminfoValues},
        {"status", &MetricsProvider::GetStatusValues},
        {"proc", &MetricsProvider::GetProcValues},
        {"debug", &MetricsProvider::GetDebugValues},
        {"MemoryInfo", &MetricsProvider::GetActivityManagerMemoryInfo},
        {"ActivityManager", &MetricsProvider::GetActivityManagerValues}};
    /** @brief Get a list of memory metrics stored in /proc/meminfo */
    Json::object GetMeminfoValues();
    /** @brief Get a list of memory metrics stored in /proc/{pid}/status */
    Json::object GetStatusValues();
    /**
     * @brief Get a list of various memory metrics stored in /proc/{pid}
     * folder.
     */
    Json::object GetProcValues();
    /**
     * @brief Get a list of memory metrics available from ActivityManager
     */
    Json::object GetActivityManagerValues();
    /**
     * @brief Get a list of memory metrics available from
     * ActivityManager#getMemoryInfo().
     */
    Json::object GetActivityManagerMemoryInfo();
    /**
     * @brief Get a list of memory metrics available from android.os.Debug
     */
    Json::object GetDebugValues();

   private:
    android::os::DebugClass android_debug_;
    /**
     * @brief Reads the given file and dumps the memory values within as a map
     */
    Json::object GetMemoryValuesFromFile(const std::string &path,
                                         const std::regex &pattern);
    /** @brief Reads the OOM Score of the app from /proc/{pid}/oom_score */
    int32_t GetOomScore();
};

}  // namespace memory_advice
