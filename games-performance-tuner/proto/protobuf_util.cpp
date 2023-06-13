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

namespace tuningfork {

void ToCQualityLevelPredictions(const QLTimePredictions& pred,
                                TuningFork_QualityLevelPredictions& c_pred) {
    c_pred.fidelity_params = new TuningFork_CProtobufSerialization[pred.size()];
    c_pred.predicted_time_us = new uint32_t[pred.size()];

    for (int i = 0; i < pred.size(); i++) {
        ToCProtobufSerialization(pred[i].first, c_pred.fidelity_params[i]);
        c_pred.predicted_time_us[i] = pred[i].second;
    }

    c_pred.size = pred.size();
}

}  // namespace tuningfork
