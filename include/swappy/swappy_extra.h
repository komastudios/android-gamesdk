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

#include <stdint.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <jni.h>

#define MAX_FRAME_BUCKETS 6


#ifdef __cplusplus
extern "C" {
#endif

// If app is using choreographer and wants to provide choreographer ticks to swappy,
// call the function below. This function must be called before the first Swappy_swap() call
// for the first time. Afterwards, call this every choreographer tick
void Swappy_onChoreographer(int64_t frameTimeNanos);

// Pass callbacks to be called each frame to trace execution
struct SwappyTracer {
    void (*preWait)(void*);
    void (*postWait)(void*);
    void (*preSwapBuffers)(void*);
    void (*postSwapBuffers)(void*, long desiredPresentationTimeMillis);
    void (*startFrame)(void*, int currentFrame, long currentFrameTimeStampMillis);
    void* userData;
    void (*swapIntervalChanged)(void*);

};
void Swappy_injectTracer(const SwappyTracer *t);

// toggle auto swap interval detection on/off
// by default, swappy will adjust the swap interval based on actual frame rendering time.
// App can set this mode to off using this function.
// If App wants to override the swap interval calculated by swappy it can again to
// Swappy_setSwapIntervalNS. Swappy will set swap interval to the overridden value and
// reset its frame timings.
// NOTE: Swappy may still change this value based on new frames rendering time. To completely
// override auto swap interval value app needs to call first to Swappy_setAutoSwapInterval(false);
void Swappy_setAutoSwapInterval(bool enabled);

// toggle statistics collection on/off
// by default, stats collection if off (and there is no overhead related to stats).
// App can turn on stats collection by calling Swappy_setStatsMode(true).
// Then, App is expected to call Swappy_recordFrameStart for each frame before starting to do
// any CPU related work related to that frame (such as updating the model).
// Stats will be logged to logcat with 'FrameStatistics' tag. An App can get the stats by calling
// Swappy_getStats
void Swappy_enableStats(bool enabled);

struct Swappy_Stats {
    // total frames swapped by swappy
    uint64_t totalFrames;

    // histogram of the number of screen refreshes a frame waited in the compositor queue after
    // rendering was completed.
    // for example:
    //     if a frame waited 2 refresh periods in the compositor queue after rendering was done,
    //     the frame will be counted in idleFrames[2]
    uint64_t idleFrames[MAX_FRAME_BUCKETS];

    // histogram of the number of screen refreshes passed between the requested presentation time
    // and the actual present time.
    // for example:
    //     if a frame was presented 2 refresh periods after the requested timestamp swappy set,
    //     the frame will be counted in lateFrames[2]
    uint64_t lateFrames[MAX_FRAME_BUCKETS];

    // histogram of the number of screen refreshes passed between two consecutive frames
    // for example:
    //     if frame N was presented 2 refresh periods after frame N-1
    //     frame N will be counted in offsetFromPrevFrame[2]
    uint64_t offsetFromPrevFrame[MAX_FRAME_BUCKETS];

    // histogram of the number of screen refreshes passed between the call to
    // Swappy_recordFrameStart and the actual present time.
    //     if a frame was presented 2 refresh periods after the call to Swappy_recordFrameStart
    //     the frame will be counted in latencyFrames[2]
    uint64_t latencyFrames[MAX_FRAME_BUCKETS];
};

void Swappy_recordFrameStart(EGLDisplay display, EGLSurface surface);

void Swappy_getStats(Swappy_Stats *);

#ifdef __cplusplus
};
#endif
