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

#include <jni.h>
#include "Log.h"

extern const char _binary_classes_dex_start;
extern const char _binary_classes_dex_end;

namespace gamesdk {


static jclass loadClass(JNIEnv *env, jobject activity, const char* name, JNINativeMethod* nativeMethods, size_t nativeMethodsSize) {
    /*
    1. Get a classloder from actvity
    2. Try to create the requested class from the activty classloader
    3. If step 2 not successful then get a classloder for dex bytes 
    4. If step 3 is  successful then register native methods
    */
    jclass activityClass = env->GetObjectClass(activity);
    jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
    jmethodID getClassLoader = env->GetMethodID(activityClass,
                                                "getClassLoader",
                                                "()Ljava/lang/ClassLoader;");
    jobject classLoaderObj = env->CallObjectMethod(activity, getClassLoader);
    jmethodID loadClass = env->GetMethodID(classLoaderClass,
                                           "loadClass",
                                           "(Ljava/lang/String;)Ljava/lang/Class;");
    jstring className = env->NewStringUTF(name);
    jclass targetClass = static_cast<jclass>(
        env->CallObjectMethod(classLoaderObj, loadClass, className));
    if (env->ExceptionCheck()) {
        env->ExceptionClear();

        jstring dexLoaderClassName = env->NewStringUTF("dalvik/system/InMemoryDexClassLoader");
        jclass imclassloaderClass = static_cast<jclass>(env->CallObjectMethod(classLoaderObj,
            loadClass, dexLoaderClassName));
        env->DeleteLocalRef(dexLoaderClassName);
        if (env->ExceptionCheck() || imclassloaderClass == nullptr) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            targetClass = nullptr;
        }  else {
            jmethodID constructor = env->GetMethodID(imclassloaderClass, "<init>",
                                                     "(Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;)V");

            size_t dex_file_size = (size_t)(&_binary_classes_dex_end-&_binary_classes_dex_start);
            auto byteBuffer = env->NewDirectByteBuffer((void*)&_binary_classes_dex_start, dex_file_size);
            jobject imclassloaderObj = env->NewObject(imclassloaderClass, constructor, byteBuffer,
                                                      classLoaderObj);

            targetClass = static_cast<jclass>(
                env->CallObjectMethod(imclassloaderObj, loadClass, className));
            if (env->ExceptionCheck()) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                ALOGE("Unable to find com.google.androidgamesdk.%s class", name);
            } else {
                env->RegisterNatives(targetClass, nativeMethods, nativeMethodsSize);
                ALOGI("Using internal com.google.androidgamesdk.%s class from dex bytes.", name);
            }
        }
    }
    env->DeleteLocalRef(className);
    return targetClass;
}

} // namespace gamesdk


