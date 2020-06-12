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

#include <atomic>
#include <memory>

#include "Trace.h"
#include "async_telemetry.h"
#include "crash_handler.h"
#include "session.h"
#include "tuningfork_internal.h"
#include "tuningfork_swappy.h"
#include "uploadthread.h"

namespace tuningfork {

class TuningForkImpl : public IdProvider {
   private:
    CrashHandler crash_handler_;
    Settings settings_;
    std::unique_ptr<Session> sessions_[2];
    Session *current_session_;
    TimePoint last_submit_time_;
    std::unique_ptr<gamesdk::Trace> trace_;
    std::vector<TimePoint> live_traces_;
    IBackend *backend_;
    UploadThread upload_thread_;
    SerializedAnnotation current_annotation_;
    std::vector<uint32_t> annotation_radix_mult_;
    AnnotationId current_annotation_id_;
    ITimeProvider *time_provider_;
    IMemInfoProvider *meminfo_provider_;
    std::vector<InstrumentationKey> ikeys_;
    std::atomic<int> next_ikey_;
    TimePoint loading_start_;
    std::unique_ptr<ProtobufSerialization> training_mode_params_;
    std::unique_ptr<AsyncTelemetry> async_telemetry_;

   public:
    TuningForkImpl(const Settings &settings, IBackend *backend,
                   ITimeProvider *time_provider,
                   IMemInfoProvider *memory_provider);

    ~TuningForkImpl();

    void InitHistogramSettings();

    void InitAnnotationRadixes();

    void InitTrainingModeParams();

    // Returns true if the fidelity params were retrieved
    TuningFork_ErrorCode GetFidelityParameters(
        const ProtobufSerialization &defaultParams,
        ProtobufSerialization &fidelityParams, uint32_t timeout_ms);

    // Returns the set annotation id or -1 if it could not be set
    uint64_t SetCurrentAnnotation(const ProtobufSerialization &annotation);

    TuningFork_ErrorCode FrameTick(InstrumentationKey id);

    TuningFork_ErrorCode FrameDeltaTimeNanos(InstrumentationKey id,
                                             Duration dt);

    // Fills handle with that to be used by EndTrace
    TuningFork_ErrorCode StartTrace(InstrumentationKey key,
                                    TraceHandle &handle);

    TuningFork_ErrorCode EndTrace(TraceHandle);

    void SetUploadCallback(TuningFork_UploadCallback cbk);

    TuningFork_ErrorCode Flush();

    TuningFork_ErrorCode Flush(TimePoint t, bool upload);

    const Settings &GetSettings() const { return settings_; }

    TuningFork_ErrorCode SetFidelityParameters(
        const ProtobufSerialization &params);

    TuningFork_ErrorCode EnableMemoryRecording(bool enable);

   private:
    MetricData *TickNanos(uint64_t compound_id, TimePoint t);

    MetricData *TraceNanos(uint64_t compound_id, Duration dt);

    TuningFork_ErrorCode CheckForSubmit(TimePoint t, MetricData *metric_data);

    bool ShouldSubmit(TimePoint t, MetricData *metric_data);

    AnnotationId DecodeAnnotationSerialization(const SerializedAnnotation &ser,
                                               bool *loading) const override;

    uint32_t GetInstrumentationKey(uint64_t compoundId) {
        return compoundId %
               settings_.aggregation_strategy.max_instrumentation_keys;
    }

    TuningFork_ErrorCode MakeCompoundId(InstrumentationKey k, AnnotationId a,
                                        uint64_t &id) override {
        int key_index;
        auto err = GetOrCreateInstrumentKeyIndex(k, key_index);
        if (err != TUNINGFORK_ERROR_OK) return err;
        id = key_index + a;
        return TUNINGFORK_ERROR_OK;
    }

    SerializedAnnotation SerializeAnnotationId(uint64_t);

    bool keyIsValid(InstrumentationKey key) const;

    TuningFork_ErrorCode GetOrCreateInstrumentKeyIndex(InstrumentationKey key,
                                                       int &index);

    bool LoadingNextScene() const { return loading_start_ != TimePoint::min(); }

    bool IsLoadingAnnotationId(uint64_t id) const;

    void SwapSessions();

    bool Debugging() const;

    void InitAsyncTelemetry();

    void CreateSessionFrameHistograms(
        Session &session, size_t size, int max_num_instrumentation_keys,
        const std::vector<Settings::Histogram> &histogram_settings);

    void CreateMemoryHistograms(Session &session);

    void AddMemoryMetrics();
};

}  // namespace tuningfork
