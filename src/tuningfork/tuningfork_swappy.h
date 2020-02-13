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
#include "swappy/swappyGL_extra.h"
#include "tuningfork_internal.h"

namespace tuningfork {

// This encapsulates the callbacks that are passed to Swappy at initialization, if it is
//  enabled + available.
class SwappyTraceWrapper {
    SwappyTracerFn swappyTracerFn_;
    SwappyTracer trace_;
    TFTraceHandle waitTraceHandle_ = 0;
    TFTraceHandle logicTraceHandle_ = 0;
public:
    SwappyTraceWrapper(const Settings& settings);
    // Swappy trace callbacks
    static void StartFrameCallback(void* userPtr, int /*currentFrame*/,
                                         long /*currentFrameTimeStampMs*/);
    static void PreWaitCallback(void* userPtr);
    static void PostWaitCallback(void* userPtr);
    static void PreSwapBuffersCallback(void* userPtr);
    static void PostSwapBuffersCallback(void* userPtr, long /*desiredPresentationTimeMs*/);
};

} // namespace tuningfork
