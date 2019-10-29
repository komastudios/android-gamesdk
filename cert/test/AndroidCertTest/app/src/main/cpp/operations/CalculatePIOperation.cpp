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
    constexpr Log::Tag TAG{"calculate_pi_native"};
    constexpr auto ITERATION_SAMPLE_PERIOD = int64_t(100);
}

//==================================================================================================

namespace {
    struct configuration {
        int threads;
        Milliseconds performance_sample_period = Milliseconds::zero();
        bool affinity;
    };

    JSON_READER(configuration) {
        JSON_REQVAR(threads);
        JSON_REQVAR(affinity);
        JSON_OPTVAR(performance_sample_period);
    }

//--------------------------------------------------------------------------------------------------

    struct datum {
        Timestamp start_time;
        Timestamp end_time;
        int64_t iterations = 0;
        double pi_approximation = 0;
    };

    JSON_WRITER(datum) {
        JSON_REQVAR(start_time);
        JSON_REQVAR(end_time);
        JSON_REQVAR(iterations);
        JSON_REQVAR(pi_approximation);
    }
}

//==================================================================================================

/**
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

                        CalculatePi(
                                GetDuration(),
                                is_test ? c.performance_sample_period : Duration::zero());
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

    /**
     * Performs an iterative calculation of PI for a time duration; note: the goal is not accuracy, but just to heat up the CPU
     * @param duration the time duration to run. If 0, run indefinitely (or until *canceled == true)
     * @param samplePeriod if > 0, a sample will be recorded every samplePeriod
     * @param canceled when set to true, stop work
     * @return the result (duration, iterations, result pi, etc)
     */
    void CalculatePi(Duration duration, Duration samplePeriod) {
        ANCER_SCOPED_TRACE("CalculatePIOperation::calculate_pi");

        auto is_test = GetMode() == Mode::DataGatherer;

        SteadyClock clock;
        auto start_time = SteadyClock::now();

        long d = 3;
        long i = 0;
        double accumulator = 1;

        datum current_datum{};
        current_datum.start_time = SteadyClock::now(); // TODO

        for ( ;; i++, d += 2, current_datum.iterations++ ) {
            if ( IsStopped()) {
                break;
            }

            double factor = 1.0 / static_cast<double>(d);

            if ( i % 2 == 0 ) {
                accumulator -= factor;
            } else {
                accumulator += factor;
            }

            if ( i % ITERATION_SAMPLE_PERIOD == 0 ) {
                auto now = SteadyClock::now();
                auto elapsed = now - start_time;

                if ((duration > Duration::zero()) && (elapsed >= duration)) {
                    // report current state and bail
                    current_datum.end_time = now;
                    current_datum.pi_approximation = 4 * accumulator;
                    Report(current_datum);
                    break;
                }

                if ((samplePeriod > Duration::zero()) &&
                        (now - current_datum.start_time >= samplePeriod)) {
                    current_datum.end_time = now;
                    current_datum.pi_approximation = 4 * accumulator;
                    Report(current_datum);

                    current_datum = datum{};
                    current_datum.start_time = now;
                }
            }
        }
    }

private:

    std::vector<std::thread> _threads;
};

EXPORT_ANCER_OPERATION(CalculatePIOperation)


#pragma clang diagnostic pop