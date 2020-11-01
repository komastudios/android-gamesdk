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

#define LOG_TAG "MemoryAdvice"
#include "Log.h"

using namespace gamesdk::jni;

namespace memory_advice {

/**
 * @brief Provides memory info from various metrics
 */
class MetricsProvider {
   public:
    /** @brief Get a list of memory metrics stored in /proc/meminfo */
    static std::map<std::string, int64_t> GetMeminfoValues();
    /** @brief Get a list of memory metrics stored in /proc/{pid}/status */
    static std::map<std::string, int64_t> GetStatusValues();
    /**
     * @brief Get a list of various memory metrics stored in /proc/{pid}
     * folder.
     */
    static std::map<std::string, int64_t> GetProcValues();
    /**
     * @brief Get a list of memory metrics available from ActivityManager
     */
    static std::map<std::string, int64_t> GetActivityManagerValues();
    /**
     * @brief Get a list of memory metrics available from
     * ActivityManager#getMemoryInfo().
     */
    static std::map<std::string, int64_t> GetActivityManagerMemoryInfo();
    /**
     * @brief Get a list of memory metrics available from android.os.Debug
     */
    static std::map<std::string, int64_t> GetDebugValues();
    static void InitActivityManager();

   private:
    static std::unique_ptr<android::app::ActivityManager> activity_manager_;
    static android::os::DebugClass android_debug_;
    /**
     * @brief Reads the given file and dumps the memory values within as a map
     */
    static std::map<std::string, int64_t> GetMemoryValuesFromFile(
        const std::string &path, const std::regex &pattern);
    /** @brief Reads the OOM Score of the app from /proc/{pid}/oom_score */
    static int64_t GetOomScore();
};

}  // namespace memory_advice
