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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-non-private-member-variables-in-classes"

#include <thread>
#include <mutex>
#include <sstream>

#include <ancer/BaseOperation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==================================================================================================

namespace {
    constexpr auto TAG = "MonitorOperation";

    struct configuration {
        Milliseconds sample_period = 500ms;
    };

    JSON_READER(configuration) {
        JSON_OPTVAR(sample_period);
    }

//--------------------------------------------------------------------------------------------------

    struct perf_info {
        double fps{0.0};
        Nanoseconds min_frame_time{0};
        Nanoseconds max_frame_time{0};
    };

    JSON_WRITER(perf_info) {
        JSON_REQVAR(fps);
        JSON_REQVAR(min_frame_time);
        JSON_REQVAR(max_frame_time);
    }

//--------------------------------------------------------------------------------------------------

    struct thermal_info {
        ThermalStatus status;
    };

    JSON_WRITER(thermal_info) {
        JSON_SETVAR(status_msg, to_string(data.status));
        JSON_REQVAR(status);
    }

//--------------------------------------------------------------------------------------------------

    struct sys_mem_info {
        long native_allocated = 0;
        long available_memory = 0;
        bool low_memory = false;
        int oom_score = 0;

        explicit sys_mem_info(const MemoryInfo& i) :
                native_allocated(i.native_heap_allocation_size), available_memory(i.available_memory)
                , oom_score(i._oom_score), low_memory(i.low_memory) {}
    };

    JSON_WRITER(sys_mem_info) {
        JSON_REQVAR(native_allocated);
        JSON_REQVAR(available_memory);
        JSON_REQVAR(oom_score);
        JSON_REQVAR(low_memory);
    }

//--------------------------------------------------------------------------------------------------

    struct datum {
        sys_mem_info memory_state;
        perf_info perf_info;
        thermal_info thermal_info;
    };

    JSON_WRITER(datum) {
        JSON_REQVAR(memory_state);
        JSON_REQVAR(perf_info);
        JSON_REQVAR(thermal_info);
    }
}

//==================================================================================================

/**
 * MonitorOperation
 * Schedules threads to run at distances in the future, when in Test mode,
 * records the distance in time between the intended time to run and the
 * time of the actual scheduled execution
 */
class MonitorOperation : public BaseOperation {
public:

    MonitorOperation() = default;

    void Start() override {
        BaseOperation::Start();
        _configuration = GetConfiguration<configuration>();
        SetHeartbeatPeriod(_configuration.sample_period);
    }

    void Stop() override {
        BaseOperation::Stop();
        std::lock_guard<std::mutex> guard(_stop_mutex);
        _stop_signal.notify_one();
    }

    void Wait() override {
        if ( !IsStopped()) {
            std::unique_lock<std::mutex> lock(_stop_mutex);
            _stop_signal.wait(lock);
        }
    }

    void OnHeartbeat(Duration elapsed) override {
        BaseOperation::OnHeartbeat(elapsed);
        auto c = GetFpsCalculator();
        Report(
                datum{
                        sys_mem_info{GetMemoryInfo()},
                        perf_info{c.GetAverageFps(),
                                c.GetMinFrameTime(),
                                c.GetMinFrameTime(),
                        },
                        thermal_info{GetThermalStatus()}
                });
    }

private:

    std::mutex _stop_mutex;
    std::condition_variable _stop_signal;
    configuration _configuration;
};

EXPORT_ANCER_OPERATION(MonitorOperation);

#pragma clang diagnostic pop
