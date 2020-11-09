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

#define LOG_TAG "MemoryAdvice"
#include "Log.h"

namespace memory_advice {

static std::unique_ptr<MemoryAdviceImpl> s_impl;

MemoryAdvice_ErrorCode Init() {
    if (s_impl.get() != nullptr) return MEMORYADVICE_ERROR_ALREADY_INITIALIZED;
    s_impl = std::make_unique<MemoryAdviceImpl>();
    return s_impl->InitializationErrorCode();
}

uint32_t TestLibraryAccess(uint32_t testValue) { return testValue; }
}  // namespace memory_advice
