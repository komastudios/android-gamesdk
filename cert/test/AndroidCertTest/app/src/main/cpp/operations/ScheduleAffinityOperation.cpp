/*
 * Copyright 2020 The Android Open Source Project
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

/*
 *
 * ScheduleAffinityOperation
 *
 * This test aims to determine reliability of thread pinning. E.g., if a thread
 * is pinned to a specific core, will the os respect that assignment?
 *
 * Basic operation is to spawn N threads per CPU, pin them to that CPU, have
 * them run some busy work and periodically report what CPU they're executing
 * on.
 *
 * Inputs:
 *
 * configuration:
 *   thread_count: [int] count of threads to spawn per cpu
 *   report_frequency: [Duration] Period of time between reporting cpu thread
 *     is running on
 *
 * Outputs:
 *
 * datum:
 *   message:[string] The action being reported, one of:
 *     "spawning_started": start of test, about to start work threads
 *     "spawning_batch": starting a batch of work threads for a cpu
 *     "setting_affinity": about to set affinity of work thread X
 *     "did_set_affinity": finished setting affinity for work thread X
 *     "work_started": work starting for a worker thread
 *     "work_running": work running (this is sent periodically
 *       while test executes)
 *     "work_finished": work finished for worker thread
 *   expected_cpu:[int]: if >= 0 cpu this worker was expected to run on
 *   thread_affinity_set: [bool] if true, expected_cpu should be checked against
 *     the datum payload's cpu id for veracity
 *
 */

#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <random>
#include <sched.h>
#include <thread>

#include <cpu-features.h>

#include <ancer/BaseOperation.hpp>
#include <ancer/DatumReporting.hpp>
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
  bool thread_affinity_set = false;
};

void WriteDatum(report_writers::Struct w, const datum& d) {
  ADD_DATUM_MEMBER(w, d, message);
  ADD_DATUM_MEMBER(w, d, expected_cpu);
  ADD_DATUM_MEMBER(w, d, thread_affinity_set);
}
} // anonymous namespace

//==============================================================================

class ScheduleAffinityOperation : public BaseOperation {
  void Start() override {
    BaseOperation::Start();

    _thread = std::thread{[this] {
      // It doesn't hurt to note what the spawning thread is doing.
      const auto original_cpu = sched_getcpu();
      Report(datum{"spawning_started", original_cpu});
      const auto c = GetConfiguration<configuration>();
      const auto time_to_run = GetDuration();

      std::random_device rd;
      std::mt19937 rng(rd());
      std::vector<std::thread> threads;

      auto core_sizes = ancer::GetCoreSizes();
      std::vector<int> big_core_indexes;
      for (int i = 0; i < core_sizes.size(); i++) {
        if (core_sizes[i] == ancer::ThreadAffinity::kBigCore) {
          big_core_indexes.push_back(i);
        }
      }
      const auto num_big_cores = big_core_indexes.size();
      for (std::size_t i = 0; i < num_big_cores;) {
        if (IsStopped()) break;

        Report(datum{"spawning_batch", original_cpu});
        for (int j = 0; j < c.thread_count; j++) {
          threads.emplace_back(
              [this, i, j, &c, time_to_run, &big_core_indexes, &rng] {
                // determine cpu core we're on
                int starting_cpu_index = sched_getcpu();

                // find a big core != current
                int big_core_index_to_use =
                    PickRandomElementExcluding(rng, big_core_indexes,
                                               sched_getcpu());

                if (big_core_index_to_use >= 0) {
                  // set affinity to the chosen cpu
                  Report(datum{"setting_affinity", big_core_index_to_use,
                               false});
                  bool success = PinThreadToCpu(big_core_index_to_use);
                  Report(datum{"did_set_affinity", big_core_index_to_use,
                               success});

                  // start the cpu stress work
                  Report(datum{"work_started", big_core_index_to_use, success});
                  StressCpu(time_to_run,
                            c.report_frequency,
                            big_core_index_to_use,
                            success);

                  Report(datum{"work_finished", big_core_index_to_use,
                               success});
                } else {
                  FatalError(TAG, "Unable to find a big-core CPU index to use");
                }
              });
        }

        // Wait for this batch to finish
        for (auto& thread : threads) {
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

  // returns a random element from `values` array which is not equal
  // to `excluding`, or -1 if no such value can be found
  int PickRandomElementExcluding(std::mt19937& rng,
                                 std::vector<int> values,
                                 int excluding) {
    // if there's only one item, return it
    if (values.size() == 1) {
      return values[0] == excluding ? -1 : values[0];
    }

    // we know the array has values != excluding, pick one at random
    std::shuffle(values.begin(), values.end(), rng);
    for (auto v : values) {
      if (v != excluding) {
        return v;
      }
    }

    return -1;
  }

  // sets the calling thread to run on the cpu with given index
  bool PinThreadToCpu(int index) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(index, &cpuset);

    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == -1) {
      std::string err;
      switch (errno) {
        case EFAULT:err = "EFAULT";
          break;
        case EINVAL:err = "EINVAL";
          break;
        case EPERM:err = "EPERM";
          break;
        case ESRCH:err = "ESRCH";
          break;
        default:err = "(errno: " + std::to_string(errno) + ")";
          break;
      }
      Log::E(TAG, "PinThreadToCpu() - unable to set; error: " + err);
      return false;
    }
    return true;
  }

  // Workload copied from CalculatePiOperation
  void StressCpu(
      const Duration duration,
      const Duration report_freq,
      const int cpu,
      bool did_set_thread_affinity) {

    const auto start = SteadyClock::now();
    const auto end = start + duration;
    auto next_report = start + report_freq;

    long d = 3;
    long i = 0;
    double accumulator = 1.0;
    while (true) {
      if (IsStopped()) break;

      const auto now = SteadyClock::now();
      if (now >= end) break;
      if (now >= next_report) {
        next_report = now + report_freq;
        Report(datum{"work_running", cpu, did_set_thread_affinity});
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
