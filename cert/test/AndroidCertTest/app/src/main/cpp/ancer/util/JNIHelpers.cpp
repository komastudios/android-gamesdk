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

#include "JNIHelpers.hpp"

#include <forward_list>

#include "Error.hpp"
#include "Log.hpp"

using namespace ancer;

//==============================================================================

namespace {
Log::Tag TAG{"ancer::util::jni"};

JavaVM *_java_vm;

jobject _activity_weak_global_ref;
}

//==============================================================================

namespace {
class LocalJNIEnvImpl final : public jni::LocalJNIEnv {
 public:
  LocalJNIEnvImpl() {
    auto attachment_status = _java_vm->GetEnv((void **) &_env, JNI_VERSION_1_6);
    if (attachment_status==JNI_EDETACHED) {
      if (_java_vm->AttachCurrentThread(&_env, nullptr)!=0) {
        ::FatalError(TAG, "LocalJNIEnv - Failed to attach current thread");
      }
      _detach_on_destroy = true;
    }
    _detach_on_destroy = false;
  }

  ~LocalJNIEnvImpl() {
    for (const auto &local_ref : _local_refs) {
      _env->DeleteLocalRef(local_ref);
    }
    if (_detach_on_destroy)
      _java_vm->DetachCurrentThread();
  }

  jint GetVersion() {
    return _env->GetVersion();
  }

  jclass DefineClass(const char *name, jobject loader, const jbyte *buf,
                     jsize bufLen) {
    return _bind_local_ref_to_env(_env->DefineClass(name, loader, buf, bufLen));
  }

  jclass FindClass(const char *name) {
    return _bind_local_ref_to_env(_env->FindClass(name));
  }

  jmethodID FromReflectedMethod(jobject method) {
    return _env->FromReflectedMethod(method);
  }

  jfieldID FromReflectedField(jobject field) {
    return _env->FromReflectedField(field);
  }

  jobject ToReflectedMethod(jclass cls, jmethodID methodID, jboolean isStatic) {
    return _bind_local_ref_to_env(_env->ToReflectedMethod(cls, methodID,
                                                          isStatic));
  }

  jclass GetSuperclass(jclass clazz) {
    return _bind_local_ref_to_env(_env->GetSuperclass(clazz));
  }

  jboolean IsAssignableFrom(jclass clazz1, jclass clazz2) {
    return _env->IsAssignableFrom(clazz1, clazz2);
  }

  jobject ToReflectedField(jclass cls, jfieldID fieldID, jboolean isStatic) {
    return _bind_local_ref_to_env(_env->ToReflectedField(cls, fieldID,
                                                         isStatic));
  }

  jint Throw(jthrowable obj) {
    return _env->Throw(obj);
  }

  jint ThrowNew(jclass clazz, const char *message) {
    return _env->ThrowNew(clazz, message);
  }

  jthrowable ExceptionOccurred() {
    return _bind_local_ref_to_env(_env->ExceptionOccurred());
  }

  void ExceptionDescribe() {
    _env->ExceptionDescribe();
  }

  void ExceptionClear() {
    _env->ExceptionClear();
  }

  void FatalError(const char *msg) {
    _env->FatalError(msg);
  }

  jint PushLocalFrame(jint capacity) {
    return _env->PushLocalFrame(capacity);
  }

  jobject PopLocalFrame(jobject result) {
    return _bind_local_ref_to_env(_env->PopLocalFrame(result));
  }

  jobject NewGlobalRef(jobject obj) {
    return _env->NewGlobalRef(obj);
  }

  void DeleteGlobalRef(jobject globalRef) {
    _env->DeleteGlobalRef(globalRef);
  }

  jboolean IsSameObject(jobject ref1, jobject ref2) {
    return _env->IsSameObject(ref1, ref2);
  }

  jobject NewLocalRef(jobject ref) {
    return _bind_local_ref_to_env(_env->NewLocalRef(ref));
  }

  jint EnsureLocalCapacity(jint capacity) {
    return _env->EnsureLocalCapacity(capacity);
  }

  jobject AllocObject(jclass clazz) {
    return _bind_local_ref_to_env(_env->AllocObject(clazz));
  }

