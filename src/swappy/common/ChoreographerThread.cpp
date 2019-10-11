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

#include <android/looper.h>
#include <jni.h>

#include "ChoreographerThread.h"
#include "Thread.h"
#include "CpuInfo.h"

#include <condition_variable>
#include <cstring>
#include <cstdlib>

#include <sched.h>
#include <pthread.h>
#include <unistd.h>

#include "ChoreographerShim.h"
#include "Log.h"
#include "Trace.h"
#include "JNIUtil.h"

namespace swappy {

// AChoreographer is supported from API 24. To allow compilation for minSDK < 24
// and still use AChoreographer for SDK >= 24 we need runtime support to call
// AChoreographer APIs.

using PFN_AChoreographer_getInstance = AChoreographer* (*)();

using PFN_AChoreographer_postFrameCallback = void (*)(AChoreographer* choreographer,
                                                  AChoreographer_frameCallback callback,
                                                  void* data);

using PFN_AChoreographer_postFrameCallbackDelayed = void (*)(AChoreographer* choreographer,
                                                        AChoreographer_frameCallback callback,
                                                        void* data,
                                                        long delayMillis);

extern "C" {

    JNIEXPORT void JNICALL
    Java_com_google_androidgamesdk_ChoreographerCallback_nOnChoreographer(JNIEnv * /*env*/,
                                                                      jobject /*this*/,
                                                                      jlong cookie,
                                                                      jlong /*frameTimeNanos*/);

}


class NDKChoreographerThread : public ChoreographerThread {
public:
    static constexpr int MIN_SDK_VERSION = 24;

    NDKChoreographerThread(Callback onChoreographer);
    ~NDKChoreographerThread() override;

private:
    void looperThread();
    void scheduleNextFrameCallback() override REQUIRES(mWaitingMutex);

