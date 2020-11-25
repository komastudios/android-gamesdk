#include <jni.h>
#include <string>
#include <sstream>

#include <memory_advice/memory_advice.h>

#define LOG_TAG "Hogger"
#include "Log.h"

void callback(MemoryAdvice_MemoryState state) {
    ALOGE("State is: %d", state);
    MemoryAdvice_removeWatcher();
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_memory_1advice_hogger_MainActivity_getAdvice(
        JNIEnv* env,
        jobject activity) {
    MemoryAdvice_initDefaultParams(env, activity);
    const char* advice;
    MemoryAdvice_getAdvice(&advice);
    ALOGE("Advice is: %s", advice);
    MemoryAdvice_setWatcher(1000, callback);

    return env->NewStringUTF(advice);
}