  jobject NewObject(jclass clazz, jmethodID methodID, ...) {
    va_list args;
    va_start(args, methodID);
    jobject result = _env->NewObjectV(clazz, methodID, args);
    va_end(args);
    return _bind_local_ref_to_env(result);
  }

  jobject NewObjectV(jclass clazz, jmethodID methodID, va_list args) {
    return _bind_local_ref_to_env(_env->NewObjectV(clazz, methodID, args));
  }

  jobject NewObjectA(jclass clazz, jmethodID methodID, const jvalue *args) {
    return _bind_local_ref_to_env(_env->NewObjectA(clazz, methodID, args));
  }

  jclass GetObjectClass(jobject obj) {
    return _bind_local_ref_to_env(_env->GetObjectClass(obj));
  }

  jboolean IsInstanceOf(jobject obj, jclass clazz) {
    return _env->IsInstanceOf(obj, clazz);
  }

  jmethodID GetMethodID(jclass clazz, const char *name, const char *sig) {
    return _env->GetMethodID(clazz, name, sig);
  }

  jobject CallObjectMethod(jobject obj, jmethodID methodID, ...) {
    jobject result;
    va_list args;
    va_start(args, methodID);
    result = _env->CallObjectMethodV(obj, methodID, args);
    va_end(args);
    return _bind_local_ref_to_env(result);
  }
  jobject CallObjectMethodV(jobject obj, jmethodID methodID, va_list args) {
    return _bind_local_ref_to_env(_env->CallObjectMethodV(obj, methodID, args));
  }
  jobject CallObjectMethodA(jobject obj, jmethodID methodID,
                            const jvalue *args) {
    return _bind_local_ref_to_env(_env->CallObjectMethodA(obj, methodID, args));
  }

#define INVOKE_TYPE_METHOD(_jtype, _jname)                                  \
    _jtype Call##_jname##Method(jobject obj, jmethodID methodID, ...)       \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = _env->Call##_jname##MethodV(obj, methodID,                 \
                    args);                                                  \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define INVOKE_TYPE_METHODV(_jtype, _jname)                                 \
    _jtype Call##_jname##MethodV(jobject obj, jmethodID methodID,           \
        va_list args) {                                                     \
      return _env->Call##_jname##MethodV(obj, methodID,  args);             \
    }
#define INVOKE_TYPE_METHODA(_jtype, _jname)                                 \
    _jtype Call##_jname##MethodA(jobject obj, jmethodID methodID,           \
                                 const jvalue* args) {                      \
      return _env->Call##_jname##MethodA(obj, methodID, args);              \
    }

#define INVOKE_TYPE(_jtype, _jname)                                         \
    INVOKE_TYPE_METHOD(_jtype, _jname)                                      \
    INVOKE_TYPE_METHODV(_jtype, _jname)                                     \
    INVOKE_TYPE_METHODA(_jtype, _jname)

  INVOKE_TYPE(jboolean, Boolean)
  INVOKE_TYPE(jbyte, Byte)
  INVOKE_TYPE(jchar, Char)
  INVOKE_TYPE(jshort, Short)
  INVOKE_TYPE(jint, Int)
  INVOKE_TYPE(jlong, Long)
  INVOKE_TYPE(jfloat, Float)
  INVOKE_TYPE(jdouble, Double)

  void CallVoidMethod(jobject obj, jmethodID methodID, ...) {
    va_list args;
    va_start(args, methodID);
    _env->CallVoidMethodV(obj, methodID, args);
    va_end(args);
  }
  void CallVoidMethodV(jobject obj, jmethodID methodID, va_list args) {
    _env->CallVoidMethodV(obj, methodID, args);
  }
  void CallVoidMethodA(jobject obj, jmethodID methodID, const jvalue *args) {
    _env->CallVoidMethodA(obj, methodID, args);
  }

#define INVOKE_NONVIRT_TYPE_METHOD(_jtype, _jname)                          \
    _jtype CallNonvirtual##_jname##Method(jobject obj, jclass clazz,        \
        jmethodID methodID, ...)                                            \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = _env->CallNonvirtual##_jname##MethodV(obj,                 \
                    clazz, methodID, args);                                 \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define INVOKE_NONVIRT_TYPE_METHODV(_jtype, _jname)                         \
    _jtype CallNonvirtual##_jname##MethodV(jobject obj, jclass clazz,       \
        jmethodID methodID, va_list args)                                   \
    { return _env->CallNonvirtual##_jname##MethodV(obj, clazz,              \
        methodID, args); }
