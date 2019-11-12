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


//==================================================================================================

namespace {
    constexpr Log::Tag TAG{"calculate_pi_native"};

    constexpr auto ITERATION_SAMPLE_PERIOD(int64_t(100));

    constexpr auto PI = 3.14159265358979323846;
}

//==================================================================================================

namespace {
    struct configuration {
        int threads{};
        Milliseconds performance_sample_period{Milliseconds::zero()};
        bool affinity{};
    };

    JSON_READER(configuration) {
        JSON_REQVAR(threads);
        JSON_REQVAR(affinity);
        JSON_OPTVAR(performance_sample_period);
    }

//--------------------------------------------------------------------------------------------------

    struct pi {
        Duration duration;
        int64_t iterations;
        double value;
        double error;
    };

    struct datum {
        pi pi;
    };

    JSON_WRITER(pi) {
        JSON_REQVAR(duration);
        JSON_REQVAR(iterations);
        JSON_REQVAR(value);
        JSON_REQVAR(error);
    }

    JSON_WRITER(datum) {
        JSON_REQVAR(pi);
    }
}

//==================================================================================================

/*
 * CalculatePIOperation
 * Computes PI, iteratively, to heat up the CPU
 */
class CalculatePIOperation : public BaseOperation {
public:

    CalculatePIOperation() = default;

    void Start() override {
        BaseOperation::Start();

        auto c = GetConfiguration<configuration>();
        auto num_cores = std::thread::hardware_concurrency();
        auto num_threads = c.threads <= 0 ? num_cores : c.threads;
        auto is_test = GetMode() == Mode::DataGatherer;

        for ( auto i = 0 ; i < num_threads ; i++ ) {
            _threads.emplace_back(
                    [this, i, is_test, c]() {
                        if ( c.affinity ) {
                            SetThreadAffinity(i);
                        }

                        CalculatePi(GetDuration(), is_test
                                                   ? c.performance_sample_period
                                                   : Duration::zero());
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
    void CalculatePi(Duration calculationDuration, Duration reportSamplingDuration) {
        ANCER_SCOPED_TRACE("CalculatePIOperation::calculate_pi");

        auto is_test = GetMode() == Mode::DataGatherer;

        long d = 3;
        long i = 0;
        double accumulator = 1;

        pi current_pi{};

        for (auto start_time = SteadyClock::now();; i++, d += 2, ++current_pi.iterations) {
            if (IsStopped()) {
                break;
            }

            double factor = 1.0 / static_cast<double>((i % 2 == 0) ? -d : d);
            accumulator += factor;

            if (i % ITERATION_SAMPLE_PERIOD == 0) {
                auto now = SteadyClock::now();
                current_pi.duration = now - start_time;

                if ((calculationDuration > Duration::zero()) &&
                    (current_pi.duration >= calculationDuration)) {
                    // report current state and bail
                    current_pi.value = 4 * accumulator;
                    current_pi.error = std::abs(current_pi.value - PI);
                    Report(datum{current_pi});
                    break;
                }

                if ((reportSamplingDuration > Duration::zero()) &&
                    (current_pi.duration >= reportSamplingDuration)) {
                    current_pi.value = 4 * accumulator;
                    current_pi.error = std::abs(current_pi.value - PI);
                    Report(datum{current_pi});

                    current_pi = pi{};
                    start_time = now;
                }
            }
        }
    }

private:
    std::vector<std::thread> _threads;
};

EXPORT_ANCER_OPERATION(CalculatePIOperation)