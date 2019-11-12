#include <condition_variable>
#include <mutex>
#include <sched.h>
#include <thread>

#include <cpu-features.h>

#include <ancer/BaseOperation.hpp>
#include <ancer/util/Basics.hpp>
#include <ancer/util/Log.hpp>
#include <ancer/util/Json.hpp>
#include <ancer/util/Time.hpp>

using namespace ancer;


//==============================================================================

namespace {
    constexpr Log::Tag TAG{"schedule_affinity"};
}

//==============================================================================

namespace {
    struct configuration {
        // How many threads should we run at one time?
        int thread_count = 1;

        // How often should threads report in?
        Milliseconds report_frequency = 100ms;
    };

    JSON_READER(configuration) {
        JSON_OPTVAR(thread_count);
        JSON_OPTVAR(report_frequency);
    }

//==============================================================================

    struct datum {
        const char* message;
        int expected_cpu;
    };

    JSON_WRITER(datum) {
        JSON_REQVAR(message);
        JSON_REQVAR(expected_cpu);
    }
} // anonymous namespace

//==============================================================================

class ScheduleAffinityOperation : public BaseOperation {
    void Start() override {
        BaseOperation::Start();

        _thread = std::thread{[this] {
            // NOTE: This will probably change since it was never explicitly
            // assigned, but it doesn't hurt to note what the spawning thread
            // is doing.
            const auto original_cpu = sched_getcpu();
            Report(datum{"spawning_started", original_cpu});
            const auto c = GetConfiguration<configuration>();
            const auto time_to_run = GetDuration();

            std::vector<std::thread> threads;
            threads.reserve(c.thread_count);

            const int cpu_count = android_getCpuCount();
            for ( int i = 0 ; i < cpu_count ; ) {
                if ( IsStopped()) break;

                Report(datum{"spawning_batch", original_cpu});

                const auto range_start = i;
                const auto range_end = range_start + c.thread_count;
                for ( ; i < range_end ; ++i ) {
                    // When we're running the last CPUs, cycle back to the
                    // beginning if we need to grab a few extra to keep
                    // things consistent.
                    const auto cpu = i % cpu_count;
                    // We're looping on ourselves, which means we've requested
                    // more CPUs than we actually have. Just stop the loop and
                    // we should be running everything.
                    if ( i >= cpu_count && cpu >= range_start ) break;

                    threads.emplace_back(
                            [this, cpu = cpu, &c, time_to_run] {
                                Report(datum{"setting_affinity", cpu});
                                cpu_set_t set;
                                CPU_ZERO(&set);
                                CPU_SET(cpu, &set);
                                if ( sched_setaffinity(
                                        0, sizeof(cpu_set_t), &set) != 0 ) {
                                    FatalError(
                                            TAG,
                                            "sched_setaffinity failed. error = %d",
                                            errno);
                                }

                                Report(datum{"work_started", cpu});
                                StressCpu(time_to_run, c.report_frequency, cpu);
                                Report(datum{"work_finished", cpu});
                            });
                }

                // Wait for this batch to finish
                for ( auto& thread : threads ) {
                    thread.join();
                }
                threads.clear();
            }

            Report(datum{"spawning_finished", original_cpu});
        }};
    }

    void Wait() override {
        _thread.join();
    }

    std::thread _thread;

//==============================================================================

    // Workload copied from CalculatePiOperation
    void StressCpu(
            const Duration duration,
            const Duration report_freq,
            const int cpu) {

        const auto start = SteadyClock::now();
        const auto end = start + duration;
        auto next_report = start + report_freq;

        long d = 3;
        long i = 0;
        double accumulator = 1.0;
        while ( true ) {
            if ( IsStopped()) break;

            const auto now = SteadyClock::now();
            if ( now >= end ) break;
            if ( now >= next_report ) {
                next_report = now + report_freq;
                Report(datum{"work_running", cpu});
            }

            const double factor = 1.0 / static_cast<double>(d);
            d += 2;
            accumulator += (i++ % 2) ? factor : -factor;
        }

        // Make sure we don't optimize away the above work.
        ForceCompute(accumulator);
    }
};

EXPORT_ANCER_OPERATION(ScheduleAffinityOperation)