#define INVOKE_NONVIRT_TYPE_METHODA(_jtype, _jname)                         \
    _jtype CallNonvirtual##_jname##MethodA(jobject obj, jclass clazz,       \
        jmethodID methodID, const jvalue* args)                             \
    { return _env->CallNonvirtual##_jname##MethodA(obj, clazz,              \
        methodID, args); }

  jobject CallNonvirtualObjectMethod(jobject obj, jclass clazz,
                                     jmethodID methodID, ...) {
    jobject result;
    va_list args;
    va_start(args, methodID);
    result = _env->CallNonvirtualObjectMethodV(obj, clazz, methodID, args);
    va_end(args);
    return _bind_local_ref_to_env(result);
  }
  jobject CallNonvirtualObjectMethodV(jobject obj, jclass clazz,
                                      jmethodID methodID, va_list args) {
    return _bind_local_ref_to_env(
        _env->CallNonvirtualObjectMethodV(obj, clazz, methodID, args));
  }
  jobject CallNonvirtualObjectMethodA(jobject obj, jclass clazz,
                                      jmethodID methodID, const jvalue *args) {
    return _bind_local_ref_to_env(
        _env->CallNonvirtualObjectMethodA(obj, clazz, methodID, args));
  }

#define INVOKE_NONVIRT_TYPE(_jtype, _jname)                                 \
    INVOKE_NONVIRT_TYPE_METHOD(_jtype, _jname)                              \
    INVOKE_NONVIRT_TYPE_METHODV(_jtype, _jname)                             \
    INVOKE_NONVIRT_TYPE_METHODA(_jtype, _jname)

  INVOKE_NONVIRT_TYPE(jboolean, Boolean)
  INVOKE_NONVIRT_TYPE(jbyte, Byte)
  INVOKE_NONVIRT_TYPE(jchar, Char)
  INVOKE_NONVIRT_TYPE(jshort, Short)
  INVOKE_NONVIRT_TYPE(jint, Int)
  INVOKE_NONVIRT_TYPE(jlong, Long)
  INVOKE_NONVIRT_TYPE(jfloat, Float)
  INVOKE_NONVIRT_TYPE(jdouble, Double)

  void CallNonvirtualVoidMethod(jobject obj, jclass clazz,
                                jmethodID methodID, ...) {
    va_list args;
    va_start(args, methodID);
    _env->CallNonvirtualVoidMethodV(obj, clazz, methodID, args);
    va_end(args);
  }
  void CallNonvirtualVoidMethodV(jobject obj, jclass clazz,
                                 jmethodID methodID, va_list args) {
    _env->CallNonvirtualVoidMethodV(obj, clazz, methodID, args);
  }
  void CallNonvirtualVoidMethodA(jobject obj, jclass clazz,
                                 jmethodID methodID, const jvalue *args) {
    _env->CallNonvirtualVoidMethodA(obj, clazz, methodID, args);
  }

  jfieldID GetFieldID(jclass clazz, const char *name, const char *sig) {
    return _env->GetFieldID(clazz, name, sig);
  }

  jobject GetObjectField(jobject obj, jfieldID fieldID) {
    return _bind_local_ref_to_env(_env->GetObjectField(obj, fieldID));
  }
  jboolean GetBooleanField(jobject obj, jfieldID fieldID) {
    return _env->GetBooleanField(obj, fieldID);
  }
  jbyte GetByteField(jobject obj, jfieldID fieldID) {
    return _env->GetByteField(obj, fieldID);
  }
  jchar GetCharField(jobject obj, jfieldID fieldID) {
    return _env->GetCharField(obj, fieldID);
  }
  jshort GetShortField(jobject obj, jfieldID fieldID) {
    return _env->GetShortField(obj, fieldID);
  }
  jint GetIntField(jobject obj, jfieldID fieldID) {
    return _env->GetIntField(obj, fieldID);
  }
  jlong GetLongField(jobject obj, jfieldID fieldID) {
    return _env->GetLongField(obj, fieldID);
  }
  jfloat GetFloatField(jobject obj, jfieldID fieldID) {
    return _env->GetFloatField(obj, fieldID);
  }
  jdouble GetDoubleField(jobject obj, jfieldID fieldID) {
    return _env->GetDoubleField(obj, fieldID);
  }

  void SetObjectField(jobject obj, jfieldID fieldID, jobject value) {
    _env->SetObjectField(obj, fieldID, value);
  }
  void SetBooleanField(jobject obj, jfieldID fieldID, jboolean value) {
    _env->SetBooleanField(obj, fieldID, value);
  }
  void SetByteField(jobject obj, jfieldID fieldID, jbyte value) {
    _env->SetByteField(obj, fieldID, value);
  }
  void SetCharField(jobject obj, jfieldID fieldID, jchar value) {
    _env->SetCharField(obj, fieldID, value);
  }
  void SetShortField(jobject obj, jfieldID fieldID, jshort value) {
    _env->SetShortField(obj, fieldID, value);
  }
  void SetIntField(jobject obj, jfieldID fieldID, jint value) {
    _env->SetIntField(obj, fieldID, value);
  }
  void SetLongField(jobject obj, jfieldID fieldID, jlong value) {
    _env->SetLongField(obj, fieldID, value);
  }
  void SetFloatField(jobject obj, jfieldID fieldID, jfloat value) {
    _env->SetFloatField(obj, fieldID, value);
  }
  void SetDoubleField(jobject obj, jfieldID fieldID, jdouble value) {
    _env->SetDoubleField(obj, fieldID, value);
  }

  jmethodID GetStaticMethodID(jclass clazz, const char *name, const char *sig) {
    return _env->GetStaticMethodID(clazz, name, sig);
  }

  jobject CallStaticObjectMethod(jclass clazz, jmethodID methodID, ...) {
    jobject result;
    va_list args;
    va_start(args, methodID);
    result = _env->CallStaticObjectMethodV(clazz, methodID, args);
    va_end(args);
    return _bind_local_ref_to_env(result);
  }
  jobject CallStaticObjectMethodV(jclass clazz, jmethodID methodID,
                                  va_list args) {
    return _bind_local_ref_to_env(
        _env->CallStaticObjectMethodV(clazz, methodID, args));
  }
  jobject CallStaticObjectMethodA(jclass clazz, jmethodID methodID,
                                  const jvalue *args) {
    return _bind_local_ref_to_env(
        _env->CallStaticObjectMethodA(clazz, methodID, args));
  }

