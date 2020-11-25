/*
 * Copyright 2020 The Android Open Source Project
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

#include "jni/jni_helper.h"
#include "memory_advice/memory_advice.h"
#include "memory_advice_internal.h"
#include "metrics_provider.h"

namespace jni = gamesdk::jni;

extern "C" {

MemoryAdvice_ErrorCode MemoryAdvice_initDefaultParams_internal(
    JNIEnv *env, jobject context) {
    jni::Init(env, context);
    return memory_advice::Init();
}

MemoryAdvice_ErrorCode MemoryAdvice_init_internal(JNIEnv *env, jobject context,
                                                  char *params) {
    jni::Init(env, context);
    return memory_advice::Init(params);
}

MemoryAdvice_ErrorCode MemoryAdvice_getMemoryState(
    MemoryAdvice_MemoryState *state) {
    return memory_advice::GetMemoryState(state);
}

MemoryAdvice_ErrorCode MemoryAdvice_getAdvice(const char **advice) {
    return memory_advice::GetAdvice(advice);
}

MemoryAdvice_ErrorCode MemoryAdvice_setWatcher(uint64_t intervalMillis,
                                               CallbackFunction callback) {
    return memory_advice::SetWatcher(intervalMillis, callback);
}

MemoryAdvice_ErrorCode MemoryAdvice_removeWatcher() {
    return memory_advice::RemoveWatcher();
}

void MEMORY_ADVICE_VERSION_SYMBOL() {
    // Intentionally empty: this function is used to ensure that the proper
    // version of the library is linked against the proper headers.
    // In case of mismatch, a linker error will be triggered because of an
    // undefined symbol, as the name of the function depends on the version.
}

}  // extern "C"
