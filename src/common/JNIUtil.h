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

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>


#include <jni.h>
#include "Log.h"



extern const char _binary_classes_dex_start;
extern const char _binary_classes_dex_end;

namespace gamesdk {

static bool saveBytesToFile(std::string fileName, const char* bytes, size_t size) {
    std::ofstream save_file(fileName, std::ios::binary);
    if (save_file.good()) {
        save_file.write(bytes, size);
        return true;
    }
    return false;
}

static bool deleteFile(std::string fileName) {
    if(remove(fileName.c_str()) != 0 )
        return false;
    else
        return true;
}


static std::string tempFileName(JNIEnv *env, jobject activity, const char* ext) {

    jclass activityClass = env->GetObjectClass(activity);
    jmethodID getCacheDir = env->GetMethodID(activityClass, "getCacheDir", "()Ljava/io/File;");
    jobject cacheDir = env->CallObjectMethod(activity, getCacheDir);

    jclass fileClass = env->FindClass("java/io/File");
    jmethodID createTempFile = env->GetStaticMethodID(fileClass, "createTempFile",
                        "(Ljava/lang/String;Ljava/lang/String;Ljava/io/File;)Ljava/io/File;");
    jstring prefix = env->NewStringUTF("ags");
    jstring suffix = env->NewStringUTF(ext);
    jobject tempFile = env->CallStaticObjectMethod(fileClass,
                                                    createTempFile,
                                                    prefix,
                                                    suffix,
                                                    cacheDir);

    jmethodID getPath  = env->GetMethodID(fileClass, "getPath", "()Ljava/lang/String;");
    jstring pathString = (jstring)env->CallObjectMethod(tempFile, getPath);

    const char *path = env->GetStringUTFChars(pathString, NULL);
    std::string tempDir(path);

    env->DeleteLocalRef(prefix);
    env->DeleteLocalRef(suffix);
    env->ReleaseStringUTFChars(pathString, path);
    return tempDir;
}



static jclass loadClass(JNIEnv *env,
                        jobject activity,
                        const char* name,
                        JNINativeMethod* nativeMethods,
                        size_t nativeMethodsSize) {
    /*
     *   1. Get a classloder from actvity
     *   2. Try to create the requested class from the activty classloader
     *   3. If step 2 not successful then get a classloder for dex bytes (in memory or file)
     *   4. If step 3 is  successful then register native methods
     */
    if(!env || !activity || !name) {
        return nullptr;
    }
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

        if (env->ExceptionCheck() || !imclassloaderClass) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            // For older SDK versions <26, where InMemoryDexClassLoader is not available
            dexLoaderClassName = env->NewStringUTF("dalvik/system/PathClassLoader");
            imclassloaderClass = static_cast<jclass>(env->CallObjectMethod(classLoaderObj,
                                                            loadClass, dexLoaderClassName));
                        env->DeleteLocalRef(dexLoaderClassName);
            if (env->ExceptionCheck() || !imclassloaderClass) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                targetClass = nullptr;
            } else {
                jmethodID constructor
                                = env->GetMethodID(imclassloaderClass,
                                                "<init>",
                                                "(Ljava/lang/String;Ljava/lang/ClassLoader;)V");
                std::string tempPath = tempFileName(env, activity, ".dex");
                size_t dex_file_size =
                                (size_t)(&_binary_classes_dex_end-&_binary_classes_dex_start);
                saveBytesToFile(tempPath, &_binary_classes_dex_start, dex_file_size);

                jstring dexPathString = env->NewStringUTF(tempPath.c_str());
                jobject imclassloaderObj = env->NewObject(imclassloaderClass,
                                                                constructor,
                                                                dexPathString,
                                                                classLoaderObj);
                env->DeleteLocalRef(dexPathString);
                targetClass = static_cast<jclass>(
                    env->CallObjectMethod(imclassloaderObj, loadClass, className));
                if (env->ExceptionCheck()) {
                    env->ExceptionDescribe();
                    env->ExceptionClear();
                    ALOGE("Unable to find com.google.androidgamesdk.%s class", name);
                } else {
                    env->RegisterNatives(targetClass, nativeMethods, nativeMethodsSize);
                    ALOGI("Using internal com.google.androidgamesdk.%s class from dex bytes.",
                                                                                        name);
                }
                deleteFile(tempPath);
            }
        }  else {
            jmethodID constructor =
                env->GetMethodID(imclassloaderClass,
                                "<init>",
                                "(Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;)V");

            size_t dex_file_size =
                        (size_t)(&_binary_classes_dex_end-&_binary_classes_dex_start);
            auto byteBuffer = env->NewDirectByteBuffer((void*)&_binary_classes_dex_start,
                                                            dex_file_size);
            jobject imclassloaderObj = env->NewObject(imclassloaderClass,
                                                            constructor,
                                                            byteBuffer,
                                                            classLoaderObj);

            targetClass = static_cast<jclass>(
                env->CallObjectMethod(imclassloaderObj, loadClass, className));
            if (env->ExceptionCheck()) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                ALOGE("Unable to find com.google.androidgamesdk.%s class", name);
            } else {
                env->RegisterNatives(targetClass, nativeMethods, nativeMethodsSize);
                ALOGI("Using internal com.google.androidgamesdk.%s class from dex bytes.",
                                                                                    name);
            }
        }
    }
    env->DeleteLocalRef(className);
    return targetClass;
}




} // namespace gamesdk