#define INVOKE_STATIC_TYPE_METHOD(_jtype, _jname)                           \
    _jtype CallStatic##_jname##Method(jclass clazz, jmethodID methodID,     \
        ...)                                                                \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = _env->CallStatic##_jname##MethodV(clazz,                   \
                    methodID, args);                                        \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define INVOKE_STATIC_TYPE_METHODV(_jtype, _jname)                          \
    _jtype CallStatic##_jname##MethodV(jclass clazz, jmethodID methodID,    \
        va_list args)                                                       \
    { return _env->CallStatic##_jname##MethodV(clazz, methodID,             \
        args); }
#define INVOKE_STATIC_TYPE_METHODA(_jtype, _jname)                          \
    _jtype CallStatic##_jname##MethodA(jclass clazz, jmethodID methodID,    \
                                       const jvalue* args)                  \
    { return _env->CallStatic##_jname##MethodA(clazz, methodID,             \
        args); }

#define INVOKE_STATIC_TYPE(_jtype, _jname)                                  \
    INVOKE_STATIC_TYPE_METHOD(_jtype, _jname)                               \
    INVOKE_STATIC_TYPE_METHODV(_jtype, _jname)                              \
    INVOKE_STATIC_TYPE_METHODA(_jtype, _jname)

  INVOKE_STATIC_TYPE(jboolean, Boolean)
  INVOKE_STATIC_TYPE(jbyte, Byte)
  INVOKE_STATIC_TYPE(jchar, Char)
  INVOKE_STATIC_TYPE(jshort, Short)
  INVOKE_STATIC_TYPE(jint, Int)
  INVOKE_STATIC_TYPE(jlong, Long)
  INVOKE_STATIC_TYPE(jfloat, Float)
  INVOKE_STATIC_TYPE(jdouble, Double)

  void CallStaticVoidMethod(jclass clazz, jmethodID methodID, ...) {
    va_list args;
    va_start(args, methodID);
    _env->CallStaticVoidMethodV(clazz, methodID, args);
    va_end(args);
  }
  void CallStaticVoidMethodV(jclass clazz, jmethodID methodID, va_list args) {
    _env->CallStaticVoidMethodV(clazz, methodID, args);
  }
  void CallStaticVoidMethodA(jclass clazz, jmethodID methodID,
                             const jvalue *args) {
    _env->CallStaticVoidMethodA(clazz, methodID, args);
  }

  jfieldID GetStaticFieldID(jclass clazz, const char *name, const char *sig) {
    return _env->GetStaticFieldID(clazz, name, sig);
  }

  jobject GetStaticObjectField(jclass clazz, jfieldID fieldID) {
    return _bind_local_ref_to_env(_env->GetStaticObjectField(clazz, fieldID));
  }
  jboolean GetStaticBooleanField(jclass clazz, jfieldID fieldID) {
    return _env->GetStaticBooleanField(clazz, fieldID);
  }
  jbyte GetStaticByteField(jclass clazz, jfieldID fieldID) {
    return _env->GetStaticByteField(clazz, fieldID);
  }
  jchar GetStaticCharField(jclass clazz, jfieldID fieldID) {
    return _env->GetStaticCharField(clazz, fieldID);
  }
  jshort GetStaticShortField(jclass clazz, jfieldID fieldID) {
    return _env->GetStaticShortField(clazz, fieldID);
  }
  jint GetStaticIntField(jclass clazz, jfieldID fieldID) {
    return _env->GetStaticIntField(clazz, fieldID);
  }
  jlong GetStaticLongField(jclass clazz, jfieldID fieldID) {
    return _env->GetStaticLongField(clazz, fieldID);
  }
  jfloat GetStaticFloatField(jclass clazz, jfieldID fieldID) {
    return _env->GetStaticFloatField(clazz, fieldID);
  }
  jdouble GetStaticDoubleField(jclass clazz, jfieldID fieldID) {
    return _env->GetStaticDoubleField(clazz, fieldID);
  }

  void SetStaticObjectField(jclass clazz, jfieldID fieldID, jobject value) {
    _env->SetStaticObjectField(clazz, fieldID, value);
  }
  void SetStaticBooleanField(jclass clazz, jfieldID fieldID, jboolean value) {
    _env->SetStaticBooleanField(clazz, fieldID, value);
  }
  void SetStaticByteField(jclass clazz, jfieldID fieldID, jbyte value) {
    _env->SetStaticByteField(clazz, fieldID, value);
  }
  void SetStaticCharField(jclass clazz, jfieldID fieldID, jchar value) {
    _env->SetStaticCharField(clazz, fieldID, value);
  }
  void SetStaticShortField(jclass clazz, jfieldID fieldID, jshort value) {
    _env->SetStaticShortField(clazz, fieldID, value);
  }
  void SetStaticIntField(jclass clazz, jfieldID fieldID, jint value) {
    _env->SetStaticIntField(clazz, fieldID, value);
  }
  void SetStaticLongField(jclass clazz, jfieldID fieldID, jlong value) {
    _env->SetStaticLongField(clazz, fieldID, value);
  }
  void SetStaticFloatField(jclass clazz, jfieldID fieldID, jfloat value) {
    _env->SetStaticFloatField(clazz, fieldID, value);
  }
  void SetStaticDoubleField(jclass clazz, jfieldID fieldID, jdouble value) {
    _env->SetStaticDoubleField(clazz, fieldID, value);
  }

  jstring NewString(const jchar *unicodeChars, jsize len) {
    return _bind_local_ref_to_env(_env->NewString(unicodeChars, len));
  }

  jsize GetStringLength(jstring string) {
    return _env->GetStringLength(string);
  }

  const jchar *GetStringChars(jstring string, jboolean *isCopy) {
    return _env->GetStringChars(string, isCopy);
  }

  void ReleaseStringChars(jstring string, const jchar *chars) {
    _env->ReleaseStringChars(string, chars);
  }

  jstring NewStringUTF(const char *bytes) {
    return _bind_local_ref_to_env(_env->NewStringUTF(bytes));
  }

  jsize GetStringUTFLength(jstring string) {
    return _env->GetStringUTFLength(string);
  }

  const char *GetStringUTFChars(jstring string, jboolean *isCopy) {
    return _env->GetStringUTFChars(string, isCopy);
  }

  void ReleaseStringUTFChars(jstring string, const char *utf) {
    _env->ReleaseStringUTFChars(string, utf);
  }

  jsize GetArrayLength(jarray array) {
    return _env->GetArrayLength(array);
  }

  jobjectArray NewObjectArray(jsize length, jclass elementClass,
                              jobject initialElement) {
    return _bind_local_ref_to_env(
        _env->NewObjectArray(length, elementClass, initialElement));
  }

  jobject GetObjectArrayElement(jobjectArray array, jsize index) {
    return _bind_local_ref_to_env(_env->GetObjectArrayElement(array, index));
  }

  void SetObjectArrayElement(jobjectArray array, jsize index, jobject value) {
    _env->SetObjectArrayElement(array, index, value);
  }

  jbooleanArray NewBooleanArray(jsize length) {
    return _bind_local_ref_to_env(_env->NewBooleanArray(length));
  }
  jbyteArray NewByteArray(jsize length) {
    return _bind_local_ref_to_env(_env->NewByteArray(length));
  }
  jcharArray NewCharArray(jsize length) {
    return _bind_local_ref_to_env(_env->NewCharArray(length));
  }
  jshortArray NewShortArray(jsize length) {
    return _bind_local_ref_to_env(_env->NewShortArray(length));
  }
  jintArray NewIntArray(jsize length) {
    return _bind_local_ref_to_env(_env->NewIntArray(length));
  }
  jlongArray NewLongArray(jsize length) {
    return _bind_local_ref_to_env(_env->NewLongArray(length));
  }
  jfloatArray NewFloatArray(jsize length) {
    return _bind_local_ref_to_env(_env->NewFloatArray(length));
  }
  jdoubleArray NewDoubleArray(jsize length) {
    return _bind_local_ref_to_env(_env->NewDoubleArray(length));
  }

  jboolean *GetBooleanArrayElements(jbooleanArray array, jboolean *isCopy) {
    return _env->GetBooleanArrayElements(array, isCopy);
  }
  jbyte *GetByteArrayElements(jbyteArray array, jboolean *isCopy) {
    return _env->GetByteArrayElements(array, isCopy);
  }
  jchar *GetCharArrayElements(jcharArray array, jboolean *isCopy) {
    return _env->GetCharArrayElements(array, isCopy);
  }
  jshort *GetShortArrayElements(jshortArray array, jboolean *isCopy) {
    return _env->GetShortArrayElements(array, isCopy);
  }
  jint *GetIntArrayElements(jintArray array, jboolean *isCopy) {
    return _env->GetIntArrayElements(array, isCopy);
  }
  jlong *GetLongArrayElements(jlongArray array, jboolean *isCopy) {
    return _env->GetLongArrayElements(array, isCopy);
  }
  jfloat *GetFloatArrayElements(jfloatArray array, jboolean *isCopy) {
    return _env->GetFloatArrayElements(array, isCopy);
  }
  jdouble *GetDoubleArrayElements(jdoubleArray array, jboolean *isCopy) {
    return _env->GetDoubleArrayElements(array, isCopy);
  }

  void ReleaseBooleanArrayElements(jbooleanArray array, jboolean *elems,
                                   jint mode) {
    _env->ReleaseBooleanArrayElements(array, elems, mode);
  }
  void ReleaseByteArrayElements(jbyteArray array, jbyte *elems,
                                jint mode) {
    _env->ReleaseByteArrayElements(array, elems, mode);
  }
  void ReleaseCharArrayElements(jcharArray array, jchar *elems,
                                jint mode) {
    _env->ReleaseCharArrayElements(array, elems, mode);
  }
  void ReleaseShortArrayElements(jshortArray array, jshort *elems,
                                 jint mode) {
    _env->ReleaseShortArrayElements(array, elems, mode);
  }
  void ReleaseIntArrayElements(jintArray array, jint *elems,
                               jint mode) {
    _env->ReleaseIntArrayElements(array, elems, mode);
  }
  void ReleaseLongArrayElements(jlongArray array, jlong *elems,
                                jint mode) {
    _env->ReleaseLongArrayElements(array, elems, mode);
  }
  void ReleaseFloatArrayElements(jfloatArray array, jfloat *elems,
                                 jint mode) {
    _env->ReleaseFloatArrayElements(array, elems, mode);
  }
  void ReleaseDoubleArrayElements(jdoubleArray array, jdouble *elems,
                                  jint mode) {
    _env->ReleaseDoubleArrayElements(array, elems, mode);
  }

  void GetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len,
                             jboolean *buf) {
    _env->GetBooleanArrayRegion(array, start, len, buf);
  }
  void GetByteArrayRegion(jbyteArray array, jsize start, jsize len,
                          jbyte *buf) {
    _env->GetByteArrayRegion(array, start, len, buf);
  }
  void GetCharArrayRegion(jcharArray array, jsize start, jsize len,
                          jchar *buf) {
    _env->GetCharArrayRegion(array, start, len, buf);
  }
  void GetShortArrayRegion(jshortArray array, jsize start, jsize len,
                           jshort *buf) {
    _env->GetShortArrayRegion(array, start, len, buf);
  }
  void GetIntArrayRegion(jintArray array, jsize start, jsize len,
                         jint *buf) {
    _env->GetIntArrayRegion(array, start, len, buf);
  }
  void GetLongArrayRegion(jlongArray array, jsize start, jsize len,
                          jlong *buf) {
    _env->GetLongArrayRegion(array, start, len, buf);
  }
  void GetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
                           jfloat *buf) {
    _env->GetFloatArrayRegion(array, start, len, buf);
  }
  void GetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
                            jdouble *buf) {
    _env->GetDoubleArrayRegion(array, start, len, buf);
  }

  void SetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len,
                             const jboolean *buf) {
    _env->SetBooleanArrayRegion(array, start, len, buf);
  }
  void SetByteArrayRegion(jbyteArray array, jsize start, jsize len,
                          const jbyte *buf) {
    _env->SetByteArrayRegion(array, start, len, buf);
  }
  void SetCharArrayRegion(jcharArray array, jsize start, jsize len,
                          const jchar *buf) {
    _env->SetCharArrayRegion(array, start, len, buf);
  }
  void SetShortArrayRegion(jshortArray array, jsize start, jsize len,
                           const jshort *buf) {
    _env->SetShortArrayRegion(array, start, len, buf);
  }
  void SetIntArrayRegion(jintArray array, jsize start, jsize len,
                         const jint *buf) {
    _env->SetIntArrayRegion(array, start, len, buf);
  }
  void SetLongArrayRegion(jlongArray array, jsize start, jsize len,
                          const jlong *buf) {
    _env->SetLongArrayRegion(array, start, len, buf);
  }
  void SetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
                           const jfloat *buf) {
    _env->SetFloatArrayRegion(array, start, len, buf);
  }
  void SetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
                            const jdouble *buf) {
    _env->SetDoubleArrayRegion(array, start, len, buf);
  }

  jint RegisterNatives(jclass clazz, const JNINativeMethod *methods,
                       jint nMethods) {
    return _env->RegisterNatives(clazz, methods, nMethods);
  }

  jint UnregisterNatives(jclass clazz) {
    return _env->UnregisterNatives(clazz);
  }

  jint MonitorEnter(jobject obj) {
    return _env->MonitorEnter(obj);
  }

  jint MonitorExit(jobject obj) {
    return _env->MonitorExit(obj);
  }

  jint GetJavaVM(JavaVM **vm) {
    return _env->GetJavaVM(vm);
  }

  void GetStringRegion(jstring str, jsize start, jsize len, jchar *buf) {
    _env->GetStringRegion(str, start, len, buf);
  }

  void GetStringUTFRegion(jstring str, jsize start, jsize len, char *buf) {
    return _env->GetStringUTFRegion(str, start, len, buf);
  }

  void *GetPrimitiveArrayCritical(jarray array, jboolean *isCopy) {
    return _env->GetPrimitiveArrayCritical(array, isCopy);
  }

  void ReleasePrimitiveArrayCritical(jarray array, void *carray, jint mode) {
    _env->ReleasePrimitiveArrayCritical(array, carray, mode);
  }

  const jchar *GetStringCritical(jstring string, jboolean *isCopy) {
    return _env->GetStringCritical(string, isCopy);
  }

  void ReleaseStringCritical(jstring string, const jchar *carray) {
    _env->ReleaseStringCritical(string, carray);
  }

  jweak NewWeakGlobalRef(jobject obj) {
    return _env->NewWeakGlobalRef(obj);
  }

  void DeleteWeakGlobalRef(jweak obj) {
    _env->DeleteWeakGlobalRef(obj);
  }

  jboolean ExceptionCheck() {
    return _env->ExceptionCheck();
  }

  jobject NewDirectByteBuffer(void *address, jlong capacity) {
    return _bind_local_ref_to_env(_env->NewDirectByteBuffer(address, capacity));
  }

  void *GetDirectBufferAddress(jobject buf) {
    return _env->GetDirectBufferAddress(buf);
  }

  jlong GetDirectBufferCapacity(jobject buf) {
    return _env->GetDirectBufferCapacity(buf);
  }

  jobjectRefType GetObjectRefType(jobject obj) {
    return _env->GetObjectRefType(obj);
  }

 private:
  template<typename JNI_TYPE>
  JNI_TYPE _bind_local_ref_to_env(const JNI_TYPE &jinstance) {
    _local_refs.push_front(jinstance);
    return jinstance;
  }

 private:
  JNIEnv *_env;

  bool _detach_on_destroy;

  std::forward_list<jobject> _local_refs;
};
} // anonymous namespace

