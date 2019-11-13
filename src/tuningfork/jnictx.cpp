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

#include "jnictx.h"

namespace tuningfork {

JniCtx::JniCtx(JNIEnv* env, jobject ctx) {
    if (env) {
        jctx_ = env->NewGlobalRef(ctx);
        env->GetJavaVM(&jvm_);
    }
}
JniCtx::JniCtx(const JniCtx& rhs) : jvm_(rhs.jvm_) {
    JNIEnv* env = Env();
    if (env) {
        jctx_ = env->NewGlobalRef(rhs.Ctx());
    }

}
JniCtx::~JniCtx() {
    if (jctx_) {
        JNIEnv* env = Env();
        if (env) {
            env->DeleteGlobalRef(jctx_);
        }
    }
}
JNIEnv* JniCtx::Env() const {
    static thread_local JNIEnv* env = nullptr;
    if (jvm_ != nullptr && env == nullptr) {
        jvm_->AttachCurrentThread(&env, NULL);
    }
    return env;
}

} // namespace tuningfork
