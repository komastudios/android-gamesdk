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
#include <array>

#include <ancer/BaseOperation.hpp>
#include <ancer/util/ThreadSyncPoint.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>
#include <unistd.h>

using namespace ancer;

// Purpose: Heats up the device to measure performance under load. Can be
// configured to use several different throttling mechanisms.

// Note: The "duration" set in configuration.json must be slightly longer than
// N times the "interval_between_samples", where N is some whole
// number, as there may be a delay of several milliseconds before the last sets
// of data are recorded.

//==============================================================================

namespace {
constexpr Log::Tag TAG{"calculate_pi_native"};

constexpr auto ITERATION_SAMPLE_PERIOD(int64_t(100));

constexpr auto PI = 3.14159265358979323846;
}  // end anonymous namespace

//==============================================================================

namespace {
const std::string wait_mutex_name = "mutex";
const std::string wait_sleep_name = "sleep";
const std::string wait_spinlock_name = "spinlock";

enum class WaitMethod { Mutex, Sleep, Spinlock };
}  // end anonymous namespace

//==============================================================================

namespace {
struct configuration {
  Milliseconds interval_between_samples{Milliseconds::zero()};
  Milliseconds interval_between_wait_periods{Milliseconds::zero()};
  Milliseconds wait_period{Milliseconds::zero()};
  Milliseconds heat_time{Milliseconds::zero()};

  bool affinity{};
  std::string wait_method_name;  // refer to `wait_mutex_name`, etc.
  WaitMethod wait_method;
};

JSON_READER(configuration) {
  JSON_REQVAR(interval_between_samples);
  JSON_REQVAR(interval_between_wait_periods);
  JSON_REQVAR(wait_period);
  JSON_REQVAR(heat_time);

  JSON_REQVAR(affinity);
  JSON_REQVAR(wait_method_name);
}
}  // end anonymous namespace

//==============================================================================

namespace {
struct pi_datum {
  double value;
  uint64_t iterations;
  uint64_t first_half_iterations;
  uint64_t last_half_iterations;
};

JSON_WRITER(pi_datum) {
  JSON_REQVAR(iterations);
  JSON_REQVAR(first_half_iterations);
  JSON_REQVAR(last_half_iterations);
}

//------------------------------------------------------------------------------

struct core_sample_datum {
  int core_id;
  std::vector<uint64_t> iterations;

  core_sample_datum(int id) { core_id = id; }
};

JSON_WRITER(core_sample_datum) {
  JSON_REQVAR(iterations);
  JSON_REQVAR(core_id);
}

struct core_performance_samples {
  std::vector<core_sample_datum> cores;
};

JSON_WRITER(core_performance_samples) { JSON_REQVAR(cores); }
}  // end anonymous namespace

//==============================================================================

/*
 * CalculatePIOperation
 * Computes PI, iteratively, to heat up the CPU
 */
class CalculateWaitPIOperation : public BaseOperation {
 public:
  CalculateWaitPIOperation() = default;

  ~CalculateWaitPIOperation() { Report(_performance_samples); }

  void Start() override {
    BaseOperation::Start();

    const auto config = ValidateConfig(GetConfiguration<configuration>());
    Report(config.wait_method_name);

    const int num_cores = std::thread::hardware_concurrency();
    _sync_point.Reset(num_cores);

    for (int i = 0; i < num_cores; ++i) {
      _performance_samples.cores.push_back(core_sample_datum(i));
    }

    for (int i = 0; i < num_cores; ++i) {
      _threads.emplace_back([this, i, config]() {
        if (config.affinity) {
          SetThreadAffinity(i);
        }
        CalculatePi(config, _performance_samples.cores[i], _sync_point);
      });
    }
  }

