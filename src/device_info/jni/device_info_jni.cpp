/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>
#include <cassert>
#include <cstdlib>
#include "third_party/nanopb/pb_encode.h"

#include "device_info/device_info.h"

extern "C" {
JNIEXPORT jbyteArray JNICALL
Java_com_google_androidgamesdk_GameSdkDeviceInfoJni_getProtoSerialized(JNIEnv* env,
                                                                jobject) {
  androidgamesdk_deviceinfo_InfoWithErrors proto;
  androidgamesdk_deviceinfo::ProtoDataHolder protoDataHolder;
  androidgamesdk_deviceinfo::createProto(proto, protoDataHolder);

  size_t bufferSize = -1;
  bool pbEncodeSizeSuccess = pb_get_encoded_size(
      &bufferSize, androidgamesdk_deviceinfo_InfoWithErrors_fields, &proto);
  assert(pbEncodeSizeSuccess);

  pb_byte_t* buffer = new pb_byte_t[bufferSize];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, bufferSize);
  bool pbEncodeSuccess =
      pb_encode(&stream, androidgamesdk_deviceinfo_InfoWithErrors_fields, &proto);
  assert(pbEncodeSuccess);

  jbyteArray result = env->NewByteArray(bufferSize);
  env->SetByteArrayRegion(result, 0, bufferSize,
                          static_cast<jbyte*>(static_cast<void*>(buffer)));

  delete[] buffer;

  return result;
}
}  // extern "C"
