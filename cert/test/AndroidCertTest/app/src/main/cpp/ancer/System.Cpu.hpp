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

#ifndef _SYSTEM_CPU_HPP
#define _SYSTEM_CPU_HPP

#include <array>
#include <string>
#include <vector>

namespace ancer {

enum class ThreadAffinity {
    kLittleCore, kMiddleCore, kBigCore, kAll, kAffinityCount = kAll
};
constexpr std::array kAffinityNames = { "Little", "Middle", "Big", "All" };

/**
 * Returns how many cores are in a given affinity category.
 */
[[nodiscard]] int NumCores(ThreadAffinity = ThreadAffinity::kAll);

/**
 * Sets our affinity to a specific core in the given group.
 * An index of -1 acts as SetThreadAffinity(affinity).
 */
bool SetThreadAffinity(int index, ThreadAffinity = ThreadAffinity::kAll);

/**
 * Sets our affinity to any/all of the cores in a group.
 */
bool SetThreadAffinity(ThreadAffinity affinity);

/**
 * Returns a vector of core sizes where the i-th entry corresponds to the i-th CPU core.
 */
const std::vector<ThreadAffinity> &GetCoreSizes();

[[nodiscard]] std::string GetCpuInfo();

}

#endif  // _SYSTEM_CPU_HPP
