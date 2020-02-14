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
#include <sched.h>
#include <thread>
#include <mutex>
#include <sstream>

#include <ancer/BaseOperation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==================================================================================================

namespace {
    constexpr Log::Tag TAG{"ThreadSchedulingOperation"};
}

//==================================================================================================

namespace {
    struct scheduler_configuration {
        int cpu_id = -1;
        Milliseconds scheduled_delay = 500ms;
    };

    JSON_CONVERTER(scheduler_configuration) {
        JSON_OPTVAR(cpu_id);
        JSON_REQVAR(scheduled_delay);
    }

//--------------------------------------------------------------------------------------------------

    struct configuration {
        std::vector<scheduler_configuration> threads;
    };

    JSON_CONVERTER(configuration) {
        JSON_REQVAR(threads);
    }

//--------------------------------------------------------------------------------------------------

    struct datum {
        Timestamp execution_start_time;
        Nanoseconds execution_start_time_error;
    };

    JSON_WRITER(datum) {
        JSON_REQVAR(execution_start_time);
        JSON_REQVAR(execution_start_time_error);
    }
}

//==================================================================================================

/*
 * ThreadSchedulingOperation
 * Schedules threads to run at distances in the future, when in Test mode,
 * records the distance in time between the intended time to run and the
 * time of the actual scheduled execution
 */
class ThreadSchedulingOperation : public BaseOperation {
public:

    ThreadSchedulingOperation() = default;

    void Start() override {
        BaseOperation::Start();
        _configuration = GetConfiguration<configuration>();

        auto is_test = GetMode() == Mode::DataGatherer;
        auto start_time = SteadyClock::now();

        for ( auto& scheduler_configuration : _configuration.threads ) {
            auto sleep_duration = scheduler_configuration.scheduled_delay;

            _threads.emplace_back(
                    [=]() {

                        auto cpu_id = scheduler_configuration.cpu_id;
                        if ( cpu_id >= 0 ) {
                            SetThreadAffinity(cpu_id);
                        }

                        const auto key = "cpu_" + (cpu_id >= 0
                                                   ? std::to_string(cpu_id)
                                                   : std::string("any"));

                        while ( !IsStopped()) {
                            auto start = SteadyClock::now();
                            auto expected_end = start + sleep_duration;
                            {
                                ANCER_SCOPED_TRACE("ThreadSchedulingOperation"
                                                   "::start::thread_loop - sleeping");
                                std::this_thread::sleep_for(sleep_duration);
                            }
                            auto end = SteadyClock::now();

                            if ( is_test ) {
                                Report(datum{end, end - expected_end});
                            }
                        }
                    });
        }

        // wait for execution duration to time out
        _threads.emplace_back(
                [=]() {
                    while ( !IsStopped()) {
                        auto now = SteadyClock::now();
                        if ( now - GetStartTime() > GetDuration()) {
                            Stop();
                            return;
                        }
                        std::this_thread::sleep_for(50ms);
                    }
                });
    }

    void Wait() override {
        for ( auto& t : _threads ) {
            if ( t.joinable()) {
                t.join();
            }
        }
    }

private:

    std::vector<std::thread> _threads;
    configuration _configuration;
};

EXPORT_ANCER_OPERATION(ThreadSchedulingOperation);