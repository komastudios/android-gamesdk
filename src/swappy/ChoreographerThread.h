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
    ChoreographerThread(std::function<void()> onChoreographer);
    ~ChoreographerThread();

    void postFrameCallbacks();

private:
    std::thread mThread;
    std::mutex mWaitingMutex;
    std::condition_variable mWaitingCondition;
    ALooper *mLooper = nullptr;
    bool mThreadRunning = false;
    AChoreographer *mChoreographer = nullptr;
    std::function<void()> mCallback;
    void looperThread();
    void onChoreographer();
    void scheduleNextFrameCallback();

    int callbacksBeforeIdle GUARDED_BY(mWaitingMutex) = 0;
    static constexpr int MAX_CALLBACKS_BEFORE_IDLE = 10;

};
