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

#include "battery_provider.h"

#include "jni/jni_wrap.h"

namespace tuningfork {

int32_t DefaultBatteryProvider::GetBatteryPercentage() {
    using namespace gamesdk::jni;

    android::content::BroadcastReceiver broadcast_receiver(nullptr);
    android::content::IntentFilter intent_filter(
        android::content::Intent::ACTION_BATTERY_CHANGED);
    java::Object obj =
        AppContext().registerReceiver(broadcast_receiver, intent_filter);
    if (!obj.IsNull()) {
        android::content::Intent battery_intent(std::move(obj));
        return (100 * battery_intent.getIntExtra(
                          android::os::BatteryManager::EXTRA_LEVEL, 0)) /
               battery_intent.getIntExtra(
                   android::os::BatteryManager::EXTRA_SCALE, 100);
    } else {
        return 0;
    }
}

int32_t DefaultBatteryProvider::GetBatteryCharge() {
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

bool DefaultBatteryProvider::IsBatteryCharging() {
    using namespace gamesdk::jni;

    android::content::BroadcastReceiver broadcast_receiver(nullptr);
    android::content::IntentFilter intent_filter(
        android::content::Intent::ACTION_BATTERY_CHANGED);
    java::Object obj =
        AppContext().registerReceiver(broadcast_receiver, intent_filter);
    if (!obj.IsNull()) {
        android::content::Intent battery_intent(std::move(obj));
        return battery_intent.getIntExtra(
            android::os::BatteryManager::EXTRA_PLUGGED, 0);
    } else {
        return false;
    }
}

bool DefaultBatteryProvider::IsBatteryReportingEnabled() { return true; }

}  // namespace tuningfork