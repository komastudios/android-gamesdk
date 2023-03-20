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

#include "protobuf_util.h"

extern "C" void TuningFork_CProtobufSerialization_Dealloc(
    TuningFork_CProtobufSerialization *c) {
    if (c->bytes) {
        ::free(c->bytes);
        c->bytes = nullptr;
        c->size = 0;
    }
}

extern "C" void TuningFork_CProtobufArray_Dealloc(
    TuningFork_CProtobufArray *array) {
    for (int i = 0; i < array->size; i++) {
        TuningFork_CProtobufSerialization_free(&(array->protobufs[i]));
    }
    free(array->protobufs);
    array->size = 0;
}
