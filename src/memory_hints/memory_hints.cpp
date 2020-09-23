/*
 * Copyright 2018 The Android Open Source Project
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

#include <string>

#include "memory_hints_internal.h"

#define LOG_TAG "MemoryHints"
#include "Log.h"

namespace memory_hints {

// A function from the internal C++ implementation.
std::string MakeSomething() {
    ALOGV("MakeSomething called");
    return "this is a test";
}

}  // namespace memory_hints
