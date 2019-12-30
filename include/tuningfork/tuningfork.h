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

/**
 * @defgroup tuningfork Tuning Fork main interface
 * The main interface to use Tuning Fork.
 * @{
 */

#pragma once

#include <stdint.h>
#include <jni.h>

/** @cond INTERNAL */

#define TUNINGFORK_MAJOR_VERSION 0
#define TUNINGFORK_MINOR_VERSION 4
#define TUNINGFORK_PACKED_VERSION ((TUNINGFORK_MAJOR_VERSION<<16)|(TUNINGFORK_MINOR_VERSION))

// Internal macros to generate a symbol to track TuningFork version, do not use directly.
#define TUNINGFORK_VERSION_CONCAT_NX(PREFIX, MAJOR, MINOR) PREFIX ## _ ## MAJOR ## _ ## MINOR
#define TUNINGFORK_VERSION_CONCAT(PREFIX, MAJOR, MINOR) TUNINGFORK_VERSION_CONCAT_NX(PREFIX, MAJOR, MINOR)
#define TUNINGFORK_VERSION_SYMBOL TUNINGFORK_VERSION_CONCAT(TuningFork_version, TUNINGFORK_MAJOR_VERSION, TUNINGFORK_MINOR_VERSION)

/** @endcond */

/**
 * @brief Instrument keys indicating time periods within a frame.
 *  Keys 64000-65535 are reserved
 */
enum InstrumentKeys {
  TFTICK_USERDEFINED_BASE = 0,
  TFTICK_SYSCPU = 64000,  ///< Frame time between ends of glSwapBuffers calls or
                          ///< Vulkan equivalent.
  TFTICK_SYSGPU = 64001,  ///< Frame time between Swappy sync fences.
  TFTICK_SWAPPY_WAIT_TIME = 64002,  ///< The time Swappy waits on sync fence.
  TFTICK_SWAPPY_SWAP_TIME =
      64003  ///< Time for call of glSwapBuffers or Vulkan equivalent.
};

/**
 * @brief A series of bytes representing a serialized protocol buffer.
 * @see CProtobufSerialization_Free for how to deallocate the memory once finished with the buffer.
 */
struct CProtobufSerialization {
    uint8_t* bytes; /// Array of bytes.
    uint32_t size; /// Size of array.
    /// Deallocation callback (may be NULL if not owned).
    void (*dealloc)(struct CProtobufSerialization*);
};

/// The instrumentation key identifies a tick point within a frame or a trace segment
typedef uint16_t TFInstrumentKey;
/// A trace handle used in TuningFork_startTrace
typedef uint64_t TFTraceHandle;
/// A time as milliseconds past the epoch.
typedef uint64_t TFTimePoint;
/// A duration in nanoseconds.
typedef uint64_t TFDuration;

/**
 * @brief All the error codes that can be returned by Tuning Fork functions.
 */
