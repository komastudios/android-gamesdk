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

#ifndef _JNI_HELPERS_HPP
#define _JNI_HELPERS_HPP

#include <functional>

#include <jni.h>

namespace ancer {
namespace jni {

void SetJavaVM(JavaVM *);

/**
 * Like JNI struct JNIEnv native environment to access the Java VM, but this one
 * releases the developer from the duty of deleting local references.
 * Based on the official JNI specification "Global and Local References" section
 * (https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/design.html)
 * "local references are valid for the duration of a native method call, and are
 * automatically freed after the native method returns". That's a problem when
 * our Android application is composed by long-running native operations meant
 * to be intensive in resource consumption. Waiting until such these native
 * operations finish could bust local resources. JNI has a facility function to
 * anticipate that deletion (DeleteLocalRef). But it's as easy to miss as it is
 * to "delete" memory allocated with "new".
 * Rather than expecting that developers will be accountable, this type replaces
 * the official JNIEnv. It offers the exact same API except that DeleteLocalRef
 * function. Any local reference obtained from an instance of this type is
 * released when such instance is destroyed.
 */
struct LocalJNIEnv {
  virtual jint GetVersion() = 0;

  virtual jclass DefineClass(const char *name, jobject loader, const jbyte *buf,
                             jsize bufLen) = 0;

  virtual jclass FindClass(const char *name) = 0;

  virtual jmethodID FromReflectedMethod(jobject method) = 0;

  virtual jfieldID FromReflectedField(jobject field) = 0;

  virtual jobject ToReflectedMethod(jclass cls, jmethodID methodID,
                                    jboolean isStatic) = 0;

  virtual jclass GetSuperclass(jclass clazz) = 0;

  virtual jboolean IsAssignableFrom(jclass clazz1, jclass clazz2) = 0;

  virtual jobject ToReflectedField(jclass cls, jfieldID fieldID,
                                   jboolean isStatic) = 0;

  virtual jint Throw(jthrowable obj) = 0;

  virtual jint ThrowNew(jclass clazz, const char *message) = 0;

  virtual jthrowable ExceptionOccurred() = 0;

  virtual void ExceptionDescribe() = 0;

  virtual void ExceptionClear() = 0;

  virtual void FatalError(const char *msg) = 0;

  virtual jint PushLocalFrame(jint capacity) = 0;

  virtual jobject PopLocalFrame(jobject result) = 0;

  virtual jobject NewGlobalRef(jobject obj) = 0;

  virtual void DeleteGlobalRef(jobject globalRef) = 0;

  // No DeleteLocalRef(jobject localRef)!

  virtual jboolean IsSameObject(jobject ref1, jobject ref2) = 0;

  virtual jobject NewLocalRef(jobject ref) = 0;

  virtual jint EnsureLocalCapacity(jint capacity) = 0;

  virtual jobject AllocObject(jclass clazz) = 0;

  virtual jobject NewObject(jclass clazz, jmethodID methodID, ...) = 0;

  virtual jobject NewObjectV(jclass clazz, jmethodID methodID,
                             va_list args) = 0;

  virtual jobject NewObjectA(jclass clazz, jmethodID methodID,
                             const jvalue *args) = 0;

  virtual jclass GetObjectClass(jobject obj) = 0;

  virtual jboolean IsInstanceOf(jobject obj, jclass clazz) = 0;

  virtual jmethodID GetMethodID(jclass clazz, const char *name,
                                const char *sig) = 0;

#define INVOKE_TYPE_METHOD(_jtype, _jname)                                    \
    virtual _jtype Call##_jname##Method(jobject obj,                          \
        jmethodID methodID, ...) = 0;
#define INVOKE_TYPE_METHODV(_jtype, _jname)                                   \
    virtual _jtype Call##_jname##MethodV(jobject obj, jmethodID methodID,     \
        va_list args) = 0;
#define INVOKE_TYPE_METHODA(_jtype, _jname)                                   \
    virtual _jtype Call##_jname##MethodA(jobject obj, jmethodID methodID,     \
        const jvalue* args) = 0;

