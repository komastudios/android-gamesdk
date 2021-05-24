// Native entry points for native memory ('malloc') allocation and deallocation.

#include <jni.h>

#include "allocator.h"

namespace {
istresser_allocator::Allocator *allocator;
}  // namespace

extern "C" JNIEXPORT void JNICALL
Java_com_google_android_apps_internal_games_memorytest_MemoryTest_initNative(
    JNIEnv *env, jclass thiz) {
  allocator = new istresser_allocator::Allocator();
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_google_android_apps_internal_games_memorytest_MemoryTest_nativeConsume(
    JNIEnv *env, jclass instance, jlong bytes) {
  return allocator->Allocate(bytes);
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_android_apps_internal_games_memorytest_MemoryTest_freeAll(
    JNIEnv *env, jclass instance) {
  return allocator->Release();
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_android_apps_internal_games_memorytest_MemoryTest_freeMemory(
    JNIEnv *env, jclass instance, jlong bytes) {
  return allocator->ReleasePartial(bytes);
}
