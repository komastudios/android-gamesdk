// This file provides the native functions for the base/MainActivity.java and
// base/Heuristic.java classes. These functions are here to help the app
// allocate and deallocate memory more efficiently. initNative function must
// be called first, before the rest of the functions can be used.

#include "test-renderer.h"
#include "test-vulkan-renderer.h"
#include "utils.h"
#include "memory.h"
#include <android/log.h>

#include <fcntl.h>  // For open
#include <jni.h>
#include <stdlib.h>
#include <sys/mman.h>  // For mmap
#include <sys/stat.h>  // For fstat
#include <unistd.h>

#include <cstdlib>
#include <list>
#include <string>

namespace {
  constexpr char kAppName[] = "istresser";
  static constexpr size_t randomFileBufSize = 16 * 1024;

  istresser_utils::Utils *utils;
  istresser_testrenderer::TestRenderer *test_renderer;
  istresser_testvulkanrenderer::TestVulkanRenderer *test_vulkan_renderer;
}  // namespace

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_initNative(
    JNIEnv *env, jclass thiz) {
  if (utils == NULL) {
    utils = new istresser_utils::Utils();
  }
}

extern "C" JNIEXPORT bool JNICALL
Java_net_jimblackler_istresser_MainActivity_writeRandomFile(
    JNIEnv *env, jobject instance, jstring path, jlong total_bytes) {
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

// Read out mmapped memory, so there's no chance some runtime optimization will
// cause the underlying file not to be read.
// TODO(b/151615602): confirm in the object file this read actually happens.
static void ReadAndIgnore(void *addr, size_t byte_len) {
  volatile char val = 0;
  char *ptr = (char*) addr;
  for (size_t i = 0; i < byte_len; ++i) {
    val = ptr[i];
  }
}

extern "C" JNIEXPORT jboolean JNICALL
Java_net_jimblackler_istresser_MainActivity_nativeConsume(
    JNIEnv *env, jobject instance, jint bytes) {
  utils->mtx.lock();
  size_t byte_count = (size_t)bytes;
  char *data = (char *)malloc(byte_count);

  if (data) {
    utils->allocated.push_back(data);
    utils->allocated_size.push_back(byte_count);
    LcgFill(data, byte_count);
  } else {
    __android_log_print(ANDROID_LOG_WARN, kAppName, "Could not allocate");
  }

  utils->mtx.unlock();
  return data != nullptr;
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_freeAll(
    JNIEnv *env, jobject instance) {
  utils->mtx.lock();
  __android_log_print(ANDROID_LOG_INFO, kAppName, "Freeing entries");
  while (!utils->allocated.empty()) {
    free(utils->allocated.front());
    utils->allocated.pop_front();
    utils->allocated_size.pop_front();
  }
  utils->mtx.unlock();
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_freeMemory(
    JNIEnv *env, jobject instance, jint bytes) {
  utils->mtx.lock();
  __android_log_print(ANDROID_LOG_INFO, kAppName, "Freeing entries");
  size_t bytes_freed = 0;
  while (!utils->allocated.empty() && bytes_freed < bytes) {
    free(utils->allocated.front());
    utils->allocated.pop_front();
    bytes_freed += utils->allocated_size.front();
    utils->allocated_size.pop_front();
  }
  utils->mtx.unlock();
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_mmapAnonFreeAll(
    JNIEnv *env, jobject instance) {
  utils->mtx.lock();
  __android_log_print(ANDROID_LOG_INFO, kAppName, "Freeing anon mmap memory");
  while (!utils->mmap_allocated.empty()) {
    if (munmap(utils->mmap_allocated.front(),
               utils->mmap_allocated_size.front()) != 0) {
      __android_log_print(ANDROID_LOG_INFO, kAppName, ": Could not release mmapped memory");
    }
    utils->mmap_allocated.pop_front();
    utils->mmap_allocated_size.pop_front();
  }
  utils->mtx.unlock();
}

extern "C" JNIEXPORT int64_t JNICALL
Java_net_jimblackler_istresser_MainActivity_mmapAnonConsume(
    JNIEnv *env, jobject instance, jlong bytes) {
  size_t byte_count = (size_t)bytes;
  // Actual mmapped length will always be a multiple of the page size, so we
  // round up to that in order to account properly.
  int32_t page_size_align = sysconf(_SC_PAGE_SIZE) - 1;
  byte_count = (byte_count + page_size_align) & ~page_size_align;

  void *addr = mmap(nullptr, byte_count, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (addr == MAP_FAILED) {
    __android_log_print(ANDROID_LOG_WARN, kAppName, "Could not mmap");
  } else {
    __android_log_print(ANDROID_LOG_INFO, kAppName, "mmapped %u bytes at %p", byte_count, addr);
    LcgFill(addr, byte_count);
    utils->mmap_allocated.push_back(addr);
    utils->mmap_allocated_size.push_back(byte_count);
  }
  return addr != MAP_FAILED ? byte_count : 0;
}

extern "C" JNIEXPORT int64_t JNICALL
Java_net_jimblackler_istresser_MainActivity_mmapFileConsume(
    JNIEnv *env, jobject instance, jstring path, jlong bytes, jlong offset) {
  const char *c_path = env->GetStringUTFChars(path, nullptr);
  auto addr = MAP_FAILED;
  size_t byte_count = (size_t) bytes;
  size_t byte_offset = (size_t) offset;

  // Offset must be a multiple of the page size, so we round down.
  int32_t page_size_align = sysconf(_SC_PAGE_SIZE) - 1;
  byte_offset &= ~page_size_align;

  int file = open(c_path, O_RDONLY);
  __android_log_print(ANDROID_LOG_INFO, kAppName, "open fd is %p", file);

  if (file != -1) {
    struct stat sb;
    if (fstat(file, &sb) == -1) {
      __android_log_print(ANDROID_LOG_WARN, kAppName, "mmapFile: fstat failed");
      close(file);
      return 0;
    }

    if (byte_offset >= sb.st_size) {
      __android_log_print(ANDROID_LOG_WARN, kAppName, "mmapFile: map begins beyond EOF");
      return 0;
    } else if (byte_count + byte_offset > sb.st_size) {
      byte_offset = sb.st_size - byte_count;
    }

    addr = mmap(nullptr, byte_count, PROT_READ, MAP_PRIVATE, file, byte_offset);
    if (addr == MAP_FAILED) {
      __android_log_print(ANDROID_LOG_WARN, kAppName, "Could not mmap physical file");
    } else {
      __android_log_print(
          ANDROID_LOG_INFO, kAppName,
          "mmapped %ld bytes at %p from physical file %s (offset %ld)",
          byte_count, addr, c_path, byte_offset);
      ReadAndIgnore(addr, byte_count);
    }
    close(file);
  }
  return addr == MAP_FAILED ? 0 : byte_count;
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_initGl(
    JNIEnv *env, jclass thiz) {
  test_renderer = new istresser_testrenderer::TestRenderer();
}

extern "C" JNIEXPORT jint JNICALL
Java_net_jimblackler_istresser_MainActivity_nativeDraw(
    JNIEnv *env, jclass clazz, jint toAllocate) {
  if (test_renderer != NULL) {
    return test_renderer->Render(toAllocate);
  } else {
    return 0;
  }
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_release(
    JNIEnv *env, jclass clazz) {
  if (test_renderer != NULL) {
    test_renderer->Release();
  }
}

extern "C"
JNIEXPORT jlong JNICALL
Java_net_jimblackler_istresser_MainActivity_vkAlloc(JNIEnv *env, jclass clazz, jlong size) {
  if (test_vulkan_renderer == NULL) {
    test_vulkan_renderer = new istresser_testvulkanrenderer::TestVulkanRenderer();
  }
  return test_vulkan_renderer->Allocate(size);
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_vkRelease(
    JNIEnv *env, jclass clazz) {
  if (test_vulkan_renderer != NULL) {
    test_vulkan_renderer->Release();
  }
}