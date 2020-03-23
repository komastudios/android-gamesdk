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

/**
 * On Linux systems (Android being just an example), temperature is approached under the notion of
 * thermal zones. A device may have several thermal zones, accessible from an especially designated
 * directory: /sys/class/thermal. Each thermal_zone is a directory or symlink to a directory named
 * "thermal_zone" followed by a number:
 *
 * /sys/class/thermal/thermal_zone[0-*]:
 *    |---type:         Type of the thermal zone (CPU, GPU, battery, ...)
 *    |---temp:         Current temperature
 *    |---...           Some more files we won't focus on
 *
 * The good news is that we got this designated folder, so we must not look further. The not so good
 * news is that it's not always easy to recognize which thermal zones relate to the device CPU.
 * That's because the type of a thermal zone doesn't follow any specific norm. It's free text that
 * could contain something as explicit as "cpu" or cryptic as "mng-mhn-23" (e.g., the chipset name).
 * Furthermore, not necessarily one unique thermal zone per device relates to the CPU. Especially in
 * manycore devices, there could be more cpu related thermal zones than cores in the device. That
 * said, it's really free the kind of associations between thermal zones, CPUs and its cores.
 *
 * This temperature module does its best trying to determine CPU-related thermal zones. It will
 * collect temperature from all of the detected ones.
 *
 * In the worst case scenario, if no CPU-related thermal zone could be identified (i.e., if no type
 * file in any thermal_zone folder contained the substring "cpu" (case insensitive), then
 * thermal_zone0 will be assumed as the CPU one.
 *
 *
 *
 * Links of interest:
 *
 * - Generic Thermal Sysfs driver How To
 *   https://www.kernel.org/doc/Documentation/thermal/sysfs-api.txt
 *
 * - Android hardware library, thermal module
 *   https://android.googlesource.com/platform/hardware/libhardware/+/master/include/hardware/thermal.h
 *   https://android.googlesource.com/platform/hardware/libhardware/+/master/modules/thermal/thermal.c
 */

#include "System.hpp"

#include <algorithm>
#include <fstream>
#include <list>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include <dirent.h>

#include "util/JNIHelpers.hpp"
#include "util/Log.hpp"

using namespace ancer;

//==================================================================================================
// Constants, types, type aliases and local variable declarations.

namespace {
//--------------------------------------------------------------------------------------------------
// Traces

Log::Tag TAG{"SystemTemperature"};

/**
 * This enum models the different statuses for this temperature capture system.
 */
enum class CaptureStatus {
  NotInitialized,   // When app launches
  Initializing,     // Happens only once per app launch
  Ready,            // to capture, after initialization
  Capturing         // (i.e., not ready to capture when is already doing it)
};
CaptureStatus _capture_status{CaptureStatus::NotInitialized};

//--------------------------------------------------------------------------------------------------
// Persistence and secondary storage for all things temperature.

using FileSystemPath = std::string;

// The CPU thermal zones catalog is a file containing the names of the identified CPU thermal zones.
FileSystemPath _cpu_thermal_zones_catalog;

/**
 * A device thermal zone. It mainly contains a type and a temperature file path.
 * The type is any arbitrary string. We focus, in particular, in those thermal zones whose types
 * contain the word "cpu" (case-insensitive).
 * The temperature file is where the OS stores the thermal zone temperatures (in Celsius
 * millidegrees).
 */
class ThermalZone {
 public:
  /**
   * Expected constructor.
   * @param path a string representing a system directory that must contain at least two files:
   *        "type" and "temp".
   */
  explicit ThermalZone(const FileSystemPath &path) : _path{path}, _type{ReadType(path)},
                                                     _temperature_path{path + "/temp"} {}

  ThermalZone(const ThermalZone &) = default;
  ThermalZone(ThermalZone &&) = default;

  ThermalZone &operator=(const ThermalZone &) = default;
  ThermalZone &operator=(ThermalZone &&other) = default;

  /**
   * Default constructor, only available to allow container allocation (e.g.,
   * std::vector::reserve()). A thermal zone constructed via this constructor isn't usable, as it
   * doesn't have any valid path. A thermal zone constructed this way is, in fact, a null object:
   *
   *   if (ThermalZone{}) {...}                                     // Skips the "then" block
   *   if (ThermalZone{"/sys/class/thermal/thermal_zone0"}) {...}   // Hits the "then" block
   */
  ThermalZone() = default;