//==============================================================================

void ancer::jni::SetJavaVM(JavaVM *vm) {
  _java_vm = vm;
}

void ancer::jni::SafeJNICall(JNIFunction &&func) {
  try {
    LocalJNIEnvImpl local_jni_env{};
    return func(&local_jni_env);
  } catch (const std::exception &e) {
    FatalError(TAG, e.what());
  }
}

//==============================================================================

void ancer::jni::Init(ancer::jni::LocalJNIEnv *env, const jobject activity) {
  if (_activity_weak_global_ref!=nullptr) {
    ancer::jni::Deinit(env);
  }
  _activity_weak_global_ref = env->NewWeakGlobalRef(activity);
}

jobject ancer::jni::GetActivityWeakGlobalRef() {
  return _activity_weak_global_ref;
}

void ancer::jni::Deinit(ancer::jni::LocalJNIEnv *env) {
  if (_activity_weak_global_ref!=nullptr) {
    env->DeleteWeakGlobalRef(_activity_weak_global_ref);
    _activity_weak_global_ref = nullptr;
  }
}

//==============================================================================

jclass ancer::jni::LoadClass(ancer::jni::LocalJNIEnv *env, jobject activity,
                             const char *class_name) {
  return (jclass) env->CallObjectMethod(
      env->CallObjectMethod(activity,
                            env->GetMethodID(env->GetObjectClass(activity),
                                             "getClassLoader", "()Ljava/lang/ClassLoader;")),
      env->GetMethodID(env->FindClass("java/lang/ClassLoader"),
                       "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;"),
      env->NewStringUTF(class_name)
  );
}

jobject ancer::jni::GetEnumField(jni::LocalJNIEnv *env, const char *class_name,
                                 const char *field_name) {
  const jclass cls = env->FindClass(class_name);
  if (cls==nullptr)
    return nullptr;

  const jfieldID field_id = env->GetStaticFieldID(cls, field_name,
                                                  (std::string{"L"} + class_name + ";").c_str());
  if (field_id==nullptr)
    return nullptr;

  return env->GetStaticObjectField(cls, field_id);
}
