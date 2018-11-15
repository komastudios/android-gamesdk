#include <jni.h>
#include <string>

namespace device_info{
std::string debug();
}

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_google_performanceutilstest_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject) {
    return env->NewStringUTF(device_info::debug().c_str());
}