    PFN_AChoreographer_getInstance mAChoreographer_getInstance = nullptr;
    PFN_AChoreographer_postFrameCallback mAChoreographer_postFrameCallback = nullptr;
    PFN_AChoreographer_postFrameCallbackDelayed mAChoreographer_postFrameCallbackDelayed = nullptr;
    void *mLibAndroid = nullptr;
    std::thread mThread;
    std::condition_variable mWaitingCondition;
    ALooper *mLooper GUARDED_BY(mWaitingMutex) = nullptr;
    bool mThreadRunning GUARDED_BY(mWaitingMutex) = false;
    AChoreographer *mChoreographer GUARDED_BY(mWaitingMutex) = nullptr;
};

NDKChoreographerThread::NDKChoreographerThread(Callback onChoreographer) :
    ChoreographerThread(onChoreographer)
{
    mLibAndroid = dlopen("libandroid.so", RTLD_NOW | RTLD_LOCAL);
    if (mLibAndroid == nullptr) {
        ALOGE("FATAL: cannot open libandroid.so: %s", strerror(errno));
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

    if (!mAChoreographer_getInstance ||
        !mAChoreographer_postFrameCallback ||
        !mAChoreographer_postFrameCallbackDelayed) {
        ALOGE("FATAL: cannot get AChoreographer symbols");
        return;
    }

    std::unique_lock<std::mutex> lock(mWaitingMutex);
    // create a new ALooper thread to get Choreographer events
    mThreadRunning = true;
    mThread = std::thread([this]() { looperThread(); });
    mWaitingCondition.wait(lock, [&]() REQUIRES(mWaitingMutex) {
        return mChoreographer != nullptr;
    });

    mInitialized = true;
}

NDKChoreographerThread::~NDKChoreographerThread()
{
    ALOGI("Destroying NDKChoreographerThread");

    if (mLibAndroid != nullptr)
      dlclose(mLibAndroid);

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
    std::lock_guard<std::mutex> lock(mWaitingMutex);

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

    const char *name = "SwappyChoreographer";

    CpuInfo cpu;
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(0, &cpu_set);

    if (cpu.getNumberOfCpus() > 0) {
        ALOGI("Swappy found %d CPUs [%s].", cpu.getNumberOfCpus(), cpu.getHardware().c_str());
        if (cpu.getNumberOfLittleCores() > 0) {
            cpu_set = cpu.getLittleCoresMask();
        }
    }

    const auto tid = gettid();
    ALOGI("Setting '%s' thread [%d-0x%x] affinity mask to 0x%x.",
          name, tid, tid, to_mask(cpu_set));
    sched_setaffinity(tid, sizeof(cpu_set), &cpu_set);

    pthread_setname_np(pthread_self(), name);

    while (mThreadRunning) {
        // mutex should be unlocked before sleeping on pollAll
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
    JavaChoreographerThread(JavaVM *vm, jobject jactivity, Callback onChoreographer);
    ~JavaChoreographerThread() override;
    static void onChoreographer(jlong cookie);
    void onChoreographer() override { ChoreographerThread::onChoreographer(); };

private:
    void scheduleNextFrameCallback() override REQUIRES(mWaitingMutex);

    JavaVM *mJVM;
    JNIEnv *mEnv = nullptr;
    jobject mJobj = nullptr;
    jmethodID mJpostFrameCallback = nullptr;
    jmethodID mJterminate = nullptr;

};

JavaChoreographerThread::JavaChoreographerThread(JavaVM *vm, 
                                                 jobject jactivity, 
                                                 Callback onChoreographer) :
        ChoreographerThread(onChoreographer),
        mJVM(vm)
{

    JNIEnv *env;
    mJVM->AttachCurrentThread(&env, nullptr);

    jclass choreographerCallbackClass = gamesdk::loadClass(env, 
                                                   jactivity, 
                                                   ChoreographerThread::CT_CLASS, 
                                                   (JNINativeMethod*)ChoreographerThread::CTNativeMethods, 
                                                   ChoreographerThread::CTNativeMethodsSize);

    if (choreographerCallbackClass==nullptr) return;

    jmethodID constructor = env->GetMethodID(
            choreographerCallbackClass,
            "<init>",
            "(J)V");

    mJpostFrameCallback = env->GetMethodID(
            choreographerCallbackClass,
            "postFrameCallback",
            "()V");

    mJterminate = env->GetMethodID(
            choreographerCallbackClass,
            "terminate",
            "()V");

    jobject choreographerCallback =
            env->NewObject(choreographerCallbackClass, constructor, reinterpret_cast<jlong>(this));

    mJobj = env->NewGlobalRef(choreographerCallback);

    mInitialized = true;
}

JavaChoreographerThread::~JavaChoreographerThread()
{
    ALOGI("Destroying JavaChoreographerThread");

    if (!mJobj) {
        return;
    }

    JNIEnv *env;
    mJVM->AttachCurrentThread(&env, nullptr);
    env->CallVoidMethod(mJobj, mJterminate);
    env->DeleteGlobalRef(mJobj);
    mJVM->DetachCurrentThread();
}

void JavaChoreographerThread::scheduleNextFrameCallback()
{
    JNIEnv *env;
    mJVM->AttachCurrentThread(&env, nullptr);
    env->CallVoidMethod(mJobj, mJpostFrameCallback);
}

void JavaChoreographerThread::onChoreographer(jlong cookie) {
    JavaChoreographerThread *me = reinterpret_cast<JavaChoreographerThread*>(cookie);
    me->onChoreographer();
}

extern "C" {

JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_ChoreographerCallback_nOnChoreographer(JNIEnv * /*env*/,
                                                                      jobject /*this*/,
                                                                      jlong cookie,
                                                                      jlong /*frameTimeNanos*/) {
    JavaChoreographerThread::onChoreographer(cookie);
}

} // extern "C"

class NoChoreographerThread : public ChoreographerThread {
public:
    NoChoreographerThread(Callback onChoreographer);

private:
    void postFrameCallbacks() override;
    void scheduleNextFrameCallback() override REQUIRES(mWaitingMutex);
};

NoChoreographerThread::NoChoreographerThread(Callback onChoreographer) :
    ChoreographerThread(onChoreographer) {
    mInitialized = true;
}


void NoChoreographerThread::postFrameCallbacks() {
    mCallback();
}

void NoChoreographerThread::scheduleNextFrameCallback() {}

const char* ChoreographerThread::CT_CLASS = "com/google/androidgamesdk/ChoreographerCallback";

const JNINativeMethod ChoreographerThread::CTNativeMethods[] = {
        {"nOnChoreographer", "(JJ)V",
         (void*)&Java_com_google_androidgamesdk_ChoreographerCallback_nOnChoreographer}};
                                                                                         
ChoreographerThread::ChoreographerThread(Callback onChoreographer):
        mCallback(onChoreographer) {}

ChoreographerThread::~ChoreographerThread() = default;

void ChoreographerThread::postFrameCallbacks()
{
    TRACE_CALL();

    // This method is called before calling to swap buffers
    // It registers to get MAX_CALLBACKS_BEFORE_IDLE frame callbacks before going idle
    // so if app goes to idle the thread will not get further frame callbacks
    std::lock_guard<std::mutex> lock(mWaitingMutex);
    if (mCallbacksBeforeIdle == 0) {
        scheduleNextFrameCallback();
    }
    mCallbacksBeforeIdle = MAX_CALLBACKS_BEFORE_IDLE;
}

void ChoreographerThread::onChoreographer()
{
    TRACE_CALL();

    {
        std::lock_guard<std::mutex> lock(mWaitingMutex);
        mCallbacksBeforeIdle--;

        if (mCallbacksBeforeIdle > 0) {
            scheduleNextFrameCallback();
        }
    }
    mCallback();
}


std::unique_ptr<ChoreographerThread>
    ChoreographerThread::createChoreographerThread(
                Type type, JavaVM *vm, jobject jactivity, Callback onChoreographer, int sdkVersion) {
    if (type == Type::App) {
        ALOGI("Using Application's Choreographer");
        return std::make_unique<NoChoreographerThread>(onChoreographer);
    }

    if (vm == nullptr || sdkVersion >= NDKChoreographerThread::MIN_SDK_VERSION) {
        ALOGI("Using NDK Choreographer");
        return std::make_unique<NDKChoreographerThread>(onChoreographer);
    }

    JNIEnv *env;
    vm->AttachCurrentThread(&env, nullptr);
    std::unique_ptr<ChoreographerThread> javaChoreographerThread = 
        std::make_unique<JavaChoreographerThread>(vm, jactivity, onChoreographer);
    if (javaChoreographerThread->isInitialized()){
        ALOGI("Using Java Choreographer");
        return javaChoreographerThread;        
    }

    ALOGI("Using no Choreographer (Best Effort)");
    return std::make_unique<NoChoreographerThread>(onChoreographer);
}

} // namespace swappy
