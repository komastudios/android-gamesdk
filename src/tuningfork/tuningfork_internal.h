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

struct Settings {
    struct AggregationStrategy {
        enum Submission {
            TICK_BASED,
            TIME_BASED
        };
        Submission method;
        uint32_t intervalms_or_count;
        uint32_t max_instrumentation_keys;
        std::vector<uint32_t> annotation_enum_size;
    };
    AggregationStrategy aggregation_strategy;
    std::vector<TFHistogram> histograms;
    const TFCache* persistent_cache;
    std::string base_uri;
    std::string api_key;
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
    virtual uint64_t DecodeAnnotationSerialization(const ProtobufSerialization& ser) const = 0;
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

// If no backend is passed, the default backend, which uploads to the google endpoint is used.
// If no timeProvider is passed, std::chrono::steady_clock is used.
// If no env is passed, there can be no upload or download.
TFErrorCode Init(const Settings &settings, const JniCtx& jni,
                 const ExtraUploadInfo* extra_info = 0,
                 Backend *backend = 0, ParamsLoader *loader = 0,
                 ITimeProvider *time_provider = 0);

// Use save_dir to initialize the persister if it's not already set
void CopySettings(const TFSettings &c_settings, const std::string& save_dir,
                  Settings &settings_out);

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

// The default histogram that is used if the user doesn't specify one in Settings
TFHistogram DefaultHistogram();

// Get the object that holds JNI env and context.
// Tuning fork must have been initialized.
const JniCtx& GetJniCtx();

} // namespace tuningfork

extern "C" {

TFErrorCode CInit(const TFSettings *c_settings, JNIEnv* env, jobject context);

// Initialize tuning fork and automatically inject tracers into Swappy.
// There will be at least 2 tick points added.
TFErrorCode TuningFork_initWithSwappy(const TFSettings* settings,
                                      JNIEnv* env, jobject context);

// Load default fidelity params from either the saved file or the file in settings.fp_default_filename, then
//  start the download thread.
TFErrorCode TuningFork_getDefaultsFromAPKAndDownloadFPs(const TFSettings* settings, JNIEnv* env, jobject context);

} // extern "C"
