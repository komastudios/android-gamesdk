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

#include <atomic>
#include <memory>
#include <thread>

#include "memory_advice_impl.h"

namespace memory_advice {

class StateWatcher {
   public:
    StateWatcher(MemoryAdviceImpl* impl, CallbackFunction callback,
                 uint64_t interval)
        : callback_(callback),
          interval_(interval),
          looping_(true),
          impl_(impl),
          thread_(std::make_unique<std::thread>(&StateWatcher::Looper, this)) {}
    ~StateWatcher();

   private:
    MemoryAdviceImpl* impl_;
    std::atomic<bool> looping_;
    std::unique_ptr<std::thread> thread_;
    CallbackFunction callback_;
    uint64_t interval_;
    void Looper();
};

}  // namespace memory_advice
