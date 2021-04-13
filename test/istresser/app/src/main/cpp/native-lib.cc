// Native entry points for native memory ('malloc') allocation and deallocation.

#include <jni.h>

#include "allocator.h"

namespace {
istresser_allocator::Allocator *allocator;
}  // namespace

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_initNative(JNIEnv *env,
                                                       jclass thiz) {
  allocator = new istresser_allocator::Allocator();
}

extern "C" JNIEXPORT jboolean JNICALL
Java_net_jimblackler_istresser_MainActivity_nativeConsume(JNIEnv *env,
                                                          jobject instance,
                                                          jlong bytes) {
  return allocator->Allocate(bytes);
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_freeAll(JNIEnv *env,
                                                    jobject instance) {
  return allocator->Release();
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_freeMemory(JNIEnv *env,
                                                       jobject instance,
                                                       jint bytes) {
  return allocator->ReleasePartial(bytes);
}
