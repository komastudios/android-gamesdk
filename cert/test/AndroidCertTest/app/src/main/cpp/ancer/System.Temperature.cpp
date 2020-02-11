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

#include "System.hpp"

#include <algorithm>
#include <fstream>
#include <queue>
#include <regex>
#include <string>
#include <thread>
#include <vector>
#include <utility>

#include <sys/types.h>
#include <dirent.h>

#include "util/JNIHelpers.hpp"
#include "util/Log.hpp"

using namespace ancer;

//==============================================================================
// Constants, types, type aliases and local variable declarations.
namespace {
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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
  TrackedTemperatureFile(const FileSystemPath &path, const bool has_changed = false) :
      _path{path}, _temperature{UNKNOWN_TEMPERATURE_MEASUREMENT}, _has_changed{has_changed} {
  }

  TrackedTemperatureFile(FileSystemPath &&path, const bool has_changed = false) :
      _path{std::move(path)}, _temperature{UNKNOWN_TEMPERATURE_MEASUREMENT},
      _has_changed{has_changed} {
  }

  const FileSystemPath &Path() const {
    return _path;
  }

  bool HasChanged() const {
    return _has_changed;
  }

  /**
   * Parses the content of the file at its path in order to capture the temperature it measures.
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
        if (_temperature==UNKNOWN_TEMPERATURE_MEASUREMENT) {
          _temperature = captured_temperature;
        } else {
          auto previous = _has_changed;
          _has_changed |= _temperature!=captured_temperature;
          if (_has_changed && previous!=_has_changed) {
            Log::D(TAG, "%s confirms that it tracks a varying temperature.", _path.c_str());
          }
        }
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
  TemperatureInCelsiusMillis _temperature;
  bool _has_changed;
};

std::vector<TrackedTemperatureFile> _tracked_temperature_files;

//------------------------------------------------------------------------------

/**
 * CPU temperatures are typically around certain range in Celsius millidegrees. During analysis of
 * temperature file candidates, their measured temperature is weighed against these boundaries to
 * confirm or veto candidacies.
 */
const ancer::TemperatureInCelsiusMillis LOWEST_REASONABLE_VALUE_IN_CELSIUS_MILLIS{24000};
const ancer::TemperatureInCelsiusMillis HIGHEST_REASONABLE_VALUE_IN_CELSIUS_MILLIS{60000};
}

//==============================================================================

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

//------------------------------------------------------------------------------
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
      _tracked_temperature_files.emplace_back(std::move(temperature_file_path), true);
    }
  } catch (std::ios_base::failure &e) {
    Log::I(TAG, "Temperature file catalog %s read open failure: \"%s\".",
           catalog_path.c_str(), e.what());
  }

  return not IsTrackedListEmpty();
}

//------------------------------------------------------------------------------
// Low-level temperature file candidate analysis.

/**
 * Given a string (e.g., file name or path), returns true if not compatible with the black list non-
 * CPU temperature files.
 */
bool NotBlacklisted(const std::string &name) {
  // Any temperature candidate path that matches any of these words (battery, camera, etc.) is
  // discarded as a possible CPU temperature value.
  static const std::vector<std::regex> REGEX_BLACKLIST{
      std::regex{R"(\.)"},
      std::regex{R"(\.\.)"},
      std::regex{"(.*)trip(.*)", std::regex::icase},
      std::regex{"(.*)batt(.*)", std::regex::icase},
      std::regex{"(.*)usb(.*)", std::regex::icase},
      std::regex{"(.*)camera(.*)", std::regex::icase},
      std::regex{"(.*)lcd(.*)", std::regex::icase},
      std::regex{"(.*)gpu(.*)", std::regex::icase},
      std::regex{"(.*)display(.*)", std::regex::icase},
      std::regex{"(.*)firmware(.*)", std::regex::icase},
      std::regex{"(.*)ambien(.*)", std::regex::icase},
      std::regex{"(.*)emul(.*)", std::regex::icase},
      std::regex{"(.*)thermistor(.*)", std::regex::icase},
      std::regex{"(.*)sensor(.*)", std::regex::icase}
  };

  bool resolution{true};

  for (const auto &blacklisted_word : REGEX_BLACKLIST) {
    if (std::regex_match(name, blacklisted_word)) {
      resolution = false;
      break;
    }
  }

  return resolution;
}

/**
 * Given a file name, returns true, if it's compatible with the temperature file name pattern.
 */
bool LooksLikeTemperatureFilename(const FileSystemPath &filename) {
  // By overwhelming evidence, temperature file names contain, at least, the lowercase word "temp".
  static const std::vector<std::regex> TEMPERATURE_FILE_REGEX{std::regex{"(.*)temp(.*)"}};

  bool resolution{false};

  for (const auto &temperature_file_pattern : TEMPERATURE_FILE_REGEX) {
    if (std::regex_match(filename, temperature_file_pattern)) {
      resolution = true;
      break;
    }
  }

  return resolution;
}

/**
 * Given a file name, returns true, if it's compatible with a CPU temperature file name (as opposed
 * to battery, gyroscope, etc.)
 */
bool LooksLikeCPUTemperatureFile(const FileSystemPath &filename) {
  return LooksLikeTemperatureFilename(filename) && NotBlacklisted(filename);
}

/**
 * Given a temperature file candidate, inspects its content to check if it's compatible with a CPU
 * temperature.
 * @return true if compatible; false otherwise.
 */