  void Wait() override {
    for (std::thread& t : _threads) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

 private:
  /*
   * Performs an iterative calculation of PI for a time duration; note: the goal
   * is not accuracy, but just to heat up the CPU.
   * @param calculationDuration the total test duration time. If 0, run
   * indefinitely
   * @param reportSamplingDuration if > 0, a sample will be recorded every
   * sampling duration
   * @return the result (duration, iterations, result pi, etc)
   */
  void CalculatePi(const configuration& config, core_sample_datum& current_core,
                   ThreadSyncPoint& sync_point) {
    ANCER_SCOPED_TRACE("CalculatePIOperation::calculate_pi");

    using namespace std::chrono;

    // Leibniz's infinite series approximation: 1 - 1/3 + 1/5 - 1/7 ... = Pi/4.
    // Each term is calculated as 1.0 / pi_d, negative if pi_k is a factor of 2.
    uint64_t pi_k = 0;  // k is 0, 1, 2, ...
    uint64_t pi_d = 3;  // denominator for the kth term of the approximation
    double accumulator = 1.0;
    uint64_t iteration_count = 0;

    pi_datum pi_data{};

    const time_point t0 = SteadyClock::now();
    time_point next_wait_time = t0 + config.interval_between_wait_periods;
    time_point next_sample_time = t0 + config.interval_between_samples;
    const time_point heat_epoch_time = t0 + config.heat_time;

    while (!IsStopped()) {
      const time_point now = SteadyClock::now();

      // Wait/throttle
      if (now >= next_wait_time) {
        switch (config.wait_method) {
          case WaitMethod::Mutex: {
            sync_point.Sync(std::this_thread::get_id());

            // Use one thread to sleep them all.
            if (current_core.core_id == 0) {
              while ((SteadyClock::now() - now) < config.wait_period)
                ;
            }

            sync_point.Sync(std::this_thread::get_id());
            break;
          }
          case WaitMethod::Sleep: {
            std::this_thread::sleep_for(config.wait_period);
            break;
          }
          case WaitMethod::Spinlock: {
            while ((SteadyClock::now() - now) < config.wait_period)
              ;
            break;
          }
        }

        next_wait_time =
            SteadyClock::now() + config.interval_between_wait_periods;
        continue;
      }

      // Update heat value
      // (This first/last half iteration count is no longer being used, but we
      // keep it in because it does contribute to slowing down the inner loop.
      // If we removed it, we wouldn't be able to compare with older results.)
      if (now < heat_epoch_time) {
        ++pi_data.first_half_iterations;
      } else {
        ++pi_data.last_half_iterations;
      }

      ++iteration_count;

      // Record performance sample
      if (now >= next_sample_time) {
        next_sample_time = SteadyClock::now() + config.interval_between_samples;
        current_core.iterations.push_back(iteration_count);
        iteration_count = 0;
      }

      // Update Pi approximation
      // Leibniz's infinite series approximation: Pi/4 = 1 - 1/3 + 1/5 - 1/7 ...
      const double factor =
          1.0 / static_cast<double>((pi_k % 2 == 0) ? -pi_d : pi_d);
      accumulator += factor;
      pi_data.value = accumulator * 4.0;

      ++pi_k;
      pi_d += 2;
      ++pi_data.iterations;
    }

    Report(pi_datum{pi_data});
  }

  configuration ValidateConfig(configuration config) const {
    if (wait_mutex_name == config.wait_method_name) {
      config.wait_method = WaitMethod::Mutex;
    } else if (wait_sleep_name == config.wait_method_name) {
      config.wait_method = WaitMethod::Sleep;
    } else if (wait_spinlock_name == config.wait_method_name) {
      config.wait_method = WaitMethod::Spinlock;
    } else {
      FatalError(TAG, "unrecognized value for config.wait_method_name");
    }
    return config;
  }

 private:
  core_performance_samples _performance_samples;
  std::vector<std::thread> _threads;
  ThreadSyncPoint _sync_point = {0};
};

EXPORT_ANCER_OPERATION(CalculateWaitPIOperation)