#include <android/log.h>
#include <jni.h>
#include <list>
#include <mutex>
#include <string>

constexpr auto appname = "grabber";

std::list<char *> allocated;
std::mutex mtx;

extern "C" JNIEXPORT bool JNICALL
Java_com_google_gamesdk_grabber_MemoryPressureService_nativeConsume(JNIEnv *env, jobject instance,
                                                          jint bytes) {
  mtx.lock();
  auto byte_count = (size_t) bytes;
  char *data = (char *) malloc(byte_count);

  if (data) {
    allocated.push_back(data);
    for (int count = 0; count < byte_count; count++) {
      data[count] = (char) count;
    }
  } else {
    __android_log_print(ANDROID_LOG_WARN, appname, "Could not allocate");
  }

  mtx.unlock();
  return data != nullptr;
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_gamesdk_grabber_MemoryPressureService_freeAll(JNIEnv *env, jobject instance) {
  mtx.lock();
  __android_log_print(ANDROID_LOG_INFO, appname, "Freeing %u entries", allocated.size());
  while(!allocated.empty()) {
    free(allocated.front());
    allocated.pop_front();
  }
  mtx.unlock();
}