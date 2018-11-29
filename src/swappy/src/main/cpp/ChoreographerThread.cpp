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

#define LOG_TAG "ChoreographerThread"

#include <jni.h>

#include "ChoreographerThread.h"
#include "Log.h"
#include "Thread.h"
#include "Trace.h"

// AChoreographer is supported from API 24. To allow compilation for minSDK < 24
// and still use AChoreographer for SDK >= 24 we need runtime support to call
// AChoreographer APIs.

typedef AChoreographer* (*PFN_AChoreographer_getInstance)();
typedef void (*PFN_AChoreographer_postFrameCallback)(AChoreographer* choreographer,
                                                  AChoreographer_frameCallback callback,
                                                  void* data);
typedef void (*PFN_AChoreographer_postFrameCallbackDelayed)(AChoreographer* choreographer,
                                                        AChoreographer_frameCallback callback,
                                                        void* data,
                                                        long delayMillis);

ChoreographerThread::ChoreographerThread(std::function<void()> onChoreographer):
        mCallback(onChoreographer) {}

ChoreographerThread::~ChoreographerThread() {};

void ChoreographerThread::postFrameCallbacks()
{
    // This method is called before calling to swap buffers
    // It register to get maxCallbacksBeforeIdle frame callbacks before going idle
    // so if app goes to idle the thread will not get further frame callbacks
    std::lock_guard lock(mWaitingMutex);
    if (callbacksBeforeIdle == 0) {
        scheduleNextFrameCallback();
    }
    callbacksBeforeIdle = MAX_CALLBACKS_BEFORE_IDLE;
}

void ChoreographerThread::onChoreographer()
{
    {
        std::lock_guard lock(mWaitingMutex);
        callbacksBeforeIdle--;

        if (callbacksBeforeIdle > 0) {
            scheduleNextFrameCallback();
        }
    }
    mCallback();
}

class NDKChoreographerThread : public ChoreographerThread {
public:
    NDKChoreographerThread(std::function<void()> onChoreographer);
    virtual ~NDKChoreographerThread() override;

private:
    void looperThread();
    virtual void scheduleNextFrameCallback() override REQUIRES(mWaitingMutex);

    PFN_AChoreographer_getInstance mAChoreographer_getInstance;
    PFN_AChoreographer_postFrameCallback mAChoreographer_postFrameCallback;
    PFN_AChoreographer_postFrameCallbackDelayed mAChoreographer_postFrameCallbackDelayed;

    void *mLibAndroid;
    std::thread mThread;
    std::condition_variable mWaitingCondition;
    ALooper *mLooper GUARDED_BY(mWaitingMutex) = nullptr;
    bool mThreadRunning GUARDED_BY(mWaitingMutex) = false;
    AChoreographer *mChoreographer GUARDED_BY(mWaitingMutex) = nullptr;
};

NDKChoreographerThread::NDKChoreographerThread(std::function<void()> onChoreographer) :
    ChoreographerThread(onChoreographer)
{
    mLibAndroid = dlopen("libandroid.so", RTLD_NOW | RTLD_LOCAL);
    if (mLibAndroid == nullptr) {
        ALOGE("cannot open libandroid.so: %s", strerror(errno));
        return;
    }

    mAChoreographer_getInstance =
            reinterpret_cast<PFN_AChoreographer_getInstance >(
                dlsym(mLibAndroid, "AChoreographer_getInstance"));

    mAChoreographer_postFrameCallback =
            reinterpret_cast<PFN_AChoreographer_postFrameCallback >(
                    dlsym(mLibAndroid, "AChoreographer_postFrameCallback"));

    mAChoreographer_postFrameCallbackDelayed =
            reinterpret_cast<PFN_AChoreographer_postFrameCallbackDelayed >(
                    dlsym(mLibAndroid, "AChoreographer_postFrameCallbackDelayed"));

    std::unique_lock<std::mutex> lock(mWaitingMutex);
    // create a new ALooper thread to get Choreographer events
    mThreadRunning = true;
    mThread = std::thread([this]() {looperThread();});
    mWaitingCondition.wait(lock, [&]() REQUIRES(mWaitingMutex) {
        return mChoreographer != nullptr;
    });
}

NDKChoreographerThread::~NDKChoreographerThread()
{
    ALOGI("Destroying NDKChoreographerThread");

    if (!mLooper) {
        return;
    }

    ALooper_acquire(mLooper);
    mThreadRunning = false;
    ALooper_wake(mLooper);
    ALooper_release(mLooper);
    mThread.join();
}

void NDKChoreographerThread::looperThread()
{
    int outFd, outEvents;
    void *outData;
    std::lock_guard lock(mWaitingMutex);

    mLooper = ALooper_prepare(0);
    if (!mLooper) {
        ALOGE("ALooper_prepare failed");
        return;
    }

    mChoreographer = mAChoreographer_getInstance();
    if (!mChoreographer) {
        ALOGE("AChoreographer_getInstance failed");
        return;
    }
    mWaitingCondition.notify_all();

    ALOGI("Staring Looper thread");
    while (mThreadRunning) {
        // mutex should be unlock before sleeping on pollAll
        mWaitingMutex.unlock();
        ALooper_pollAll(-1, &outFd, &outEvents, &outData);
        mWaitingMutex.lock();
    }
    ALOGI("Terminating Looper thread");

    return;
}

void NDKChoreographerThread::scheduleNextFrameCallback()
{
    AChoreographer_frameCallback frameCallback =
            [](long frameTimeNanos, void *data) {
                reinterpret_cast<NDKChoreographerThread*>(data)->onChoreographer();
            };

    mAChoreographer_postFrameCallbackDelayed(mChoreographer, frameCallback, this, 1);
}

