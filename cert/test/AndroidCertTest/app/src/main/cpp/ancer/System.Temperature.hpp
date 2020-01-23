/*
 * Copyright 2019 The Android Open Source Project
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

#ifndef _SYSTEM_TEMPERATURE_HPP
#define _SYSTEM_TEMPERATURE_HPP

#include <vector>

namespace ancer {
namespace internal {
/**
 * The first time after clean install, it asynchronously scans the OS /sys folder for temperature
 * files. Once the get identified, subsequent executions of this function won't re-scan again.
 * The scanning process discards any file whose name or path contains certain words like battery,
 * gyroscope, etc. The goal is to track CPU temperatures exclusively.
 * The scanning process also discards files whose content is anything other than a single number
 * representing a reasonable temperature in Celsius millidegrees. By reasonable, we understand any
 * value between 25°C and 60°C.
 *
 * @param run_async if true, the scan for temperature files happens on a background thread.
 *                  Attempting to capture temperatures won't succeed until the background scan
 *                  finishes.
 * @return true if a file scanning was required. False if the results of a previous scan were
 *              available.
 */
bool InitTemperatureCapture(const bool run_async = false);

/**
 * Performs a final analysis about tracked temperature files. If any of them hasn't experienced
 * changes during the operation execution, it's considered an invalid temperature file and removed
 * from the catalog for future executions.
 */
void DeinitTemperatureCapture();
}

/**
 * Direct mapping of the thermal constants defined in android.os.PowerManager:
 * https://developer.android.com/reference/android/os/PowerManager.html
 */
enum class ThermalStatus {
  None = 0,
  Light = 1,
  Moderate = 2,
  Severe = 3,
  Critical = 4,
  Emergency = 5,
  Shutdown = 6,
  Unknown = -1 // added for unsupported API/devices
};

// TODO(tmillican@google.com): No reason these need to be std::strings.
inline std::string to_string(ThermalStatus status) {
  switch (status) {
    case ThermalStatus::None: return "none";
    case ThermalStatus::Light: return "light";
    case ThermalStatus::Moderate: return "moderate";
    case ThermalStatus::Severe: return "severe";
    case ThermalStatus::Critical: return "critical";
    case ThermalStatus::Emergency: return "emergency";
    case ThermalStatus::Shutdown: return "shutdown";
    case ThermalStatus::Unknown: return "unknown";
  }
}

inline std::ostream &operator<<(std::ostream &os, ThermalStatus status) {
  os << to_string(status);
  return os;
}

/**
 * (Android Q and up) Gets the current thermal status as reported by the android powermanager.
 * Note that this still returns -1 (unknown) on the majority of devices.
 */
ThermalStatus GetThermalStatus();

using TemperatureInCelsiusMillis = int;

const TemperatureInCelsiusMillis UNKNOWN_TEMPERATURE_MEASUREMENT{-99999};

/**
 * When the capture status is ready, it captures all temperature measurements from temperature files
 * being tracked. If any of the files isn't temporarily available or the capture status isn't ready,
 * a measurement of UNKNOWN_TEMPERATURE_MEASUREMENT is returned for that file.
 *
 * @return a vector containing temperature measurements in Celsius millidegrees. Each entry
 * corresponds to an identified temperature file being tracked.
 */
std::vector<TemperatureInCelsiusMillis> CaptureTemperatures();
}

#endif // _SYSTEM_TEMPERATURE_HPP