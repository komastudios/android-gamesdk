/*
 * Copyright 2019 The Android Open Source Project
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

#include <jni.h>
#include <Log.h>
#include <map>
#include "SwappyDisplayManager.h"
#include "Settings.h"

#define LOG_TAG "SwappyDisplayManager"


extern "C" const unsigned char SwappyDisplayManager_bytes[];
extern "C" unsigned long long SwappyDisplayManager_bytes_len;

extern "C" {

JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_SwappyDisplayManager_nSetSupportedRefreshRates(
    JNIEnv *env,
    jobject /* this */,
    jlong cookie,
    jlongArray refreshRates,
    jintArray modeIds);

JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_SwappyDisplayManager_nOnRefreshRateChanged(
    JNIEnv *env,
    jobject /* this */,
    jlong cookie,
    jlong refreshPeriod,
    jlong appOffset,
    jlong sfOffset);
}

namespace swappy {

// Load a Java classes from an in-memory dex file
// Equivalent Java code:
// import dalvik.system.InMemoryDexClassLoader;
// ...
// Class LoadClassFromDexBytes(ClassLoader parentLoader, String name, java.nio.ByteBuffer bytes) {
//   InMemoryDexClassLoader dexLoader = new InMemoryDexClassLoader(bytes, classLoader);
//   return dexLoader.loadClass(name);
// }
jclass LoadClassFromDexBytes(JNIEnv* env, jobject parentClassLoaderObj, const char* name,
                                    const unsigned char* bytes, size_t bytes_len) {
    jmethodID loadClassMethod = env->GetMethodID(env->FindClass("java/lang/ClassLoader"),
                                           "loadClass",
                                           "(Ljava/lang/String;)Ljava/lang/Class;");
    jstring dexLoaderClassName = env->NewStringUTF("dalvik/system/InMemoryDexClassLoader");
    jclass imclassloaderClass = static_cast<jclass>(env->CallObjectMethod(parentClassLoaderObj,
        loadClassMethod, dexLoaderClassName));
    env->DeleteLocalRef(dexLoaderClassName);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return nullptr;
    }
    jmethodID constructor = env->GetMethodID(imclassloaderClass, "<init>",
                                             "(Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;)V");
    auto byteBuffer = env->NewDirectByteBuffer((void*)bytes, bytes_len);
    jobject imclassloaderObj = env->NewObject(imclassloaderClass, constructor, byteBuffer,
                                              parentClassLoaderObj);
    jstring swappyDisplayManagerName = env->NewStringUTF(name);
    jclass swappyDisplayManagerClass = static_cast<jclass>(
        env->CallObjectMethod(imclassloaderObj, loadClassMethod, swappyDisplayManagerName));
    env->DeleteLocalRef(swappyDisplayManagerName);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return nullptr;
    }
    return swappyDisplayManagerClass;
}

static jclass SwappyDisplayManagerFromDexBytes(JNIEnv* env, jobject classLoaderObj) {
    jclass swappyDisplayManagerClass = LoadClassFromDexBytes(env, classLoaderObj,
        "com/google/androidgamesdk/SwappyDisplayManager",
        SwappyDisplayManager_bytes, SwappyDisplayManager_bytes_len);
    JNINativeMethod SDMNativeMethods[2] = {
        {"nSetSupportedRefreshRates", "(J[J[I)V",
         (void*)&Java_com_google_androidgamesdk_SwappyDisplayManager_nSetSupportedRefreshRates},
        {"nOnRefreshRateChanged", "(JJJJ)V",
         (void*)&Java_com_google_androidgamesdk_SwappyDisplayManager_nOnRefreshRateChanged}};
    env->RegisterNatives(swappyDisplayManagerClass, SDMNativeMethods, 2);
    return swappyDisplayManagerClass;
}

SwappyDisplayManager::SwappyDisplayManager(JavaVM* vm, jobject mainActivity) : mJVM(vm) {
    JNIEnv *env;
    mJVM->AttachCurrentThread(&env, nullptr);

    // Since we may call this from ANativeActivity we cannot use env->FindClass as the classpath
    // will be empty. Instead we need to work a bit harder
    jclass activity = env->GetObjectClass(mainActivity);
    jclass classLoader = env->FindClass("java/lang/ClassLoader");
    jmethodID getClassLoader = env->GetMethodID(activity,
                                                "getClassLoader",
                                                "()Ljava/lang/ClassLoader;");
    jmethodID loadClass = env->GetMethodID(classLoader,
                                           "loadClass",
                                           "(Ljava/lang/String;)Ljava/lang/Class;");
    jobject classLoaderObj = env->CallObjectMethod(mainActivity, getClassLoader);
    jstring swappyDisplayManagerName =
            env->NewStringUTF("com/google/androidgamesdk/SwappyDisplayManager");

    jclass swappyDisplayManagerClass = static_cast<jclass>(
            env->CallObjectMethod(classLoaderObj, loadClass, swappyDisplayManagerName));
    env->DeleteLocalRef(swappyDisplayManagerName);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        swappyDisplayManagerClass = SwappyDisplayManagerFromDexBytes(env,classLoaderObj);
        if (swappyDisplayManagerClass == nullptr) {
            ALOGE("Unable to find com.google.androidgamesdk.SwappyDisplayManager class");
            ALOGE("Did you integrate extras ?");
            return;
        }
        else {
            ALOGI("Using internal SwappyDisplayManager class from dex bytes.");
        }
    }

    jmethodID constructor = env->GetMethodID(
            swappyDisplayManagerClass,
            "<init>",
            "(JLandroid/app/Activity;)V");
    mSetPreferredRefreshRate = env->GetMethodID(
            swappyDisplayManagerClass,
            "setPreferredRefreshRate",
            "(I)V");
    mTerminate = env->GetMethodID(
            swappyDisplayManagerClass,
            "terminate",
            "()V");
    jobject swappyDisplayManager = env->NewObject(swappyDisplayManagerClass,
                                                  constructor,
                                                  (jlong)this,
                                                  mainActivity);
    mJthis = env->NewGlobalRef(swappyDisplayManager);

    mInitialized = true;
}

