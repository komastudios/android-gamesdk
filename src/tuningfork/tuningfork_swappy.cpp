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

#include "tuningfork_internal.h"

#define LOG_TAG "TuningFork"
#include "Log.h"

namespace tuningfork {

SwappyTraceWrapper::SwappyTraceWrapper(const Settings& settings, const JniCtx& jni)
        : swappyTracerFn_(settings.c_settings.swappy_tracer_fn),
          trace_({}) {
    trace_.startFrame = StartFrameCallback;
    trace_.preWait =  PreWaitCallback;
    trace_.postWait = PostWaitCallback;
    trace_.preSwapBuffers = PreSwapBuffersCallback;
    trace_.postSwapBuffers = PostSwapBuffersCallback;
    trace_.userData = this;
    swappyTracerFn_(&trace_);
}

// Swappy trace callbacks
void SwappyTraceWrapper::StartFrameCallback(void* userPtr, int /*currentFrame*/,
                                     long /*currentFrameTimeStampMs*/) {
        SwappyTraceWrapper* _this = (SwappyTraceWrapper*)userPtr;
        auto err = TuningFork_frameTick(TFTICK_SYSCPU);
        if (err!=TFERROR_OK) {
            ALOGE("Error ticking %d : %d", TFTICK_SYSCPU, err);
        }
    }
void SwappyTraceWrapper::PreWaitCallback(void* userPtr) {
    SwappyTraceWrapper* _this = (SwappyTraceWrapper*)userPtr;
    auto err = TuningFork_startTrace(TFTICK_SWAPPY_WAIT_TIME, &_this->waitTraceHandle_);
    if (err!=TFERROR_OK) {
        ALOGE("Error tracing %d : %d", TFTICK_SWAPPY_WAIT_TIME, err);
    }
}
void SwappyTraceWrapper::PostWaitCallback(void* userPtr) {
    SwappyTraceWrapper *_this = (SwappyTraceWrapper *) userPtr;
    if (_this->waitTraceHandle_) {
        TuningFork_endTrace(_this->waitTraceHandle_);
        _this->waitTraceHandle_ = 0;
    }
    auto err=TuningFork_frameTick(TFTICK_SYSGPU);
    if (err!=TFERROR_OK) {
        ALOGE("Error ticking %d : %d", TFTICK_SYSGPU, err);
    }
}
void SwappyTraceWrapper::PreSwapBuffersCallback(void* userPtr) {
    SwappyTraceWrapper* _this = (SwappyTraceWrapper*)userPtr;
    auto err = TuningFork_startTrace(TFTICK_SWAPPY_SWAP_TIME, &_this->swapTraceHandle_);
    if (err!=TFERROR_OK) {
        ALOGE("Error tracing %d : %d", TFTICK_SWAPPY_SWAP_TIME, err);
    }
}
void SwappyTraceWrapper::PostSwapBuffersCallback(void* userPtr, long /*desiredPresentationTimeMs*/) {
    SwappyTraceWrapper *_this = (SwappyTraceWrapper *) userPtr;
    if (_this->swapTraceHandle_) {
        TuningFork_endTrace(_this->swapTraceHandle_);
        _this->swapTraceHandle_ = 0;
    }
}

} // namespace tuningfork
