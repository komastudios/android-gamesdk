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

#include "tuningfork_impl.h"

#include <chrono>
#include <cinttypes>
#include <map>
#include <sstream>
#include <vector>

#define LOG_TAG "TuningFork"
#include "Log.h"
#include "annotation_util.h"
#include "histogram.h"
#include "http_backend/http_backend.h"
#include "memory_telemetry.h"
#include "metric.h"
#include "tuningfork_utils.h"

namespace tuningfork {

TuningForkImpl::TuningForkImpl(const Settings &settings, IBackend *backend,
                               ITimeProvider *time_provider,
                               IMemInfoProvider *meminfo_provider)
    : settings_(settings),
      trace_(gamesdk::Trace::create()),
      backend_(backend),
      upload_thread_(backend),
      current_annotation_id_(MetricId::FrameTime(0, 0)),
      time_provider_(time_provider),
      meminfo_provider_(meminfo_provider),
      ikeys_(settings.aggregation_strategy.max_instrumentation_keys),
      next_ikey_(0),
      loading_start_(TimePoint::min()) {
    ALOGI(
        "TuningFork Settings:\n  method: %d\n  interval: %d\n  n_ikeys: %d\n  "
        "n_annotations: %zu"
        "\n  n_histograms: %zu\n  base_uri: %s\n  api_key: %s\n  fp filename: "
        "%s\n  itimeout: %d"
        "\n  utimeout: %d",
        settings.aggregation_strategy.method,
        settings.aggregation_strategy.intervalms_or_count,
        settings.aggregation_strategy.max_instrumentation_keys,
        settings.aggregation_strategy.annotation_enum_size.size(),
        settings.histograms.size(), settings.base_uri.c_str(),
        settings.api_key.c_str(),
        settings.default_fidelity_parameters_filename.c_str(),
        settings.initial_request_timeout_ms,
        settings.ultimate_request_timeout_ms);

    last_submit_time_ = time_provider_->Now();

    InitHistogramSettings();
    InitAnnotationRadixes();
    InitTrainingModeParams();

    size_t max_num_frametime_metrics = 0;
    int max_ikeys = settings.aggregation_strategy.max_instrumentation_keys;

    if (annotation_radix_mult_.size() == 0 || max_ikeys == 0)
        ALOGE(
            "Neither max_annotations nor max_instrumentation_keys can be zero");
    else
        max_num_frametime_metrics = max_ikeys * annotation_radix_mult_.back();
    int max_num_other_metrics =
        MemoryRecordType::END;  // Number of memory metrics
    for (int i = 0; i < 2; ++i) {
        sessions_[i] = std::make_unique<Session>(max_num_frametime_metrics +
                                                 max_num_other_metrics);
        CreateSessionFrameHistograms(*sessions_[i], max_num_frametime_metrics,
                                     max_ikeys, settings_.histograms);
        MemoryTelemetry::CreateMemoryHistograms(*sessions_[i],
                                                meminfo_provider_);
    }
    current_session_ = sessions_[0].get();
    live_traces_.resize(max_num_frametime_metrics);
    for (auto &t : live_traces_) t = TimePoint::min();
    auto crash_callback = [this]() -> bool {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        TuningFork_ErrorCode ret = this->Flush();
        ALOGI("Crash flush result : %d", ret);
        return true;
    };

    crash_handler_.Init(crash_callback);

    // Check if there are any files waiting to be uploaded
    // + merge any histograms that are persisted.
    upload_thread_.InitialChecks(*current_session_, *this,
                                 settings_.c_settings.persistent_cache);

    InitAsyncTelemetry();

    ALOGI("TuningFork initialized");
}

TuningForkImpl::~TuningForkImpl() {
    // Stop the threads before we delete Tuning Fork internals
    upload_thread_.Stop();
    async_telemetry_->Stop();
}

void TuningForkImpl::CreateSessionFrameHistograms(
    Session &session, size_t size, int max_num_instrumentation_keys,
    const std::vector<Settings::Histogram> &histogram_settings) {
    InstrumentationKey ikey = 0;
    for (uint64_t i = 0; i < size; ++i) {
        auto aid = i / max_num_instrumentation_keys;
        SerializedAnnotation ann;
        annotation_util::SerializeAnnotationId(aid, ann,
                                               annotation_radix_mult_);
        if (IsLoadingAnnotationId(aid)) {
            if (ikey == 0) {
                session.CreateLoadingTimeSeries(LoadingTimeMetric(ann),
                                                MetricId::LoadingTime(aid));
            }
            // Don't create histograms for other instrument keys when loading
        } else {
            auto &h =
                histogram_settings[ikey < histogram_settings.size() ? ikey : 0];
            session.CreateFrameTimeHistogram(FrameTimeMetric(ikey, ann),
                                             MetricId::FrameTime(aid, ikey), h);
        }
        ++ikey;
        if (ikey >= max_num_instrumentation_keys) ikey = 0;
    }
}

// Return the set annotation id or -1 if it could not be set
MetricId TuningForkImpl::SetCurrentAnnotation(
    const ProtobufSerialization &annotation) {
    current_annotation_ = annotation;
    bool loading;
    auto id = DecodeAnnotationSerialization(annotation, &loading);
    if (id == annotation_util::kAnnotationError) {
        ALOGW("Error setting annotation of size %zu", annotation.size());
        current_annotation_id_ = MetricId::FrameTime(0, 0);
        return MetricId{annotation_util::kAnnotationError};
    } else {
        // Check if we have started or stopped loading
        if (LoadingNextScene()) {
            if (!loading) {
                // We've stopped loading: record the time
                auto dt = time_provider_->Now() - loading_start_;
                auto h = current_session_->GetData<LoadingTimeMetricData>(
                    MetricId::LoadingTime(
                        current_annotation_id_.detail.annotation));
                if (h) h->Record(dt);
                int cnt =
                    (int)std::chrono::duration_cast<std::chrono::milliseconds>(
                        dt)
                        .count();
                loading_start_ = TimePoint::min();
            }
        } else {
            if (loading) {
                // We've just switched to a loading screen
                loading_start_ = time_provider_->Now();
            }
        }
        ALOGV("Set annotation id to %" PRIu64, id);
        current_annotation_id_ = MetricId::FrameTime(id, 0);
        return current_annotation_id_;
    }
}

AnnotationId TuningForkImpl::DecodeAnnotationSerialization(
    const SerializedAnnotation &ser, bool *loading) const {
    auto aid = annotation_util::DecodeAnnotationSerialization(
        ser, annotation_radix_mult_, settings_.loading_annotation_index,
        settings_.level_annotation_index, loading);
    return aid;
}
TuningFork_ErrorCode TuningForkImpl::MakeCompoundId(InstrumentationKey key,
                                                    AnnotationId annotation_id,
                                                    MetricId &id) {
    int key_index;
    auto err = GetOrCreateInstrumentKeyIndex(key, key_index);
    if (err != TUNINGFORK_ERROR_OK) return err;
    id = MetricId::FrameTime(annotation_id, key_index);
    return TUNINGFORK_ERROR_OK;
}
bool TuningForkImpl::IsLoadingAnnotationId(AnnotationId id) const {
    if (settings_.loading_annotation_index == -1) return false;
    int value;
    if (annotation_util::Value(id, settings_.loading_annotation_index,
                               annotation_radix_mult_,
                               value) == annotation_util::NO_ERROR) {
        return value > 1;
    } else {
        return false;
    }
}

TuningFork_ErrorCode TuningForkImpl::GetFidelityParameters(
    const ProtobufSerialization &default_params,
    ProtobufSerialization &params_ser, uint32_t timeout_ms) {
    std::string experiment_id;
    if (settings_.EndpointUri().empty()) {
        ALOGW("The base URI in Tuning Fork TuningFork_Settings is invalid");
        return TUNINGFORK_ERROR_BAD_PARAMETER;
    }
    if (settings_.api_key.empty()) {
        ALOGE("The API key in Tuning Fork TuningFork_Settings is invalid");
        return TUNINGFORK_ERROR_BAD_PARAMETER;
    }
    Duration timeout =
        (timeout_ms <= 0)
            ? std::chrono::milliseconds(settings_.initial_request_timeout_ms)
            : std::chrono::milliseconds(timeout_ms);
    HttpRequest web_request(settings_.EndpointUri(), settings_.api_key,
                            timeout);
    auto result = backend_->GenerateTuningParameters(
        web_request, training_mode_params_.get(), params_ser, experiment_id);
    if (result == TUNINGFORK_ERROR_OK) {
        RequestInfo::CachedValue().current_fidelity_parameters = params_ser;
    } else if (training_mode_params_.get()) {
        RequestInfo::CachedValue().current_fidelity_parameters =
            *training_mode_params_;
    }
    RequestInfo::CachedValue().experiment_id = experiment_id;
    if (Debugging() && jni::IsValid()) {
        backend_->UploadDebugInfo(web_request);
    }
    return result;
}
TuningFork_ErrorCode TuningForkImpl::GetOrCreateInstrumentKeyIndex(
    InstrumentationKey key, int &index) {
    int nkeys = next_ikey_;
    for (int i = 0; i < nkeys; ++i) {
        if (ikeys_[i] == key) {
            index = i;
            return TUNINGFORK_ERROR_OK;
        }
    }
    // Another thread could have incremented next_ikey while we were checking,
    // but we mandate that different threads not use the same key, so we are OK
    // adding our key, if we can.
    int next = next_ikey_++;
    if (next < ikeys_.size()) {
        ikeys_[next] = key;
        index = next;
        return TUNINGFORK_ERROR_OK;
    } else {
        next_ikey_--;
    }
    return TUNINGFORK_ERROR_INVALID_INSTRUMENT_KEY;
}
TuningFork_ErrorCode TuningForkImpl::StartTrace(InstrumentationKey key,
                                                TraceHandle &handle) {
    if (LoadingNextScene())
        return TUNINGFORK_ERROR_OK;  // No recording when loading

    MetricId id{0};
    auto err =
        MakeCompoundId(key, current_annotation_id_.detail.annotation, id);
    if (err != TUNINGFORK_ERROR_OK) return err;
    handle = id.base;
    trace_->beginSection("TFTrace");
    live_traces_[handle] = time_provider_->Now();
    return TUNINGFORK_ERROR_OK;
}

TuningFork_ErrorCode TuningForkImpl::EndTrace(TraceHandle h) {
    if (LoadingNextScene())
        return TUNINGFORK_ERROR_OK;  // No recording when loading
    if (h >= live_traces_.size()) return TUNINGFORK_ERROR_INVALID_TRACE_HANDLE;
    auto i = live_traces_[h];
    if (i != TimePoint::min()) {
        trace_->endSection();
        TraceNanos(MetricId{h}, time_provider_->Now() - i);
        live_traces_[h] = TimePoint::min();
        return TUNINGFORK_ERROR_OK;
    } else {
        return TUNINGFORK_ERROR_INVALID_TRACE_HANDLE;
    }
}

TuningFork_ErrorCode TuningForkImpl::FrameTick(InstrumentationKey key) {
    if (LoadingNextScene())
        return TUNINGFORK_ERROR_OK;  // No recording when loading
    MetricId id{0};
    auto err =
        MakeCompoundId(key, current_annotation_id_.detail.annotation, id);
    if (err != TUNINGFORK_ERROR_OK) return err;
    trace_->beginSection("TFTick");
    current_session_->Ping(time_provider_->SystemNow());
    auto t = time_provider_->Now();
    auto p = TickNanos(id, t);
    if (p) CheckForSubmit(t, p);
    trace_->endSection();
    return TUNINGFORK_ERROR_OK;
}

TuningFork_ErrorCode TuningForkImpl::FrameDeltaTimeNanos(InstrumentationKey key,
                                                         Duration dt) {
    if (LoadingNextScene())
        return TUNINGFORK_ERROR_OK;  // No recording when loading
    MetricId id{0};
    auto err =
        MakeCompoundId(key, current_annotation_id_.detail.annotation, id);
    if (err != TUNINGFORK_ERROR_OK) return err;
    auto p = TraceNanos(id, dt);
    if (p) CheckForSubmit(time_provider_->Now(), p);
    return TUNINGFORK_ERROR_OK;
}

MetricData *TuningForkImpl::TickNanos(MetricId compound_id, TimePoint t) {
    // Find the appropriate histogram and add this time
    auto p = current_session_->GetData<FrameTimeMetricData>(compound_id);
    if (p)
        p->Tick(t);
    else
        ALOGW("Bad id or limit of number of metrics reached");
    return p;
}

MetricData *TuningForkImpl::TraceNanos(MetricId compound_id, Duration dt) {
    // Find the appropriate histogram and add this time
    auto h = current_session_->GetData<FrameTimeMetricData>(compound_id);
    if (h)
        h->Record(dt);
    else
        ALOGW("Bad id or limit of number of metrics reached");
    return h;
}

void TuningForkImpl::SetUploadCallback(TuningFork_UploadCallback cbk) {
    upload_thread_.SetUploadCallback(cbk);
}

bool TuningForkImpl::ShouldSubmit(TimePoint t, MetricData *histogram) {
    auto method = settings_.aggregation_strategy.method;
    auto count = settings_.aggregation_strategy.intervalms_or_count;
    switch (settings_.aggregation_strategy.method) {
        case Settings::AggregationStrategy::Submission::TIME_BASED:
            return (t - last_submit_time_) >= std::chrono::milliseconds(count);
        case Settings::AggregationStrategy::Submission::TICK_BASED:
            if (histogram) return histogram->Count() >= count;
    }
    return false;
}

TuningFork_ErrorCode TuningForkImpl::CheckForSubmit(TimePoint t,
                                                    MetricData *histogram) {
    TuningFork_ErrorCode ret_code = TUNINGFORK_ERROR_OK;
    if (ShouldSubmit(t, histogram)) {
        ret_code = Flush(t, true);
    }
    return ret_code;
}

void TuningForkImpl::InitHistogramSettings() {
    auto max_keys = settings_.aggregation_strategy.max_instrumentation_keys;
    if (max_keys != settings_.histograms.size()) {
        InstrumentationKey default_keys[] = {TFTICK_RAW_FRAME_TIME,
                                             TFTICK_PACED_FRAME_TIME,
                                             TFTICK_CPU_TIME, TFTICK_GPU_TIME};
        // Add histograms that are missing
        auto key_present = [this](InstrumentationKey k) {
            for (auto &h : settings_.histograms) {
                if (k == h.instrument_key) return true;
            }
            return false;
        };
        std::vector<InstrumentationKey> to_add;
        for (auto &k : default_keys) {
            if (!key_present(k)) {
                if (settings_.histograms.size() < max_keys) {
                    ALOGI(
                        "Couldn't get histogram for key index %d. Using "
                        "default histogram",
                        k);
                    settings_.histograms.push_back(
                        Settings::DefaultHistogram(k));
                } else {
                    ALOGE(
                        "Can't fit default histograms: change "
                        "max_instrumentation_keys");
                }
            }
        }
    }
    for (uint32_t i = 0; i < max_keys; ++i) {
        if (i > settings_.histograms.size()) {
            ALOGW(
                "Couldn't get histogram for key index %d. Using default "
                "histogram",
                i);
            settings_.histograms.push_back(Settings::DefaultHistogram(-1));
        } else {
            int index;
            GetOrCreateInstrumentKeyIndex(
                settings_.histograms[i].instrument_key, index);
        }
    }
    // If there was an instrument key but no other settings, update the
    // histogram
    auto check_histogram = [](Settings::Histogram &h) {
        if (h.bucket_max == 0 || h.n_buckets == 0) {
            h = Settings::DefaultHistogram(h.instrument_key);
        }
    };
    for (auto &h : settings_.histograms) {
        check_histogram(h);
    }
    ALOGI("Settings::Histograms");
    for (uint32_t i = 0; i < settings_.histograms.size(); ++i) {
        auto &h = settings_.histograms[i];
        ALOGI("ikey: %d min: %f max: %f nbkts: %d", h.instrument_key,
              h.bucket_min, h.bucket_max, h.n_buckets);
    }
}

void TuningForkImpl::InitAnnotationRadixes() {
    annotation_util::SetUpAnnotationRadixes(
        annotation_radix_mult_,
        settings_.aggregation_strategy.annotation_enum_size);
}

TuningFork_ErrorCode TuningForkImpl::Flush() {
    auto t = std::chrono::steady_clock::now();
    return Flush(t, false);
}

void TuningForkImpl::SwapSessions() {
    if (current_session_ == sessions_[0].get()) {
        sessions_[1]->ClearData();
        current_session_ = sessions_[1].get();
    } else {
        sessions_[0]->ClearData();
        current_session_ = sessions_[0].get();
    }
    async_telemetry_->SetSession(current_session_);
}
TuningFork_ErrorCode TuningForkImpl::Flush(TimePoint t, bool upload) {
    ALOGV("Flush %d", upload);
    TuningFork_ErrorCode ret_code;
    current_session_->SetInstrumentationKeys(ikeys_);
    if (upload_thread_.Submit(current_session_, upload)) {
        SwapSessions();
        ret_code = TUNINGFORK_ERROR_OK;
    } else {
        ret_code = TUNINGFORK_ERROR_PREVIOUS_UPLOAD_PENDING;
    }
    if (upload) last_submit_time_ = t;
    return ret_code;
}

void TuningForkImpl::InitTrainingModeParams() {
    auto cser = settings_.c_settings.training_fidelity_params;
    if (cser != nullptr)
        training_mode_params_ = std::make_unique<ProtobufSerialization>(
            ToProtobufSerialization(*cser));
}

TuningFork_ErrorCode TuningForkImpl::SetFidelityParameters(
    const ProtobufSerialization &params) {
    auto flush_result = Flush();
    if (flush_result != TUNINGFORK_ERROR_OK) {
        ALOGW("Warning, previous data could not be flushed.");
        SwapSessions();
    }
    RequestInfo::CachedValue().current_fidelity_parameters = params;
    // We clear the experiment id here.
    RequestInfo::CachedValue().experiment_id = "";
    return TUNINGFORK_ERROR_OK;
}

bool TuningForkImpl::Debugging() const {
#ifndef NDEBUG
    // Always return true if we are a debug build
    return true;
#else
    // Otherwise, check the APK and system settings
    if (jni::IsValid())
        return apk_utils::GetDebuggable();
    else
        return false;
#endif
}

TuningFork_ErrorCode TuningForkImpl::EnableMemoryRecording(bool enable) {
    if (meminfo_provider_ != nullptr) {
        meminfo_provider_->SetEnabled(enable);
    }
    return TUNINGFORK_ERROR_OK;
}

void TuningForkImpl::InitAsyncTelemetry() {
    async_telemetry_ = std::make_unique<AsyncTelemetry>(time_provider_);
    MemoryTelemetry::SetUpAsyncWork(*async_telemetry_, meminfo_provider_);
    async_telemetry_->SetSession(current_session_);
    async_telemetry_->Start();
}

}  // namespace tuningfork
