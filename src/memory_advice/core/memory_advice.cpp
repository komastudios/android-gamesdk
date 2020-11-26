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

#include "memory_advice/memory_advice.h"

#include <string>

#include "memory_advice_impl.h"
#include "memory_advice_internal.h"
#include "state_watcher.h"

#define LOG_TAG "MemoryAdvice"
#include "Log.h"
#include "json11/json11.hpp"

namespace memory_advice {

using namespace json11;

extern const char* parameters_string;

static MemoryAdviceImpl* s_impl;
static std::unique_ptr<StateWatcher> s_watcher;

MemoryAdvice_ErrorCode Init(const char* params) {
    if (s_impl != nullptr) return MEMORYADVICE_ERROR_ALREADY_INITIALIZED;
    s_impl = new MemoryAdviceImpl(params);
    return s_impl->InitializationErrorCode();
}

MemoryAdvice_ErrorCode Init() { return Init(parameters_string); }

MemoryAdvice_ErrorCode GetAdvice(const char** advice) {
    if (s_impl == nullptr) return MEMORYADVICE_ERROR_NOT_INITIALIZED;
    *advice = Json(s_impl->GetAdvice()).dump().c_str();
    return MEMORYADVICE_ERROR_OK;
}

MemoryAdvice_ErrorCode GetMemoryState(MemoryAdvice_MemoryState* state) {
    if (s_impl == nullptr) return MEMORYADVICE_ERROR_NOT_INITIALIZED;
    *state = s_impl->GetMemoryState();
    return MEMORYADVICE_ERROR_OK;
}

MemoryAdvice_ErrorCode GetAvailableMemory(int64_t* estimate) {
    if (s_impl == nullptr) return MEMORYADVICE_ERROR_NOT_INITIALIZED;
    *estimate = s_impl->GetAvailableMemory();
    return MEMORYADVICE_ERROR_OK;
}

MemoryAdvice_ErrorCode SetWatcher(uint64_t intervalMillis,
                                  MemoryAdvice_WatcherCallback callback) {
    if (s_impl == nullptr) return MEMORYADVICE_ERROR_NOT_INITIALIZED;
    if (s_watcher.get() != nullptr)
        return MEMORYADVICE_ERROR_WATCHER_ALREADY_SET;
    s_watcher =
        std::make_unique<StateWatcher>(s_impl, callback, intervalMillis);
    return MEMORYADVICE_ERROR_OK;
}

MemoryAdvice_ErrorCode RemoveWatcher() {
    if (s_impl == nullptr) return MEMORYADVICE_ERROR_NOT_INITIALIZED;
    if (s_watcher.get() != nullptr) s_watcher.reset();
    return MEMORYADVICE_ERROR_OK;
}

}  // namespace memory_advice
