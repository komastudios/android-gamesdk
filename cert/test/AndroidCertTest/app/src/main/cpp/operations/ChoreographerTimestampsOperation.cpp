/*
 * Copyright 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * This test gathers timestamps from Choreographer for later comparison with
 * VSYNC traces from Systrace.
 *
 * Choreographer can be used by applications to determine deltas between frames
 * timestamps and aid in updating animations and similar state.
 *
 * Input configuration:
 * - frame_limit: The number of frames for which to capture timestamps.
 *
 * Output report:
 * - timestamp: An individual timestamp captured from Choreographer.
 */

#include <mutex>

#include <ancer/BaseOperation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/LibAndroid.hpp>
#include <ancer/Reporter.hpp>

using namespace ancer;

// =============================================================================

namespace {
constexpr Log::Tag TAG{"ChoreographerTimestampsOperation"};

const std::string marker = "ancer::clock_sync";

// -----------------------------------------------------------------------------

struct configuration {
  int frame_limit;
};

JSON_READER(configuration) { JSON_REQVAR(frame_limit); }

// -----------------------------------------------------------------------------

struct timestamp_datum {
  long timestamp;
};

void WriteDatum(report_writers::Struct w, const timestamp_datum& d) {
  ADD_DATUM_MEMBER(w, d, timestamp);
}
}  // namespace

// =============================================================================

namespace {
const auto getInstance = libandroid::GetFP_AChoreographer_getInstance();
const auto postFrameCallback =
    libandroid::GetFP_AChoreographer_postFrameCallback();
}  // namespace

class ChoreographerTimestampsOperation : public BaseOperation {
 public:
  ChoreographerTimestampsOperation() = default;

  void Start() override {
    BaseOperation::Start();

    _configuration = GetConfiguration<configuration>();

    if (getInstance == nullptr || postFrameCallback == nullptr) {
      FatalError(TAG, "Required library functions failed to load");
    }

    SetSystraceClockSync();
    SetupFrameCallback();
  }

  void Wait() override {
    bool wait = true;
    while (wait) {
      std::lock_guard guard{_counter_mutex};
      wait = _counter < _configuration.frame_limit;
    }
  }

 private:
  void SetSystraceClockSync() const {
    const auto now = SteadyClock::now();
    const auto time_ns = static_cast<int64_t>(now.time_since_epoch().count());
    const std::string str = marker + "(" + std::to_string(time_ns) + ")";
    auto trace = gamesdk::ScopedTrace(str.c_str());
  }

  void SetupFrameCallback() {
    AChoreographer* instance = getInstance();
    postFrameCallback(instance, FrameCallback, this);
  }

  static void FrameCallback(long frame_time_nanos, void* data) {
    if (data == nullptr) return;
    auto self = reinterpret_cast<ChoreographerTimestampsOperation*>(data);

    std::lock_guard guard{self->_counter_mutex};
    self->Report(timestamp_datum{frame_time_nanos});
    ++(self->_counter);

    // The counter must be incremented after a timestamp has been logged, but
    // neither the counter nor its mutex can be accessed after the counter limit
    // is reached, since that condition is the only thing keeping the operation
    // from being destroyed in the Wait() method.

    if (self->_counter < self->_configuration.frame_limit) {
      self->SetupFrameCallback();
    }
  }

 private:
  configuration _configuration;

  int _counter = 0;
  std::mutex _counter_mutex;
};

EXPORT_ANCER_OPERATION(ChoreographerTimestampsOperation)
