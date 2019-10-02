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

#include "tuningfork_internal.h"

#include <cinttypes>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <sstream>
#include <atomic>

#define LOG_TAG "TuningFork"
#include "Log.h"
#include "Trace.h"

#include "histogram.h"
#include "prong.h"
#include "uploadthread.h"
#include "ge_backend.h"
#include "annotation_util.h"
#include "crash_handler.h"
#include "tuningfork_utils.h"

/* Annotations come into tuning fork as a serialized protobuf. The protobuf can only have
 * enums in it. We form an integer annotation id from the annotation interpreted as a mixed-radix
 * number. E.g. say we have the following in the proto:
 * enum A { A_1 = 1, A_2 = 2, A_3 = 3};
 * enum B { B_1 = 1, B_2 = 2};
 * enum C { C_1 = 1};
 * message Annotation { optional A a = 1; optional B b = 2; optional C c = 3};
 * Then a serialization of 'b : B_1' might be:
 * 0x16 0x01
 * https://developers.google.com/protocol-buffers/docs/encoding
 * Note the shift of 3 bits for the key.
 *
 * Assume we have 2 possible instrumentation keys: NUM_IKEY = 2
 *
 * The annotation id then is (0 + 1*4 + 0)*NUM_IKEY = 8, where the factor of 4 comes from the radix
 * associated with 'a', i.e. 3 values for enum A + option missing
 *
 * A compound id is formed from the annotation id and the instrument key:
 * compound_id = annotation_id + instrument_key;
 *
 * So for instrument key 1, the compound_id with the above annotation is 9
 *
 * This compound_id is used to look up a histogram in the ProngCache.
 *
 * annotation_radix_mult_ stores the multiplied radixes, so for the above, it is:
 * [4 4*3 4*3*2] = [4 12 24]
 * and the maximum number of annotations is 24
 *
 * */

namespace tuningfork {

typedef uint64_t AnnotationId;

class ChronoTimeProvider : public ITimeProvider {
public:
    virtual TimePoint Now() {
        return std::chrono::steady_clock::now();
    }
    virtual SystemTimePoint SystemNow() {
        return std::chrono::system_clock::now();
    }
};

class TuningForkImpl : public IdProvider {
private:
    CrashHandler crash_handler_;
    Settings settings_;
    std::unique_ptr<ProngCache> prong_caches_[2];
    ProngCache *current_prong_cache_;
    TimePoint last_submit_time_;
    std::unique_ptr<gamesdk::Trace> trace_;
    std::vector<TimePoint> live_traces_;
    Backend *backend_;
    ParamsLoader *loader_;
    UploadThread upload_thread_;
    SerializedAnnotation current_annotation_;
    std::vector<uint32_t> annotation_radix_mult_;
    AnnotationId current_annotation_id_;
    ITimeProvider *time_provider_;
    std::vector<InstrumentationKey> ikeys_;
    std::atomic<int> next_ikey_;
    JniCtx jni_;
    TimePoint loading_start_;
public:
    TuningForkImpl(const Settings& settings,
                   const JniCtx& jni,
                   const ExtraUploadInfo& extra_upload_info,
                   Backend *backend,
                   ParamsLoader *loader,
                   ITimeProvider *time_provider);

    void InitHistogramSettings();

    void InitAnnotationRadixes();

    // Returns true if the fidelity params were retrieved
    TFErrorCode GetFidelityParameters(const ProtobufSerialization& defaultParams,
                    ProtobufSerialization &fidelityParams, uint32_t timeout_ms);

    // Returns the set annotation id or -1 if it could not be set
    uint64_t SetCurrentAnnotation(const ProtobufSerialization &annotation);

    TFErrorCode FrameTick(InstrumentationKey id);

    TFErrorCode FrameDeltaTimeNanos(InstrumentationKey id, Duration dt);

    // Fills handle with that to be used by EndTrace
    TFErrorCode StartTrace(InstrumentationKey key, TraceHandle& handle);

    TFErrorCode EndTrace(TraceHandle);

