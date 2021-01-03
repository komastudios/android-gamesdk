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

#include <android/api-level.h>

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "histogram.h"
#include "jni/jni_wrap.h"
#include "metricdata.h"
#include "settings.h"

namespace tuningfork {

struct BatteryMetric {
    int32_t percentage_, current_charge_;
    Duration time_since_process_start_;
    bool app_on_foreground_, is_charging_;
};

struct BatteryMetricData : public MetricData {
    MetricId metric_id_;
    std::vector<BatteryMetric> data_;

    BatteryMetricData(MetricId metric_id)
        : MetricData(MetricType()), metric_id_(metric_id) {}

    void Record(bool app_on_foreground, Duration time_since_process_start) {
        std::ostringstream out;
        out << time_since_process_start.count();
        using namespace gamesdk::jni;
        BatteryMetric metric;
        metric.app_on_foreground_ = app_on_foreground;
        metric.current_charge_ = GetBatteryCharge();
        metric.time_since_process_start_ = time_since_process_start;

        android::content::BroadcastReceiver broadcast_receiver(nullptr);
        android::content::IntentFilter intent_filter(
            android::content::Intent::ACTION_BATTERY_CHANGED);
        java::Object obj =
            AppContext().registerReceiver(broadcast_receiver, intent_filter);
        if (!obj.IsNull()) {
            android::content::Intent battery_intent(std::move(obj));
            metric.percentage_ =
                (100 * battery_intent.getIntExtra(
                           android::os::BatteryManager::EXTRA_LEVEL, 0)) /
                battery_intent.getIntExtra(
                    android::os::BatteryManager::EXTRA_SCALE, 100);
            metric.is_charging_ = battery_intent.getIntExtra(
                android::os::BatteryManager::EXTRA_PLUGGED, 0);
        } else {
            metric.percentage_ = 0;
            metric.is_charging_ = false;
        }
        metric.current_charge_ = GetBatteryCharge();

        data_.push_back(metric);
    }

    int GetBatteryCharge() {
#if __ANDROID_API__ >= 21
        using namespace gamesdk::jni;
        java::Object obj = AppContext().getSystemService(
            android::content::Context::BATTERY_SERVICE);
        if (!obj.IsNull()) {
            android::os::BatteryManager battery_manager(std::move(obj));
            return battery_manager.getIntProperty(
                android::os::BatteryManager::BATTERY_PROPERTY_CHARGE_COUNTER);
        } else {
            return 0;
        }
#else
        return 0;
#endif
    }

    virtual void Clear() override { data_.erase(data_.begin(), data_.end()); }
    virtual size_t Count() const override { return data_.size(); }
    static Metric::Type MetricType() { return Metric::Type::BATTERY; }
};

}  // namespace tuningfork
