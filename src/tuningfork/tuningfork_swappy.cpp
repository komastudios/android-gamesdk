/*
 * Copyright 2019 The Android Open Source Project
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

#include "tuningfork_swappy.h"

#define LOG_TAG "TuningFork"
#include "Log.h"

namespace tuningfork {

SwappyTraceWrapper::SwappyTraceWrapper(const Settings& settings)
        : swappyTracerFn_(settings.c_settings.swappy_tracer_fn),
          trace_({}) {
    uint32_t swappyVersion = settings.c_settings.swappy_version;
    if (swappyVersion < SWAPPY_VERSION_1_3) {
        auto pre13_trace = reinterpret_cast<SwappyTracerPre1_3*>(&trace_);
        pre13_trace->startFrame = StartFrameCallbackPre1_3;
        pre13_trace->preWait =  PreWaitCallbackPre1_3;
        pre13_trace->postWait = PostWaitCallbackPre1_3;
    } else {
        trace_.startFrame = StartFrameCallback;
        trace_.preWait =  PreWaitCallback;
        trace_.postWait = PostWaitCallback;
    }
    trace_.preSwapBuffers = PreSwapBuffersCallback;
    trace_.postSwapBuffers = PostSwapBuffersCallback;
    trace_.userData = this;
    swappyTracerFn_(&trace_);
}

// Swappy trace callbacks
void SwappyTraceWrapper::StartFrameCallback(void* userPtr, int /*currentFrame*/,
                                     long /*currentFrameTimeStampMs*/) {
    SwappyTraceWrapper* _this = (SwappyTraceWrapper*)userPtr;
    auto err = TuningFork_frameTick(TFTICK_PACED_FRAME_TIME);
    if (err!=TFERROR_OK) {
        ALOGE("Error ticking %d : %d", TFTICK_PACED_FRAME_TIME, err);
    }
}

void SwappyTraceWrapper::PreWaitCallback(void* userPtr) {
}

void SwappyTraceWrapper::PostWaitCallback(void* userPtr, long cpuTimeNs, long gpuTimeNs) {
    SwappyTraceWrapper *_this = (SwappyTraceWrapper *) userPtr;
    auto err = TuningFork_frameDeltaTimeNanos(TFTICK_CPU_TIME, cpuTimeNs);
    if (err != TFERROR_OK) {
        ALOGE("Error ticking %d : %d", TFTICK_CPU_TIME, err);
    }
    err = TuningFork_frameDeltaTimeNanos(TFTICK_GPU_TIME, gpuTimeNs);
    if (err != TFERROR_OK) {
        ALOGE("Error ticking %d : %d", TFTICK_GPU_TIME, err);
    }
    err = TuningFork_frameDeltaTimeNanos(TFTICK_RAW_FRAME_TIME, std::max(cpuTimeNs, gpuTimeNs));
    if (err != TFERROR_OK) {
        ALOGE("Error ticking %d : %d", TFTICK_RAW_FRAME_TIME, err);
    }
}

void SwappyTraceWrapper::PreSwapBuffersCallback(void* userPtr) {
    // We no longer record the swap time
}

void SwappyTraceWrapper::PostSwapBuffersCallback(void* userPtr, long /*desiredPresentationTimeMs*/) {
    // We no longer record the swap time
}

// Callbacks for swappy version < 1.3 where we don't have direct GPU time
void SwappyTraceWrapper::StartFrameCallbackPre1_3(void* userPtr, int /*currentFrame*/,
                                     long /*currentFrameTimeStampMs*/) {
    SwappyTraceWrapper* _this = (SwappyTraceWrapper*)userPtr;
    // There's no distinction between RAW and PACED frame time for swappy < 1.3
    // since we can't get real raw frame time.
    auto err = TuningFork_frameTick(TFTICK_RAW_FRAME_TIME);
    if (err!=TFERROR_OK) {
        ALOGE("Error ticking %d : %d", TFTICK_RAW_FRAME_TIME, err);
    }
    err = TuningFork_frameTick(TFTICK_PACED_FRAME_TIME);
    if (err!=TFERROR_OK) {
        ALOGE("Error ticking %d : %d", TFTICK_PACED_FRAME_TIME, err);
    }
    err = TuningFork_startTrace(TFTICK_CPU_TIME, &_this->logicTraceHandle_);
    if (err!=TFERROR_OK) {
        ALOGE("Error tracing %d : %d", TFTICK_CPU_TIME, err);
    }
}

void SwappyTraceWrapper::PreWaitCallbackPre1_3(void* userPtr) {
    SwappyTraceWrapper* _this = (SwappyTraceWrapper*)userPtr;
    if (_this->logicTraceHandle_) {
        TuningFork_endTrace(_this->logicTraceHandle_);
        _this->logicTraceHandle_ = 0;
    }
}

void SwappyTraceWrapper::PostWaitCallbackPre1_3(void* userPtr) {
    SwappyTraceWrapper *_this = (SwappyTraceWrapper *) userPtr;
    // This is not real GPU time
    auto err=TuningFork_frameTick(TFTICK_GPU_TIME);
    if (err!=TFERROR_OK) {
        ALOGE("Error ticking %d : %d", TFTICK_GPU_TIME, err);
    }
}

} // namespace tuningfork
