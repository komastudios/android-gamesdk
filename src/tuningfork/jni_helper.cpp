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

#include "jni_helper.h"

#define CHECK_FOR_JNI_EXCEPTION_AND_RETURN(A) if (jni.RawExceptionCheck()) { \
        std::string exception_msg = jni.GetExceptionMessage(); \
        ALOGW("%s", exception_msg.c_str()); return A; }

namespace tuningfork {

namespace jni {

// A helper class that makes calling methods easier and also keeps track of object/string references
//  and deletes them when the helper is destroyed.
Helper::Helper(JNIEnv* env, jobject activity) : env_(env) {
    jclass activity_clazz = env->GetObjectClass(activity);
    jmethodID get_class_loader = env->GetMethodID(
        activity_clazz, "getClassLoader", "()Ljava/lang/ClassLoader;");
    activity_class_loader_ =
            env->CallObjectMethod(activity, get_class_loader);

    jclass class_loader = env->FindClass("java/lang/ClassLoader");

    find_class_ = env->GetMethodID(
        class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
}
Helper::~Helper() {
    for(auto& o: objs_)
        env_->DeleteLocalRef(o);
}

jclass Helper::FindClass(const char* class_name) const {
    jclass jni_class = env_->FindClass(class_name);

    if (jni_class == NULL) {
        // FindClass would have thrown.
        env_->ExceptionClear();
        jstring class_jname = env_->NewStringUTF(class_name);
        jni_class =
                (jclass)(env_->CallObjectMethod(activity_class_loader_, find_class_, class_jname));
        env_->DeleteLocalRef(class_jname);
    }
    return jni_class;
}

Helper::ClassAndObject Helper::NewObjectV(const char * cclz, const char* ctorSig, va_list argptr) {
    jclass clz = FindClass(cclz);
    jmethodID constructor = env_->GetMethodID(clz, "<init>", ctorSig);
    jobject o = env_->NewObjectV(clz, constructor, argptr);
    objs_.push_back(o);
    return {clz, o};
}
Helper::ClassAndObject Helper::NewObject(const char * cclz, const char* ctorSig, ...) {
    va_list argptr;
    va_start(argptr, ctorSig);
    auto o = NewObjectV(cclz, ctorSig, argptr);
    va_end(argptr);
    return o;
}
jobject Helper::CallObjectMethod(const ClassAndObject& obj, const char* name, const char* sig,
                                    ...) {
    jmethodID mid = env_->GetMethodID(obj.clz, name, sig);
    va_list argptr;
    va_start(argptr, sig);
    jobject o = env_->CallObjectMethodV(obj.obj, mid, argptr);
    va_end(argptr);
    objs_.push_back(o);
    return o;
}
jobject Helper::CallStaticObjectMethod(const ClassAndObject& obj, const char* name,
                                          const char* sig,
                                          ...) {
    jmethodID mid = env_->GetStaticMethodID(obj.clz, name, sig);
    va_list argptr;
    va_start(argptr, sig);
    jobject o = env_->CallStaticObjectMethodV(obj.clz, mid, argptr);
    va_end(argptr);
    objs_.push_back(o);
    return o;
}
String Helper::CallStringMethod(const ClassAndObject& obj, const char* name,
                                        const char* sig, ...) {
    jmethodID mid = env_->GetMethodID(obj.clz, name, sig);
    va_list argptr;
    va_start(argptr, sig);
    jobject o = env_->CallObjectMethodV(obj.obj, mid, argptr);
    va_end(argptr);
    String s(env_, (jstring)o);
    return s;
    }
Helper::ClassAndObject Helper::Cast(jobject o, const std::string& clz) const {
    if(clz.empty())
        return {env_->GetObjectClass(o), o};
    else
        return {FindClass(clz.c_str()), o};
}
void Helper::CallVoidMethod(const ClassAndObject& obj, const char* name, const char* sig, ...) {
    jmethodID mid = env_->GetMethodID(obj.clz, name, sig);
    va_list argptr;
    va_start(argptr, sig);
    env_->CallVoidMethodV(obj.obj, mid, argptr);
    va_end(argptr);
}
int Helper::CallIntMethod(const ClassAndObject& obj, const char* name, const char* sig, ...) {
    jmethodID mid = env_->GetMethodID(obj.clz, name, sig);
    va_list argptr;
    va_start(argptr, sig);
    int r = env_->CallIntMethodV(obj.obj, mid, argptr);
    va_end(argptr);
        return r;
}
String Helper::NewString(const std::string& s) const {
    auto js = env_->NewStringUTF(s.c_str());
    return String(env_, js);
}
std::string Helper::GetExceptionMessage() {
    std::string msg;
    jthrowable exception = env_->ExceptionOccurred();
    env_->ExceptionClear();
    jmethodID toString = env_->GetMethodID(FindClass("java/lang/Object"),
                                           "toString", "()Ljava/lang/String;");
    jstring s = (jstring)env_->CallObjectMethod(exception, toString);
    const char* utf = env_->GetStringUTFChars(s, nullptr);
    msg = utf;
    env_->ReleaseStringUTFChars(s, utf);
    env_->DeleteLocalRef(s);
    return msg;
}
bool Helper::CheckForException(std::string& msg) {
    if(env_->ExceptionCheck()) {
        msg = GetExceptionMessage();
        return true;
    }
    return false;
}
Helper::ClassAndObject Helper::GetObjectField(const ClassAndObject& o, const char* field_name,
                                         const char* sig) {
    jfieldID fid = env_->GetFieldID(o.clz, field_name, sig);
    if(!RawExceptionCheck()) {
        auto out = env_->GetObjectField(o.obj, fid);
        return {nullptr, out};
    } else {
        return {nullptr, nullptr};
    }
}
int Helper::GetIntField(const ClassAndObject& o, const char* field_name) {
    jfieldID fid = env_->GetFieldID(o.clz, field_name, "I");
    if(!RawExceptionCheck())
        return env_->GetIntField(o.obj, fid);
    else
        return BAD_FIELD;
}
std::vector<char> Helper::GetByteArrayBytes(jbyteArray jbs) {
    jboolean isCopy;
    jbyte* bs = env_->GetByteArrayElements(jbs, &isCopy);
    std::vector<char> ret(bs, bs + env_->GetArrayLength(jbs));
    if (isCopy) {
        env_->ReleaseByteArrayElements(jbs, bs, JNI_ABORT);
    }
    return ret;
}

} // namespace jni

} // namespace tuningfork
