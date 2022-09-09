#include <android/input.h>
#include <android/keycodes.h>
#include <jni.h>

#include <string>

#include "paddleboat/paddleboat.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_memory_1advice_myapplication_MainActivity_stringFromJNI(
    JNIEnv* env, jobject /* this */ thiz) {
    std::string hello = "Hello from C++";
    Paddleboat_init(env, thiz);
    if (Paddleboat_getBackButtonConsumed()) {
        hello = "true";
    } else {
        hello = "false";
    }
    return env->NewStringUTF(hello.c_str());
}