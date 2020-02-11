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
#include <vector>

#include <ancer/BaseOperation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/util/Basics.hpp>
#include <ancer/util/Log.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==================================================================================================

namespace {
constexpr Log::Tag TAG{"CpusetOperation"};
} // anonymous namespace

//==================================================================================================

namespace {
struct configuration {
  // How often should threads check that affinity is working fine?
  Milliseconds checkpoint_frequency = 100ms;
};

JSON_READER(configuration) {
  JSON_OPTVAR(checkpoint_frequency);
}

//--------------------------------------------------------------------------------------------------

/**
 * This functional test reports progress at three different stages.
 */
enum class TestStage {
  /**
   * Thread setup is the stage in which threads get spawned, bound to a few cores each. This is
   * reported once per working thread.
   */
      ThreadSetup,

  /**
   * A thread in progress reports whether is still running on a core among the ones was affine to.
   * There are several progress reports per working thread.
   */
      ThreadInProgress,

  /**
   * When the whole test finishes, a last line is reported to tell whether the CPUSET feature was
   * enabled at all, whether the big.LITTLE core numbering ran from "big" to "LITTLE".
   */
      TestShutdown
};

struct datum {
  TestStage stage;

  std::vector<int> affine_to_cores;

  bool cpuset_enabled = false;
  bool bigger_cores_first = false;

  bool is_core_among_affine = false;

  static datum on_thread_setup(const std::set<int> &cores, const bool success) {
    datum datum;

    datum.stage = TestStage::ThreadSetup;
    datum.affine_to_cores = std::vector<int>{std::cbegin(cores), std::cend(cores)};
    datum.cpuset_enabled = success;

    return datum;
  }

  static datum on_thread_progress(const bool is_core_among_affine) {
    datum datum;

    datum.stage = TestStage::ThreadInProgress;
    datum.is_core_among_affine = is_core_among_affine;

    return datum;
  }

  static datum on_test_finish(const bool cpuset_enabled,
                              const bool bigger_cores_first) {
    datum datum;

    datum.stage = TestStage::TestShutdown;
    datum.cpuset_enabled = cpuset_enabled;
    datum.bigger_cores_first = bigger_cores_first;

    return datum;
  }
};

void WriteDatum(report_writers::Struct writer, const datum &data) {
  switch (data.stage) {
    case TestStage::ThreadSetup: {
      writer.AddItem("stage", "Thread setup");
      ADD_DATUM_MEMBER(writer, data, cpuset_enabled);
      ADD_DATUM_MEMBER(writer, data, affine_to_cores);
    }
      break;

    case TestStage::ThreadInProgress: {
      writer.AddItem("stage", "Thread in progress");
      ADD_DATUM_MEMBER(writer, data, is_core_among_affine);
    }
      break;

    case TestStage::TestShutdown : {
      writer.AddItem("stage", "Test summary");
      ADD_DATUM_MEMBER(writer, data, cpuset_enabled);
      ADD_DATUM_MEMBER(writer, data, bigger_cores_first);
    }
      break;
  }
}
} // anonymous namespace

//==================================================================================================

namespace {
constexpr pid_t THIS_THREAD_PID{0};
constexpr int OK{0};

/**
 * Applies a formula to select a subset of device cores, taking as input a thread ordinal.
 * It doesn't set these cores to any thread yet. This function only designates a few cores.
 * The formula it applies is such that a thread will get assigned not more than half of the
 * available cores. Also, the majority of the cores will get assigned almost evenly to the same
 * number of threads each, with a few outliers. Possibly one of the cores will get assigned to just
 * one thread whereas another could get assigned to almost all threads.
 *
 * @param thread_ord the thread ordinal (0, 1, ...).
 * @return a set of CPU cores, provided that the set won't contain more than half of the available
 * ones in the device.
 */
std::set<int> DesignateCores(const int thread_ord) {
  auto total_cores = std::thread::hardware_concurrency();
  std::set<int> cores{};

  for (auto iteration = 1; iteration <= total_cores / 2; ++iteration) {
    int core = (thread_ord < total_cores ?
                total_cores - iteration * (thread_ord + 1) :
                (iteration + 1) * (thread_ord + 1)) % total_cores;
    core = std::abs(core);

    Log::V(TAG, "Core %d assigned to working thread %d", core, thread_ord);
    cores.insert(core);
  }

  return cores;
}

/**
 * Calls the OS scheduling function that assigns affine cores to a thread. For the sake of this test
 * the thread is always "current thread" (i.e., pid_t = 0). The idea of this function is to make the
 * underlying OS function call, its parameters, more readable in the context where it's used.
 *
 * @param cpuset the cpuset_t structure containing the cores to assign.
 * @return the same output than the underlying OS function.
 */
inline auto ApplyCpusetToThisThread(const cpu_set_t *cpuset) {
  return sched_setaffinity(THIS_THREAD_PID, sizeof(cpu_set_t), cpuset);
}

/**
 * Obtains, for current thread its available cores. It does it calling the OS function that
 * populates a cpuset_t. The goal is to make it more readable than the original OS function.
 *
 * @param cpuset output reference where the available cores will be set.
 * @return the same output than the underlying OS function.
 */
inline auto GetThisThreadCpuset(cpu_set_t *cpuset) {
  return sched_getaffinity(THIS_THREAD_PID, sizeof(cpu_set_t), cpuset);
}

/**
 * The meat of this functional test. This function receives a set of cores and sets these to the
 * invoking thread. It does it applying the cpuset feature.
 *
 * @param cores a set of CPU cores
 * @return true if the core assignment via cpuset feature succeeded
 */
bool NarrowCoresForThisThread(const std::set<int> &cores) {
  bool success{false};

  cpu_set_t cpuset_in;
  cpu_set_t cpuset_out;

  CPU_ZERO(&cpuset_in);
  for (const int core : cores) {
    CPU_SET(core, &cpuset_in);
  }

  if (ApplyCpusetToThisThread(&cpuset_in) == OK && GetThisThreadCpuset(&cpuset_out) == OK) {
    if (CPU_EQUAL(&cpuset_in, &cpuset_out)) {
      success = true;
    } else {
      Log::E(TAG, "Cpuset sched_setaffinity() ended OK but didn't set affinity");
    }
  } else {
    Log::E(TAG, "Cpuset sched_setaffinity() failure (errno = %d)", errno);
  }

  return success;
}
} // anonymous namespace

