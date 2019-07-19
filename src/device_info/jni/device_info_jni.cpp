#include <jni.h>
#include <cassert>
#include <cstdlib>
#include "third_party/nanopb/pb_encode.h"

#include "device_info/device_info.h"


#include <android/log.h>

#define LOG_TAG "DeviceInfoJni"
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__);

extern "C" {
JNIEXPORT jbyteArray JNICALL
Java_com_google_androidgamesdk_GameSdkDeviceInfoJni_getProtoSerialized(JNIEnv* env,
                                                                jobject) {
  ALOGW("Java_com_google_androidgamesdk_GameSdkDeviceInfoJni_getProtoSerialized");
  finsky_dfe_GameSdkDeviceInfoWithErrors proto;
  androidgamesdk_deviceinfo::ProtoDataHolder protoDataHolder;
  ALOGW("Calling androidgamesdk_deviceinfo::createProto");
  androidgamesdk_deviceinfo::createProto(proto, protoDataHolder);
  ALOGW("Java_com_google_androidgamesdk_GameSdkDeviceInfoJni_getProtoSerialized DONE");

  size_t bufferSize = -1;
  bool pbEncodeSizeSuccess = pb_get_encoded_size(
      &bufferSize, finsky_dfe_GameSdkDeviceInfoWithErrors_fields, &proto);
  assert(pbEncodeSizeSuccess);

  pb_byte_t* buffer = new pb_byte_t[bufferSize];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, bufferSize);
  bool pbEncodeSuccess =
      pb_encode(&stream, finsky_dfe_GameSdkDeviceInfoWithErrors_fields, &proto);
  assert(pbEncodeSuccess);

  jbyteArray result = env->NewByteArray(bufferSize);
  env->SetByteArrayRegion(result, 0, bufferSize,
                          static_cast<jbyte*>(static_cast<void*>(buffer)));

  delete[] buffer;

  return result;
}
}  // extern "C"
