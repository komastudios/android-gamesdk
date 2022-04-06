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

#include <algorithm>
#include <chrono>

#include "memory_advice_utils.h"
#include "system_utils.h"

constexpr double BYTES_IN_GB = 1024 * 1024 * 1024;

namespace memory_advice {

using namespace json11;

MemoryAdviceImpl::MemoryAdviceImpl(const char* params,
                                   IMetricsProvider* metrics_provider,
                                   IPredictor* realtime_predictor,
                                   IPredictor* available_predictor)
    : metrics_provider_(metrics_provider),
      realtime_predictor_(realtime_predictor),
      available_predictor_(available_predictor) {
    if (metrics_provider_ == nullptr) {
        default_metrics_provider_ = std::make_unique<DefaultMetricsProvider>();
        metrics_provider_ = default_metrics_provider_.get();
    }
    if (realtime_predictor_ == nullptr) {
        default_realtime_predictor_ = std::make_unique<DefaultPredictor>();
        realtime_predictor_ = default_realtime_predictor_.get();
    }
    if (available_predictor_ == nullptr) {
        default_available_predictor_ = std::make_unique<DefaultPredictor>();
        available_predictor_ = default_available_predictor_.get();
    }

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
    initialization_error_code_ = ProcessAdvisorParameters(params);
    if (initialization_error_code_ != MEMORYADVICE_ERROR_OK) {
        return;
    }
    baseline_ = GenerateBaselineMetrics();
    baseline_["constant"] = GenerateConstantMetrics();
    build_ = utils::GetBuildInfo();
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
    Json::object advice = GetAdvice();
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
    // TODO(b/219040574): this is not a reliable/available number currently
    Json::object advice = GetAdvice();
    if (advice.find("metrics") != advice.end()) {
        Json::object metrics = advice["metrics"].object_items();
        if (metrics.find("predictedAvailable") != metrics.end()) {
            return static_cast<int64_t>(
                metrics["predictedAvailable"].number_value());
        }
    }
    return 0L;
}

float MemoryAdviceImpl::GetPercentageAvailableMemory() {
    Json::object advice = GetAdvice();
    if (advice.find("metrics") == advice.end()) return 0.0f;
    Json::object metrics = advice["metrics"].object_items();
    if (metrics.find("predictedUsage") == metrics.end()) return 0.0f;
    return 100.0f * (1.0f - static_cast<float>(
                                metrics["predictedUsage"].number_value()));
}

Json::object MemoryAdviceImpl::GetAdvice() {
    CheckCancelledWatchers();

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
    data["build"] = build_;

    if (variable_spec.find("predictRealtime") != variable_spec.end() &&
        variable_spec.at("predictRealtime").bool_value()) {
        variable_metrics["predictedUsage"] =
            Json(realtime_predictor_->Predict(data));
    }

    if (variable_spec.find("availableRealtime") != variable_spec.end() &&
        variable_spec.at("availableRealtime").bool_value()) {
        variable_metrics["predictedAvailable"] =
            Json(BYTES_IN_GB * available_predictor_->Predict(data));
    }
    Json::array warnings;
    Json::object heuristics =
        advisor_parameters_.at("heuristics").object_items();

    if (heuristics.find("formulas") != heuristics.end()) {
        for (auto& entry : heuristics["formulas"].object_items()) {
            for (auto& formula_object : entry.second.array_items()) {
                std::string formula = formula_object.string_value();
                formula.erase(std::remove_if(formula.begin(), formula.end(),
                                             (int (*)(int))std::isspace),
                              formula.end());
                if (utils::EvaluateBoolean(formula, variable_metrics)) {
                    Json::object warning;
                    warning["formula"] = formula;
                    warning["level"] = entry.first;
                    warnings.push_back(warning);
                }
            }
        }
    }

    if (!warnings.empty()) {
        advice["warnings"] = warnings;
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
    IMetricsProvider::MetricsFunction metrics_function, Json fields) {
    double start_time = MillisecondsSinceEpoch();
    Json::object metrics = (metrics_provider_->*metrics_function)();
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

MemoryAdvice_ErrorCode MemoryAdviceImpl::RegisterWatcher(
    uint64_t intervalMillis, MemoryAdvice_WatcherCallback callback,
    void* user_data) {
    std::lock_guard<std::mutex> guard(active_watchers_mutex_);
    active_watchers_.push_back(std::make_unique<StateWatcher>(
        this, callback, user_data, intervalMillis));
    return MEMORYADVICE_ERROR_OK;
}

MemoryAdvice_ErrorCode MemoryAdviceImpl::UnregisterWatcher(
    MemoryAdvice_WatcherCallback callback) {
    // We can't simply erase the watcher because the callback thread might still
    // be running which would mean blocking here for it to finish. So signal to
    // the thread to exit and put the StateWatcher object on a cancelled list
    // that is checked periodically.
    std::lock_guard<std::mutex> guard(active_watchers_mutex_);
    std::vector<WatcherContainer::iterator> to_move;
    // Search for watchers with the same callback.
    for (auto it = active_watchers_.begin(); it != active_watchers_.end();
         ++it) {
        if ((*it)->Callback() == callback) {
            to_move.push_back(it);
        }
    }

    if (to_move.empty()) return MEMORYADVICE_ERROR_WATCHER_NOT_FOUND;

    std::lock_guard<std::mutex> guard2(cancelled_watchers_mutex_);
    for (auto it : to_move) {
        (*it)->Cancel();
        cancelled_watchers_.push_back(
            std::move(*it));  // Put StateWatcher on cancelled list.
        active_watchers_.erase(it);
    }
    return MEMORYADVICE_ERROR_OK;
}

void MemoryAdviceImpl::CheckCancelledWatchers() {
    std::lock_guard<std::mutex> guard(cancelled_watchers_mutex_);
    auto it = cancelled_watchers_.begin();
    while (it != cancelled_watchers_.end()) {
        if (!(*it)->ThreadRunning())
            it = cancelled_watchers_.erase(
                it);  // Calls destructor on StateWatcher
        else
            ++it;
    }
}

}  // namespace memory_advice
