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

#include "memory_advice/memory_advice.h"

// These functions are implemented in memory_advice.cpp.
// They are mostly the same as the C interface, but take C++ types.

namespace memory_advice {

MemoryAdvice_ErrorCode Init();
MemoryAdvice_ErrorCode Init(const char* params);
MemoryAdvice_ErrorCode GetAdvice(MemoryAdvice_JsonSerialization* advice);
MemoryAdvice_ErrorCode GetMemoryState(MemoryAdvice_MemoryState* state);
MemoryAdvice_ErrorCode GetAvailableMemory(int64_t* estimate);
MemoryAdvice_ErrorCode RegisterWatcher(uint64_t intervalMillis,
                                       MemoryAdvice_WatcherCallback callback,
                                       void* user_data);
MemoryAdvice_ErrorCode UnregisterWatcher(MemoryAdvice_WatcherCallback callback);

}  // namespace memory_advice
