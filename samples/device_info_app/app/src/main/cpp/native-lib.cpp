#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_google_androidgamesdk_GameSdkDeviceInfoJni_getProtoSerialized(JNIEnv* env,
                                                                jobject);