//==================================================================================================

namespace {
// Loops approximating the Euler's number. Just an excuse to keep a thread busy. Just an excuse not
// to use the Pi calculation looper one more time.
void Eulers_Number_Looper(
    const Duration report_freq,
    const std::function<void(const long double, const unsigned long long)> &on_checkpoint_time,
    const std::function<bool(void)> &must_stop) {
  const auto start = SteadyClock::now();
  auto next_report = start + report_freq;

  long double e{1};
  long double addend{1};
  unsigned long long iteration{0};

  while (not must_stop()) {
    const auto now = SteadyClock::now();
    if (now >= next_report) {
      next_report = now + report_freq;
      on_checkpoint_time(e, iteration);
      Log::V(TAG, "e=%.40Lf", e);
    }

    addend /= ++iteration;
    e += addend;
  }

  // Make sure we don't optimize away the above work.
  ForceCompute(e);
}
} // anonymous namespace

//==================================================================================================

class SaneCpusetOperation : public BaseOperation {
 public:
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

    // Here's where this functional test verdict is reported.
    Report(datum::on_test_finish(_cpuset_enabled, _bigger_cores_first));
  }

//--------------------------------------------------------------------------------------------------

 private:
  /**
   * Checks how many available cores the device has and spawns ten times as many working threads.
   */
  void ScatterWorkingThreads() {
    const auto &core_sizes = ancer::GetCoreSizes();
    _working_threads.resize(core_sizes.size());

    ThreadAffinity last_core_size{ThreadAffinity::kBigCore};
    int thread_number{-1};

    std::for_each(
        std::cbegin(core_sizes),
        std::cend(core_sizes),
        [this, &last_core_size, &thread_number](const auto &core_size) {
          this->_bigger_cores_first &= (last_core_size >= core_size);
          last_core_size = core_size;

          // If the device has 4 cores, we'll spawn 40 threads (each assigned to at most 2 cores).
          // If 8 cores, 80 threads with each assigned to at most 4 cores. And so on.
          for (auto iteration = 0; iteration < 10; ++iteration) {
            this->_working_threads.emplace_back(
                this->SpawnWorkingThreadOnSpecificCores(++thread_number)
            );
          }
        });
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
    return std::thread{[this, thread_ord]() {
      auto affine_cores = ::DesignateCores(thread_ord);
      auto cpuset_setup_ok = ::NarrowCoresForThisThread(affine_cores);

      Report(datum::on_thread_setup(affine_cores, cpuset_setup_ok));

      if (cpuset_setup_ok) {
        ::Eulers_Number_Looper(
            this->_configuration.checkpoint_frequency,
            [this, &affine_cores, thread_ord](const long double e,
                                              const unsigned long long iterations) {
              auto current_core = sched_getcpu();
              auto current_thread_is_on_affine_core =
                  affine_cores.find(current_core) != affine_cores.end();

              Report(datum::on_thread_progress(current_thread_is_on_affine_core));

              if (not current_thread_is_on_affine_core) {
                Log::W(TAG, "Thread %d found on non-affine core %d.", thread_ord, current_core);
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
    Log::E(TAG, "CPUSET feature unavailable on device.");
    _cpuset_enabled = false;
    Stop();
  }

 private:
  bool _cpuset_enabled = true;
  bool _bigger_cores_first = true;

  std::vector<std::thread> _working_threads;
  configuration _configuration;
};

EXPORT_ANCER_OPERATION(SaneCpusetOperation)
