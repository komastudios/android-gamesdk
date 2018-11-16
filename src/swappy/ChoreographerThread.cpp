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

#include "ChoreographerThread.h"
#include "Log.h"

#define LOG_TAG "ChoreographerThread"

#include "Thread.h"
#include "Trace.h"

#if __ANDROID_API__ >= 24
ChoreographerThread::ChoreographerThread(
        void (*onChoreographer)(void* data),
        void *data) :
    mCallback(onChoreographer), mCallbackData(data),
    mThread(looperThreadWrapper, this), mThreadRunning(true)
{
    std::unique_lock<std::mutex> lock(mWaitingMutex);
    // create a new ALooper thread to get Choreographer events
    mThreadRunning = true;
    mWaitingCondition.wait(lock, [&]() { return mChoreographer != nullptr; });
}

ChoreographerThread::~ChoreographerThread()
{
    if (mLooper) {
        ALooper_acquire(mLooper);
        mThreadRunning = false;
        ALooper_wake(mLooper);
        ALooper_release(mLooper);
        mThread.join();
    }
}

void *ChoreographerThread::looperThreadWrapper(void *data)
{
    ChoreographerThread *me = reinterpret_cast<ChoreographerThread *>(data);
    me->looperThread();
    return NULL;
}

void ChoreographerThread::looperThread()
{
    int outFd, outEvents;
    void *outData;

    mLooper = ALooper_prepare(0);
    if (!mLooper) {
        ALOGE("ALooper_prepare failed");
        return;
    }

    mChoreographer = AChoreographer_getInstance();
    if (!mChoreographer) {
        ALOGE("AChoreographer_getInstance failed");
        return;
    }
    mWaitingCondition.notify_all();

    while (mThreadRunning) {
        ALooper_pollAll(-1, &outFd, &outEvents, &outData);
    }

    return;
}

void ChoreographerThread::postFrameCallbacks()
{
    // This method is called before calling to swap buffers
    // It register to get maxCallbacksBeforeIdle frame callbacks before going idle
    // so if app goes to idle the thread will not get further frame callbacks
    std::unique_lock<std::mutex> lock(mWaitingMutex);
    if (callbacksBeforeIdle == 0) {
        AChoreographer_postFrameCallbackDelayed(mChoreographer, onChoreographerWrapper, this, 1);
    }
    callbacksBeforeIdle = maxCallbacksBeforeIdle;
}

void ChoreographerThread::onChoreographerWrapper(long frameTimeNanos, void *data)
{
    ChoreographerThread *me = reinterpret_cast<ChoreographerThread *>(data);
    me->onChoreographer(frameTimeNanos);
}

void ChoreographerThread::onChoreographer(long frameTimeNanos)
{
    {
        std::unique_lock<std::mutex> lock(mWaitingMutex);
        callbacksBeforeIdle--;

        if (callbacksBeforeIdle > 0) {
            AChoreographer_postFrameCallbackDelayed(mChoreographer, onChoreographerWrapper, this, 1);
        }
    }
    mCallback(mCallbackData);
}
#else
ChoreographerThread::ChoreographerThread(
        void (*onChoreographer)(void* data),
        void *data) :
        mCallback(onChoreographer), mCallbackData(data) {}

void ChoreographerThread::postFrameCallbacks()
{
    // call the callback immediately
    mCallback(mCallbackData);
}

ChoreographerThread::~ChoreographerThread() {}
#endif