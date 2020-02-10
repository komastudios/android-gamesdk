#include "TestRenderer.h"
#include <android/log.h>
#include <jni.h>
#include <list>
#include <mutex>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

constexpr auto appname = "istresser";
static constexpr uint32_t HASH_PRIME = 214013;

std::list<char *> allocated;
std::mutex mtx;
TestRenderer *testRenderer;
static uint32_t current_hash = 1234567890;

static void hash_fill(void *addr, size_t byte_len) {
  uint32_t *data = (uint32_t *) addr;
  size_t word_len = byte_len / 4;
  for (size_t word_count = 0; word_count < word_len; ++word_count) {
    current_hash ^= word_count;
    current_hash *= HASH_PRIME;
    *data = current_hash;
    ++data;
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
    hash_fill(data, byte_count);
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
  }
  mtx.unlock();
}

extern "C" JNIEXPORT size_t JNICALL
Java_net_jimblackler_istresser_MainActivity_mmapAnonConsume(JNIEnv *env, jobject instance, jint bytes) {
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
    hash_fill(addr, byte_count);
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
                                                   jclass thiz,
                                                   jobject assets) {
  testRenderer = new TestRenderer();
}

extern "C"
JNIEXPORT int JNICALL
Java_net_jimblackler_istresser_MainActivity_nativeDraw(JNIEnv *env,
                                                       jclass clazz) {
  return testRenderer->render();
}
extern "C"
JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_release(JNIEnv *env, jclass clazz) {
  testRenderer->release();
}