  const std::string &Type() const {
    return _type;
  }

  const FileSystemPath &Path() const {
    return _path;
  }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
  const FileSystemPath &TemperaturePath() const {
    return _temperature_path;
  }
#pragma clang diagnostic pop

  /**
   * True if this thermal zone points to a non-empty path, and its type is known.
   */
  explicit inline operator bool() const {
    return not _path.empty() && not _type.empty();
  }

  /**
   * Retrieves this thermal zone temperature by accessing the OS file where its value is stored.
   *
   * @return if the reading goes OK, returns the value in Celsius millidegrees. Otherwise the
   *         constant ancer::UNKNOWN_TEMPERATURE_MEASUREMENT as an indication that the capture was
   *         unsuccessful.
   */
  TemperatureInCelsiusMillis CaptureTemperature() {
    ancer::TemperatureInCelsiusMillis captured_temperature{UNKNOWN_TEMPERATURE_MEASUREMENT};

    std::ifstream temperature_stream{_temperature_path};
    if (temperature_stream.is_open()) {
      std::string stream_line;
      if (std::getline(temperature_stream, stream_line)) {
        captured_temperature = std::stoi(stream_line);
        Log::V(TAG, "%s got %d Celsius millidegrees.", _path.c_str(), captured_temperature);
      } else {
        Log::W(TAG, "%s couldn't be read.", _temperature_path.c_str());
      }
    } else {
      Log::W(TAG, "%s couldn't be opened.", _temperature_path.c_str());
    }

    return captured_temperature;
  }

 private:
  /**
   * Invoked during instance constructions, it access the thermal zone type file and returns its
   * content.
   */
  std::string ReadType(const FileSystemPath &thermal_zone_path) {
    std::string type_content;
    FileSystemPath _type_path{thermal_zone_path + "/type"};

    std::ifstream type_stream{_type_path};
    if (type_stream.is_open()) {
      if (std::getline(type_stream, type_content)) {
        Log::I(TAG, "Thermal zone %s type is \"%s\".",
               thermal_zone_path.c_str(), type_content.c_str());
      } else {
        Log::E(TAG, "%s couldn't be read.", _type_path.c_str());
      }
    } else {
      Log::E(TAG, "%s couldn't be opened.", _type_path.c_str());
    }

    return type_content;
  }

 private:
  std::string _type;
  FileSystemPath _path;
  FileSystemPath _temperature_path;
};

std::list<ThermalZone> _cpu_thermal_zones;
}

//==================================================================================================
// Internal functions for CPU temperature detection and capture.

