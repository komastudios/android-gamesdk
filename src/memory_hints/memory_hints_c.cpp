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

#include "memory_hints/memory_hints.h"
#include "memory_hints_internal.h"

extern "C" {

void MemoryHints_init_internal(JNIEnv *env, jobject context) {
    // TODO: surely call into the C++ implementation classes
    // TODO: surely initialize the JNI?
}

uint64_t MemoryHints_doSomething() {
    // TODO: do something useful.
    if (memory_hints::MakeSomething() == "this is a test") {
        return 42;
    }

    return 0;
}

void MEMORY_HINTS_VERSION_SYMBOL() {
    // Intentionally empty: this function is used to ensure that the proper
    // version of the library is linked against the proper headers.
    // In case of mismatch, a linker error will be triggered because of an
    // undefined symbol, as the name of the function depends on the version.
}

}  // extern "C" {
