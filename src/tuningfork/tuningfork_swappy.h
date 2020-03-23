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

#pragma once

#include "tuningfork/tuningfork.h"
#include "swappy/swappy_common.h"
#include "tuningfork_internal.h"

// For versions of Swappy < 1.3, the post-wait callback didn't give gpuTime
typedef SwappyPreWaitCallback SwappyWaitCallback;
struct SwappyTracerPre1_3 {
    SwappyWaitCallback preWait;
    SwappyWaitCallback postWait;
    SwappyPreSwapBuffersCallback preSwapBuffers;
    SwappyPostSwapBuffersCallback postSwapBuffers;
    SwappyStartFrameCallback startFrame;
    void* userData;
    SwappySwapIntervalChangedCallback swapIntervalChanged;
};
constexpr int SWAPPY_VERSION_1_3 = ((1<<16) | 3);

namespace tuningfork {

// This encapsulates the callbacks that are passed to Swappy at initialization, if it is
//  enabled + available.
class SwappyTraceWrapper {
    SwappyTracerFn swappyTracerFn_;
    SwappyTracer trace_;
    TFTraceHandle logicTraceHandle_ = 0;
public:
    SwappyTraceWrapper(const Settings& settings);
    // Swappy trace callbacks
    static void StartFrameCallback(void* userPtr, int /*currentFrame*/,
                                         long /*currentFrameTimeStampMs*/);
    static void PreWaitCallback(void* userPtr);
    static void PostWaitCallback(void* userPtr, long cpu_time_ns, long gpu_time_ns);
    static void PreSwapBuffersCallback(void* userPtr);
    static void PostSwapBuffersCallback(void* userPtr, long /*desiredPresentationTimeMs*/);

    static void StartFrameCallbackPre1_3(void* userPtr, int /*currentFrame*/,
                                         long /*currentFrameTimeStampMs*/);
    static void PreWaitCallbackPre1_3(void* userPtr);
    static void PostWaitCallbackPre1_3(void* userPtr);
};

} // namespace tuningfork
