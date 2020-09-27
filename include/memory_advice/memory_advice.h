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

/*
 * This is the main interface to the Android Games Memory Advice API. Currently
 * this is only the skeleton of the library, with minimum/no functionality.
 *
 * TODO: Complete this documentation as the library finalizes.
 */

/**
 * @defgroup memory_advice Memory Advice main interface
 * The main interface to use Memory Advice.
 * @{
 */

#pragma once

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @cond INTERNAL */

#define MEMORY_ADVICE_MAJOR_VERSION 1
#define MEMORY_ADVICE_MINOR_VERSION 0
#define MEMORY_ADVICE_PACKED_VERSION                            \
    ANDROID_GAMESDK_PACKED_VERSION(MEMORY_ADVICE_MAJOR_VERSION, \
                                   MEMORY_ADVICE_MINOR_VERSION)

// Internal macros to generate a symbol to track Memory Advice version, do not
// use directly.
#define MEMORY_ADVICE_VERSION_CONCAT_NX(PREFIX, MAJOR, MINOR) \
    PREFIX##_##MAJOR##_##MINOR
#define MEMORY_ADVICE_VERSION_CONCAT(PREFIX, MAJOR, MINOR) \
    MEMORY_ADVICE_VERSION_CONCAT_NX(PREFIX, MAJOR, MINOR)
#define MEMORY_ADVICE_VERSION_SYMBOL                          \
    MEMORY_ADVICE_VERSION_CONCAT(MemoryAdvice_version,        \
                                 MEMORY_ADVICE_MAJOR_VERSION, \
                                 MEMORY_ADVICE_MINOR_VERSION)

// Internal init function. Do not call directly.
void MemoryAdvice_init_internal(JNIEnv *env, jobject context);

// Internal function to track MemoryAdvice version bundled in a binary. Do not
// call directly. If you are getting linker errors related to
// MemoryAdvice_version_x_y, you probably have a mismatch between the header
// used at compilation and the actually library used by the linker.
void MEMORY_ADVICE_VERSION_SYMBOL();

/** @endcond */

/**
 * Initialize the Memory Advice library. This must be called before any other
 * function.
 */
static inline void MemoryAdvice_init(JNIEnv *env, jobject context) {
    // This call ensures that the header and the linked library are from the
    // same version (if not, a linker error will be triggered because of an
    // undefined symbol).
    MEMORY_ADVICE_VERSION_SYMBOL();
    MemoryAdvice_init_internal(env, context);
}

/**
 * A simple function to test the library access. If linked correctly, this
 * function will return the testValue provided.
 */
int MemoryAdvice_testLibraryAccess(int testValue);

#ifdef __cplusplus
}  // extern "C" {
#endif