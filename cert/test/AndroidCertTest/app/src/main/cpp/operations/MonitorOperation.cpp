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

#include <thread>
#include <mutex>
#include <sstream>

#include <ancer/BaseOperation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==============================================================================

namespace {
    constexpr auto TAG = "MonitorOperation";

    struct configuration {
        Milliseconds sample_period = 500ms;
    };

    JSON_READER(configuration) {
        JSON_OPTVAR(sample_period);
    }

//------------------------------------------------------------------------------

    struct perf_info {
        double fps{0.0};
        Nanoseconds min_frame_time{0};
        Nanoseconds max_frame_time{0};
    };

    void WriteDatum(report_writers::Struct w, const perf_info& p) {
        ADD_DATUM_MEMBER(w, p, fps);
        ADD_DATUM_MEMBER(w, p, min_frame_time);
        ADD_DATUM_MEMBER(w, p, max_frame_time);
    }

//------------------------------------------------------------------------------

    struct thermal_info {
        ThermalStatus status;
    };

    void WriteDatum(report_writers::Struct w, const thermal_info& i) {
        // TODO(tmillican@google.com): Switch once we have better enum-to-string
        //  support.
        w.AddItem("status_msg", to_string(i.status));
        w.AddItem("status", (int)i.status);
    }

//------------------------------------------------------------------------------

    struct sys_mem_info {
        long native_allocated = 0;
        long available_memory = 0;
        bool low_memory = false;
        int oom_score = 0;

        explicit sys_mem_info(const MemoryInfo& i)
        : native_allocated(i.native_heap_allocation_size)
        , available_memory(i.available_memory)
        , oom_score(i._oom_score)
        , low_memory(i.low_memory) {
        }
    };

    void WriteDatum(report_writers::Struct w, const sys_mem_info& i) {
        ADD_DATUM_MEMBER(w, i, native_allocated);
        ADD_DATUM_MEMBER(w, i, available_memory);
        ADD_DATUM_MEMBER(w, i, oom_score);
        ADD_DATUM_MEMBER(w, i, low_memory);
    }

//------------------------------------------------------------------------------

    struct datum {
        sys_mem_info memory_state;
        perf_info perf_info;
        thermal_info thermal_info;
    };

    void WriteDatum(report_writers::Struct w, const datum& d) {
        ADD_DATUM_MEMBER(w, d, memory_state);
        ADD_DATUM_MEMBER(w, d, perf_info);
        ADD_DATUM_MEMBER(w, d, thermal_info);
    }
} // anonymous namespace

//==============================================================================

/*
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