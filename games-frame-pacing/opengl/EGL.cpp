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

#include "EGL.h"

#include <Trace.h>
#include <dlfcn.h>

#include <vector>

#define LOG_TAG "Swappy::EGL"

#include "Log.h"

using namespace std::chrono_literals;

namespace swappy {

std::unique_ptr<EGL> EGL::create(std::chrono::nanoseconds fenceTimeout) {
    auto eglLib = dlopen("libEGL.so", RTLD_LAZY | RTLD_LOCAL);
    if (eglLib == nullptr) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Can't load libEGL");
#endif
        return nullptr;
    }
    auto eglGetProcAddress = reinterpret_cast<eglGetProcAddress_type>(
        dlsym(eglLib, "eglGetProcAddress"));
    if (eglGetProcAddress == nullptr) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Failed to load eglGetProcAddress");
#endif
        return nullptr;
    }

    auto eglSwapBuffers =
        reinterpret_cast<eglSwapBuffers_type>(dlsym(eglLib, "eglSwapBuffers"));
    if (eglSwapBuffers == nullptr) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Failed to load eglSwapBuffers");
#endif
        return nullptr;
    }

    auto eglPresentationTimeANDROID =
        reinterpret_cast<eglPresentationTimeANDROID_type>(
            eglGetProcAddress("eglPresentationTimeANDROID"));
    if (eglPresentationTimeANDROID == nullptr) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Failed to load eglPresentationTimeANDROID");
#endif
        return nullptr;
    }

    auto eglCreateSyncKHR = reinterpret_cast<eglCreateSyncKHR_type>(
        eglGetProcAddress("eglCreateSyncKHR"));
    if (eglCreateSyncKHR == nullptr) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Failed to load eglCreateSyncKHR");
#endif
        return nullptr;
    }

    auto eglDestroySyncKHR = reinterpret_cast<eglDestroySyncKHR_type>(
        eglGetProcAddress("eglDestroySyncKHR"));
    if (eglDestroySyncKHR == nullptr) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Failed to load eglDestroySyncKHR");
#endif
        return nullptr;
    }

    auto eglGetSyncAttribKHR = reinterpret_cast<eglGetSyncAttribKHR_type>(
        eglGetProcAddress("eglGetSyncAttribKHR"));
    if (eglGetSyncAttribKHR == nullptr) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Failed to load eglGetSyncAttribKHR");
#endif
        return nullptr;
    }

    auto eglGetError =
        reinterpret_cast<eglGetError_type>(eglGetProcAddress("eglGetError"));
    if (eglGetError == nullptr) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Failed to load eglGetError");
#endif
        return nullptr;
    }

    auto eglSurfaceAttrib = reinterpret_cast<eglSurfaceAttrib_type>(
        eglGetProcAddress("eglSurfaceAttrib"));
    if (eglSurfaceAttrib == nullptr) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Failed to load eglSurfaceAttrib");
#endif
        return nullptr;
    }

    // stats may not be supported on all versions
    auto eglGetNextFrameIdANDROID =
        reinterpret_cast<eglGetNextFrameIdANDROID_type>(
            eglGetProcAddress("eglGetNextFrameIdANDROID"));

#if SWAPPY_VERBOSE_LOGGING
    if (eglGetNextFrameIdANDROID == nullptr) {
        ALOGI("Failed to load eglGetNextFrameIdANDROID");
    }
#endif

    auto eglGetFrameTimestampsANDROID =
        reinterpret_cast<eglGetFrameTimestampsANDROID_type>(
            eglGetProcAddress("eglGetFrameTimestampsANDROID"));

#if SWAPPY_VERBOSE_LOGGING
    if (eglGetFrameTimestampsANDROID == nullptr) {
        ALOGI("Failed to load eglGetFrameTimestampsANDROID");
    }
#endif

    auto egl = std::make_unique<EGL>(fenceTimeout, eglGetProcAddress,
                                     ConstructorTag{});
    egl->eglLib = eglLib;
    egl->eglSwapBuffers = eglSwapBuffers;
    egl->eglGetProcAddress = eglGetProcAddress;
    egl->eglPresentationTimeANDROID = eglPresentationTimeANDROID;
    egl->eglCreateSyncKHR = eglCreateSyncKHR;
    egl->eglDestroySyncKHR = eglDestroySyncKHR;
    egl->eglGetSyncAttribKHR = eglGetSyncAttribKHR;
    egl->eglGetError = eglGetError;
    egl->eglSurfaceAttrib = eglSurfaceAttrib;
    egl->eglGetNextFrameIdANDROID = eglGetNextFrameIdANDROID;
    egl->eglGetFrameTimestampsANDROID = eglGetFrameTimestampsANDROID;
    return egl;
}

