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

#include <cmath>
#include <thread>
#include <mutex>
#include <sstream>

#include <ancer/BaseOperation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==============================================================================

namespace {
constexpr Log::Tag TAG{"fake_operation"};
}

//==============================================================================

namespace {
struct configuration {
  Milliseconds report_period;
  int int_value{};
  Milliseconds duration_value{Milliseconds::zero()};
  bool bool_value{};
  std::string string_value;
};

JSON_READER(configuration) {
  JSON_REQVAR(report_period);
  JSON_REQVAR(int_value);
  JSON_REQVAR(duration_value);
  JSON_REQVAR(bool_value);
  JSON_REQVAR(string_value);
}

//------------------------------------------------------------------------------

enum class EventType {
  Start, Heartbeat, StopOrdered, WaitStarted, WaitFinished, Destruction
};
constexpr const char* const EventTypeNames[] = {
    "Start", "Heartbeat", "StopOrdered", "WaitStarted", "WaitFinished",
    "Destruction"
};

/*
 * The datum simply forwards configuration values to the report.
 * This is to confirm that received configuration values are parsed
 * and persisted correctly.
 */
struct datum {
  EventType event;
  Milliseconds report_period;
  int int_value;
  Milliseconds duration_value;
  bool bool_value;
  std::string string_value;
};

JSON_WRITER(datum) {
  JSON_REQENUM(event, EventTypeNames);
  JSON_REQVAR(report_period);
  JSON_REQVAR(int_value);
  JSON_REQVAR(duration_value);
  JSON_REQVAR(string_value);
  JSON_REQVAR(bool_value);
}
}

//==============================================================================

/*
 * FakeOperation
 * This operation exists for integration testing. It serves
 * to emit datums which relay the configuration values our to the report.
 */
class FakeOperation : public BaseOperation {
 public:

  FakeOperation() {
    Report(EventType::Destruction);
  }

  void Start() override {
    BaseOperation::Start();
    _configuration = GetConfiguration<configuration>();

    Report(EventType::Start);
    SetHeartbeatPeriod(_configuration.report_period);
  }

  void OnHeartbeat(Duration elapsed) override {
    BaseOperation::OnHeartbeat(elapsed);
    Report(EventType::Heartbeat);
  }

  void Stop() override {
    BaseOperation::Stop();
    Report(EventType::StopOrdered);
    std::lock_guard<std::mutex> guard(_stop_mutex);
    _stop_signal.notify_one();
  }

  void Wait() override {
    Report(EventType::WaitStarted);
    if (!IsStopped()) {
      std::unique_lock<std::mutex> lock(_stop_mutex);
      _stop_signal.wait(lock);
    }
    Report(EventType::WaitFinished);
  }

 private:

  void Report(EventType event) {
    BaseOperation::Report(datum{
        event,
        _configuration.report_period,
        _configuration.int_value,
        _configuration.duration_value,
        _configuration.bool_value,
        _configuration.string_value
    });
  }

 private:
  configuration _configuration;
  std::mutex _stop_mutex;
  std::condition_variable _stop_signal;
};

EXPORT_ANCER_OPERATION(FakeOperation)