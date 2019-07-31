#include <android/log.h>
#include <jni.h>
#include <string>

constexpr auto appname = "istresser";

extern "C" JNIEXPORT jstring JNICALL
Java_net_jimblackler_istresser_MainActivity_stringFromJNI(
    JNIEnv *env,
    jobject /* this */) {
  std::string hello = "Hello from C++";
  return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_net_jimblackler_istresser_MainActivity_nativeConsume(JNIEnv *env, jobject instance, jint bytes) {
  __android_log_print(ANDROID_LOG_VERBOSE, appname, "Allocating %d", bytes);

  auto byte_count = (size_t) bytes;
  char *data = (char *) malloc(byte_count);
  for (int count = 0; count < byte_count; count++) {
    data[count] = (char) count;
  }
}