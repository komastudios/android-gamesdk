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

#include <jni.h>
#include <android/log.h>
#include <string>


namespace ancer::Log {
    // Strongly-typed log tag to avoid ambiguity.
    // e.g., Log::I("This should be the format string '%s'", "But it was treated as the tag");
    struct Tag { const char* tag; };

// A bunch of overloads so the format can be a std::string or const char*.
// Also defines a single-string version that takes the format string and re-passes it as a string
// argument. These are deprecated but exist since there are so many existing calls that assemble the
// string up-front instead of actually using the printf-style functionality.
// These should be marked as deprecated in the future, but are left as-is for now.
#define DEFINE_LOG_OVERLOADS(Func, LogLevel) \
    template <typename... Args> void Func(Tag tag, const char* fmt, Args&&... args) noexcept \
    { __android_log_print(LogLevel, tag.tag, fmt, std::forward<Args>(args)...); } \
    template <typename... Args> void Func(Tag tag, const std::string& fmt, Args&&... args) noexcept \
    { __android_log_print(LogLevel, tag.tag, fmt.c_str(), std::forward<Args>(args)...); } \
    /*[[deprecated]]*/ inline void Func(Tag tag, const char* str) noexcept \
    { __android_log_print(LogLevel, tag.tag, "%s", str); } \
    /*[[deprecated]]*/ inline void Func(Tag tag, const std::string& str) noexcept \
    { __android_log_print(LogLevel, tag.tag, "%s", str.c_str()); }

    // @formatter:off
    DEFINE_LOG_OVERLOADS(D, ANDROID_LOG_DEBUG)
    DEFINE_LOG_OVERLOADS(I, ANDROID_LOG_INFO)
    DEFINE_LOG_OVERLOADS(W, ANDROID_LOG_WARN)
    DEFINE_LOG_OVERLOADS(E, ANDROID_LOG_ERROR)
    DEFINE_LOG_OVERLOADS(F, ANDROID_LOG_FATAL)
#ifndef NDEBUG
    DEFINE_LOG_OVERLOADS(V, ANDROID_LOG_VERBOSE)
#else
    template <typename... Args> constexpr void V(Args&&...) noexcept {}
#endif
    // @formatter:on

#undef DEFINE_LOG_OVERLOADS
}