    void SetUploadCallback(UploadCallback cbk);

    TFErrorCode Flush();

    TFErrorCode Flush(TimePoint t, bool upload);

    const JniCtx& GetJniCtx() const {
        return jni_;
    }
private:
    Prong *TickNanos(uint64_t compound_id, TimePoint t);

    Prong *TraceNanos(uint64_t compound_id, Duration dt);

    TFErrorCode CheckForSubmit(TimePoint t, Prong *prong);

    bool ShouldSubmit(TimePoint t, Prong *prong);

    AnnotationId DecodeAnnotationSerialization(const SerializedAnnotation &ser,
                                               bool* loading) const override;

    uint32_t GetInstrumentationKey(uint64_t compoundId) {
        return compoundId % settings_.aggregation_strategy.max_instrumentation_keys;
    }

    TFErrorCode MakeCompoundId(InstrumentationKey k, AnnotationId a, uint64_t& id) override {
        int key_index;
        auto err = GetOrCreateInstrumentKeyIndex(k, key_index);
        if (err!=TFERROR_OK) return err;
        id = key_index + a;
        return TFERROR_OK;
    }

    SerializedAnnotation SerializeAnnotationId(uint64_t);

    bool keyIsValid(InstrumentationKey key) const;

    TFErrorCode GetOrCreateInstrumentKeyIndex(InstrumentationKey key, int& index);

    bool LoadingNextScene() const { return loading_start_!=TimePoint::min(); }