SwappyDisplayManager::~SwappyDisplayManager() {
    JNIEnv *env;
    mJVM->AttachCurrentThread(&env, nullptr);

    if (mJthis) {
        env->CallVoidMethod(mJthis, mTerminate);
        env->DeleteGlobalRef(mJthis);
    }
}

std::shared_ptr<SwappyDisplayManager::RefreshRateMap>
SwappyDisplayManager::getSupportedRefreshRates() {
    std::unique_lock<std::mutex> lock(mMutex);

    mCondition.wait(lock, [&]() { return mSupportedRefreshRates.get() != nullptr; });
    return mSupportedRefreshRates;
}

void SwappyDisplayManager::setPreferredRefreshRate(int index) {
    JNIEnv *env;
    mJVM->AttachCurrentThread(&env, nullptr);

    env->CallVoidMethod(mJthis, mSetPreferredRefreshRate, index);
}

// Helper class to wrap JNI entry points to SwappyDisplayManager
class SwappyDisplayManagerJNI {
public:
    static void onSetSupportedRefreshRates(jlong,
                                           std::shared_ptr<SwappyDisplayManager::RefreshRateMap>);
    static void onRefreshRateChanged(jlong, long, long, long);
};

void SwappyDisplayManagerJNI::onSetSupportedRefreshRates(jlong cookie,
        std::shared_ptr<SwappyDisplayManager::RefreshRateMap> refreshRates) {
    auto *sDM = reinterpret_cast<SwappyDisplayManager*>(cookie);

    std::lock_guard<std::mutex> lock(sDM->mMutex);
    sDM->mSupportedRefreshRates = std::move(refreshRates);
    sDM->mCondition.notify_one();
}

void SwappyDisplayManagerJNI::onRefreshRateChanged(jlong /*cookie*/,
                                                   long refreshPeriod,
                                                   long appOffset,
                                                   long sfOffset) {
    using std::chrono::nanoseconds;
    Settings::DisplayTimings displayTimings;
    displayTimings.refreshPeriod = nanoseconds(refreshPeriod);
    displayTimings.appOffset = nanoseconds(appOffset);
    displayTimings.sfOffset = nanoseconds(sfOffset);
    Settings::getInstance()->setDisplayTimings(displayTimings);
}

extern "C" {

JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_SwappyDisplayManager_nSetSupportedRefreshRates(
                                                                            JNIEnv *env,
                                                                            jobject /* this */,
                                                                            jlong cookie,
                                                                            jlongArray refreshRates,
                                                                            jintArray modeIds) {
    int length = env->GetArrayLength(refreshRates);
    auto refreshRatesMap =
            std::make_shared<SwappyDisplayManager::RefreshRateMap>();

    jlong *refreshRatesArr = env->GetLongArrayElements(refreshRates, 0);
    jint *modeIdsArr = env->GetIntArrayElements(modeIds, 0);
    for (int i = 0; i < length; i++) {
        (*refreshRatesMap)[std::chrono::nanoseconds(refreshRatesArr[i])] = modeIdsArr[i];
    }
    env->ReleaseLongArrayElements(refreshRates, refreshRatesArr, 0);
    env->ReleaseIntArrayElements(modeIds, modeIdsArr, 0);

    SwappyDisplayManagerJNI::onSetSupportedRefreshRates(cookie, refreshRatesMap);
}

JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_SwappyDisplayManager_nOnRefreshRateChanged(JNIEnv *env,
                                                                          jobject /* this */,
                                                                          jlong cookie,
                                                                          jlong refreshPeriod,
                                                                          jlong appOffset,
                                                                          jlong sfOffset) {
    SwappyDisplayManagerJNI::onRefreshRateChanged(cookie, refreshPeriod, appOffset, sfOffset);
}

} // extern "C"

} // namespace swappy
