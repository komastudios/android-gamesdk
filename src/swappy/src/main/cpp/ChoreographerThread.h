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

#include <android/choreographer.h>
#include <android/looper.h>
#include <thread>
#include "Thread.h"

class ChoreographerThread {
public:
    virtual ~ChoreographerThread() = 0;

    virtual void postFrameCallbacks();

protected:
    ChoreographerThread(std::function<void()> onChoreographer);
    virtual void scheduleNextFrameCallback() REQUIRES(mWaitingMutex) = 0;
    virtual void onChoreographer();

    std::mutex mWaitingMutex;
    int callbacksBeforeIdle GUARDED_BY(mWaitingMutex) = 0;
    std::function<void()> mCallback;

    static constexpr int MAX_CALLBACKS_BEFORE_IDLE = 10;
};

class ChoreographerThreadFactory {
public:
    enum ChoreographerType {
        // choreographer ticks are provided by application
        APP_CHOREOGRAPHER,

        // register internally with choreographer
        SWAPPY_CHOREOGRAPHER,
    };

    static std::unique_ptr<ChoreographerThread> createChoreographerThread(
            ChoreographerType type, JavaVM *vm, std::function<void()> onChoreographer);

private:
    static int getSDKVersion(JavaVM *vm);
    static bool isChoreographerCallbackClassLoaded(JavaVM *vm);
};

