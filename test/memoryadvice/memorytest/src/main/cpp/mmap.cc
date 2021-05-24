// Native entry points for mmap methods.
#include <jni.h>

#include "memory.h"
#include "mmap-allocator.h"

namespace {
constexpr char kAppName[] = "memorytest";
static constexpr size_t randomFileBufSize = 16 * 1024;
istresser_mmap_allocator::MmapAllocator *mmap_allocator;

}  // namespace

extern "C" JNIEXPORT jboolean JNICALL
Java_com_google_android_apps_internal_games_memorytest_MemoryTest_writeRandomFile(
    JNIEnv *env, jclass instance, jstring path, jlong total_bytes) {
  char buf[randomFileBufSize];

  const char *c_path = env->GetStringUTFChars(path, nullptr);
  if (c_path == nullptr) {
    return false;
  }

  FILE *file = fopen(c_path, "w");
  if (file == nullptr) {
    return false;
  }

  for (size_t bytes_written = 0; bytes_written < total_bytes;
       bytes_written += randomFileBufSize) {
    LcgFill(buf, randomFileBufSize);
    if (fwrite(buf, 1, randomFileBufSize, file) != randomFileBufSize) {
      return false;
    }
  }

  fclose(file);
  return true;
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_android_apps_internal_games_memorytest_MemoryTest_initMMap(
    JNIEnv *env, jclass clazz) {
  mmap_allocator = new istresser_mmap_allocator::MmapAllocator();
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_android_apps_internal_games_memorytest_MemoryTest_mmapAnonFreeAll(
    JNIEnv *env, jclass instance) {
  mmap_allocator->AnonFreeAll();
}

extern "C" JNIEXPORT int64_t JNICALL
Java_com_google_android_apps_internal_games_memorytest_MemoryTest_mmapAnonConsume(
    JNIEnv *env, jclass instance, jlong bytes) {
  return mmap_allocator->Consume(bytes);
}

extern "C" JNIEXPORT int64_t JNICALL
Java_com_google_android_apps_internal_games_memorytest_MemoryTest_mmapFileConsume(
    JNIEnv *env, jclass instance, jstring path, jlong bytes, jlong offset) {
  return mmap_allocator->MmapFileConsume(env->GetStringUTFChars(path, nullptr),
                                         bytes, offset);
}