enum TFErrorCode {
  TFERROR_OK = 0,  ///< No error
  TFERROR_NO_SETTINGS =
      1,  ///< No tuningfork_settings.bin found in assets/tuningfork.
  TFERROR_NO_SWAPPY = 2,  ///< Not able to find the required Swappy functions.
  TFERROR_INVALID_DEFAULT_FIDELITY_PARAMS =
      3,  ///< `fpDefaultFileNum` is out of range.
  TFERROR_NO_FIDELITY_PARAMS =
      4,  ///< No fidelity parameters found at initialization.
  TFERROR_TUNINGFORK_NOT_INITIALIZED =
      5,  ///< A call was made before Tuning Fork was initialized.
  TFERROR_INVALID_ANNOTATION =
      6,  ///< Invalid parameter to `TuningFork_setCurrentAnnotation`.
  TFERROR_INVALID_INSTRUMENT_KEY =
      7,  ///< Invalid instrument key passed to a tick function.
  TFERROR_INVALID_TRACE_HANDLE =
      8,                ///< Invalid handle passed to `TuningFork_endTrace`.
  TFERROR_TIMEOUT = 9,  ///< Timeout in request for fidelity parameters.
  TFERROR_BAD_PARAMETER = 10,      ///< Generic bad parameter.
  TFERROR_B64_ENCODE_FAILED = 11,  ///< Could not encode a protobuf.
  TFERROR_JNI_BAD_VERSION = 12,    ///< Jni error - obsolete
  TFERROR_JNI_BAD_THREAD = 13,     ///< Jni error - obsolete
  TFERROR_JNI_BAD_ENV = 14,        ///< Jni error - obsolete
  TFERROR_JNI_EXCEPTION =
      15,  ///< Jni error - an exception was thrown. See logcat output.
  TFERROR_JNI_BAD_JVM = 16,  ///< Jni error - obsolete
  TFERROR_NO_CLEARCUT = 17,  ///< Obsolete
  TFERROR_NO_FIDELITY_PARAMS_IN_APK =
      18,  ///< No dev_tuningfork_fidelityparams_#.bin found in
           ///< assets/tuningfork.
  TFERROR_COULDNT_SAVE_OR_DELETE_FPS =
      19,  ///< Error calling `TuningFork_saveOrDeleteFidelityParamsFile`.
  TFERROR_PREVIOUS_UPLOAD_PENDING =
      20,  ///< Can't upload since another request is pending.
  TFERROR_UPLOAD_TOO_FREQUENT =
      21,                    ///< Too frequent calls to `TuningFork_flush`.
  TFERROR_NO_SUCH_KEY = 22,  ///< No such key when accessing file cache.
  TFERROR_BAD_FILE_OPERATION = 23,  ///< General file error.
  TFERROR_BAD_SETTINGS = 24,        ///< Invalid tuningfork_settings.bin file.
  TFERROR_ALREADY_INITIALIZED =
      25,  ///< TuningFork_init was called more than once.
  TFERROR_NO_SETTINGS_ANNOTATION_ENUM_SIZES =
      26,  ///< Missing part of tuningfork_settings.bin.
  TFERROR_DOWNLOAD_THREAD_ALREADY_STARTED =
      27,  ///< `TuningFork_startFidelityParamDownloadThread`
           ///< was called more than once, or called when TuningFork_init has
           ///< already started download.
  TFERROR_PLATFORM_NOT_SUPPORTED =
      28,  ///< The game or app is run on a platform not supporting Tuning fork.
           ///< Only used by Unity plugin.
};

/**
 * @defgroup TFCache Tuning Fork cache utilities
 * Optional persistent cache object to use with Tuning Fork.
 * @{
 */

/**
 * @brief Pointer to a function that can be attached to TFCache::get
 *
 * Function that will be called to get a value for a key.
 * @see TFCache
 */
typedef TFErrorCode (* PFnTFCacheGet )(uint64_t key, CProtobufSerialization* value,
                                     void* user_data);

/**
 * @brief Pointer to a function that can be attached to TFCache::set
 *
 * Function that will be called to set a value for a key.
 * @see TFCache
 */
typedef TFErrorCode (* PFnTFCacheSet )(uint64_t key, const CProtobufSerialization* value,
                                     void* user_data);

/**
 * @brief Pointer to a function that can be attached to TFCache::remove
 *
 * Function that will be called to remove an entry in the cache.
 * @see TFCache
 */
typedef TFErrorCode (* PFnTFCacheRemove )(uint64_t key, void* user_data);

/**
 * @brief An object used to cache upload data when no connection is available.
 *  If you do not supply one of these, data is saved to a temporary file.
 */
struct TFCache {
  void* user_data; ///< Data passed to each callback.
  PFnTFCacheSet set; ///< Function to set a value for a key.
  PFnTFCacheGet get; ///< Function to get a value for a key.
  PFnTFCacheRemove remove; ///< Function to remove an entry in the cache.
};

/** @} */

/**
 * @brief Pointer to a function that can be attached to TFSettings::fidelity_params_callback
 *
 * Function that will be called with the fidelity parameters that are downloaded.
 * @see TFSettings
 */
typedef void (* ProtoCallback )(const CProtobufSerialization*);

/**
 * @brief Pointer to a function that can be passed to TuningFork_setUploadCallback.
 *
 * Function that will be called on a separate thread every time TuningFork performs an upload.
 * @see TFSettings
 */