EGL::~EGL() {
    if (eglLib) {
        dlclose(eglLib);
    }
}
void EGL::resetSyncFence(EGLDisplay display) {
    std::lock_guard<std::mutex> lock(mSyncFenceMutex);

    if (mFenceWaiter.waitForIdle() && mSyncFence != EGL_NO_SYNC_KHR) {
        EGLBoolean result = eglDestroySyncKHR(display, mSyncFence);
        if (result == EGL_FALSE) {
#if SWAPPY_VERBOSE_LOGGING
            ALOGE("Failed to destroy sync fence");
#endif
        }
    }

    mSyncFence = eglCreateSyncKHR(display, EGL_SYNC_FENCE_KHR, nullptr);

    if (mSyncFence != EGL_NO_SYNC_KHR) {
        // kick of the thread work to wait for the fence and measure its time
        mFenceWaiter.onFenceCreation(display, mSyncFence);
    }
#if SWAPPY_VERBOSE_LOGGING
    else {
        ALOGE("Failed to create sync fence");
    }
#endif
}

bool EGL::lastFrameIsComplete(EGLDisplay display) {
    std::lock_guard<std::mutex> lock(mSyncFenceMutex);

    // This will be the case on the first frame
    if (mSyncFence == EGL_NO_SYNC_KHR) {
        return true;
    }

    EGLint status = 0;
    EGLBoolean result =
        eglGetSyncAttribKHR(display, mSyncFence, EGL_SYNC_STATUS_KHR, &status);
    if (result == EGL_FALSE) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Failed to get sync status");
#endif
        return true;
    }

    if (status == EGL_SIGNALED_KHR) {
        return true;
    } else if (status == EGL_UNSIGNALED_KHR) {
        return false;
    } else {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Unexpected sync status: %d", status);
#endif
        return true;
    }
}

bool EGL::setPresentationTime(EGLDisplay display, EGLSurface surface,
                              std::chrono::steady_clock::time_point time) {
    eglPresentationTimeANDROID(display, surface,
                               time.time_since_epoch().count());
    return EGL_TRUE;
}

bool EGL::statsSupported() {
    return (eglGetNextFrameIdANDROID != nullptr &&
            eglGetFrameTimestampsANDROID != nullptr);
}

std::pair<bool, EGLuint64KHR> EGL::getNextFrameId(EGLDisplay dpy,
                                                  EGLSurface surface) const {
    if (eglGetNextFrameIdANDROID == nullptr) {
        ALOGE_ONCE("stats are not supported on this platform");
        return {false, 0};
    }

    EGLuint64KHR frameId;
    EGLBoolean result = eglGetNextFrameIdANDROID(dpy, surface, &frameId);
    if (result == EGL_FALSE) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Failed to get next frame ID");
#endif
        return {false, 0};
    }

    return {true, frameId};
}

std::unique_ptr<EGL::FrameTimestamps> EGL::getFrameTimestamps(
    EGLDisplay dpy, EGLSurface surface, EGLuint64KHR frameId) const {
#if (not defined ANDROID_NDK_VERSION) || ANDROID_NDK_VERSION >= 15
    if (eglGetFrameTimestampsANDROID == nullptr) {
        ALOGE_ONCE("stats are not supported on this platform");
        return nullptr;
    }
    const std::vector<EGLint> timestamps = {
        EGL_REQUESTED_PRESENT_TIME_ANDROID,
        EGL_RENDERING_COMPLETE_TIME_ANDROID,
        EGL_COMPOSITION_LATCH_TIME_ANDROID,
        EGL_DISPLAY_PRESENT_TIME_ANDROID,
    };

    std::vector<EGLnsecsANDROID> values(timestamps.size());

    EGLBoolean result =
        eglGetFrameTimestampsANDROID(dpy, surface, frameId, timestamps.size(),
                                     timestamps.data(), values.data());
    if (result == EGL_FALSE) {
        EGLint reason = eglGetError();
        if (reason == EGL_BAD_SURFACE) {
            eglSurfaceAttrib(dpy, surface, EGL_TIMESTAMPS_ANDROID, EGL_TRUE);
        } else {
            ALOGE_ONCE("Failed to get timestamps for frame %llu",
                       (unsigned long long)frameId);
        }
        return nullptr;
    }

    // try again if we got some pending stats
    for (auto i : values) {
        if (i == EGL_TIMESTAMP_PENDING_ANDROID) return nullptr;
    }

    std::unique_ptr<EGL::FrameTimestamps> frameTimestamps =
        std::make_unique<EGL::FrameTimestamps>();
    frameTimestamps->requested = values[0];
    frameTimestamps->renderingCompleted = values[1];
    frameTimestamps->compositionLatched = values[2];
    frameTimestamps->presented = values[3];

    return frameTimestamps;
#else
    return nullptr;
#endif
}