#define INVOKE_TYPE(_jtype, _jname)                                           \
    INVOKE_TYPE_METHOD(_jtype, _jname)                                        \
    INVOKE_TYPE_METHODV(_jtype, _jname)                                       \
    INVOKE_TYPE_METHODA(_jtype, _jname)

  INVOKE_TYPE(jobject, Object)
  INVOKE_TYPE(jboolean, Boolean)
  INVOKE_TYPE(jbyte, Byte)
  INVOKE_TYPE(jchar, Char)
  INVOKE_TYPE(jshort, Short)
  INVOKE_TYPE(jint, Int)
  INVOKE_TYPE(jlong, Long)
  INVOKE_TYPE(jfloat, Float)
  INVOKE_TYPE(jdouble, Double)

#undef INVOKE_TYPE
#undef INVOKE_TYPE_METHOD
#undef INVOKE_TYPE_METHODV
#undef INVOKE_TYPE_METHODA

  virtual void CallVoidMethod(jobject obj, jmethodID methodID, ...) = 0;
  virtual void CallVoidMethodV(jobject obj, jmethodID methodID,
                               va_list args) = 0;
  virtual void CallVoidMethodA(jobject obj, jmethodID methodID,
                               const jvalue *args) = 0;

#define INVOKE_NONVIRT_TYPE_METHOD(_jtype, _jname)                            \
    virtual _jtype CallNonvirtual##_jname##Method(jobject obj, jclass clazz,  \
        jmethodID methodID, ...) = 0;
#define INVOKE_NONVIRT_TYPE_METHODV(_jtype, _jname)                           \
    virtual _jtype CallNonvirtual##_jname##MethodV(jobject obj, jclass clazz, \
        jmethodID methodID, va_list args) = 0;
#define INVOKE_NONVIRT_TYPE_METHODA(_jtype, _jname)                           \
    virtual _jtype CallNonvirtual##_jname##MethodA(jobject obj, jclass clazz, \
        jmethodID methodID, const jvalue* args) = 0;

#define INVOKE_NONVIRT_TYPE(_jtype, _jname)                                   \
    INVOKE_NONVIRT_TYPE_METHOD(_jtype, _jname)                                \
    INVOKE_NONVIRT_TYPE_METHODV(_jtype, _jname)                               \
    INVOKE_NONVIRT_TYPE_METHODA(_jtype, _jname)

  INVOKE_NONVIRT_TYPE(jobject, Object)
  INVOKE_NONVIRT_TYPE(jboolean, Boolean)
  INVOKE_NONVIRT_TYPE(jbyte, Byte)
  INVOKE_NONVIRT_TYPE(jchar, Char)
  INVOKE_NONVIRT_TYPE(jshort, Short)
  INVOKE_NONVIRT_TYPE(jint, Int)
  INVOKE_NONVIRT_TYPE(jlong, Long)
  INVOKE_NONVIRT_TYPE(jfloat, Float)
  INVOKE_NONVIRT_TYPE(jdouble, Double)

#undef INVOKE_NONVIRT_TYPE
#undef INVOKE_NONVIRT_TYPE_METHOD
#undef INVOKE_NONVIRT_TYPE_METHODV
#undef INVOKE_NONVIRT_TYPE_METHODA

  virtual void CallNonvirtualVoidMethod(jobject obj, jclass clazz,
                                        jmethodID methodID, ...) = 0;
  virtual void CallNonvirtualVoidMethodV(jobject obj, jclass clazz,
                                         jmethodID methodID, va_list args) = 0;
  virtual void CallNonvirtualVoidMethodA(jobject obj, jclass clazz,
                                         jmethodID methodID,
                                         const jvalue *args) = 0;

  virtual jfieldID GetFieldID(jclass clazz, const char *name,
                              const char *sig) = 0;

  virtual jobject GetObjectField(jobject obj, jfieldID fieldID) = 0;
  virtual jboolean GetBooleanField(jobject obj, jfieldID fieldID) = 0;
  virtual jbyte GetByteField(jobject obj, jfieldID fieldID) = 0;
  virtual jchar GetCharField(jobject obj, jfieldID fieldID) = 0;
  virtual jshort GetShortField(jobject obj, jfieldID fieldID) = 0;
  virtual jint GetIntField(jobject obj, jfieldID fieldID) = 0;
  virtual jlong GetLongField(jobject obj, jfieldID fieldID) = 0;
  virtual jfloat GetFloatField(jobject obj, jfieldID fieldID) = 0;
  virtual jdouble GetDoubleField(jobject obj, jfieldID fieldID) = 0;

  virtual void SetObjectField(jobject obj, jfieldID fieldID, jobject value) = 0;
  virtual void SetBooleanField(jobject obj, jfieldID fieldID,
                               jboolean value) = 0;
  virtual void SetByteField(jobject obj, jfieldID fieldID, jbyte value) = 0;
  virtual void SetCharField(jobject obj, jfieldID fieldID, jchar value) = 0;
  virtual void SetShortField(jobject obj, jfieldID fieldID, jshort value) = 0;
  virtual void SetIntField(jobject obj, jfieldID fieldID, jint value) = 0;
  virtual void SetLongField(jobject obj, jfieldID fieldID, jlong value) = 0;
  virtual void SetFloatField(jobject obj, jfieldID fieldID, jfloat value) = 0;
  virtual void SetDoubleField(jobject obj, jfieldID fieldID, jdouble value) = 0;

  virtual jmethodID GetStaticMethodID(jclass clazz, const char *name,
                                      const char *sig) = 0;

