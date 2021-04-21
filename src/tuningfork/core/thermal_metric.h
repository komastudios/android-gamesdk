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

#include <android/api-level.h>

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "battery_provider.h"
#include "histogram.h"
#include "jni/jni_wrap.h"
#include "metricdata.h"
#include "settings.h"
#include "system_utils.h"

namespace tuningfork {

struct ThermalMetric {
    typedef enum ThermalState {
        // Device had no thermal state data, due to low api level.
        THERMAL_STATE_UNSPECIFIED = 0,
        // No thermal problems.
        THERMAL_STATE_NONE = 1,
        // Light throttling where UX is not impacted.
        THERMAL_STATE_LIGHT = 2,
        // Moderate throttling where UX is not largely impacted.
        THERMAL_STATE_MODERATE = 3,
        // Severe throttling where UX is largely impacted.
        THERMAL_STATE_SEVERE = 4,
        // Platform has done everything to reduce power.
        THERMAL_STATE_CRITICAL = 5,
        // Key components in platform are shutting down due to thermal
        // condition.
        // Device functionalities are limited.
        THERMAL_STATE_EMERGENCY = 6,
        // Indicates that the device needs shutdown immediately.
        THERMAL_STATE_SHUTDOWN = 7,
    } ThermalState;

    ThermalState thermal_state_;
    Duration time_since_process_start_;

    ThermalMetric(ThermalState thermal_state, Duration time_since_process_start)
        : thermal_state_(thermal_state),
          time_since_process_start_(time_since_process_start) {}
};

struct ThermalMetricData : public MetricData {
    MetricId metric_id_;
    std::vector<ThermalMetric> data_;

    ThermalMetricData(MetricId metric_id)
        : MetricData(MetricType()), metric_id_(metric_id) {}

    void Record(Duration time_since_process_start) {
        ThermalMetric metric(GetThermalState(), time_since_process_start);
        data_.push_back(metric);
    }

    virtual void Clear() override { data_.erase(data_.begin(), data_.end()); }
    virtual size_t Count() const override { return data_.size(); }
    static Metric::Type MetricType() { return Metric::Type::THERMAL; }

    ThermalMetric::ThermalState GetThermalState() {
        if (gamesdk::GetSystemPropAsInt("ro.build.version.sdk") >= 29 &&
            gamesdk::jni::IsValid()) {
            using namespace gamesdk::jni;
            java::Object obj = AppContext().getSystemService(
                android::content::Context::POWER_SERVICE);
            CHECK_FOR_JNI_EXCEPTION_AND_RETURN(
                ThermalMetric::THERMAL_STATE_UNSPECIFIED);
            if (!obj.IsNull()) {
                android::os::PowerManager power_manager(std::move(obj));
                int status = power_manager.getCurrentThermalStatus();
                if (status < 0 || status > 6) {
                    return ThermalMetric::THERMAL_STATE_UNSPECIFIED;
                } else {
                    // Adding +1 here because we want 0 to be thermal status
                    // 'unspecified', which shifts the entire enum by 1
                    return static_cast<ThermalMetric::ThermalState>(status + 1);
                }
            } else {
                return ThermalMetric::THERMAL_STATE_UNSPECIFIED;
            }
        } else {
            return ThermalMetric::THERMAL_STATE_UNSPECIFIED;
        }
    }
};

}  // namespace tuningfork
