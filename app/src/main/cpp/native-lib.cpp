#include <android/log.h>
#include <jni.h>
#include <list>
#include <mutex>
#include <string>

constexpr auto appname = "istresser";

std::list<char *> allocated;
std::mutex mtx;

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_nativeConsume(JNIEnv *env, jobject instance,
                                                          jint bytes) {
  mtx.lock();
  auto byte_count = (size_t) bytes;
  char *data = (char *) malloc(byte_count);
  allocated.push_back(data);
  if (!data) {
    __android_log_print(ANDROID_LOG_WARN, appname, "Could not allocate");
    return;
  }
  for (int count = 0; count < byte_count; count++) {
    data[count] = (char) count;
  }
  mtx.unlock();
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_freeAll(JNIEnv *env, jobject instance) {
  mtx.lock();
  __android_log_print(ANDROID_LOG_INFO, appname, "Freeing %lu entries", allocated.size());
  while(!allocated.empty()) {
    free(allocated.front());
    allocated.pop_front();
  }
  mtx.unlock();
}