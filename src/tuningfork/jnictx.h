/*
 * Copyright 2019 The Android Open Source Project
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

#pragma once

#include <jni.h>

namespace tuningfork {
    
// a class that stores an app's JVM and context
class JniCtx {
    JavaVM* jvm_;
    jobject jctx_; // Global reference to the app's context
  public:
    JniCtx(JNIEnv* env, jobject ctx);
    JniCtx(const JniCtx& rhs);
    JniCtx& operator=(const JniCtx& rhs) = delete;
    ~JniCtx();
    JavaVM* Jvm() const { return jvm_; }
    jobject Ctx() const { return jctx_; }
    // Calling Env() will call AttachCurrentThread automatically.
    JNIEnv* Env() const;
};

} // namespace tuningfork
