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

#pragma once

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <memory>
#include <string>
#include <vector>

#include "apk_utils.h"
#include "json11/json11.hpp"
#include "memory_advice/memory_advice.h"
#include "tensorflow/lite/create_op_resolver.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/interpreter_builder.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model_builder.h"
#include "tensorflow/lite/op_resolver.h"

namespace memory_advice {

using namespace json11;

/**
A class to help predict memory limits using tensorflow lite models.
*/
class Predictor {
   private:
    std::unique_ptr<tflite::Interpreter> interpreter;
    std::vector<std::string> features;
    std::unique_ptr<tflite::FlatBufferModel> model;
    std::unique_ptr<tflite::OpResolver> resolver;
    tflite::StderrReporter error_reporter;
    std::unique_ptr<apk_utils::NativeAsset> model_asset;
    const char* model_buffer;
    size_t model_capacity;

    float GetFromPath(std::string feature, Json::object data);

   public:
    MemoryAdvice_ErrorCode Init(std::string model_file,
                                std::string features_file);
    float Predict(Json::object data);
};

}  // namespace memory_advice