    bool IsLoadingAnnotationId(uint64_t id) const;

};

static std::unique_ptr<TuningForkImpl> s_impl;
static std::unique_ptr<ChronoTimeProvider> s_chrono_time_provider
        = std::make_unique<ChronoTimeProvider>();
static GEBackend s_backend;
static ParamsLoader s_loader;
static ExtraUploadInfo s_extra_upload_info;
static std::unique_ptr<SwappyTraceWrapper> s_swappy_tracer;

TFErrorCode Init(const Settings &settings,
                 const JniCtx& jni,
                 const ExtraUploadInfo* extra_upload_info,
                 Backend *backend,
                 ParamsLoader *loader,
                 ITimeProvider *time_provider) {

    if (s_impl.get() != nullptr)
        return TFERROR_ALREADY_INITIALIZED;

    if (extra_upload_info == nullptr) {
        s_extra_upload_info = UploadThread::BuildExtraUploadInfo(jni);
        extra_upload_info = &s_extra_upload_info;
    }

    if (backend == nullptr) {
        bool backend_inited = s_backend.Init(jni, settings, *extra_upload_info)==TFERROR_OK;
        if(backend_inited) {
            ALOGI("TuningFork.GoogleEndpoint: OK");
            backend = &s_backend;
            loader = &s_loader;
        } else {
            ALOGW("TuningFork.GoogleEndpoint: FAILED");
        }
    }

    if (time_provider == nullptr) {
        time_provider = s_chrono_time_provider.get();
    }

    s_impl = std::make_unique<TuningForkImpl>(settings, jni, *extra_upload_info,
                                              backend, loader, time_provider);

    // Set up the Swappy tracer after TuningFork is initialized
    if (settings.c_settings.swappy_tracer_fn != nullptr) {
        s_swappy_tracer = std::unique_ptr<SwappyTraceWrapper>(
            new SwappyTraceWrapper(settings, jni));
    }
    return TFERROR_OK;
}

TFErrorCode GetFidelityParameters(const ProtobufSerialization &defaultParams,
                           ProtobufSerialization &params, uint32_t timeout_ms) {
    if (!s_impl) {
        return TFERROR_TUNINGFORK_NOT_INITIALIZED;
    } else {
        return s_impl->GetFidelityParameters(defaultParams,
                                             params, timeout_ms);
    }
}

TFErrorCode FrameTick(InstrumentationKey id) {
    if (!s_impl) {
        return TFERROR_TUNINGFORK_NOT_INITIALIZED;
    } else {
        return s_impl->FrameTick(id);
    }
}

TFErrorCode FrameDeltaTimeNanos(InstrumentationKey id, Duration dt) {
    if (!s_impl) {
        return TFERROR_TUNINGFORK_NOT_INITIALIZED;
    } else {
        return s_impl->FrameDeltaTimeNanos(id, dt);
    }
}

TFErrorCode StartTrace(InstrumentationKey key, TraceHandle& handle) {
    if (!s_impl) {
        return TFERROR_TUNINGFORK_NOT_INITIALIZED;
    } else {
        return s_impl->StartTrace(key, handle);
    }
}

TFErrorCode EndTrace(TraceHandle h) {
    if (!s_impl) {
        return TFERROR_TUNINGFORK_NOT_INITIALIZED;
    } else {
        return s_impl->EndTrace(h);
    }
}

TFErrorCode SetCurrentAnnotation(const ProtobufSerialization &ann) {
    if (!s_impl) {
        return TFERROR_TUNINGFORK_NOT_INITIALIZED;
    } else {
        if (s_impl->SetCurrentAnnotation(ann)==-1) {
            return TFERROR_INVALID_ANNOTATION;
        } else {
            return TFERROR_OK;
        }
    }
}

TFErrorCode SetUploadCallback(UploadCallback cbk) {
    if (!s_impl) {
        return TFERROR_TUNINGFORK_NOT_INITIALIZED;
    } else {
        s_impl->SetUploadCallback(cbk);
        return TFERROR_OK;
    }
}

TFErrorCode Flush() {
    if (!s_impl) {
        return TFERROR_TUNINGFORK_NOT_INITIALIZED;
    } else {
        return s_impl->Flush();
    }
}

TFErrorCode Destroy() {
    if (!s_impl) {
        return TFERROR_TUNINGFORK_NOT_INITIALIZED;
    } else {
        s_backend.KillThreads();
        s_impl.reset();
        return TFERROR_OK;
    }
}

const JniCtx& GetJniCtx() {
    if (!s_impl) {
        static JniCtx empty_jni(nullptr, nullptr);
        return empty_jni;
    } else {
        return s_impl->GetJniCtx();
    }
}

TuningForkImpl::TuningForkImpl(const Settings& settings,
                               const JniCtx& jni,
                               const ExtraUploadInfo& extra_upload_info,
                               Backend *backend,
                               ParamsLoader *loader,
                               ITimeProvider *time_provider) :
                                     settings_(settings),
                                     trace_(gamesdk::Trace::create()),
                                     backend_(backend),
                                     loader_(loader),
                                     upload_thread_(backend, extra_upload_info),
                                     current_annotation_id_(0),
                                     time_provider_(time_provider),
                                     ikeys_(settings.aggregation_strategy.max_instrumentation_keys),
                                     next_ikey_(0),
                                     jni_(jni),
                                     loading_start_(TimePoint::min()) {
    ALOGI("TuningFork Settings:\n  method: %d\n  interval: %d\n  n_ikeys: %d\n  n_annotations: %zu\n"
          "  n_histograms: %zu\n  base_uri: %s\n  api_key: %s\n  fp filename: %s\n  itimeout: %d\n  utimeout: %d",
          settings.aggregation_strategy.method,
          settings.aggregation_strategy.intervalms_or_count,
          settings.aggregation_strategy.max_instrumentation_keys,
          settings.aggregation_strategy.annotation_enum_size.size(),
          settings.histograms.size(),
          settings.base_uri.c_str(),
          settings.api_key.c_str(),
          settings.default_fidelity_parameters_filename.c_str(),
          settings.initial_request_timeout_ms,
          settings.ultimate_request_timeout_ms
         );

    if (time_provider_ == nullptr) {
        time_provider_ = s_chrono_time_provider.get();
    }

    last_submit_time_ = time_provider_->Now();

    InitHistogramSettings();
    InitAnnotationRadixes();

    size_t max_num_prongs_ = 0;
    int max_ikeys = settings.aggregation_strategy.max_instrumentation_keys;

    if (annotation_radix_mult_.size() == 0 || max_ikeys == 0)
        ALOGE("Neither max_annotations nor max_instrumentation_keys can be zero");
    else
        max_num_prongs_ = max_ikeys * annotation_radix_mult_.back();
    auto serializeId = [this](uint64_t id) { return SerializeAnnotationId(id); };
    auto isLoading = [this](uint64_t id) { return IsLoadingAnnotationId(id); };
    prong_caches_[0] = std::make_unique<ProngCache>(max_num_prongs_, max_ikeys,
                                                    settings_.histograms, serializeId,
                                                    isLoading);
    prong_caches_[1] = std::make_unique<ProngCache>(max_num_prongs_, max_ikeys,
                                                    settings_.histograms, serializeId,
                                                    isLoading);
    current_prong_cache_ = prong_caches_[0].get();
    live_traces_.resize(max_num_prongs_);
    for (auto &t: live_traces_) t = TimePoint::min();
    auto crash_callback = [this]()->bool {
                              std::stringstream ss;
                              ss << std::this_thread::get_id();
                              TFErrorCode ret = this->Flush();
                              ALOGI("Crash flush result : %d", ret);
                              return true;
                          };

    crash_handler_.Init(crash_callback);

    // Check if there are any files waiting to be uploaded
    // + merge any histograms that are persisted.
    upload_thread_.InitialChecks(*current_prong_cache_,
                                 *this,
                                 settings_.c_settings.persistent_cache);

    ALOGI("TuningFork initialized");
}

// Return the set annotation id or -1 if it could not be set
uint64_t TuningForkImpl::SetCurrentAnnotation(const ProtobufSerialization &annotation) {
    current_annotation_ = annotation;
    bool loading;
    auto id = DecodeAnnotationSerialization(annotation, &loading);
    if (id == annotation_util::kAnnotationError) {
        ALOGW("Error setting annotation of size %zu", annotation.size());
        current_annotation_id_ = 0;
        return annotation_util::kAnnotationError;
    }
    else {
        // Check if we have started or stopped loading
        if (LoadingNextScene()) {
            if (!loading) {
                // We've stopped loading: record the time
                auto dt = time_provider_->Now() - loading_start_;
                TraceNanos(current_annotation_id_, dt);
                int cnt = (int)std::chrono::duration_cast<std::chrono::milliseconds>(dt).count();
                bool isLoading = IsLoadingAnnotationId(current_annotation_id_);
                ALOGI("Scene loading %" PRIu64 " (%s) took %d ms", current_annotation_id_, isLoading?"true":"false", cnt);
                current_annotation_id_ = id;
                loading_start_ = TimePoint::min();
            }
        }
        else {
            if (loading) {
                // We've just switched to a loading screen
                loading_start_ = time_provider_->Now();
            }
        }
        ALOGV("Set annotation id to %" PRIu64, id);
        current_annotation_id_ = id;
        return current_annotation_id_;
    }
}

AnnotationId TuningForkImpl::DecodeAnnotationSerialization(const SerializedAnnotation &ser,
                                                           bool* loading) const {
    auto id = annotation_util::DecodeAnnotationSerialization(ser, annotation_radix_mult_,
                                                             settings_.loading_annotation_index,
                                                             loading);
     // Shift over to leave room for the instrument id
    return id * settings_.aggregation_strategy.max_instrumentation_keys;
}

SerializedAnnotation TuningForkImpl::SerializeAnnotationId(AnnotationId id) {
    SerializedAnnotation ann;
    AnnotationId a = id / settings_.aggregation_strategy.max_instrumentation_keys;
    annotation_util::SerializeAnnotationId(a, ann, annotation_radix_mult_);
    return ann;
}

bool TuningForkImpl::IsLoadingAnnotationId(AnnotationId id) const {
    if (settings_.loading_annotation_index==-1) return false;
    AnnotationId a = id / settings_.aggregation_strategy.max_instrumentation_keys;
    int value;
    if (annotation_util::Value(a, settings_.loading_annotation_index,
                               annotation_radix_mult_, value) == annotation_util::NO_ERROR) {
        return value > 1;
    }
    else {
        return false;
    }
}

TFErrorCode TuningForkImpl::GetFidelityParameters(
                                           const ProtobufSerialization& defaultParams,
                                           ProtobufSerialization &params_ser, uint32_t timeout_ms) {
    if(loader_) {
        std::string experiment_id;
        if (settings_.base_uri.empty()) {
            ALOGW("The base URI in Tuning Fork TFSettings is invalid");
            return TFERROR_BAD_PARAMETER;
        }
        if (settings_.api_key.empty()) {
          ALOGE("The API key in Tuning Fork TFSettings is invalid");
          return TFERROR_BAD_PARAMETER;
        }
        auto result = loader_->GetFidelityParams(jni_,
            UploadThread::BuildExtraUploadInfo(jni_),
            settings_.base_uri, settings_.api_key, params_ser,
            experiment_id, timeout_ms);
        upload_thread_.SetCurrentFidelityParams(params_ser, experiment_id);
        return result;
    }
    else
        return TFERROR_TUNINGFORK_NOT_INITIALIZED;
}
TFErrorCode TuningForkImpl::GetOrCreateInstrumentKeyIndex(InstrumentationKey key, int& index) {
    int nkeys = next_ikey_;
    for (int i=0; i<nkeys; ++i) {
        if (ikeys_[i] == key) {
            index = i;
            return TFERROR_OK;
        }
    }
    // Another thread could have incremented next_ikey while we were checking, but we
    //  mandate that different threads not use the same key, so we are OK adding
    //  our key, if we can.
    int next = next_ikey_++;
    if (next<ikeys_.size()) {
        ikeys_[next] = key;
        index = next;
        return TFERROR_OK;
    }
    else {
        next_ikey_--;
    }
    return TFERROR_INVALID_INSTRUMENT_KEY;
}
TFErrorCode TuningForkImpl::StartTrace(InstrumentationKey key, TraceHandle& handle) {
    if (LoadingNextScene()) return TFERROR_OK; // No recording when loading
    auto err = MakeCompoundId(key, current_annotation_id_, handle);
    if (err!=TFERROR_OK) return err;
    trace_->beginSection("TFTrace");
    live_traces_[handle] = time_provider_->Now();
    return TFERROR_OK;
}

TFErrorCode TuningForkImpl::EndTrace(TraceHandle h) {
    if (LoadingNextScene()) return TFERROR_OK; // No recording when loading
    if (h>=live_traces_.size()) return TFERROR_INVALID_TRACE_HANDLE;
    auto i = live_traces_[h];
    if (i != TimePoint::min()) {
        trace_->endSection();
        TraceNanos(h, time_provider_->Now() - i);
        live_traces_[h] = TimePoint::min();
        return TFERROR_OK;
    } else {
        return TFERROR_INVALID_TRACE_HANDLE;
    }
}

TFErrorCode TuningForkImpl::FrameTick(InstrumentationKey key) {
    if (LoadingNextScene()) return TFERROR_OK; // No recording when loading
    uint64_t compound_id;
    auto err = MakeCompoundId(key, current_annotation_id_, compound_id);
    if (err!=TFERROR_OK) return err;
    trace_->beginSection("TFTick");
    current_prong_cache_->Ping(time_provider_->SystemNow());
    auto t = time_provider_->Now();
    auto p = TickNanos(compound_id, t);
    if (p)
        CheckForSubmit(t, p);
    trace_->endSection();
    return TFERROR_OK;
}

TFErrorCode TuningForkImpl::FrameDeltaTimeNanos(InstrumentationKey key, Duration dt) {
    if (LoadingNextScene()) return TFERROR_OK; // No recording when loading
    uint64_t compound_id;
    auto err = MakeCompoundId(key, current_annotation_id_, compound_id);
    if (err!=TFERROR_OK) return err;
    auto p = TraceNanos(compound_id, dt);
    if (p)
        CheckForSubmit(time_provider_->Now(), p);
    return TFERROR_OK;
}

Prong *TuningForkImpl::TickNanos(uint64_t compound_id, TimePoint t) {
    // Find the appropriate histogram and add this time
    Prong *p = current_prong_cache_->Get(compound_id);
    if (p)
        p->Tick(t);
    else
        ALOGW("Bad id or limit of number of prongs reached");
    return p;
}

Prong *TuningForkImpl::TraceNanos(uint64_t compound_id, Duration dt) {
    // Find the appropriate histogram and add this time
    Prong *h = current_prong_cache_->Get(compound_id);
    if (h)
        h->Trace(dt);
    else
        ALOGW("Bad id or limit of number of prongs reached");
    return h;
}

void TuningForkImpl::SetUploadCallback(UploadCallback cbk) {
    upload_thread_.SetUploadCallback(cbk);
}


bool TuningForkImpl::ShouldSubmit(TimePoint t, Prong *prong) {
    auto method = settings_.aggregation_strategy.method;
    auto count = settings_.aggregation_strategy.intervalms_or_count;
    switch (settings_.aggregation_strategy.method) {
        case Settings::AggregationStrategy::Submission::TIME_BASED:
            return (t - last_submit_time_) >=
                   std::chrono::milliseconds(count);
        case Settings::AggregationStrategy::Submission::TICK_BASED:
            if (prong)
                return prong->Count() >= count;
    }
    return false;
}

TFErrorCode TuningForkImpl::CheckForSubmit(TimePoint t, Prong *prong) {
    TFErrorCode ret_code = TFERROR_OK;
    if (ShouldSubmit(t, prong)) {
        ret_code = Flush(t, true);
    }
    return ret_code;
}

TFHistogram DefaultHistogram() {
    TFHistogram default_histogram;
    default_histogram.instrument_key = -1;
    default_histogram.bucket_min = 10;
    default_histogram.bucket_max = 40;
    default_histogram.n_buckets = Histogram::kDefaultNumBuckets;
    return default_histogram;
}

void TuningForkImpl::InitHistogramSettings() {
    TFHistogram default_histogram = DefaultHistogram();
    for(uint32_t i=0; i<settings_.aggregation_strategy.max_instrumentation_keys; ++i) {
        if(settings_.histograms.size()<=i) {
            ALOGW("Couldn't get histogram for key index %d. Using default histogram", i);
            settings_.histograms.push_back(default_histogram);
        }
        else {
            int index;
            GetOrCreateInstrumentKeyIndex(settings_.histograms[i].instrument_key, index);
        }
    }
    ALOGV("TFHistograms");
    for(uint32_t i=0; i< settings_.histograms.size(); ++i) {
        auto& h = settings_.histograms[i];
        ALOGV("ikey: %d min: %f max: %f nbkts: %d", h.instrument_key, h.bucket_min, h.bucket_max,
              h.n_buckets);
    }
}

void TuningForkImpl::InitAnnotationRadixes() {
    annotation_util::SetUpAnnotationRadixes(annotation_radix_mult_,
                                            settings_.aggregation_strategy.annotation_enum_size);
}

TFErrorCode TuningForkImpl::Flush() {
    auto t = std::chrono::steady_clock::now();
    return Flush(t, false);
}

TFErrorCode TuningForkImpl::Flush(TimePoint t, bool upload) {
    ALOGV("Flush %d", upload);
    TFErrorCode ret_code;
    current_prong_cache_->SetInstrumentKeys(ikeys_);
    if (upload_thread_.Submit(current_prong_cache_, upload)) {
        if (current_prong_cache_ == prong_caches_[0].get()) {
            prong_caches_[1]->Clear();
            current_prong_cache_ = prong_caches_[1].get();
        } else {
            prong_caches_[0]->Clear();
            current_prong_cache_ = prong_caches_[0].get();
        }
        ret_code = TFERROR_OK;
    } else {
        ret_code = TFERROR_PREVIOUS_UPLOAD_PENDING;
    }
    if (upload)
        last_submit_time_ = t;
    return ret_code;
}

} // namespace tuningfork {
