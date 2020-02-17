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

// The temperature files catalog is a file containing the names of the identified temperature files
// to track.
FileSystemPath _temperature_files_catalog;

/**
 * This type mainly holds a temperature file path. The first time the app runs (i.e., clean install)
 * this structure is created per identified temperature file after a scanning process. Yet, certain
 * identified files don't really track a temperature. This type indicates, after a whole operation
 * completed, whether the temperature this file is supposed to track has ever changed. If not, this
 * file is removed from the catalog.
 */
class TrackedTemperatureFile {
 public:
  TrackedTemperatureFile(const FileSystemPath &path) :
      _path{path} {
  }

  TrackedTemperatureFile(FileSystemPath &&path) :
      _path{std::move(path)} {
  }

  const FileSystemPath &Path() const {
    return _path;
  }

  /**
   * Parses the content of the file at its path in order to capture the temperature it measures.
   *
   * @return if the reading goes OK, returns the value in Celsius millidegrees. Otherwise the
   * constant ancer::UNKNOWN_TEMPERATURE_MEASUREMENT as an indication that the capture was
   * unsuccessful.
   */
  TemperatureInCelsiusMillis Capture() {
    ancer::TemperatureInCelsiusMillis captured_temperature{UNKNOWN_TEMPERATURE_MEASUREMENT};

    std::ifstream temperature_stream{_path};
    if (temperature_stream.is_open()) {
      std::string stream_line;
      if (std::getline(temperature_stream, stream_line)) {
        captured_temperature = std::stoi(stream_line);
        Log::V(TAG, "%s got %d Celsius millidegrees.", _path.c_str(), captured_temperature);
      } else {
        Log::W(TAG, "%s couldn't be read.", _path.c_str());
      }
    } else {
      Log::W(TAG, "%s couldn't be opened.", _path.c_str());
    }

    return captured_temperature;
  }

 private:
  FileSystemPath _path;
};

std::vector<TrackedTemperatureFile> _tracked_temperature_files;

//------------------------------------------------------------------------------

class ThermalZone {
 public:
  ThermalZone() = default;
  explicit ThermalZone(const FileSystemPath &path) : _type{ReadType(path)},
                                                     _temperature_path{path + "/temp"} {}

  ThermalZone(const ThermalZone &) = default;
  ThermalZone(ThermalZone &&) = default;

  ThermalZone &operator=(const ThermalZone &) = default;
  ThermalZone &operator=(ThermalZone &&other) = default;

  const std::string &Type() const {
    return _type;
  }

  const FileSystemPath &TemperaturePath() const {
    return _temperature_path;
  }

  operator bool() const {
    // TODO(dagum): check also that temp file exists. Check full readiness
    return not _type.empty() && not _temperature_path.empty();
  }

 private:
  std::string ReadType(const FileSystemPath &thermal_zone_path) {
    std::string type_content;
    FileSystemPath _type_path{thermal_zone_path + "/type"};

    std::ifstream type_stream{_type_path};
    if (type_stream.is_open()) {
      if (std::getline(type_stream, type_content)) {
        Log::I(TAG, "%s type is \"%s\".", thermal_zone_path.c_str(), type_content.c_str());
      } else {
        Log::W(TAG, "%s couldn't be read.", _type_path.c_str());
      }
    } else {
      Log::W(TAG, "%s couldn't be opened.", _type_path.c_str());
    }

    return type_content;
  }

 private:
  std::string _type;
  FileSystemPath _temperature_path;
};
}

//==================================================================================================

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
// Tracked files and catalog functions.

/**
 * Returns a string containing the path to the internal catalog were detected temperature files are
 * stored. This catalog is written the first time post-clean install, with the results of a full
 * scan for temperature file candidates.
 */
const FileSystemPath &GetTemperatureFilesCatalog() {
  if (_temperature_files_catalog.empty()) {
    _temperature_files_catalog = InternalDataPath() + "/temperature_files.txt";
  }

  return _temperature_files_catalog;
}

/**
 * True if no temperature files were detected.
 */
bool IsTrackedListEmpty() {
  return _tracked_temperature_files.empty();
}

/**
 * Records the list of tracked temperature files to its catalog.
 * @return True if the list isn't empty.
 */
bool StoreTemperatureFileListToCatalog() {
  std::ofstream catalog_stream{};
  catalog_stream.exceptions(catalog_stream.exceptions() | std::ios::failbit);
  const auto &catalog_path{GetTemperatureFilesCatalog()};

  try {
    catalog_stream.open(catalog_path, std::ios::trunc);
    for (const auto &tracked_file : _tracked_temperature_files) {
      catalog_stream << tracked_file.Path() << std::endl;
    }
    Log::I(TAG, "Temperature file list stored to catalog %s.", catalog_path.c_str());
  } catch (std::ios_base::failure &e) {
    Log::E(TAG, "Temperature file catalog %s write open failure: \"%s\".",
           catalog_path.c_str(), e.what());
  }

  return not IsTrackedListEmpty();
}

/**
 * Restores the list of tracked temperature files from its catalog.
 * @return true if the catalog wasn't empty.
 */