#define INVOKE_STATIC_TYPE_METHOD(_jtype, _jname)                             \
    virtual _jtype CallStatic##_jname##Method(jclass clazz,                   \
        jmethodID methodID, ...) = 0;
#define INVOKE_STATIC_TYPE_METHODV(_jtype, _jname)                            \
    virtual _jtype CallStatic##_jname##MethodV(jclass clazz,                  \
        jmethodID methodID, va_list args) = 0;
#define INVOKE_STATIC_TYPE_METHODA(_jtype, _jname)                            \
    virtual _jtype CallStatic##_jname##MethodA(jclass clazz,                  \
        jmethodID methodID, const jvalue* args) = 0;

#define INVOKE_STATIC_TYPE(_jtype, _jname)                                    \
    INVOKE_STATIC_TYPE_METHOD(_jtype, _jname)                                 \
    INVOKE_STATIC_TYPE_METHODV(_jtype, _jname)                                \
    INVOKE_STATIC_TYPE_METHODA(_jtype, _jname)

  INVOKE_STATIC_TYPE(jobject, Object)
  INVOKE_STATIC_TYPE(jboolean, Boolean)
  INVOKE_STATIC_TYPE(jbyte, Byte)
  INVOKE_STATIC_TYPE(jchar, Char)
  INVOKE_STATIC_TYPE(jshort, Short)
  INVOKE_STATIC_TYPE(jint, Int)
  INVOKE_STATIC_TYPE(jlong, Long)
  INVOKE_STATIC_TYPE(jfloat, Float)
  INVOKE_STATIC_TYPE(jdouble, Double)

