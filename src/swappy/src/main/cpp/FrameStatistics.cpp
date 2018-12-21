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

#include "FrameStatistics.h"

#define LOG_TAG "FrameStatistics"

#include <cmath>

#include "EGL.h"

#include "Log.h"


namespace swappy {

void FrameStatistics::updateIdleFrames(EGL::FrameTimestamps& frameStats) {
    int64_t deltaTimeNano = frameStats.compositionLatched - frameStats.renderingCompleted;
    // add 1ms to smooth the results
    deltaTimeNano += 1000000;

    int64_t numIdleFrames = deltaTimeNano / mRefreshPeriod.count();

    if (numIdleFrames >= MAX_FRAME_BUCKETS)
        numIdleFrames = MAX_FRAME_BUCKETS - 1;

    mStats.idleFrames[numIdleFrames]++;
}

void FrameStatistics::updateLateFrames(EGL::FrameTimestamps& frameStats) {
    int64_t deltaTimeNano = frameStats.presented - frameStats.requested;
    // add 1ms to smooth the results
    deltaTimeNano += 1000000;

    int64_t numLateFrames = deltaTimeNano / mRefreshPeriod.count();

    if (numLateFrames >= MAX_FRAME_BUCKETS)
        numLateFrames = MAX_FRAME_BUCKETS - 1;

    ALOGE("presented/requested delta %f apart", (frameStats.presented - frameStats.requested) / 1e6f);
    mStats.lateFrames[numLateFrames]++;
}

void FrameStatistics::updateOffsetFromPrevFrame(swappy::EGL::FrameTimestamps &frameStats) {
    static EGLnsecsANDROID prevFrame = 0;
    int64_t deltaTimeNano = frameStats.presented - prevFrame;
    // add 1ms to smooth the results
    deltaTimeNano += 1000000;
    int64_t numFrames = deltaTimeNano / mRefreshPeriod.count();
    if (numFrames >= MAX_FRAME_BUCKETS)
        numFrames = MAX_FRAME_BUCKETS - 1;

    mStats.offsetFromPrevFrame[numFrames]++;
    prevFrame = frameStats.presented;
}

// called once per swap
void FrameStatistics::capture(EGLDisplay dpy, EGLSurface surface) {
    // first get the next frame id
    std::optional<EGLuint64KHR> nextFrameId = mEgl->getNextFrameId(dpy, surface);
    if (nextFrameId) {
        mPendingFrames.push_back({dpy, surface, nextFrameId.value()});
    }

    if (mPendingFrames.size() == 0) return;

    EGLFrame &frame = mPendingFrames.front();
    // make sure we don't lag behind the stats too much
    if  (nextFrameId.value() - frame.id > MAX_FRAME_LAG) {
        while (mPendingFrames.size() > 1)
            mPendingFrames.erase(mPendingFrames.begin());
        return;
    }

    std::unique_ptr<EGL::FrameTimestamps> frameStats =
            mEgl->getFrameTimestamps(frame.dpy, frame.surface, frame.id);

    if (!frameStats) return;

    mPendingFrames.erase(mPendingFrames.begin());

    mStats.totalFrames++;
    updateIdleFrames(*frameStats);
    updateLateFrames(*frameStats);
    updateOffsetFromPrevFrame(*frameStats);
}

} // namespace swappy