bool IsContentCompatibleWithReasonableCelsiusMillis(const FileSystemPath &temperature_candidate) {
  // Temperature file candidates contain a number (possibly negative).
  static const std::regex TEMPERATURE_MEASUREMENT_REGEX{R"(-?\d+(\.\d*)?)"};

  bool resolution{false};

  std::ifstream candidate_stream{temperature_candidate};
  if (candidate_stream.is_open()) {
    std::string stream_line;
    if (std::getline(candidate_stream, stream_line)) {
      std::smatch matching;
      if (std::regex_match(stream_line, matching, TEMPERATURE_MEASUREMENT_REGEX)) {
        int temperature = std::stoi(matching[0].str());
        if (LOWEST_REASONABLE_VALUE_IN_CELSIUS_MILLIS <= temperature &&
            temperature <= HIGHEST_REASONABLE_VALUE_IN_CELSIUS_MILLIS) {
          Log::I(TAG, "Temperature file candidate %s seems to hold a reasonable CPU temperature "
                      "(%d Celsius millidegrees). Will be tracked.",
                 temperature_candidate.c_str(), temperature);
          resolution = true;
        } else {
          Log::D(TAG, "Temperature file candidate %s doesn't seem a reasonable CPU temperature "
                      "(\"%d\").",
                 temperature_candidate.c_str(), temperature);
        }
      } else {
        Log::D(TAG, "Temperature file candidate %s first line doesn't seem a number (\"%s\").",
               temperature_candidate.c_str(), stream_line.c_str());
      }
    }
  } else {
    Log::D(TAG, "Temperature file candidate %s couldn't be opened.", temperature_candidate.c_str());
  }

  return resolution;
}

//------------------------------------------------------------------------------
// High-order directory scan for CPU temperature files.

/**
 * Checks a directory path for temperature file children. Doesn't drill down recursively. Instead,
 * it queues children directories to be scanned in a later call to this function.
 * This function analyzes all direct children of the received directory.
 * If they are files, their names and content are checked to see if compatible with a CPU
 * temperature file. If so, they are appended to the tracked file list.
 * If children are sub-directories, they are queued for subsequent sub-directory scans unless their
 * directory names suggest they are related to non-CPU components (like sensors).
 *
 * @param full_directory absolute path to the directory to scan.
 * @param directories_to_scan queue where upcoming directories to scan are held.
 */
void ScanDirectory_SingleLevel(const FileSystemPath &full_directory,
                               std::queue<FileSystemPath> &directories_to_scan) {
  Log::V(TAG, "Scanning %s...", full_directory.c_str());

  auto make_absolute = [&full_directory](const char *filename) {
    return full_directory + "/" + filename;
  };

  auto *directory = opendir(full_directory.c_str());
  if (directory) {
    struct dirent *entry;
    while ((entry = readdir(directory))) {
      switch (entry->d_type) {
        case DT_DIR: {
          if (NotBlacklisted(entry->d_name)) {
            directories_to_scan.emplace(make_absolute(entry->d_name));
          }
          break;
        }

        case DT_REG: {
          if (LooksLikeCPUTemperatureFile(entry->d_name)) {
            const FileSystemPath temperature_candidate{make_absolute(entry->d_name)};
            if (IsContentCompatibleWithReasonableCelsiusMillis(temperature_candidate)) {
              _tracked_temperature_files.emplace_back(temperature_candidate);
            }
          }
          break;
        }

        default: {
          // NO-OP
        }
      }
    }
  } else {
    Log::V(TAG, "Access to directory %s error: %d", full_directory.c_str(), errno);
  }

  closedir(directory);
}

/**
 * Receives an initial directory and scans it for temperature files. The search includes sub-
 * directories in a breadth-first fashion.
 *
 * @param initial_directory self-explanatory.
 * @return true if the search found at least one temperature file.
 */
bool ScanDirectoryTree_BFS(const FileSystemPath &initial_directory) {
  std::queue<FileSystemPath> directories_to_scan;
  directories_to_scan.push(initial_directory);

  // This loop follows a BFS (breadth-first search) logic. Given a directory tree, starts from its
  // root, if the root has children that are directories, these children will follow. If these
  // children have children (root's grandchildren), these grandchildren will follow. This as opposed
  // to a recursive, depth-first search strategy  Both would lead to the same result but this one is
  // simpler to implement: a queue called directories-to-scan pops directories from its head and
  // queues new ones to its tail.
  while (not directories_to_scan.empty()) {
    const auto current_directory_name = directories_to_scan.front();
    directories_to_scan.pop();

    ScanDirectory_SingleLevel(current_directory_name, directories_to_scan);
  }

  return not IsTrackedListEmpty();
}
}

//==============================================================================

bool ancer::internal::InitTemperatureCapture(const bool run_async) {
  if (not IsCaptureStatus(CaptureStatus::NotInitialized)) {
    return false;
  }
  SetCaptureStatus(CaptureStatus::Initializing);

  if (IsTrackedListEmpty()) {
    if (not RetrieveTemperatureFileListFromCatalog()) {
      Log::W(TAG, "A /sys folder full-scan to detect temperature files couldn't be avoided.");

      static auto initialization_steps = []() {
        ScanDirectoryTree_BFS("/sys");
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

void ancer::internal::DeinitTemperatureCapture() {
  std::vector<TrackedTemperatureFile> new_list;
  for (auto &tracked_file : _tracked_temperature_files) {
    if (not tracked_file.HasChanged()) {
      Log::I(TAG, "Unchanged %s removed from catalog.", tracked_file.Path().c_str());
    } else {
      new_list.push_back(tracked_file);
    }
  }

  if (_tracked_temperature_files.size()!=new_list.size()) {
    if (not new_list.empty()) {
      _tracked_temperature_files = std::move(new_list);
      StoreTemperatureFileListToCatalog();
    } else {
      Log::W(TAG, "Skipping catalog update because all entries would be removed.");
    }
  }
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

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
