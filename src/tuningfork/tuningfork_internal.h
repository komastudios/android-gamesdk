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

#pragma once

#include "tuningfork/tuningfork.h"
#include "tuningfork/tuningfork_extra.h"
#include "tuningfork/protobuf_util.h"
#include "swappy/swappyGL_extra.h"

#include <stdint.h>
#include <string>
#include <chrono>
#include <vector>
#include <jni.h>

#include "jnictx.h"

class AAsset;

namespace tuningfork {

// The instrumentation key identifies a tick point within a frame or a trace segment
typedef uint16_t InstrumentationKey;
typedef uint64_t TraceHandle;
typedef std::chrono::steady_clock::time_point TimePoint;
typedef std::chrono::steady_clock::duration Duration;
typedef std::chrono::system_clock::time_point SystemTimePoint;
typedef std::chrono::system_clock::duration SystemDuration;

struct TimeInterval {
    std::chrono::system_clock::time_point start, end;
};

struct TFHistogram {
    int32_t instrument_key;
    float bucket_min;
    float bucket_max;
    int32_t n_buckets;
};

struct Settings {
    struct AggregationStrategy {
        enum class Submission {
            TICK_BASED,
            TIME_BASED
        };
        Submission method;
        uint32_t intervalms_or_count;
        uint32_t max_instrumentation_keys;
        std::vector<uint32_t> annotation_enum_size;
    };
    TFSettings c_settings;
    AggregationStrategy aggregation_strategy;
    std::vector<TFHistogram> histograms;
    std::string base_uri;
    std::string api_key;
    std::string default_fp_filename;
    uint32_t initial_request_timeout_ms;
    uint32_t ultimate_request_timeout_ms;
    int32_t loading_annotation_index;
    int32_t level_annotation_index;
};

// Extra information that is uploaded with the ClearCut proto.
struct ExtraUploadInfo {
    std::string experiment_id;
    std::string session_id;
    uint64_t total_memory_bytes;
    uint32_t gl_es_version;
    std::string build_fingerprint;
    std::string build_version_sdk;
    std::vector<uint64_t> cpu_max_freq_hz;
    std::string apk_package_name;
    uint32_t apk_version_code;
    uint32_t tuningfork_version;
};

class IdProvider {
  public:
    virtual uint64_t DecodeAnnotationSerialization(const ProtobufSerialization& ser,
                                                   bool* loading = nullptr) const = 0;
    virtual TFErrorCode MakeCompoundId(InstrumentationKey k,
                                       uint64_t annotation_id,
                                       uint64_t& id);
};

class Backend {
public:
    virtual ~Backend() {};
    virtual TFErrorCode Process(const std::string& tuningfork_log_event) = 0;
};

class ParamsLoader {
public:
    virtual ~ParamsLoader() {};
    virtual TFErrorCode GetFidelityParams(const JniCtx& jni,
                                          const ExtraUploadInfo& info,
                                          const std::string& url_base,
                                          const std::string& api_key,
                                          ProtobufSerialization &fidelity_params,
                                          std::string& experiment_id,
                                          uint32_t timeout_ms);
};

// TODO(willosborn): remove this
class ProtoPrint {
public:
    virtual ~ProtoPrint() {};
    virtual void Print(const ProtobufSerialization &tuningfork_log_event);
};

// You can provide your own time source rather than steady_clock by inheriting this and passing
//   it to init.
class ITimeProvider {
public:
    virtual std::chrono::steady_clock::time_point Now() = 0;
    virtual std::chrono::system_clock::time_point SystemNow() = 0;
};

// This encapsulates the callbacks that are passed to Swappy at initialization, if it is
//  enabled + available.
class SwappyTraceWrapper {
    SwappyTracerFn swappyTracerFn_;
    SwappyTracer trace_;
    TFTraceHandle waitTraceHandle_ = 0;
    TFTraceHandle swapTraceHandle_ = 0;
public:
    SwappyTraceWrapper(const Settings& settings, const JniCtx& jni);
    // Swappy trace callbacks
    static void StartFrameCallback(void* userPtr, int /*currentFrame*/,
                                         long /*currentFrameTimeStampMs*/);
    static void PreWaitCallback(void* userPtr);
    static void PostWaitCallback(void* userPtr);
    static void PreSwapBuffersCallback(void* userPtr);
    static void PostSwapBuffersCallback(void* userPtr, long /*desiredPresentationTimeMs*/);
};

// If no backend is passed, the default backend, which uploads to the google endpoint is used.
// If no timeProvider is passed, std::chrono::steady_clock is used.
// If no env is passed, there can be no upload or download.
TFErrorCode Init(const Settings &settings, const JniCtx& jni,
                 const ExtraUploadInfo* extra_info = 0,
                 Backend *backend = 0, ParamsLoader *loader = 0,
                 ITimeProvider *time_provider = 0);

// Use save_dir to initialize the persister if it's not already set
void CheckSettings(Settings &c_settings, const std::string& save_dir);

// Blocking call to get fidelity parameters from the server.
// Returns true if parameters could be downloaded within the timeout, false otherwise.
// Note that once fidelity parameters are downloaded, any timing information is recorded
//  as being associated with those parameters.
// If you subsequently call GetFidelityParameters, any data that is already collected will be
// submitted to the backend.
TFErrorCode GetFidelityParameters(const ProtobufSerialization& defaultParams,
                           ProtobufSerialization &params, uint32_t timeout_ms);

// Protobuf serialization of the current annotation
TFErrorCode SetCurrentAnnotation(const ProtobufSerialization &annotation);

// Record a frame tick that will be associated with the instrumentation key and the current
//   annotation
TFErrorCode FrameTick(InstrumentationKey id);

// Record a frame tick using an external time, rather than system time
TFErrorCode FrameDeltaTimeNanos(InstrumentationKey id, Duration dt);

// Start a trace segment
TFErrorCode StartTrace(InstrumentationKey key, TraceHandle& handle);

// Record a trace with the key and annotation set using startTrace
TFErrorCode EndTrace(TraceHandle h);

TFErrorCode SetUploadCallback(UploadCallback cbk);

TFErrorCode Flush();

TFErrorCode Destroy();

// The default histogram that is used if the user doesn't specify one in Settings
TFHistogram DefaultHistogram();

// Get the object that holds JNI env and context.
// Tuning fork must have been initialized.
const JniCtx& GetJniCtx();

// Load default fidelity params from either the saved file or the file in settings.fp_default_filename, then
//  start the download thread.
TFErrorCode GetDefaultsFromAPKAndDownloadFPs(const Settings& settings, const JniCtx& jni);

TFErrorCode KillDownloadThreads();

// Load settings from assets/tuningfork/tuningfork_settings.bin.
// Ownership of @p settings is passed to the caller: call
//  TFSettings_Free to deallocate data stored in the struct.
// Returns TFERROR_OK and fills 'settings' if the file could be loaded.
// Returns TFERROR_NO_SETTINGS if the file was not found.
TFErrorCode FindSettingsInApk(Settings* settings, const JniCtx& jni);

} // namespace tuningfork