bool RetrieveTemperatureFileListFromCatalog() {
  std::ifstream catalog_stream{};
  catalog_stream.exceptions(catalog_stream.exceptions() | std::ios::failbit);
  const auto &catalog_path{GetTemperatureFilesCatalog()};

  try {
    catalog_stream.open(catalog_path);
    _tracked_temperature_files.clear();
    for (std::string temperature_file_path; std::getline(catalog_stream, temperature_file_path);) {
      Log::I(TAG, "Retrieving temperature file path %s.", temperature_file_path.c_str());
      _tracked_temperature_files.emplace_back(std::move(temperature_file_path));
    }
  } catch (std::ios_base::failure &e) {
    Log::I(TAG, "Temperature file catalog %s read open failure: \"%s\".",
           catalog_path.c_str(), e.what());
  }

  return not IsTrackedListEmpty();
}

//==================================================================================================

bool IsThermalZoneSubdir(const std::string &subdirectory) {
  static const std::regex THERMAL_ZONE_REGEX{R"(thermal_zone\d+)"};
  return std::regex_match(subdirectory, THERMAL_ZONE_REGEX);
}

bool IsThermalZone0(const ThermalZone &thermal_zone) {
  static const std::regex THERMAL_ZONE0_REGEX{"(.*)/thermal_zone0/(.*)"};
  return std::regex_match(thermal_zone.TemperaturePath(), THERMAL_ZONE0_REGEX);
}

bool IsCpu(const std::string &type) {
  // If you wonder why a vector for just one entry: for now we have identified one single regular
  // expression for cpu's. Although in certain devices no thermal zone type explicitly matches it.
  // We foresee that over time we'll identify alternative patterns. Then we'll append these to this
  // vector.
  static const std::vector<std::regex> CPU_TYPE_REGEXES{
      std::regex{"(.*)cpu(.*)", std::regex::icase}
  };

  bool resolution{false};

  for (const auto &cpu_type_regex : CPU_TYPE_REGEXES) {
    if (std::regex_match(type, cpu_type_regex)) {
      resolution = true;
      break;
    }
  }

  return resolution;
}

//==================================================================================================

auto FindThermalZones() {
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

        default: {
          // NO-OP
        }
      }
    }
  } else {
    Log::V(TAG, "Access to directory %s error: %d", thermal_zones_dir.c_str(), errno);
  }
  closedir(directory);

  return thermal_zones;
}

std::list<ThermalZone> FilterCPUThermalZones(std::list<ThermalZone> &&thermal_zones) {
  // Backup plan: if no thermal zone type is "CPU", we default on thermal zone 0.
  ThermalZone thermal_zone0;

  thermal_zones.remove_if([&thermal_zone0](const auto &thermal_zone) {
    if (IsThermalZone0(thermal_zone)) {
      thermal_zone0 = thermal_zone;
    }

    return not IsCpu(thermal_zone.Type());
  });

  if (thermal_zones.empty()) {
    thermal_zones.emplace_back(std::move(thermal_zone0));
  }

  return thermal_zones;
}

void SetTemperatureFilesToTrack(const std::list<ThermalZone> &thermal_zones) {
  _tracked_temperature_files.clear();
  std::for_each(std::cbegin(thermal_zones), std::cend(thermal_zones),
                [](const auto &thermal_zone) {
                  if (thermal_zone) {
                    _tracked_temperature_files.push_back(
                        TrackedTemperatureFile(thermal_zone.TemperaturePath())
                    );
                  } // TODO(dagum): else log error couldn't have happened
                });
}

bool ScanThermalZonesToDetectCPUTemperatures() {
  // If read backwards, this one-liner tells all:
  // 1) It finds all device thermal zones.
  // 2) It filters the ones related to the CPU.
  // 3) It sets the temperature files for these filtered zones to track during tests.
  SetTemperatureFilesToTrack(FilterCPUThermalZones(FindThermalZones()));
  return not IsTrackedListEmpty();
}
}

//==================================================================================================

bool ancer::internal::InitTemperatureCapture(const bool run_async) {
  if (not IsCaptureStatus(CaptureStatus::NotInitialized)) {
    return false;
  }
  SetCaptureStatus(CaptureStatus::Initializing);

  if (IsTrackedListEmpty()) {
    if (not RetrieveTemperatureFileListFromCatalog()) {
      static auto initialization_steps = []() {
        ScanThermalZonesToDetectCPUTemperatures();
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

std::vector<ancer::TemperatureInCelsiusMillis> ancer::CaptureTemperatures() {
  std::vector<ancer::TemperatureInCelsiusMillis> temperature_measurements;

  if (IsCaptureStatus(CaptureStatus::Ready)) {
    SetCaptureStatus(CaptureStatus::Capturing);
    for (auto &temperature_file : _tracked_temperature_files) {
      temperature_measurements.push_back(temperature_file.Capture());
    }
    SetCaptureStatus(CaptureStatus::Ready);
  } else {
    for (auto &temperature_file : _tracked_temperature_files) {
      temperature_measurements.push_back(ancer::UNKNOWN_TEMPERATURE_MEASUREMENT);
    }
  }

  return temperature_measurements;
}

//--------------------------------------------------------------------------------------------------

ThermalStatus ancer::GetThermalStatus() {
  ThermalStatus status = ThermalStatus::Unknown;

  jni::SafeJNICall(
      [&status](jni::LocalJNIEnv *env) {
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
        if (status_int >= 0 && status_int <= 6) {
          status = static_cast<ThermalStatus>(status_int);
        }
      });

  return status;
}
