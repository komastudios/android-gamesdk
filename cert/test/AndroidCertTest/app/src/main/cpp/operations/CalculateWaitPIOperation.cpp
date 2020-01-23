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
#include <mutex>
#include <sstream>
#include <thread>
#include <unistd.h>

#include <ancer/BaseOperation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==================================================================================================

namespace {
    constexpr Log::Tag TAG{"calculate_pi_native"};

    constexpr auto ITERATION_SAMPLE_PERIOD(int64_t(100));

    constexpr auto PI = 3.14159265358979323846;
}

//==================================================================================================

namespace {
    struct configuration {
        Milliseconds performance_sample_period{Milliseconds::zero()};
        bool affinity{};
        int iteration_wait_count_;
        int iteration_wait_time_;
    };

    JSON_READER(configuration) {
        JSON_REQVAR(performance_sample_period);
        JSON_REQVAR(affinity);
        JSON_REQVAR(iteration_wait_count_);
        JSON_REQVAR(iteration_wait_time_);
    }

//--------------------------------------------------------------------------------------------------

    struct pi {
        Duration duration;
        int64_t iterations;
        double value;
        double error;
    };

    void WriteDatum(report_writers::Struct w, const pi& p) {
        ADD_DATUM_MEMBER(w, p, iterations);
    }


    struct datum {
        pi pi;
    };

    void WriteDatum(report_writers::Struct w, const datum& d) {
        ADD_DATUM_MEMBER(w, d, pi);
    }
}

//==================================================================================================

/*
 * CalculatePIOperation
 * Computes PI, iteratively, to heat up the CPU
 */
class CalculateWaitPIOperation : public BaseOperation {
public:

    CalculateWaitPIOperation() = default;

    void Start() override {
        BaseOperation::Start();

        auto c = GetConfiguration<configuration>();
        auto num_cores = std::thread::hardware_concurrency();
        auto is_test = true;

        for ( auto i = 0 ; i < num_cores ; i++ ) {
            _threads.emplace_back(
                    [this, i, is_test, c]() {
                        if ( c.affinity ) {
                            SetThreadAffinity(i);
                        }

                        CalculatePi(c);
                    });
        }
    }

    void Wait() override {
        for ( std::thread& t : _threads ) {
            if ( t.joinable()) {
                t.join();
            }
        }
    }

private:

    /*
     * Performs an iterative calculation of PI for a time duration; note: the goal is not accuracy,
     * but just to heat up the CPU.
     * @param calculationDuration the total test duration time. If 0, run indefinitely
     * @param reportSamplingDuration if > 0, a sample will be recorded every sampling duration
     * @return the result (duration, iterations, result pi, etc)
     */
    void CalculatePi(configuration config) {
        ANCER_SCOPED_TRACE("CalculatePIOperation::calculate_pi");

        int frame_count_since_rest = 0;

        long d = 3;
        long i = 0;
        double accumulator = 1;

        pi current_pi{};

        for (auto start_time = SteadyClock::now();; i++, d += 2, ++current_pi.iterations) {
            if (++frame_count_since_rest == config.iteration_wait_count_) {
                usleep(config.iteration_wait_time_);
                frame_count_since_rest = 0;
            }

            if (IsStopped()) {
                break;
            }

            double factor = 1.0 / static_cast<double>((i % 2 == 0) ? -d : d);
            accumulator += factor;

            if (i % ITERATION_SAMPLE_PERIOD == 0) {
                auto now = SteadyClock::now();
                current_pi.duration = now - start_time;

                if ((config.performance_sample_period > Duration::zero()) &&
                    (current_pi.duration >= config.performance_sample_period)) {
                    current_pi.value = 4 * accumulator;
                    current_pi.error = std::abs(current_pi.value - PI);
                    Report(datum{current_pi});

                    current_pi = pi{Duration::zero(), current_pi.iterations};
                    start_time = now;
                }
            }
        }
    }

private:
    std::vector<std::thread> _threads;
};

EXPORT_ANCER_OPERATION(CalculateWaitPIOperation)