namespace {
/**
 * True if the current temperature capturing system is in the status received as parameter.
 */
bool IsCaptureStatus(const CaptureStatus status) {
  return _capture_status==status;
}

/**
 * Sets this temperature capture system status to the received parameter.
 */
void SetCaptureStatus(const CaptureStatus &status) {
  _capture_status = status;
}

//--------------------------------------------------------------------------------------------------
// CPU thermal zone catalog functions.

/**
 * Returns a string containing the path to the internal catalog were detected CPU temperatures are
 * stored. This catalog is written during the first launch after a clean install, and read
 * thereafter.
 */
const FileSystemPath &GetCPUThermalZonesCatalog() {
  if (_cpu_thermal_zones_catalog.empty()) {
    _cpu_thermal_zones_catalog = InternalDataPath() + "/cpu_thermal_zones.txt";
  }

  return _cpu_thermal_zones_catalog;
}

/**
 * True if no CPU thermal zones were detected.
 */
bool IsCPUThermalZonesCatalogEmpty() {
  return _cpu_thermal_zones.empty();
}

/**
 * Records the list of CPU thermal zones to its catalog.
 *
 * @return True if the list isn't empty.
 */
bool StoreTemperatureFileListToCatalog() {
  std::ofstream catalog_stream{};
  catalog_stream.exceptions(catalog_stream.exceptions() | std::ios::failbit);
  const auto &catalog_path{GetCPUThermalZonesCatalog()};

  try {
    catalog_stream.open(catalog_path, std::ios::trunc);
    for (const auto &cpu_thermal_zone : _cpu_thermal_zones) {
      catalog_stream << cpu_thermal_zone.Path() << std::endl;
    }
    Log::I(TAG, "CPU thermal zone list stored to catalog %s.", catalog_path.c_str());
  } catch (std::ios_base::failure &e) {
    Log::E(TAG, "CPU thermal zone catalog %s write open failure: \"%s\".",
           catalog_path.c_str(), e.what());
  }

  return not IsCPUThermalZonesCatalogEmpty();
}

/**
 * Restores the list of CPU thermal zones from its catalog.
 *
 * @return true if the catalog wasn't empty.
 */
bool RetrieveCPUThermalZoneListFromCatalog() {
  std::ifstream catalog_stream{};
  catalog_stream.exceptions(catalog_stream.exceptions() | std::ios::failbit);
  const auto &catalog_path{GetCPUThermalZonesCatalog()};

  try {
    catalog_stream.open(catalog_path);
    _cpu_thermal_zones.clear();
    for (std::string thermal_zone_path; std::getline(catalog_stream, thermal_zone_path);) {
      Log::I(TAG, "Retrieving thermal zone path %s.", thermal_zone_path.c_str());
      _cpu_thermal_zones.emplace_back(std::move(thermal_zone_path));
    }
  } catch (std::ios_base::failure &e) {
    Log::I(TAG, "CPU thermal zone catalog %s read open failure: \"%s\".",
           catalog_path.c_str(), e.what());
  }

  return not IsCPUThermalZonesCatalogEmpty();
}

//--------------------------------------------------------------------------------------------------
// CPU thermal zone detection functions (all via regex patterns)

/**
 * True if a subdirectory follows the pattern "thermal_zone<SEQUENTIAL NUMBER>".
 */
bool IsThermalZoneSubdir(const std::string &subdirectory) {
  static const std::regex THERMAL_ZONE_REGEX{R"(thermal_zone\d+)"};
  return std::regex_match(subdirectory, THERMAL_ZONE_REGEX);
}

/**
 * True if the thermal zone type has some hint that suggests is linked to the CPU.
 */
bool IsCpuRelated(const ThermalZone &thermal_zone) {
  // If you wonder why a vector for just one entry: for now we have identified one single regular
  // expression for cpu's. Although in certain devices no thermal zone type explicitly matches it.
  // We foresee that over time we'll identify alternative patterns. If that happens at all, then
  // we'll append these to this vector.
  static const std::vector<std::regex> CPU_TYPE_REGEXES{
      std::regex{".*cpu.*", std::regex::icase}
  };

  bool resolution{false};

  for (const auto &cpu_type_regex : CPU_TYPE_REGEXES) {
    if (std::regex_match(thermal_zone.Type(), cpu_type_regex)) {
      resolution = true;
      break;
    }
  }

  return resolution;
}

//==================================================================================================

/**
 * Scans a specific system directory looking for thermal zone subdirectories.
 *
 * @return the list of the device available thermal zones.
 */
std::list<ThermalZone> ArrangeDeviceThermalZoneList() {
  static const FileSystemPath thermal_zones_dir{"/sys/class/thermal/"};

  std::list<ThermalZone> thermal_zones;

  auto *directory = opendir(thermal_zones_dir.c_str());
  if (directory) {
    struct dirent *entry;
    while ((entry = readdir(directory))) {
      switch (entry->d_type) {
        case DT_DIR:
        case DT_LNK: {
          if (IsThermalZoneSubdir(entry->d_name)) {
            thermal_zones.emplace_back(thermal_zones_dir + entry->d_name);
          }
          break;
        }

        default:break;
      }
    }
  } else {
    Log::V(TAG, "Access to directory %s error: %d", thermal_zones_dir.c_str(), errno);
  }
  closedir(directory);

  return thermal_zones;
}

/**
 * Receives a list of thermal zones and strips the ones that are not CPU-related.
 *
 * @param thermal_zones a list of thermal zones.
 * @return the same list, but without those entries that weren't CPU-related. If no thermal zone
 *         seems to be CPU-related, it defaults to the first thermal zone retrieved.
 */
std::list<ThermalZone> FilterCPUThermalZones(std::list<ThermalZone> &&thermal_zones) {
  // Backup plan: if no thermal zone type is "CPU", we'll default on the first thermal zone.
  ThermalZone default_thermal_zone;
  if (not thermal_zones.empty()) { default_thermal_zone = thermal_zones.front(); }

  thermal_zones.remove_if([](const auto &thermal_zone) {
    return not IsCpuRelated(thermal_zone);
  });

  if (thermal_zones.empty() && default_thermal_zone) {
    thermal_zones.emplace_back(std::move(default_thermal_zone));
  }

  return thermal_zones;
}

/**
 * Inspects all thermal zones in the device, filtering those whose types indicate that they relate
 * to the CPU. The temperatures of these CPU thermal zones will be captured during test execution.
 *
 * @return true if at least one CPU thermal zone was detected.
 */
bool DetectCPURelatedThermalZones() {
  _cpu_thermal_zones = std::move(FilterCPUThermalZones(ArrangeDeviceThermalZoneList()));
  return not IsCPUThermalZonesCatalogEmpty();
}
}

