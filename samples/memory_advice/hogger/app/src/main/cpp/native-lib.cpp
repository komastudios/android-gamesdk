#include <jni.h>
#include <string>
#include <sstream>
#include <list>
#include <mutex>
#include <random>

#include <memory_advice/memory_advice.h>

#define LOG_TAG "Hogger"
#include "Log.h"

void callback(MemoryAdvice_MemoryState state) {
    ALOGE("State is: %d", state);
    MemoryAdvice_removeWatcher();
}

extern "C" JNIEXPORT jint JNICALL
Java_com_memory_1advice_hogger_MainActivity_initMemoryAdvice(
        JNIEnv* env,
        jobject activity) {
    return MemoryAdvice_initDefaultParams(env, activity);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_memory_1advice_hogger_MainActivity_getMemoryAdvice(
        JNIEnv* env,
        jobject activity) {
    MemoryAdvice_JsonSerialization advice;
    MemoryAdvice_getAdvice(&advice);
    ALOGE("Advice is: %s", advice.json);
    jstring ret = env->NewStringUTF(advice.json);
    MemoryAdvice_JsonSerialization_free(&advice);
    MemoryAdvice_setWatcher(1000, callback);
    return ret;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_memory_1advice_hogger_MainActivity_getMemoryState(
        JNIEnv* env,
        jobject activity) {
    MemoryAdvice_MemoryState state = MEMORYADVICE_STATE_UNKNOWN;
    MemoryAdvice_getMemoryState(&state);
    return state;
}

static std::list<std::unique_ptr<char[]>> allocated_bytes_list;
static std::mutex allocated_mutex;

static void FillRandom(char* bytes, size_t size) {
    std::minstd_rand rng;
    int32_t* ptr = reinterpret_cast<int32_t*>(bytes);
    std::generate(ptr, ptr + size/4, [&]() -> int32_t { return rng(); });
    // Don't worry about filling the last few bytes if size isn't a multiple of 4.
}

extern "C" JNIEXPORT void JNICALL
Java_com_memory_1advice_hogger_MainActivity_allocate(
        JNIEnv* env,
        jobject activity,
        jlong nbytes) {
    std::lock_guard<std::mutex> guard(allocated_mutex);
    auto allocated_bytes = new char[nbytes];
    // Note that without setting the memory, the available memory figure doesn't go down.
    FillRandom(allocated_bytes, nbytes);
    allocated_bytes_list.push_back(std::unique_ptr<char[]> {allocated_bytes });
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_memory_1advice_hogger_MainActivity_deallocate(
        JNIEnv* env,
        jobject activity) {
    std::lock_guard<std::mutex> guard(allocated_mutex);
    if (allocated_bytes_list.size() > 0) {
        allocated_bytes_list.pop_back();
        return true;
    } else {
        return false;
    }
}
