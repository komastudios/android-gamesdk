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

#include <mutex>

#include <ancer/BaseOperation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/LibAndroid.hpp>
#include <ancer/Reporter.hpp>
// #include <ancer/util/Json.hpp>

using namespace ancer;

namespace {
constexpr Log::Tag TAG{"ChoreographerTimestampsOperation"};

struct datum {
  long timestamp;
};

void WriteDatum(report_writers::Struct w, const datum& d) {
  ADD_DATUM_MEMBER(w, d, timestamp);
}
}  // namespace

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
    SetupFrameCallback();
  }

  void Wait() override {
    bool wait = true;
    while (wait) {
      std::lock_guard guard{_counter_mutex};
      wait = _counter < _counter_limit;
    }
  }

 private:
  void SetupFrameCallback() {
    if (getInstance == nullptr || postFrameCallback == nullptr) {
      FatalError(TAG, "Function pointers are invalid.");
    }

    AChoreographer* instance = getInstance();
    postFrameCallback(instance, FrameCallback, this);
  }

  static void FrameCallback(long frame_time_nanos, void* data) {
    if (data == nullptr) return;
    auto self = reinterpret_cast<ChoreographerTimestampsOperation*>(data);

    std::lock_guard guard{self->_counter_mutex};
    if (self->_counter < self->_counter_limit) {
      ++(self->_counter);
      Log::D(TAG, "%ld", frame_time_nanos);
      self->Report(datum{frame_time_nanos});
      self->SetupFrameCallback();
    }
  }

 private:
  int _counter = 0;
  const int _counter_limit = 10;
  std::mutex _counter_mutex;
};

EXPORT_ANCER_OPERATION(ChoreographerTimestampsOperation)
