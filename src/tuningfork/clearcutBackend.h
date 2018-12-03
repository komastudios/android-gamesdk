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

#pragma once

#include <sstream>
#include <jni.h>
#include <string>

#include "tuningfork.h"

namespace tuningfork {

class ClearcutBackend : public Backend {
public:
    // Return false if google play services are not available
    bool Init(JNIEnv* env, jobject activity);

    ~ClearcutBackend() override;
    bool GetFidelityParams(ProtobufSerialization &fidelity_params, size_t timeout_ms) override;
    bool Process(const ProtobufSerialization &tuningfork_log_event) override;

private:
    static const std::string LOG_SOURCE;
    static const char* LOG_TAG;

    JNIEnv* env;
    JavaVM* vm;
    jstring ccName;
    jobject clearcutLogger;
    jmethodID newEvent;
    jmethodID logMethod;

    bool isGooglePlayServiceAvailable(JNIEnv* env, jobject context);
    bool getEnv(JNIEnv** env);
    bool _init(JNIEnv* env, jobject activity, bool anonymousLogging);
    bool checkException(JNIEnv* env); ///Need to check for exceptions after each JNI call
};

} //namespace tuningfork {