class JavaChoreographerThread : public ChoreographerThread {
public:
    JavaChoreographerThread(JavaVM *vm, std::function<void()> onChoreographer);
    virtual ~JavaChoreographerThread() override;
    static void onChoreographer(jlong cookie);
    virtual void onChoreographer() override {ChoreographerThread::onChoreographer();};

private:
    virtual void scheduleNextFrameCallback() override REQUIRES(mWaitingMutex);

    JavaVM *mJVM;
    JNIEnv *mEnv = nullptr;
    jobject mChorMan = nullptr;
    jmethodID mChorMan_postFrameCallback = nullptr;
    jmethodID mChorMan_Terminate = nullptr;

};

JavaChoreographerThread::JavaChoreographerThread(JavaVM *vm,
                                                 std::function<void()> onChoreographer) :
        ChoreographerThread(onChoreographer),
        mJVM(vm)
{

    JNIEnv *env;
    mJVM->AttachCurrentThread(&env, nullptr);

    jclass choreographerCallbackClass = env->FindClass("com/google/swappy/ChoreographerCallback");

    jmethodID constructor = env->GetMethodID(
            choreographerCallbackClass,
            "<init>",
            "(J)V");

    mChorMan_postFrameCallback = env->GetMethodID(
            choreographerCallbackClass,
            "postFrameCallback",
            "()V");

    mChorMan_Terminate = env->GetMethodID(
            choreographerCallbackClass,
            "Terminate",
            "()V");

    jobject choreographerCallback = env->NewObject(choreographerCallbackClass, constructor, reinterpret_cast<jlong>(this));

    mChorMan = env->NewGlobalRef(choreographerCallback);
}

JavaChoreographerThread::~JavaChoreographerThread()
{
    ALOGI("Destroying JavaChoreographerThread");

    if (!mChorMan) {
        return;
    }

    JNIEnv *env;
    mJVM->AttachCurrentThread(&env, nullptr);
    env->CallVoidMethod(mChorMan, mChorMan_Terminate);
    env->DeleteGlobalRef(mChorMan);
    mJVM->DetachCurrentThread();
}

void JavaChoreographerThread::scheduleNextFrameCallback()
{
    JNIEnv *env;
    mJVM->AttachCurrentThread(&env, nullptr);
    env->CallVoidMethod(mChorMan, mChorMan_postFrameCallback);
}

void JavaChoreographerThread::onChoreographer(jlong cookie) {
    JavaChoreographerThread *me = reinterpret_cast<JavaChoreographerThread*>(cookie);
    me->onChoreographer();
}

extern "C" {

JNIEXPORT void JNICALL
Java_com_google_swappy_ChoreographerCallback_nOnChoreographer(JNIEnv * /* env */, jobject /* this */,
                                                              jlong cookie, jlong frameTimeNanos) {
    JavaChoreographerThread::onChoreographer(cookie);
}

} // extern "C"

class NoChoreographerThread : public ChoreographerThread {
public:
    NoChoreographerThread(std::function<void()> onChoreographer);

private:
    virtual void postFrameCallbacks() override;
    virtual void scheduleNextFrameCallback() override REQUIRES(mWaitingMutex);
};

NoChoreographerThread::NoChoreographerThread(std::function<void()> onChoreographer) :
    ChoreographerThread(onChoreographer) {}


void NoChoreographerThread::postFrameCallbacks() {
    mCallback();
}

void NoChoreographerThread::scheduleNextFrameCallback() {}

int ChoreographerThreadFactory::getSDKVersion(JavaVM *vm)
{
    JNIEnv *env;
    vm->AttachCurrentThread(&env, nullptr);

    jclass buildClass = env->FindClass("android/os/Build$VERSION");
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        ALOGE("Failed to get Build.VERSION class");
        return 0;
    }

    jfieldID sdk_int = env->GetStaticFieldID(buildClass, "SDK_INT", "I");
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        ALOGE("Failed to get Build.VERSION.SDK_INT field");
        return 0;
    }

    jint sdk = env->GetStaticIntField(buildClass, sdk_int);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        ALOGE("Failed to get SDK version");
        return 0;
    }

    ALOGI("SDK version = %d", sdk);
    return sdk;
}

bool ChoreographerThreadFactory::isChoreographerCallbackClassLoaded(JavaVM *vm)
{
    JNIEnv *env;
    vm->AttachCurrentThread(&env, nullptr);

    env->FindClass("com/google/swappy/ChoreographerCallback");
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        return false;
    }

    return true;
}

std::unique_ptr<ChoreographerThread>
        ChoreographerThreadFactory::createChoreographerThread(
                ChoreographerType type, JavaVM *vm, std::function<void()> onChoreographer) {
    if (type == APP_CHOREOGRAPHER) {
        ALOGI("Using Application's Choreographer");
        return std::make_unique<NoChoreographerThread>(onChoreographer);
    }

    if (getSDKVersion(vm) >= 24) {
        ALOGI("Using NDK Choreographer");
        return std::make_unique<NDKChoreographerThread>(onChoreographer);
    }

    if (isChoreographerCallbackClassLoaded(vm)){
        ALOGI("Using Java Choreographer");
        return std::make_unique<JavaChoreographerThread>(vm, onChoreographer);
    }

    ALOGI("Using no Choreographer (Best Effort)");
    return std::make_unique<NoChoreographerThread>(onChoreographer);
}
