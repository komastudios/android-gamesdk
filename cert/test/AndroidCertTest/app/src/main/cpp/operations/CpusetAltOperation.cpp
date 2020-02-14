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

/**
 * This functional test attempts to confirm that the CPUSET feature is enabled and works as expected
 * in a device. To verify that, will stress the CPUSET feature to assign a series of threads to
 * specific cores.
 * Each thread loops through the duration of the operation approximating e (the Euler's number).
 * From time to time, each thread stops to confirm that isn't running on any CPU that is not any of
 * its assigned ones.
 */

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <sched.h>
#include <set>
#include <thread>
#include <unordered_set>

#include <ancer/BaseOperation.hpp>
#include <ancer/util/Basics.hpp>
#include <ancer/util/Log.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==============================================================================

namespace {
constexpr Log::Tag TAG{"CpusetOperation"};
} // anonymous namespace

//==============================================================================

namespace {
const Milliseconds DEFAULT_FREQUENCY{100ms};

struct configuration {
  // How often should threads check that affinity is working fine?
  Milliseconds checkpoint_frequency{DEFAULT_FREQUENCY};
};

JSON_READER(configuration) {
  JSON_OPTVAR(checkpoint_frequency);
}
} // anonymous namespace

//==============================================================================

namespace {
/**
 * Applies a formula to select some of cores (from the available ones in the device) to a given
 * thread. The formula is such that a thread will get assigned not more than half of the available
 * cores. Also, the majority of the cores will get assigned almost evenly to the same number of
 * threads each, with a few outliers. Possibly one of the cores will get assigned to just one thread
 * whereas another could get assigned to almost all threads.
 *
 * @param thread_ord the thread ordinal (0, 1, ...).
 * @return a set of CPU cores, provided that the set won't contain more than half of the available
 * ones in the device.
 */
std::unordered_set<int> DesignateCores(const int thread_ord) {
  auto total_cores = std::thread::hardware_concurrency();
  std::unordered_set<int> cores{};

  for (auto iteration = 1; iteration <= total_cores / 2; ++iteration) {
    int core = (thread_ord < total_cores ?
                total_cores - iteration * (thread_ord + 1) :
                (iteration + 1) * (thread_ord + 1)) % total_cores;
    cores.insert(std::abs(core));
  }

  return cores;
}

/**
 * The meat of this functional test. This function receives a set of cores and, using the cpuset
 * feature, will make the thread in which it was invoked affine to these cores.
 *
 * @param cores a set of CPU cores
 * @return true if the core assignment via cpuset feature succeeded
 */
bool NarrowCoresForThisThread(const std::unordered_set<int> &cores) {
  bool success{false};

  cpu_set_t cpuset_in;
  cpu_set_t cpuset_out;

  CPU_ZERO(&cpuset_in);
  for (const int core : cores) {
    CPU_SET(core, &cpuset_in);
  }

  if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset_in) == 0 &&
      sched_getaffinity(0, sizeof(cpu_set_t), &cpuset_out) == 0 &&
      CPU_EQUAL(&cpuset_in, &cpuset_out)) {
    success = true;
  } else {
    Log::E(TAG, "Cpuset sched_setaffinity() failure (errno = %d)", errno);
  }

  return success;
}
} // anonymous namespace

//==============================================================================

namespace {

// Given an integer, calculates its factorial
long double CalculateFactorial(unsigned long long number) {
  long double factorial = 1.0;

  while (number > 0) {
    factorial *= number;
    --number;
  }

  return factorial;
}

// Loops approximating the Euler's number. This version is intentionally non-optimized to keep the
// CPU busy.
void Eulers_Number_Looper_NonOptimized(
    const Duration report_freq,
    const std::function<void(const long double, const unsigned long long)> &on_checkpoint_time,
    const std::function<bool(void)> &must_stop) {
  const auto start = SteadyClock::now();
  auto next_report = start + report_freq;

  long double e{0};
  unsigned long long iteration{0};

  while (not must_stop()) {
    const auto now = SteadyClock::now();
    if (now >= next_report) {
      next_report = now + report_freq;
      on_checkpoint_time(e, iteration);
    }

    e += 1.0 / CalculateFactorial(iteration++);
  }

  // Make sure we don't optimize away the above work.
  ForceCompute(e);
}
} // anonymous namespace

//==============================================================================

class CpusetAltOperation : public BaseOperation {
  void Start() override {
    BaseOperation::Start();

    _configuration = GetConfiguration<configuration>();

    ScatterWorkingThreads();
  }

  void Wait() override {
    for (std::thread &t : _working_threads) {
      if (t.joinable()) {
        t.join();
      }
    }
    _working_threads.clear();
  }

//--------------------------------------------------------------------------------------------------

 private:
  /**
   * Checks how many available cores the device has and spawns twice as many working threads.
   */
  void ScatterWorkingThreads() {
    auto total_cores = std::thread::hardware_concurrency();

    for (auto thread_ord = 0; thread_ord < total_cores * 2; ++thread_ord) {
      _working_threads.emplace_back(SpawnWorkingThreadOnSpecificCores(thread_ord));
    }
  }

  /**
   * Starts a working thread by assigning a few cores (not more than a half of all available ones).
   * The thread will stay looping calculating the Euler's number. From time to time will check back
   * that the assigned core is still among the previously assigned subset.
   * Any CPU core mismatch makes this test to fail.
   *
   * @param thread_ord a thread ordinal (0, 1, ...)
   * @return a started thread calculating the Euler's number.
   */
  std::thread SpawnWorkingThreadOnSpecificCores(const int thread_ord) {
    return std::thread{[this, &thread_ord]() {
      auto affine_cores = ::DesignateCores(thread_ord);
      if (::NarrowCoresForThisThread(affine_cores)) {
        ::Eulers_Number_Looper_NonOptimized(
            this->_configuration.checkpoint_frequency,
            [this, &affine_cores](const long double e, const unsigned long long iterations) {
              if (affine_cores.find(sched_getcpu()) == affine_cores.end()) {
                this->OnCpusetDisabledDetected();
              }
            },
            std::bind(&BaseOperation::IsStopped, this)
        );
      } else {
        this->OnCpusetDisabledDetected();
      }
    }};
  }

  /**
   * Sets the test verdict and stops its execution.
   */
  void OnCpusetDisabledDetected() {
    this->Stop();
    Log::I(TAG, "CPUSET feature unavailable on device.");
  }

 private:
  std::vector<std::thread> _working_threads;
  configuration _configuration;
};

EXPORT_ANCER_OPERATION(CpusetAltOperation)