#undef INVOKE_STATIC_TYPE
#undef INVOKE_STATIC_TYPE_METHOD
#undef INVOKE_STATIC_TYPE_METHODV
#undef INVOKE_STATIC_TYPE_METHODA

  virtual void CallStaticVoidMethod(jclass clazz, jmethodID methodID, ...) = 0;
  virtual void CallStaticVoidMethodV(jclass clazz, jmethodID methodID,
                                     va_list args) = 0;
  virtual void CallStaticVoidMethodA(jclass clazz, jmethodID methodID,
                                     const jvalue *args) = 0;

  virtual jfieldID GetStaticFieldID(jclass clazz, const char *name,
                                    const char *sig) = 0;

  virtual jobject GetStaticObjectField(jclass clazz, jfieldID fieldID) = 0;
  virtual jboolean GetStaticBooleanField(jclass clazz, jfieldID fieldID) = 0;
  virtual jbyte GetStaticByteField(jclass clazz, jfieldID fieldID) = 0;
  virtual jchar GetStaticCharField(jclass clazz, jfieldID fieldID) = 0;
  virtual jshort GetStaticShortField(jclass clazz, jfieldID fieldID) = 0;
  virtual jint GetStaticIntField(jclass clazz, jfieldID fieldID) = 0;
  virtual jlong GetStaticLongField(jclass clazz, jfieldID fieldID) = 0;
  virtual jfloat GetStaticFloatField(jclass clazz, jfieldID fieldID) = 0;
  virtual jdouble GetStaticDoubleField(jclass clazz, jfieldID fieldID) = 0;

  virtual void SetStaticObjectField(jclass clazz, jfieldID fieldID,
                                    jobject value) = 0;
  virtual void SetStaticBooleanField(jclass clazz, jfieldID fieldID,
                                     jboolean value) = 0;
  virtual void SetStaticByteField(jclass clazz, jfieldID fieldID,
                                  jbyte value) = 0;
  virtual void SetStaticCharField(jclass clazz, jfieldID fieldID,
                                  jchar value) = 0;
  virtual void SetStaticShortField(jclass clazz, jfieldID fieldID,
                                   jshort value) = 0;
  virtual void SetStaticIntField(jclass clazz, jfieldID fieldID,
                                 jint value) = 0;
  virtual void SetStaticLongField(jclass clazz, jfieldID fieldID,
                                  jlong value) = 0;
  virtual void SetStaticFloatField(jclass clazz, jfieldID fieldID,
                                   jfloat value) = 0;
  virtual void SetStaticDoubleField(jclass clazz, jfieldID fieldID,
                                    jdouble value) = 0;

  virtual jstring NewString(const jchar *unicodeChars, jsize len) = 0;

  virtual jsize GetStringLength(jstring string) = 0;

  virtual const jchar *GetStringChars(jstring string, jboolean *isCopy) = 0;

  virtual void ReleaseStringChars(jstring string, const jchar *chars) = 0;

  virtual jstring NewStringUTF(const char *bytes) = 0;

  virtual jsize GetStringUTFLength(jstring string) = 0;

  virtual const char *GetStringUTFChars(jstring string, jboolean *isCopy) = 0;

  virtual void ReleaseStringUTFChars(jstring string, const char *utf) = 0;

  virtual jsize GetArrayLength(jarray array) = 0;

  virtual jobjectArray NewObjectArray(jsize length, jclass elementClass,
                                      jobject initialElement) = 0;

  virtual jobject GetObjectArrayElement(jobjectArray array, jsize index) = 0;

  virtual void SetObjectArrayElement(jobjectArray array, jsize index,
                                     jobject value) = 0;

  virtual jbooleanArray NewBooleanArray(jsize length) = 0;
  virtual jbyteArray NewByteArray(jsize length) = 0;
  virtual jcharArray NewCharArray(jsize length) = 0;
  virtual jshortArray NewShortArray(jsize length) = 0;
  virtual jintArray NewIntArray(jsize length) = 0;
  virtual jlongArray NewLongArray(jsize length) = 0;
  virtual jfloatArray NewFloatArray(jsize length) = 0;
  virtual jdoubleArray NewDoubleArray(jsize length) = 0;

  virtual jboolean *GetBooleanArrayElements(jbooleanArray array,
                                            jboolean *isCopy) = 0;
  virtual jbyte *GetByteArrayElements(jbyteArray array, jboolean *isCopy) = 0;
  virtual jchar *GetCharArrayElements(jcharArray array, jboolean *isCopy) = 0;
  virtual jshort *GetShortArrayElements(jshortArray array,
                                        jboolean *isCopy) = 0;
  virtual jint *GetIntArrayElements(jintArray array, jboolean *isCopy) = 0;
  virtual jlong *GetLongArrayElements(jlongArray array, jboolean *isCopy) = 0;
  virtual jfloat *GetFloatArrayElements(jfloatArray array,
                                        jboolean *isCopy) = 0;
  virtual jdouble *GetDoubleArrayElements(jdoubleArray array,
                                          jboolean *isCopy) = 0;

  virtual void ReleaseBooleanArrayElements(jbooleanArray array, jboolean *elems,
                                           jint mode) = 0;
  virtual void ReleaseByteArrayElements(jbyteArray array, jbyte *elems,
                                        jint mode) = 0;
  virtual void ReleaseCharArrayElements(jcharArray array, jchar *elems,
                                        jint mode) = 0;
  virtual void ReleaseShortArrayElements(jshortArray array, jshort *elems,
                                         jint mode) = 0;
  virtual void ReleaseIntArrayElements(jintArray array, jint *elems,
                                       jint mode) = 0;
  virtual void ReleaseLongArrayElements(jlongArray array, jlong *elems,
                                        jint mode) = 0;
  virtual void ReleaseFloatArrayElements(jfloatArray array, jfloat *elems,
                                         jint mode) = 0;
  virtual void ReleaseDoubleArrayElements(jdoubleArray array, jdouble *elems,
                                          jint mode) = 0;

  virtual void GetBooleanArrayRegion(jbooleanArray array, jsize start,
                                     jsize len, jboolean *buf) = 0;
  virtual void GetByteArrayRegion(jbyteArray array, jsize start, jsize len,
                                  jbyte *buf) = 0;
  virtual void GetCharArrayRegion(jcharArray array, jsize start, jsize len,
                                  jchar *buf) = 0;
  virtual void GetShortArrayRegion(jshortArray array, jsize start, jsize len,
                                   jshort *buf) = 0;
  virtual void GetIntArrayRegion(jintArray array, jsize start, jsize len,
                                 jint *buf) = 0;
  virtual void GetLongArrayRegion(jlongArray array, jsize start, jsize len,
                                  jlong *buf) = 0;
  virtual void GetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
                                   jfloat *buf) = 0;
  virtual void GetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
                                    jdouble *buf) = 0;

  virtual void SetBooleanArrayRegion(jbooleanArray array, jsize start,
                                     jsize len, const jboolean *buf) = 0;
  virtual void SetByteArrayRegion(jbyteArray array, jsize start, jsize len,
                                  const jbyte *buf) = 0;
  virtual void SetCharArrayRegion(jcharArray array, jsize start, jsize len,
                                  const jchar *buf) = 0;
  virtual void SetShortArrayRegion(jshortArray array, jsize start, jsize len,
                                   const jshort *buf) = 0;
  virtual void SetIntArrayRegion(jintArray array, jsize start, jsize len,
                                 const jint *buf) = 0;
  virtual void SetLongArrayRegion(jlongArray array, jsize start, jsize len,
                                  const jlong *buf) = 0;
  virtual void SetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
                                   const jfloat *buf) = 0;
  virtual void SetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
                                    const jdouble *buf) = 0;

  virtual jint RegisterNatives(jclass clazz, const JNINativeMethod *methods,
                               jint nMethods) = 0;

  virtual jint UnregisterNatives(jclass clazz) = 0;

  virtual jint MonitorEnter(jobject obj) = 0;

  virtual jint MonitorExit(jobject obj) = 0;

  virtual jint GetJavaVM(JavaVM **vm) = 0;

  virtual void GetStringRegion(jstring str, jsize start, jsize len,
                               jchar *buf) = 0;

  virtual void GetStringUTFRegion(jstring str, jsize start, jsize len,
                                  char *buf) = 0;

  virtual void *GetPrimitiveArrayCritical(jarray array, jboolean *isCopy) = 0;

  virtual void ReleasePrimitiveArrayCritical(jarray array, void *carray,
                                             jint mode) = 0;

  virtual const jchar *GetStringCritical(jstring string, jboolean *isCopy) = 0;

  virtual void ReleaseStringCritical(jstring string, const jchar *carray) = 0;

  virtual jweak NewWeakGlobalRef(jobject obj) = 0;

  virtual void DeleteWeakGlobalRef(jweak obj) = 0;

  virtual jboolean ExceptionCheck() = 0;

  virtual jobject NewDirectByteBuffer(void *address, jlong capacity) = 0;

  virtual void *GetDirectBufferAddress(jobject buf) = 0;

  virtual jlong GetDirectBufferCapacity(jobject buf) = 0;

  virtual jobjectRefType GetObjectRefType(jobject obj) = 0;
};

/**
 * Convenience definition for a function type that receives a LocalJNIEnv
 * pointer and doesn't return any value.
 */
using JNIFunction = std::function<void(LocalJNIEnv *)>;

/**
 * A native call to a JNI function that ensures no local reference leakage.
 * @param func the JNI function to invoke.
 */
void SafeJNICall(JNIFunction &&func);

}
}

#endif  // _JNI_HELPERS_HPP