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
    void (*postSwapBuffers)(void*);
    void (*startFrame)(void*);
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

// toggle stats on/off
// adyabr TODO: doc
void Swappy_setStatsMode(bool enabled);

struct Swappy_Stats {
    uint64_t totalFrames;
    uint64_t idleFrames[MAX_FRAME_BUCKETS];
    uint64_t lateFrames[MAX_FRAME_BUCKETS];
    uint64_t offsetFromPrevFrame[MAX_FRAME_BUCKETS];
    uint64_t latencyFrames[MAX_FRAME_BUCKETS];
};

void Swappy_getStats(Swappy_Stats *);

#ifdef __cplusplus
};
#endif
