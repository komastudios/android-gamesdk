/*
 * Copyright 2021 The Android Open Source Project
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

#include "predictor.h"

#include <stdlib.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <map>
#include <sstream>
#include <streambuf>
#include <string>

#include "Log.h"
#include "apk_utils.h"
#include "jni/jni_wrap.h"
#include "memory_advice/memory_advice.h"

#define LOG_TAG "MemoryAdvice:DeviceProfiler"

namespace memory_advice {

using namespace tflite;
using namespace json11;

MemoryAdvice_ErrorCode Predictor::Init(std::string model_file,
                                       std::string features_file) {
    apk_utils::NativeAsset features_asset(features_file.c_str());
    std::string features_string(
        static_cast<const char*>(AAsset_getBuffer(features_asset)));

    features_string =
        features_string.substr(features_string.find_first_of('\n') + 1);
    features_string =
        features_string.substr(0, features_string.find_first_of(']'));
    int pos = 0;
    while ((pos = features_string.find_first_of('\n')) != std::string::npos) {
        std::string line(features_string.substr(0, pos));
        features.push_back(
            line.substr(line.find_first_of("/") + 1,
                        line.find_last_of("\"") - line.find_first_of("/") - 1));
        features_string = features_string.substr(pos + 1);
    }

    model_asset = std::make_unique<apk_utils::NativeAsset>(model_file.c_str());
    const char* model_buffer =
        static_cast<const char*>(AAsset_getBuffer(*model_asset));
    const size_t model_capacity =
        static_cast<size_t>(AAsset_getLength(*model_asset));

    model = tflite::FlatBufferModel::BuildFromBuffer(
        model_buffer, model_capacity, &error_reporter);
    std::unique_ptr<OpResolver> resolver = tflite::CreateOpResolver();

    if (InterpreterBuilder(*model, *resolver)(&interpreter) != kTfLiteOk) {
        return MEMORYADVICE_ERROR_TFLITE_MODEL_INVALID;
    }
    std::vector<int> sizes;
    sizes.push_back(features.size());
    if (interpreter->ResizeInputTensor(0, sizes) != kTfLiteOk) {
        return MEMORYADVICE_ERROR_TFLITE_MODEL_INVALID;
    }
    if (interpreter->AllocateTensors() != kTfLiteOk) {
        return MEMORYADVICE_ERROR_TFLITE_MODEL_INVALID;
    }

    return MEMORYADVICE_ERROR_OK;
}

float Predictor::GetFromPath(std::string feature, Json::object data) {
    int pos = 0;
    const Json::object* search = &data;
    while ((pos = feature.find_first_of("/")) != std::string::npos) {
        search = &(search->at(feature.substr(0, pos)).object_items());
        feature = feature.substr(pos + 1);
    }

    Json result = search->at(feature);

    if (result.is_number()) {
        return static_cast<float>(result.number_value());
    } else if (result.is_bool()) {
        return result.bool_value() ? 1.0f : 0.0f;
    } else {
        return 0.0f;
    }
}

float Predictor::Predict(Json::object data) {
    for (int idx = 0; idx != features.size(); idx++) {
        interpreter->typed_input_tensor<float>(0)[idx] =
            GetFromPath(features[idx], data);
    }

    interpreter->Invoke();

    return *(interpreter->typed_output_tensor<float>(0));
}

}  // namespace memory_advice