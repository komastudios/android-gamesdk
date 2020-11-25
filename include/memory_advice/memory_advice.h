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
#define MEMORY_ADVICE_BUGFIX_VERSION 0
#define MEMORY_ADVICE_PACKED_VERSION                            \
    ANDROID_GAMESDK_PACKED_VERSION(MEMORY_ADVICE_MAJOR_VERSION, \
                                   MEMORY_ADVICE_MINOR_VERSION, \
                                   MEMORY_ADVICE_BUGFIX_VERSION)

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

/**
 * @brief All the error codes that can be returned by MemoryAdvice functions.
 */
typedef enum MemoryAdvice_ErrorCode {
    MEMORYADVICE_ERROR_OK = 0,  ///< No error
    MEMORYADVICE_ERROR_NOT_INITIALIZED =
        1,  ///< A call was made before MemoryAdvice was initialized.
    MEMORYADVICE_ERROR_ALREADY_INITIALIZED =
        2,  ///< MemoryAdvice_init was called more than once.
    MEMORYADVICE_ERROR_LOOKUP_TABLE_INVALID =
        3,  ///< The provided lookup table was not a valid json object.
    MEMORYADVICE_ERROR_ADVISOR_PARAMETERS_INVALID =
        4,  ///< The provided advisor parameters was not a valid json object.
    MEMORYADVICE_ERROR_WATCHER_ALREADY_SET =
        5,  ///< SetWatcher function was called while there was already a
            ///< watcher in place.
} MemoryAdvice_ErrorCode;

/**
 * @brief All possible memory states that can be reported by the library.
 */
typedef enum MemoryAdvice_MemoryState {
    MEMORYADVICE_STATE_UNKNOWN = 0,  ///< The memory state cannot be determined.
    MEMORYADVICE_STATE_OK =
        1,  ///< The application can safely allocate significant memory.
    MEMORYADVICE_STATE_APPROACHING_LIMIT =
        2,  ///< The application should not allocate significant memory.
    MEMORYADVICE_STATE_CRITICAL =
        3,  ///< The application should free memory as soon as possible, until
            ///< the memory state changes.
} MemoryAdvice_MemoryState;

typedef void (*CallbackFunction)(MemoryAdvice_MemoryState);

// Internal init function. Do not call directly.
MemoryAdvice_ErrorCode MemoryAdvice_initDefaultParams_internal(JNIEnv *env,
                                                               jobject context);

// Internal init function. Do not call directly.
MemoryAdvice_ErrorCode MemoryAdvice_init_internal(JNIEnv *env, jobject context,
                                                  char *params);

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
static inline MemoryAdvice_ErrorCode MemoryAdvice_initDefaultParams(
    JNIEnv *env, jobject context) {
    // This call ensures that the header and the linked library are from the
    // same version (if not, a linker error will be triggered because of an
    // undefined symbol).
    MEMORY_ADVICE_VERSION_SYMBOL();
    return MemoryAdvice_initDefaultParams_internal(env, context);
}

/**
 * Initialize the Memory Advice library. This must be called before any other
 * function.
 */
static inline MemoryAdvice_ErrorCode MemoryAdvice_init(JNIEnv *env,
                                                       jobject context,
                                                       char *params) {
    // This call ensures that the header and the linked library are from the
    // same version (if not, a linker error will be triggered because of an
    // undefined symbol).
    MEMORY_ADVICE_VERSION_SYMBOL();
    return MemoryAdvice_init_internal(env, context, params);
}

// Returns the current memory state
MemoryAdvice_ErrorCode MemoryAdvice_getMemoryState(
    MemoryAdvice_MemoryState *state);

// Returns an advice object, in form of a JSON
MemoryAdvice_ErrorCode MemoryAdvice_getAdvice(const char **advice);

// Use to set a watcher that checks for memory state and invokes the callback
// function if state goes critical
MemoryAdvice_ErrorCode MemoryAdvice_setWatcher(uint64_t intervalMillis,
                                               CallbackFunction callback);

// Removes the watcher set
MemoryAdvice_ErrorCode MemoryAdvice_removeWatcher();

#ifdef __cplusplus
}  // extern "C" {
#endif
