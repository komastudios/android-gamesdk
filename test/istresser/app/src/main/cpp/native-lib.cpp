#include "TestRenderer.h"
#include <android/log.h>
#include <jni.h>
#include <list>
#include <mutex>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

constexpr auto appname = "istresser";

static const int PRIME_1 = 214013;
static const int PRIME_2 = 2531011;

std::list<char *> allocated;
std::mutex mtx;
TestRenderer* testRenderer;

extern "C" JNIEXPORT bool JNICALL
Java_net_jimblackler_istresser_MainActivity_nativeConsume(JNIEnv *env, jobject instance,
                                                          jint bytes) {
  mtx.lock();
  auto byte_count = (size_t) bytes;
  char *data = (char *) malloc(byte_count);

  if (data) {
    allocated.push_back(data);
    int value = rand();
    for (int count = 0; count < byte_count; count++) {
      value *= PRIME_1;
      value += PRIME_2;
      data[count] = (char) value;
    }
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
  while(!allocated.empty()) {
    free(allocated.front());
    allocated.pop_front();
  }
  mtx.unlock();
}

extern "C" JNIEXPORT size_t JNICALL
Java_net_jimblackler_istresser_MainActivity_mmapConsume(JNIEnv *env, jobject instance, jint bytes) {
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
    }
    return addr != MAP_FAILED ? byte_count : 0;
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_freeLastAllocation(JNIEnv *env, jobject instance) {
  mtx.lock();
  __android_log_print(ANDROID_LOG_INFO, appname, "Freeing last entry");
  free(allocated.front());
  allocated.pop_front();
  mtx.unlock();
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
Java_net_jimblackler_istresser_MainActivity_initGl(JNIEnv* env,
                                                   jclass thiz,
                                                   jobject assets) {
  testRenderer = new TestRenderer();
}

extern "C"
JNIEXPORT int JNICALL
Java_net_jimblackler_istresser_MainActivity_nativeDraw(JNIEnv* env,
                                                       jclass clazz) {
  return testRenderer->render();
}
extern "C"
JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_release(JNIEnv* env, jclass clazz) {
  testRenderer->release();
}
