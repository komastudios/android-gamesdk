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
    MemoryAdvice_JsonSerialization advice;
    MemoryAdvice_getAdvice(&advice);
    ALOGE("Advice is: %s", advice.json);
    jstring ret = env->NewStringUTF(advice.json);
    MemoryAdvice_JsonSerialization_free(&advice);
    // TODO(bkaya): Reintroduce this line after state logic is fixed
    //MemoryAdvice_setWatcher(1000, callback);
    return ret;
}
