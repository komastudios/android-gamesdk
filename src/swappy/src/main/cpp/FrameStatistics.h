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

#include "EGL.h"

#include <array>
#include <map>
#include <vector>

#include <swappy/swappy_extra.h>
namespace swappy {

class FrameStatistics {
public:
    explicit FrameStatistics(std::shared_ptr<EGL> egl,
                             std::chrono::nanoseconds refreshPeriod) :
            mEgl(egl), mRefreshPeriod(refreshPeriod) {};
    ~FrameStatistics() = default;

    void capture(EGLDisplay dpy, EGLSurface surface);

    Swappy_Stats getStats() { return mStats; }

private:
    static constexpr int MAX_FRAME_LAG = 5;

    void updateIdleFrames(EGL::FrameTimestamps& frameStats);
    void updateLateFrames(EGL::FrameTimestamps& frameStats);
    void updateOffsetFromPrevFrame(EGL::FrameTimestamps& frameStats);

    std::shared_ptr<EGL> mEgl;
    const std::chrono::nanoseconds mRefreshPeriod;

    struct EGLFrame {
        EGLDisplay dpy;
        EGLSurface surface;
        EGLuint64KHR id;
    };
    std::vector<EGLFrame> mPendingFrames;

    Swappy_Stats mStats = {0};
};

} //namespace swappy
