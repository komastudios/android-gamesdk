#include "TestRenderer.h"
#include <android/log.h>
#include <cstdio>
#include <fcntl.h>
#include <jni.h>
#include <list>
#include <mutex>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

constexpr auto appname = "istresser";
static constexpr uint32_t LCG_PRIME1 = 214013;
static constexpr uint32_t LCG_PRIME2 = 2531011;

std::list<char *> allocated;
std::list<size_t> allocated_size;
std::mutex mtx;
TestRenderer *testRenderer;

static void lcg_fill(void *addr, size_t byte_len) {
  uint32_t lcg_current = rand();
  uint32_t *data = (uint32_t *) addr;
  size_t word_len = byte_len / 4;
  for (size_t word_count = 0; word_count < word_len; ++word_count) {
    lcg_current *= LCG_PRIME1;
    lcg_current += LCG_PRIME2;
    data[word_count] = lcg_current;
  }
}

void read_and_ignore(void *addr, size_t byte_len) {
  /*volatile*/ char val = 0;
  char *ptr = (char*) addr;
  for (size_t i = 0; i < byte_len; ++i) {
    val = ptr[i];
  }
}

extern "C" JNIEXPORT bool JNICALL
Java_net_jimblackler_istresser_MainActivity_nativeConsume(JNIEnv *env, jobject instance,
                                                          jint bytes) {
  mtx.lock();
  auto byte_count = (size_t) bytes;
  char *data = (char *) malloc(byte_count);

  if (data) {
    allocated.push_back(data);
    allocated_size.push_back(byte_count);
    lcg_fill(data, byte_count);
  } else {
    __android_log_print(ANDROID_LOG_WARN, appname, "Could not allocate");
  }

  mtx.unlock();
  return data != nullptr;
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_freeAll(JNIEnv *env, jobject instance) {
  mtx.lock();
  __android_log_print(ANDROID_LOG_INFO, appname, "Freeing %u entries", allocated.size());
  while (!allocated.empty()) {
    free(allocated.front());
    allocated.pop_front();
    allocated_size.pop_front();
  }
  mtx.unlock();
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_freeMemory(JNIEnv *env, jobject instance, jint bytes) {
  mtx.lock();
  __android_log_print(ANDROID_LOG_INFO, appname, "Freeing %u entries", allocated.size());
  size_t bytes_freed = 0;
  while (!allocated.empty() && bytes_freed < bytes) {
    free(allocated.front());
    allocated.pop_front();
    bytes_freed += allocated_size.front();
    allocated_size.pop_front();
  }
  mtx.unlock();
}

extern "C" JNIEXPORT size_t JNICALL
Java_net_jimblackler_istresser_MainActivity_mmapAnonConsume(JNIEnv *env, jobject instance,
                                                            jint bytes) {
  auto byte_count = (size_t) bytes;
  // Actual mmapped length will always be a multiple of the page size, so we round up to that in
  // order to account properly.
  long page_size_align = sysconf(_SC_PAGE_SIZE) - 1;
  byte_count = (byte_count + page_size_align) & ~page_size_align;

  auto addr = mmap(NULL, byte_count, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (addr == MAP_FAILED) {
    __android_log_print(ANDROID_LOG_WARN, appname, "Could not mmap");
  } else {
    __android_log_print(ANDROID_LOG_INFO, appname, "mmapped %u bytes at %p", byte_count, addr);
    lcg_fill(addr, byte_count);
  }
  return addr != MAP_FAILED ? byte_count : 0;
}

extern "C" JNIEXPORT size_t JNICALL
Java_net_jimblackler_istresser_MainActivity_mmapFileConsume(
    JNIEnv *env, jobject instance, jstring path, jint bytes, jint offset) {
  const char *c_path = env->GetStringUTFChars(path, NULL);
  auto addr = MAP_FAILED;
  size_t byte_count = (size_t) bytes;
  size_t byte_offset = (size_t) offset;

  // Offset must be a multiple of the page size, so we round down.
  long page_size_align = sysconf(_SC_PAGE_SIZE) - 1;
  byte_offset &= ~page_size_align;

  int file = open(c_path, O_RDONLY);
  __android_log_print(ANDROID_LOG_INFO, appname, "open fd is %p", file);

  if (file != -1) {
    struct stat sb;
    if (fstat(file, &sb) == -1) {
      __android_log_print(ANDROID_LOG_WARN, appname, "mmapFile: fstat failed");
      close(file);
      return 0;
    }

    if (byte_offset >= sb.st_size) {
      __android_log_print(ANDROID_LOG_WARN, appname, "mmapFile: map begins beyond EOF");
      return 0;
    } else if (byte_count + byte_offset > sb.st_size) {
      byte_offset = sb.st_size - byte_count;
    }

    addr = mmap(NULL, byte_count, PROT_READ, MAP_PRIVATE, file, byte_offset);
    if (addr == MAP_FAILED) {
      __android_log_print(ANDROID_LOG_WARN, appname, "Could not mmap physical file");
    } else {
      __android_log_print(
          ANDROID_LOG_INFO, appname,
          "mmapped %ld bytes at %p from physical file %s (offset %ld)",
          byte_count, addr, c_path, byte_offset);
      read_and_ignore(addr, byte_count);
    }
    close(file);
  }
  return addr != MAP_FAILED ? byte_count : 0;
}

extern "C"
JNIEXPORT bool JNICALL
Java_net_jimblackler_istresser_Heuristic_tryAlloc(JNIEnv *env, jobject thiz, jint bytes) {
  auto byte_count = (size_t) bytes;
  char *data = (char *) malloc(byte_count);
  if (data) {
    free(data);
    return true;
  }
  return false;
}

extern "C"
JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_initGl(JNIEnv *env,
                                                   jclass thiz) {
  testRenderer = new TestRenderer();
}

extern "C"
JNIEXPORT int JNICALL
Java_net_jimblackler_istresser_MainActivity_nativeDraw(JNIEnv *env,
                                                       jclass clazz,
                                                       int toAllocate) {
  return testRenderer->render(toAllocate);
}
extern "C"
JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_release(JNIEnv *env, jclass clazz) {
  testRenderer->release();
}
