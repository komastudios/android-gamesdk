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

#pragma once

#include <jni.h>
#include <vector>
#include <string>

#define CHECK_FOR_JNI_EXCEPTION_AND_RETURN(A) if (jni.RawExceptionCheck()) { \
        std::string exception_msg = jni.GetExceptionMessage(); \
        ALOGW("%s", exception_msg.c_str()); return A; }

namespace tuningfork {

namespace jni {

// A wrapper around a jni jstring.
// Releases the jstring and any c string pointer generated from it upon destruction.
class String {
    JNIEnv* env_;
    jstring j_str_;
    const char* c_str_;
  public:
    String(JNIEnv* env, jstring s) : env_(env), j_str_(s), c_str_(nullptr) {}
    String(String&& rhs) : env_(rhs.env_), j_str_(rhs.j_str_), c_str_(rhs.c_str_) {}
    String(const String& rhs) : env_(rhs.env_), j_str_(rhs.j_str_), c_str_(nullptr) {
        if (j_str_!=nullptr) {
            j_str_ = reinterpret_cast<jstring>(env_->NewLocalRef(j_str_));
        }
    }
    String& operator=(const String& rhs) {
        Release();
        env_ = rhs.env_;
        if (rhs.j_str_!=nullptr) {
            j_str_ = reinterpret_cast<jstring>(env_->NewLocalRef(rhs.j_str_));
        }
        return *this;
    }
    jstring J() const { return j_str_;}
    const char* C() {
        if (c_str_==nullptr && j_str_!=nullptr) {
            c_str_ = env_->GetStringUTFChars(j_str_, nullptr);
        }
        return c_str_;
    }
    ~String() {
        Release();
    }
    void Release() {
        if (c_str_!=nullptr)
            env_->ReleaseStringUTFChars(j_str_, c_str_);
        if (j_str_!=nullptr)
            env_->DeleteLocalRef(j_str_);
    }
};

// A helper class that makes calling methods easier and also keeps track of object references
//  and deletes them when the helper is destroyed.
class Helper {
    JNIEnv* env_;
    std::vector<jobject> objs_;
    jmethodID find_class_;
    jobject activity_class_loader_;
  public:
    struct ClassAndObject {
        jclass clz;
        jobject obj;
    };
    static constexpr int BAD_FIELD = -1;
    Helper(JNIEnv* env, jobject activity);
    ~Helper();

    JNIEnv* env() const { return env_; }

    jclass FindClass(const char* class_name) const;

    ClassAndObject NewObjectV(const char * cclz, const char* ctorSig, va_list argptr);
    ClassAndObject NewObject(const char * cclz, const char* ctorSig, ...);
    jni::String NewString(const std::string& s) const;

    // If clz is empty, get the class from the object
    ClassAndObject Cast(jobject o, const std::string& clz="") const;

    // These methods take a variable number of arguments and have the return the type indicated.
    // The arguments are passed directly to JNI and it's not type-safe, so:
    //   All object arguments should be jobjects, NOT ClassAndObject.
    //   All string arguments should be jstrings, NOT jni:String.
    jobject CallObjectMethod(const ClassAndObject& obj, const char* name, const char* sig, ...);
    jobject CallStaticObjectMethod(const ClassAndObject& obj, const char* name,
                                   const char* sig, ...);
    jni::String CallStringMethod(const ClassAndObject& obj, const char* name, const char* sig, ...);
    void CallVoidMethod(const ClassAndObject& obj, const char* name, const char* sig, ...);
    int CallIntMethod(const ClassAndObject& obj, const char* name, const char* sig, ...);

    bool RawExceptionCheck() const { return env_->ExceptionCheck(); }
    std::string GetExceptionMessage();
    // Do a RawExceptionCheck and return the result of it, filling in the msg with the
    //  exception message if one was thrown.
    bool CheckForException(std::string& msg);

    // Returns a null object if the field could not be found (and exception is set)
    ClassAndObject GetObjectField(const ClassAndObject& o, const char* field_name, const char* sig);
    // Returns BAD_FIELD is the field could not be found (and exception is set)
    int GetIntField(const ClassAndObject& o, const char* field_name);
    std::vector<char> GetByteArrayBytes(jbyteArray jbs);
};

} // namespace jni

} // namespace tuningfork
