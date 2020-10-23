#include <jni.h>
#include <string>
#include <sstream>

#include <memory_advice/memory_advice.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_memory_1advice_hogger_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject activity) {
    MemoryAdvice_init(env, activity);
    std::stringstream hello;
    hello << "Hello from C++. Library returned: ";
    hello << MemoryAdvice_testLibraryAccess(42);

    return env->NewStringUTF(hello.str().c_str());
}