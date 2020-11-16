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

#pragma once

#include <memory>

#include "device_profiler.h"
#include "metrics_provider.h"

namespace memory_advice {

using namespace json11;

class MemoryAdviceImpl {
   private:
    std::unique_ptr<MetricsProvider> metrics_provider_;
    std::unique_ptr<DeviceProfiler> device_profiler_;
    Json::object advisor_parameters_;
    Json::object baseline_;
    Json::object device_profile_;

    MemoryAdvice_ErrorCode initialization_error_code_ = MEMORYADVICE_ERROR_OK;

    MemoryAdvice_ErrorCode ProcessAdvisorParameters(const char* parameters);
    Json::object GenerateMetricsFromFields(Json::object fields);
    Json::object ExtractValues(
        MetricsProvider::MetricsFunction metrics_function, Json fields);
    double MillisecondsSinceEpoch();
    Json GetValue(Json::object object, std::string key);

   public:
    MemoryAdviceImpl();
    Json::object GetAdvice();
    Json::object GenerateVariableMetrics();
    Json::object GenerateConstantMetrics();

    MemoryAdvice_ErrorCode InitializationErrorCode() const {
        return initialization_error_code_;
    }
};

}  // namespace memory_advice