EGL::FenceWaiter::FenceWaiter(std::chrono::nanoseconds fenceTimeout,
                              eglGetProcAddress_type getProcAddress)
    : mFenceTimeout(fenceTimeout) {
    std::unique_lock<std::mutex> lock(mFenceWaiterLock);

    eglClientWaitSyncKHR = reinterpret_cast<eglClientWaitSyncKHR_type>(
        getProcAddress("eglClientWaitSyncKHR"));
#if SWAPPY_VERBOSE_LOGGING
    if (eglClientWaitSyncKHR == nullptr)
        ALOGE("Failed to load eglClientWaitSyncKHR");
#endif

    eglDestroySyncKHR = reinterpret_cast<eglDestroySyncKHR_type>(
        getProcAddress("eglDestroySyncKHR"));
#if SWAPPY_VERBOSE_LOGGING
    if (eglDestroySyncKHR == nullptr) ALOGE("Failed to load eglDestroySyncKHR");
#endif

    mFenceWaiter = Thread([this]() { threadMain(); });
}

EGL::FenceWaiter::~FenceWaiter() {
    {
        std::lock_guard<std::mutex> lock(mFenceWaiterLock);
        mFenceWaiterRunning = false;
        mFenceWaiterCondition.notify_all();
    }
    mFenceWaiter.join();
}

bool EGL::FenceWaiter::waitForIdle() {
    std::lock_guard<std::mutex> lock(mFenceWaiterLock);
    mFenceWaiterCondition.wait(
        mFenceWaiterLock,
        [this]() REQUIRES(mFenceWaiterLock) { return !mFenceWaiterPending; });
    return mSyncFence != EGL_NO_SYNC_KHR;
}

void EGL::FenceWaiter::onFenceCreation(EGLDisplay display,
                                       EGLSyncKHR syncFence) {
    std::lock_guard<std::mutex> lock(mFenceWaiterLock);
    mDisplay = display;
    mSyncFence = syncFence;
    mFenceWaiterPending = true;
    mFenceWaiterCondition.notify_all();
}

void EGL::FenceWaiter::threadMain() {
    std::lock_guard<std::mutex> lock(mFenceWaiterLock);
    while (mFenceWaiterRunning) {
        // wait for new fence object
        mFenceWaiterCondition.wait(
            mFenceWaiterLock, [this]() REQUIRES(mFenceWaiterLock) {
                return mFenceWaiterPending || !mFenceWaiterRunning;
            });

        if (!mFenceWaiterRunning) {
            break;
        }

        gamesdk::ScopedTrace tracer("Swappy: GPU frame time");
        const auto startTime = std::chrono::steady_clock::now();
        EGLBoolean result = eglClientWaitSyncKHR(mDisplay, mSyncFence, 0,
                                                 mFenceTimeout.count());
#if SWAPPY_VERBOSE_LOGGING
        switch (result) {
            case EGL_FALSE:
                ALOGE("Failed to wait sync");
                break;
            case EGL_TIMEOUT_EXPIRED_KHR:
                ALOGE("Timeout waiting for fence");
                break;
        }
#endif
        if (result != EGL_CONDITION_SATISFIED_KHR) {
            result = eglDestroySyncKHR(mDisplay, mSyncFence);
#if SWAPPY_VERBOSE_LOGGING
            if (result == EGL_FALSE) {
                ALOGE("Failed to destroy sync fence");
            }
#endif
            mSyncFence = EGL_NO_SYNC_KHR;
        }

        mFencePendingTime = std::chrono::steady_clock::now() - startTime;

        mFenceWaiterPending = false;
        mFenceWaiterCondition.notify_all();
    }
}

std::chrono::nanoseconds EGL::FenceWaiter::getFencePendingTime() const {
    // return mFencePendingTime without a lock to avoid blocking the main thread
    // worst case, the time will be of some previous frame
    return mFencePendingTime.load();
}

}  // namespace swappy