typedef void (* UploadCallback )(const char*, size_t n);

struct SwappyTracer;

/**
 * @brief Pointer to Swappy_injectTracers that can be attached to TFSettings::swappy_tracer_fn.
 * @see TFSettings
 */
typedef void (* SwappyTracerFn )(const SwappyTracer*);

/**
 * @brief Initialization settings
 *   Zero any values that are not being used.
 */
struct TFSettings {
  /**
   * Cache object to be used for upload data persistence.
   * If unset, data is persisted to /data/local/tmp/tuningfork
   */
  const TFCache* persistent_cache;
  /**
   * The address of the Swappy_injectTracers function.
   * If this is unset, you need to call TuningFork_tick yourself.
   * If it is set, telemetry for 4 instrument keys is automatically recorded.
   */
  SwappyTracerFn swappy_tracer_fn;
  /**
   * Callback
   * If set, this is called with the fidelity parameters that are downloaded.
   * If unset, you need to call TuningFork_getFidelityParameters yourself.
   */
  ProtoCallback fidelity_params_callback;
  /**
   * A serialized protobuf containing the fidelity parameters to be uploaded
   *  for training.
   * Set this to nullptr if you are not using training mode. Note that these
   *  are used instead of the default parameters loaded from the APK, if they
   *  are present and there are neither a successful download nor saved parameters.
   */
  const CProtobufSerialization* training_fidelity_params;
  /**
   * A null-terminated string containing the endpoint that Tuning Fork will connect
   * to for parameter, upload and debug requests.
   * This overrides the value in base_uri in the settings proto and is intended
   * for debugging purposes only.
   */
  const char* endpoint_uri_override;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Deallocate any memory owned by the procol buffer serialization.
 * @param ser A protocol buffer serialization
 */
inline void CProtobufSerialization_Free(CProtobufSerialization* ser) {
    if (ser->dealloc) {
        ser->dealloc(ser);
        ser->dealloc = NULL;
    }
}

/** @cond INTERNAL */

// Internal init function. Do not call directly.
TFErrorCode TuningFork_init_internal(const TFSettings *settings, JNIEnv* env, jobject context);

// Internal function to track TuningFork version bundled in a binary. Do not call directly.
// If you are getting linker errors related to TuningFork_version_x_y, you probably have a
// mismatch between the header used at compilation and the actually library used by the linker.
void TUNINGFORK_VERSION_SYMBOL();

/** @endcond */

/**
 * @brief Initialize Tuning Fork. This must be called before any other functions.
 *
 * The library will load histogram and annotation settings from your tuningfork_settings.bin file.
 * @see TFSettings for the semantics of how other settings change initialization behaviour.
 *
 * @param settings a TFSettings structure
 * @param env a JNIEnv
 * @param context the app context
 *
 * @return TFERROR_OK if successful, TFERROR_NO_SETTINGS if no settings could be found,
 *  TFERROR_BAD_SETTINGS if your tuningfork_settings.bin file was invalid or
 *  TFERROR_ALREADY_INITIALIZED if tuningfork was already initialized.
 */
static inline TFErrorCode TuningFork_init(const TFSettings *settings, JNIEnv* env, jobject context) {
    // This call ensures that the header and the linked library are from the same version
    // (if not, a linker error will be triggered because of an undefined symbol).
    TUNINGFORK_VERSION_SYMBOL();
    return TuningFork_init_internal(settings, env, context);
}

// The functions below will return TFERROR_TUNINGFORK_NOT_INITIALIZED if TuningFork_init
//  has not first been called.

/**
 * @brief A blocking call to get fidelity parameters from the server.
 * You do not need to call this if you pass in a fidelity_params_callback as part of the settings
 *  to TuningFork_init.
 * Note that once fidelity parameters are downloaded, any timing information is recorded
 *  as being associated with those parameters.
 * If you subsequently call GetFidelityParameters and a new set of parameters is downloaded,
 *  any data that is already collected will be submitted to the backend.
 * Ownership of 'params' is transferred to the caller, so they must call params->dealloc
 *  when they are done with it.
 * The parameter request is sent to:
 *  ${url_base}+'applications/'+package_name+'/apks/'+version_number+':generateTuningParameters'.
 * @param defaultParams these will be assumed current if no parameters could be downloaded.
 * @param[out] params
 * @param timeout_ms time to wait before returning from this call when no connection can be made.
 *  If zero or negative, the value in Settings.initial_request_timeout_ms is used.
 * @return TFERROR_TIMEOUT if there was a timeout before params could be downloaded.
 *  TFERROR_OK on success.
 */
TFErrorCode TuningFork_getFidelityParameters(
    const CProtobufSerialization* defaultParams,
    CProtobufSerialization* params, uint32_t timeout_ms);

/**
 * @brief Set the current annotation.
 * @param annotation the protobuf serialization of the current annotation.
 * @return TFERROR_INVALID_ANNOTATION if annotation is inconsistent with the settings.
 * @return TFERROR_OK on success.
 */
TFErrorCode TuningFork_setCurrentAnnotation(const CProtobufSerialization* annotation);

/**
 * @brief Record a frame tick that will be associated with the instrumentation key and the current
 *   annotation.
 * NB: calling the tick or trace functions from different threads is allowed, but a single
 *  instrument key should always be ticked from the same thread.
 * @param key an instrument key
 * @see the reserved instrument keys above
 * @return TFERROR_INVALID_INSTRUMENT_KEY if the instrument key is invalid.
 *  TFERROR_OK on success.
 */
TFErrorCode TuningFork_frameTick(TFInstrumentKey key);

/**
 * @brief Record a frame tick using an external time, rather than system time.
 * @param key an instrument key
 * @see the reserved instrument keys above
 * @param dt the duration you wish to record (in nanoseconds)
 * @return TFERROR_INVALID_INSTRUMENT_KEY if the instrument key is invalid.
 *  TFERROR_OK on success.
 */
TFErrorCode TuningFork_frameDeltaTimeNanos(TFInstrumentKey key, TFDuration dt);

/**
 * @brief Start a trace segment.
 * @param key an instrument key
 * @see the reserved instrument keys above
 * @param[out] handle this is filled with a new handle on success.
 * @return TFERROR_INVALID_INSTRUMENT_KEY if the instrument key is invalid.
 *  TFERROR_OK on success.
 */
TFErrorCode TuningFork_startTrace(TFInstrumentKey key, TFTraceHandle* handle);

/**
 * @brief Stop and record a trace segment.
 * @param handle this is a handle previously returned by TuningFork_startTrace
 * @return TFERROR_INVALID_TRACE_HANDLE if the handle is invalid.
 * TFERROR_OK on success.
 */
TFErrorCode TuningFork_endTrace(TFTraceHandle handle);

/**
 * @brief Force upload of the current histograms.
 * @return TFERROR_OK if the upload could be initiated.
 *  TFERROR_PREVIOUS_UPLOAD_PENDING if there is a previous upload blocking this one.
 *  TFERROR_UPLOAD_TOO_FREQUENT if less than a minute has elapsed since the previous upload.
 */
TFErrorCode TuningFork_flush();

/**
 * @brief Set a callback to be called on a separate thread every time TuningFork performs an upload.
 * @param cbk
 * @return TFERROR_OK unless Tuning Fork wasn't initialized.
 */
TFErrorCode TuningFork_setUploadCallback(UploadCallback cbk);

/**
 * @brief Clean up all memory owned by Tuning Fork and kill any threads.
 * @return TFERROR_OK unless Tuning Fork wasn't initialized.
 */
TFErrorCode TuningFork_destroy();

/**
 * @brief Set the currently active fidelity parameters.
 * This function overrides any parameters that have been downloaded if in experiment mode.
 * Use when, for instance, the player has manually changed game quality  settings.
 * This flushes (i.e. uploads) any data associated with any previous parameters.
 * @param params The protocol buffer encoded parameters.
 * @return TFERROR_OK if the parameters could be set.
 */
TFErrorCode TuningFork_setFidelityParameters(const CProtobufSerialization* params);

#ifdef __cplusplus
}
#endif

/** @} */
