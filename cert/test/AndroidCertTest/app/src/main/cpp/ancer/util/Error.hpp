/*
 * Copyright 2019 The Android Open Source Project
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

#include <exception>

#include "Log.hpp"


namespace ancer {
// To be called when something goes wrong that makes testing impossible.
// Keep in mind these are meant to be run for/by developers, so error handling basically comes down
// to "Log it, crash, get someone to look at it": Even a bad config file is reason to terminate.
    template <typename... Args>
    [[noreturn]] void FatalError(Args&& ... args) noexcept {
        if constexpr ( sizeof...(args) > 0 ) {
            Log::F(std::forward<Args>(args)...);
        }
        // TODO: Flush logs?
        std::terminate();
    }
}
