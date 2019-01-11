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

#include <chrono>
#include <mutex>
#include <optional>
#include <thread>

#include <EGL/egl.h>
#include <EGL/eglext.h>

using std::chrono::nanoseconds;

namespace swappy {

class EGL {
  private:
    // Allows construction with std::unique_ptr from a static method, but disallows construction
    // outside of the class since no one else can construct a ConstructorTag
    struct ConstructorTag {
    };

  public:
    struct FrameTimestamps {
        EGLnsecsANDROID requested;
        EGLnsecsANDROID renderingCompleted;
        EGLnsecsANDROID compositionLatched;
        EGLnsecsANDROID presented;
    };

    explicit EGL(nanoseconds refreshPeriod, ConstructorTag)
        : mRefreshPeriod(refreshPeriod) {}

    static std::unique_ptr<EGL> create(nanoseconds refreshPeriod);

    void resetSyncFence(EGLDisplay display);
    bool lastFrameIsComplete(EGLDisplay display);
    bool setPresentationTime(EGLDisplay display,
                             EGLSurface surface,
                             std::chrono::steady_clock::time_point time);
    nanoseconds getFencePendingTime() { return mFenceWaiter.getFencePendingTime(); }

    // for stats
    std::optional<EGLuint64KHR> getNextFrameId(EGLDisplay dpy, EGLSurface surface);
    std::unique_ptr<FrameTimestamps> getFrameTimestamps(EGLDisplay dpy, EGLSurface surface, EGLuint64KHR frameId);

  private:
    const nanoseconds mRefreshPeriod;

    using eglPresentationTimeANDROID_type = EGLBoolean (*)(EGLDisplay, EGLSurface, EGLnsecsANDROID);
    eglPresentationTimeANDROID_type eglPresentationTimeANDROID = nullptr;
    using eglCreateSyncKHR_type = EGLSyncKHR (*)(EGLDisplay, EGLenum, const EGLint *);
    eglCreateSyncKHR_type eglCreateSyncKHR = nullptr;
    using eglDestroySyncKHR_type = EGLBoolean (*)(EGLDisplay, EGLSyncKHR);
    eglDestroySyncKHR_type eglDestroySyncKHR = nullptr;
    using eglGetSyncAttribKHR_type = EGLBoolean (*)(EGLDisplay, EGLSyncKHR, EGLint, EGLint *);
    eglGetSyncAttribKHR_type eglGetSyncAttribKHR = nullptr;


    using eglGetError_type = EGLint (*)(void);
    eglGetError_type eglGetError = nullptr;
    using eglSurfaceAttrib_type = EGLBoolean (*)(EGLDisplay, EGLSurface, EGLint, EGLint);
    eglSurfaceAttrib_type eglSurfaceAttrib = nullptr;
    using eglGetNextFrameIdANDROID_type = EGLBoolean (*)(EGLDisplay, EGLSurface, EGLuint64KHR *);
    eglGetNextFrameIdANDROID_type eglGetNextFrameIdANDROID = nullptr;
    using eglGetFrameTimestampsANDROID_type =  EGLBoolean (*)(EGLDisplay, EGLSurface,
            EGLuint64KHR, EGLint, const EGLint *, EGLnsecsANDROID *);
    eglGetFrameTimestampsANDROID_type eglGetFrameTimestampsANDROID = nullptr;

    std::mutex mSyncFenceMutex;
    EGLSyncKHR mSyncFence = EGL_NO_SYNC_KHR;

    class FenceWaiter {
    public:
        FenceWaiter();
        ~FenceWaiter();

        void onFenceCreation(EGLDisplay display, EGLSyncKHR syncFence);
        void flush();
        nanoseconds getFencePendingTime();

    private:
        using eglClientWaitSyncKHR_type = EGLBoolean (*)(EGLDisplay, EGLSyncKHR, EGLint, EGLTimeKHR);
        eglClientWaitSyncKHR_type eglClientWaitSyncKHR = nullptr;

        void threadMain();
        std::thread mFenceWaiter;
        std::mutex mFenceWaiterLock;
        std::condition_variable mFenceWaiterCondition;
        bool mFenceWaiterRunning = true;
        bool mFenceWaiterPending = false;
        std::atomic<nanoseconds> mFencePendingTime;
        EGLDisplay mDisplay;
        EGLSyncKHR mSyncFence = EGL_NO_SYNC_KHR;
    };

    FenceWaiter mFenceWaiter;
};

} // namespace swappy
