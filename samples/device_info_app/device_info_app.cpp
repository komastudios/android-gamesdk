/*
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
#include <string>

#include "device_info/device_info.h"

extern "C" {
JNIEXPORT jbyteArray

JNICALL
Java_com_google_deviceinfotest_MainActivity_jniGetProtoSerialized(
                                          JNIEnv *env, jobject) {
  androidgamesdk_deviceinfo::ProtoByteArray protoByteArray =
    androidgamesdk_deviceinfo::getProtoSerialized();
  jbyteArray result = env->NewByteArray(protoByteArray.size);
  env->SetByteArrayRegion(result, 0,
    protoByteArray.size, (jbyte*)protoByteArray.data);
  free(protoByteArray.data);
  return result;
}
}  // extern "C"
