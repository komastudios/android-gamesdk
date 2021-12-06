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

#include "memory_advice_impl.h"

#include <chrono>

namespace memory_advice {

using namespace json11;

MemoryAdviceImpl::MemoryAdviceImpl(const char* params) {
    metrics_provider_ = std::make_unique<MetricsProvider>();
    realtime_predictor_ = std::make_unique<Predictor>();
    available_predictor_ = std::make_unique<Predictor>();
    oom_predictor_ = std::make_unique<Predictor>();

    initialization_error_code_ =
        realtime_predictor_->Init("realtime.tflite", "realtime_features.json");
    if (initialization_error_code_ != MEMORYADVICE_ERROR_OK) {
        return;
    }
    initialization_error_code_ = available_predictor_->Init(
        "available.tflite", "available_features.json");
    if (initialization_error_code_ != MEMORYADVICE_ERROR_OK) {
        return;
    }
    initialization_error_code_ =
        oom_predictor_->Init("oom.tflite", "oom_features.json");
    if (initialization_error_code_ != MEMORYADVICE_ERROR_OK) {
        return;
    }
    initialization_error_code_ = ProcessAdvisorParameters(params);
    if (initialization_error_code_ != MEMORYADVICE_ERROR_OK) {
        return;
    }
    baseline_ = GenerateBaselineMetrics();
    baseline_["constant"] = GenerateConstantMetrics();
}

MemoryAdvice_ErrorCode MemoryAdviceImpl::ProcessAdvisorParameters(
    const char* parameters) {
    std::string err;
    advisor_parameters_ = Json::parse(parameters, err).object_items();
    if (!err.empty()) {
        ALOGE("Error while parsing advisor parameters: %s", err.c_str());
        return MEMORYADVICE_ERROR_ADVISOR_PARAMETERS_INVALID;
    }

    return MEMORYADVICE_ERROR_OK;
}

MemoryAdvice_MemoryState MemoryAdviceImpl::GetMemoryState() {
    Json::object advice = GetAdviceDeprecated();
    if (advice.find("warnings") != advice.end()) {
        Json::array warnings = advice["warnings"].array_items();
        for (auto& it : warnings) {
            if (it.object_items().at("level").string_value() == "red") {
                return MEMORYADVICE_STATE_CRITICAL;
            }
        }
        return MEMORYADVICE_STATE_APPROACHING_LIMIT;
    }
    return MEMORYADVICE_STATE_OK;
}

int64_t MemoryAdviceImpl::GetAvailableMemory() {
    Json::object advice = GetAdviceDeprecated();
    if (advice.find("predictions") != advice.end()) {
        int64_t smallest_estimate = 0;
        Json::object predictions = advice["predictions"].object_items();
        for (auto& it : predictions) {
            if (smallest_estimate == 0 ||
                it.second.number_value() < smallest_estimate) {
                smallest_estimate = it.second.number_value();
            }
        }
        return smallest_estimate;
    }
    return 0;
}

Json::object MemoryAdviceImpl::GetAdvice() {
    std::lock_guard<std::mutex> lock(advice_mutex_);
    double start_time = MillisecondsSinceEpoch();
    Json::object advice;
    Json::object data;
    Json::object variable_spec = advisor_parameters_.at("metrics")
                                     .object_items()
                                     .at("variable")
                                     .object_items();
    Json::object variable_metrics = GenerateVariableMetrics();

    data["baseline"] = baseline_;
    data["sample"] = variable_metrics;
    // TODO(b/209602631): add build info here
    // data["build"] = build_;

    if (variable_spec.find("predictRealtime") != variable_spec.end() &&
        variable_spec.at("predictRealtime").bool_value()) {
        variable_metrics["predictedUsage"] =
            Json(realtime_predictor_->Predict(data));
    }

    if (variable_spec.find("availableRealtime") != variable_spec.end() &&
        variable_spec.at("availableRealtime").bool_value()) {
        variable_metrics["predictedAvailable"] =
            Json(realtime_predictor_->Predict(data));
    }

    advice["metrics"] = variable_metrics;
    return advice;
}

Json::object MemoryAdviceImpl::GetAdviceDeprecated() {
    std::lock_guard<std::mutex> lock(advice_mutex_);
    double start_time = MillisecondsSinceEpoch();
    Json::object advice;
    advice["metrics"] = GenerateVariableMetrics();
    Json::object device_limit =
        device_profile_.at("limits").object_items().at("limit").object_items();
    Json::object device_baseline = device_profile_.at("limits")
                                       .object_items()
                                       .at("baseline")
                                       .object_items();

    if (advisor_parameters_.find("heuristics") != advisor_parameters_.end()) {
        Json::array warnings;
        Json::object heuristics =
            advisor_parameters_["heuristics"].object_items();

        for (auto& it : heuristics) {
            std::string key = it.first;
            Json::object heuristic = it.second.object_items();

            Json metric_value = GetValue(advice["metrics"].object_items(), key);
            Json device_limit_value = GetValue(device_limit, key);
            Json device_baseline_value = GetValue(device_baseline, key);
            Json baseline_value = GetValue(baseline_, key);

            if (metric_value.is_null() || device_limit_value.is_null() ||
                device_baseline_value.is_null() || baseline_value.is_null()) {
                continue;
            }

            bool increasing = (device_limit_value > device_baseline_value);

            // Fires warnings as baseline-relative metrics approach ratios of
            // the device's baseline- relative limit. Example: "oom_score":
            // {"deltaLimit": {"red": 0.85, "yellow": 0.75}}
            if (heuristic.find("deltaLimit") != heuristic.end()) {
                Json::object delta_limit =
                    heuristic["deltaLimit"].object_items();
                double limit_value = device_limit_value.number_value() -
                                     device_baseline_value.number_value();
                double relative_value =
                    metric_value.number_value() - baseline_value.number_value();
                std::string level;
                if (increasing
                        ? relative_value >
                              limit_value * delta_limit["red"].number_value()
                        : relative_value <
                              limit_value * delta_limit["red"].number_value()) {
                    level = "red";
                } else if (increasing
                               ? relative_value >
                                     limit_value *
                                         delta_limit["yellow"].number_value()
                               : relative_value <
                                     limit_value *
                                         delta_limit["yellow"].number_value()) {
                    level = "yellow";
                }
                if (!level.empty()) {
                    Json::object warning;
                    Json::object trigger = {{"deltaLimit", delta_limit}};
                    warning[key] = trigger;
                    warning["level"] = level;
                    warnings.push_back(warning);
                }
            }

            // Fires warnings as metrics approach ratios of the device's limit.
            // Example: "VmRSS": {"limit": {"red": 0.90, "yellow": 0.75}}
            if (heuristic.find("limit") != heuristic.end()) {
                Json::object limit = heuristic["limit"].object_items();
                std::string level;
                if (increasing ? metric_value.number_value() >
                                     device_limit_value.number_value() *
                                         limit["red"].number_value()
                               : metric_value.number_value() *
                                         limit["red"].number_value() <
                                     device_limit_value.number_value()) {
                    level = "red";
                } else if (increasing ? metric_value.number_value() >
                                            device_limit_value.number_value() *
                                                limit["yellow"].number_value()
                                      : metric_value.number_value() *
                                                limit["yellow"].number_value() <
                                            device_limit_value.number_value()) {
                    level = "yellow";
                }
                if (!level.empty()) {
                    Json::object warning;
                    Json::object trigger = {{"limit", limit}};
                    warning[key] = trigger;
                    warning["level"] = level;
                    warnings.push_back(warning);
                }
            }

            // Fires warnings as metrics approach ratios of the device baseline.
            // Example: "availMem": {"baselineRatio": {"red": 0.30, "yellow":
            // 0.40}}
            if (heuristic.find("baselineRatio") != heuristic.end()) {
                Json::object baseline_ratio =
                    heuristic["baselineRatio"].object_items();
                std::string level;
                if (increasing ? metric_value.number_value() >
                                     baseline_value.number_value() *
                                         baseline_ratio["red"].number_value()
                               : metric_value.number_value() <
                                     baseline_value.number_value() *
                                         baseline_ratio["red"].number_value()) {
                    level = "red";
                } else if (increasing
                               ? metric_value.number_value() >
                                     baseline_value.number_value() *
                                         baseline_ratio["yellow"].number_value()
                               : metric_value.number_value() <
                                     baseline_value.number_value() *
                                         baseline_ratio["yellow"]
                                             .number_value()) {
                    level = "yellow";
                }
                if (!level.empty()) {
                    Json::object warning;
                    Json::object trigger = {{"baselineRatio", baseline_ratio}};
                    warning[key] = trigger;
                    warning["level"] = level;
                    warnings.push_back(warning);
                }
            }
        }
        if (!warnings.empty()) {
            advice["warnings"] = warnings;
        }
    }

    if (device_limit.find("stressed") != device_limit.end()) {
        Json::object stressed = device_limit["stressed"].object_items();
        if (stressed.find("applicationAllocated") != stressed.end()) {
            double application_allocated =
                stressed["applicationAllocated"].number_value();
            Json::object predictions;
            Json::object predictions_params =
                advisor_parameters_["predictions"].object_items();

            for (auto& it : predictions_params) {
                std::string key = it.first;

                Json metric_value =
                    GetValue(advice["metrics"].object_items(), key);
                Json device_limit_value = GetValue(device_limit, key);
                Json device_baseline_value = GetValue(device_baseline, key);
                Json baseline_value = GetValue(baseline_, key);

                if (metric_value.is_null() || device_limit_value.is_null() ||
                    device_baseline_value.is_null() ||
                    baseline_value.is_null()) {
                    continue;
                }

                double delta =
                    metric_value.number_value() - baseline_value.number_value();
                double device_delta = device_limit_value.number_value() -
                                      device_baseline_value.number_value();
                if (device_delta == 0) {
                    continue;
                }

                double percentage_estimate = delta / device_delta;

                predictions[key] =
                    application_allocated * (1 - percentage_estimate);
            }

            advice["predictions"] = predictions;
        }
    }
    advice["meta"] =
        (Json::object){{"duration", MillisecondsSinceEpoch() - start_time}};
    return advice;
}

Json MemoryAdviceImpl::GetValue(Json::object object, std::string key) {
    if (object.find(key) != object.end()) {
        return object[key];
    }
    for (auto& it : object) {
        Json value = GetValue(it.second.object_items(), key);
        if (!value.is_null()) {
            return value;
        }
    }
    return Json();
}

Json::object MemoryAdviceImpl::GenerateMetricsFromFields(Json::object fields) {
    Json::object metrics;
    for (auto& it : metrics_provider_->metrics_categories_) {
        if (fields.find(it.first) != fields.end()) {
            metrics[it.first] = ExtractValues(it.second, fields[it.first]);
        }
    }
    metrics["meta"] = (Json::object){{"time", MillisecondsSinceEpoch()}};
    return metrics;
}

Json::object MemoryAdviceImpl::ExtractValues(
    MetricsProvider::MetricsFunction metrics_function, Json fields) {
    double start_time = MillisecondsSinceEpoch();
    Json::object metrics = (metrics_provider_.get()->*metrics_function)();
    Json::object extracted_metrics;
    if (fields.bool_value()) {
        extracted_metrics = metrics;
    } else {
        for (auto& it : fields.object_items()) {
            if (it.second.bool_value() &&
                metrics.find(it.first) != metrics.end()) {
                extracted_metrics[it.first] = metrics[it.first];
            }
        }
    }

    extracted_metrics["_meta"] = {
        {"duration", Json(MillisecondsSinceEpoch() - start_time)}};
    return extracted_metrics;
}

double MemoryAdviceImpl::MillisecondsSinceEpoch() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
        .count();
}

Json::object MemoryAdviceImpl::GenerateVariableMetrics() {
    return GenerateMetricsFromFields(advisor_parameters_.at("metrics")
                                         .object_items()
                                         .at("variable")
                                         .object_items());
}

Json::object MemoryAdviceImpl::GenerateBaselineMetrics() {
    return GenerateMetricsFromFields(advisor_parameters_.at("metrics")
                                         .object_items()
                                         .at("baseline")
                                         .object_items());
}

Json::object MemoryAdviceImpl::GenerateConstantMetrics() {
    return GenerateMetricsFromFields(advisor_parameters_.at("metrics")
                                         .object_items()
                                         .at("constant")
                                         .object_items());
}

}  // namespace memory_advice
