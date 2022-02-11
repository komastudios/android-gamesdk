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
    MEMORYADVICE_ERROR_WATCHER_NOT_FOUND =
        5,  ///< UnregisterWatcher was called with an invalid callback.
    MEMORYADVICE_ERROR_TFLITE_MODEL_INVALID =
        6,  ///< A correct TFLite model was not provided.
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

/**
 * @brief A char* representing a serialized json object.
 * @see MemoryAdvice_JsonSerialization_free for how to deallocate
 * the memory once finished with the buffer.
 */
typedef struct MemoryAdvice_JsonSerialization {
    char *json;     ///< String for the json object.
    uint32_t size;  ///< Size of the json string.
    ///< Deallocation callback (may be NULL if not owned).
    void (*dealloc)(struct MemoryAdvice_JsonSerialization *);
} MemoryAdvice_JsonSerialization;

typedef void (*MemoryAdvice_WatcherCallback)(MemoryAdvice_MemoryState state,
                                             void *user_data);

// Internal init function. Do not call directly.
MemoryAdvice_ErrorCode MemoryAdvice_initDefaultParams_internal(JNIEnv *env,
                                                               jobject context);

// Internal init function. Do not call directly.
MemoryAdvice_ErrorCode MemoryAdvice_init_internal(JNIEnv *env, jobject context,
                                                  const char *params);

// Internal function to track MemoryAdvice version bundled in a binary. Do not
// call directly. If you are getting linker errors related to
// MemoryAdvice_version_x_y, you probably have a mismatch between the header
// used at compilation and the actually library used by the linker.
void MEMORY_ADVICE_VERSION_SYMBOL();

/** @endcond */

/**
 * @brief Initialize the Memory Advice library. This must be called before any
 * other functions.
 *
 * This version of the init function will use the library provided default
 * params.
 *
 * @param env a JNIEnv
 * @param context the app context
 *
 * @return MEMORYADVICE_ERROR_OK if successful,
 * @return MEMORYADVICE_ERROR_ALREADY_INITIALIZED if Memory Advice was already
 * initialized.
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
 * @brief Initialize the Memory Advice library. This must be called before any
 * other functions.
 *
 * This version of the init function will read the given params instead of
 * using the library provided default params.
 *
 * @param env a JNIEnv
 * @param context the app context
 * @param params the advisor parameters to run the library with
 *
 * @return MEMORYADVICE_ERROR_OK if successful,
 * @return MEMORYADVICE_ERROR_ADVISOR_PARAMETERS_INVALID if the provided
 * parameters are not a valid JSON object,
 * @return MEMORYADVICE_ERROR_ALREADY_INITIALIZED if Memory Advice was already
 * initialized.
 */
static inline MemoryAdvice_ErrorCode MemoryAdvice_init(JNIEnv *env,
                                                       jobject context,
                                                       const char *params) {
    // This call ensures that the header and the linked library are from the
    // same version (if not, a linker error will be triggered because of an
    // undefined symbol).
    MEMORY_ADVICE_VERSION_SYMBOL();
    return MemoryAdvice_init_internal(env, context, params);
}

/**
 * @brief Returns the current memory state.
 *
 * @param state a pointer to a MemoryAdvice_MemoryState, in which the
 * memory state will be written
 *
 * @return MEMORYADVICE_ERROR_OK if successful,
 * @return MEMORYADVICE_ERROR_NOT_INITIALIZED if Memory Advice was not yet
 * initialized.
 */
MemoryAdvice_ErrorCode MemoryAdvice_getMemoryState(
    MemoryAdvice_MemoryState *state);

/**
 * @brief Returns the advice regarding the current memory state.
 *
 * @param advice a pointer to a struct, in which the memory advice will be
 * written.
 *
 * @return MEMORYADVICE_ERROR_OK if successful,
 * @return MEMORYADVICE_ERROR_NOT_INITIALIZED if Memory Advice was not yet
 * initialized.
 */
MemoryAdvice_ErrorCode MemoryAdvice_getAdvice(
    MemoryAdvice_JsonSerialization *advice);

/**
 * @brief Calculates an estimate for the amount of memory that can safely be
 * allocated, in bytes.
 *
 * @param estimate a pointer to an int64_t, in which the estimate will
 * be written
 *
 * @return MEMORYADVICE_ERROR_OK if successful,
 * @return MEMORYADVICE_ERROR_NOT_INITIALIZED if Memory Advice was not yet
 * initialized.
 */
MemoryAdvice_ErrorCode MemoryAdvice_getAvailableMemory(int64_t *estimate);

/**
 * @brief Registers a watcher that polls the Memory Advice library periodically,
 * and invokes the watcher callback when the memory state goes critical.
 *
 * This function creates another thread that calls MemoryAdvice_getMemoryState
 * every `intervalMillis` milliseconds. If the returned state is not
 * MEMORYADVICE_STATE_OK, then calls the watcher callback with the current
 * state.
 *
 * @param intervalMillis the interval at which the Memory Advice library will be
 * polled
 * @param callback the callback function that will be invoked if memory goes
 * critical
 * @param user_data context to pass to the callback function
 *
 * @return MEMORYADVICE_ERROR_OK if successful,
 * @return MEMORYADVICE_ERROR_NOT_INITIALIZED if Memory Advice was not yet
 * initialized,
 */
MemoryAdvice_ErrorCode MemoryAdvice_registerWatcher(
    uint64_t intervalMillis, MemoryAdvice_WatcherCallback callback,
    void *user_data);

/**
 * @brief Deallocate any memory owned by the json serialization.
 * @param ser A json serialization
 */
inline void MemoryAdvice_JsonSerialization_free(
    MemoryAdvice_JsonSerialization *ser) {
    if (ser->dealloc) {
        ser->dealloc(ser);
        ser->dealloc = NULL;
    }
}

/**
 * @brief Removes all watchers with the given callback that were previously
 * registered using
 * {@link MemoryAdvice_registerWatcher}.
 *
 * @return MEMORYADVICE_ERROR_OK if successful,
 * @return MEMORYADVICE_ERROR_NOT_INITIALIZED if Memory Advice was not yet
 * initialized.
 * @return MEMORYADVICE_ERROR_WATCHER_NOT_FOUND if the given callback wasn't
 * previously registered.
 */
MemoryAdvice_ErrorCode MemoryAdvice_unregisterWatcher(
    MemoryAdvice_WatcherCallback callback);

#ifdef __cplusplus
}  // extern "C" {
#endif
