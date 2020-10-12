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
#include <string>

#define LOG_TAG "MemoryAdvice"
#include "Log.h"

namespace memory_advice {

// The class that provides memory info from various metrics
class MetricsProvider {
   public:
    static std::map<std::string, int64_t> GetMeminfoValues();
    static std::map<std::string, int64_t> GetStatusValues();
    static std::map<std::string, int64_t> GetProcValues();

   private:
    static std::map<std::string, int64_t> GetMemoryValuesFromFile(
        const std::string &path);
    static int64_t GetOomScore();
};

}  // namespace memory_advice
