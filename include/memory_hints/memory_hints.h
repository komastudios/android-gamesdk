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

/*
 * TODO: complete this doc
 * This is the main interface to [...].
 *
 * It is part of the Android Games SDK and [...].
 *
 * See the documentation at [...].
 */

/**
 * @defgroup memory_hints Memory Hints main interface
 * The main interface to use Memory Hints.
 * @{
 */

#pragma once

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @cond INTERNAL */

#define MEMORY_HINTS_MAJOR_VERSION 1
#define MEMORY_HINTS_MINOR_VERSION 0
#define MEMORY_HINTS_PACKED_VERSION                            \
    ANDROID_GAMESDK_PACKED_VERSION(MEMORY_HINTS_MAJOR_VERSION, \
                                   MEMORY_HINTS_MINOR_VERSION)

// Internal macros to generate a symbol to track Memory Hints version, do not use
// directly.
#define MEMORY_HINTS_VERSION_CONCAT_NX(PREFIX, MAJOR, MINOR) \
    PREFIX##_##MAJOR##_##MINOR
#define MEMORY_HINTS_VERSION_CONCAT(PREFIX, MAJOR, MINOR) \
    MEMORY_HINTS_VERSION_CONCAT_NX(PREFIX, MAJOR, MINOR)
#define MEMORY_HINTS_VERSION_SYMBOL                                           \
    MEMORY_HINTS_VERSION_CONCAT(MemoryHints_version, MEMORY_HINTS_MAJOR_VERSION, \
                              MEMORY_HINTS_MINOR_VERSION)

// Internal init function. Do not call directly.
void MemoryHints_init_internal(JNIEnv* env, jobject context);

// Internal function to track MemoryHints version bundled in a binary. Do not
// call directly. If you are getting linker errors related to
// MemoryHints_version_x_y, you probably have a mismatch between the header used
// at compilation and the actually library used by the linker.
void MEMORY_HINTS_VERSION_SYMBOL();

/** @endcond */

/**
 * TODO: complete this doc
 */
static inline void MemoryHints_init(JNIEnv* env, jobject context) {
    // This call ensures that the header and the linked library are from the
    // same version (if not, a linker error will be triggered because of an
    // undefined symbol).
    MEMORY_HINTS_VERSION_SYMBOL();
    MemoryHints_init_internal(env, context);
}

/**
 * TODO: complete this doc
 */
int TuningFork_doSomething();