//==================================================================================================

bool ancer::internal::InitTemperatureCapture(const bool run_async) {
  if (not IsCaptureStatus(CaptureStatus::NotInitialized)) {
    return false;
  }
  SetCaptureStatus(CaptureStatus::Initializing);

  if (IsCPUThermalZonesCatalogEmpty()) {
    if (not RetrieveCPUThermalZoneListFromCatalog()) {
      static auto initialization_steps = []() {
        DetectCPURelatedThermalZones();
        StoreTemperatureFileListToCatalog();
        SetCaptureStatus(CaptureStatus::Ready);
      };

      if (run_async) {
        std::thread{initialization_steps}.detach();
      } else {
        initialization_steps();
      }
      return true;
    }
  }

  SetCaptureStatus(CaptureStatus::Ready);
  return false;
}

//--------------------------------------------------------------------------------------------------

ancer::TemperatureInCelsiusMillis ancer::CaptureMaxTemperature() {
  ancer::TemperatureInCelsiusMillis max_temperature{ancer::UNKNOWN_TEMPERATURE_MEASUREMENT};

  if (IsCaptureStatus(CaptureStatus::Ready)) {
    SetCaptureStatus(CaptureStatus::Capturing);
    for (auto &cpu_thermal_zone : _cpu_thermal_zones) {
      max_temperature = std::max(max_temperature, cpu_thermal_zone.CaptureTemperature());
    }
    SetCaptureStatus(CaptureStatus::Ready);
  }

  return max_temperature;
}

//--------------------------------------------------------------------------------------------------

ThermalStatus ancer::GetThermalStatus() {
  ThermalStatus status = ThermalStatus::Unknown;

  jni::SafeJNICall(
      [&status](jni::LocalJNIEnv *env) {
        static constexpr int THERMAL_STATUS_MIN{0};
        static constexpr int THERMAL_STATUS_MAX{6};

        jobject activity = env->NewLocalRef(jni::GetActivityWeakGlobalRef());
        jclass activity_class = env->GetObjectClass(activity);

        // get the memory info object from our activity

        jmethodID get_system_helpers_mid = env->GetMethodID(
            activity_class,
            "getSystemHelpers",
            "()Lcom/google/gamesdk/gamecert/operationrunner/util/SystemHelpers;"
        );
        jobject system_helpers_instance = env->CallObjectMethod(
            activity,
            get_system_helpers_mid);
        jclass system_helpers_class =
            env->GetObjectClass(system_helpers_instance);

        jmethodID get_thermal_status_mid = env->GetMethodID(
            system_helpers_class, "getCurrentThermalStatus", "()I");

        // we assume that the java api will return one of the defined thermal
        // status values, but the api is new as of Q and AFAIK only supported on
        // pixel
        jint status_int =
            env->CallIntMethod(system_helpers_instance, get_thermal_status_mid);
        if (status_int >= THERMAL_STATUS_MIN && status_int <= THERMAL_STATUS_MAX) {
          status = static_cast<ThermalStatus>(status_int);
        }
      });

  return status;
}
