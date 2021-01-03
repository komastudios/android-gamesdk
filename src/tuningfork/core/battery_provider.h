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

#include "common.h"

namespace tuningfork {

// Provider for system battery information.
class IBatteryProvider {
   public:
    virtual ~IBatteryProvider() {}
    virtual int32_t GetBatteryPercentage() = 0;
    virtual int32_t GetBatteryCharge() = 0;
    virtual bool IsBatteryCharging() = 0;
    virtual bool IsBatteryReportingEnabled() = 0;
};

// Implementation that uses JNI calls to ActivityManager.
class DefaultBatteryProvider : public IBatteryProvider {
   public:
    int32_t GetBatteryPercentage() override;
    int32_t GetBatteryCharge() override;
    bool IsBatteryCharging() override;
    bool IsBatteryReportingEnabled() override;
};

}  // namespace tuningfork