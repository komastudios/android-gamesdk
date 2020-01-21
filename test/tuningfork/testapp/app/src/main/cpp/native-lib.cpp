#include <jni.h>
#include <string>

extern "C" int shared_main(int argc, char * argv[]);

extern "C" JNIEXPORT jstring JNICALL
Java_com_tuningfork_testapp_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    int argc = 1;
    char *argv[] = {"testapp"};
    shared_main(argc, argv);

    std::string hello = "Unit tests ran successfully";
    return env->NewStringUTF(hello.c_str());
}
