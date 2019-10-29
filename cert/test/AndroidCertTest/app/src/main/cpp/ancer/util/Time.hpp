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

#include <chrono>
#include <utility>


// Note that the work has been done to automatically support converting these to/from JSON. This
// includes using floating-point and automatic conversion for inputs, so a user's "0.5sec" will
// automatically be converted into Milliseconds{500}.
namespace ancer {
    using SteadyClock = std::chrono::steady_clock;

    // An arbitrary timestamp & duration. No guarantee is given as to what the underlying period is;
    // only that they are consistent and comparable on the same device. If a period needs to be in a
    // specific unit, use duration_cast.
    using Timestamp = SteadyClock::time_point;
    using Duration = decltype(std::declval<Timestamp>() - std::declval<Timestamp>());

    using Seconds = std::chrono::seconds;
    using Milliseconds = std::chrono::milliseconds;
    using Nanoseconds = std::chrono::nanoseconds;

    // For when there are specific storage requirements or working with a floating-point type is
    // more convenient (e.g., rendering code that's written with a delta in seconds).
    // Not that JSON serialization to/from these types is not currently supported; this can change
    // if it becomes an issue.
    template <typename T> using SecondsAs      = std::chrono::duration<T, Seconds::period>;
    template <typename T> using MillisecondsAs = std::chrono::duration<T, Milliseconds::period>;
    template <typename T> using NanosecondsAs  = std::chrono::duration<T, Nanoseconds::period>;

    using std::chrono::duration_cast;
    using namespace std::chrono_literals;
} // namespace ancer

#include "Time.inl"
