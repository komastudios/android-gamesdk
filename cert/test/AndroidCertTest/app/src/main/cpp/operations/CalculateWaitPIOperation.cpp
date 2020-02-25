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

/**
 * Heats up the device to measure performance under load. It spawns as many working threads as cores
 * are available in the device. It can be configured to use several different throttling mechanisms.
 *
 * Input configuration:
 * - affinity: if true, each thread will get assigned 1:1 to a particular CPU core. Default is
 *             false.
 * - looping_period: the duration of a calculate Pi loop (until waiting a while for the next loop).
 * - wait_period: the time a thread spends between two pi calculation loops.
 * - wait_method: specifies "how" threads wait. There are three different mechanisms:
 *     - "spinlock": the thread loops checking whether the wait period was accomplished. That said,
 *                   the thread is waiting but not sleeping.
 *     - "sleep": the thread sleeps for the duration of wait_period. The OS will awake it
 *                afterwards.
 *     - "mutex": perhaps a better name would have been "synchronize". In this mechanism, the first
 *                thread that starts waiting, actually waits for all the other threads to also start
 *                waiting. Once they all wait, none of them will resume calculating pi until the
 *                rest of them are ready to resume. In other words, the first one to reach the wait
 *                period, will wait longer than the rest: it won't resume until the last one who
 *                waited is ready to resume.
 *
 *
 *
 * Output report: lines are grouped under two major groups: operation_id = CalculateWaitPIOperation
 *                and MonitorOperation.
 * * MonitorOperation includes, among other data, temperature_info that informs, among other data
 *   points, max_cpu_temperature at a given timestamp.
 *
 * * CalculateWaitPIOperation has an initial line that reports:
 *   - wait_method as a string (see wait_method_name in Input Configuration section)
 *   - affinity
 *   Thereafter, any other CalculateWaitPIOperation informs:
 *   - thread_id: a pthread ID that the line refers to.
 *   - cpu_id: only makes sense if affinity is true. Otherwise, just the random CPU at the time of
 *             reporting this line.
 *   - t0: time where Pi started being calculated, or resumed calculating.
 *   - t1: time where Pi calculation got suspended (wait interval).
 *   - iterations: how many Pi compute loops occurred between t0 and t1.
 */

#include <array>
#include <cmath>
#include <mutex>
#include <sstream>

#include <ancer/BaseOperation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/ThreadSyncPoint.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;

// Note: The "duration" set in configuration.json must be slightly longer than
// N times the "interval_between_samples", where N is some whole
// number, as there may be a delay of several milliseconds before the last sets
// of data are recorded.

//==============================================================================

namespace {
constexpr Log::Tag TAG{"CalculateWaitPIOperation"};
}  // end anonymous namespace

//==============================================================================

namespace {
constexpr std::string_view kMutexName = "mutex";
constexpr std::string_view kSleepName = "sleep";
constexpr std::string_view kSpinlockName = "spinlock";
}  // end anonymous namespace

//==============================================================================

namespace {
/**
 * In the wait-mutex method, this sync point helps get all thread wait together.
 */
ThreadSyncPoint sync_point = {0};

struct thread_datum;  // forward declaration

/**
 * This type models a pointer to function that, based on the wait_method, will be any of the three
 * options (mutex, spinlock or sleep).
 */
using p_wait_function_t = void (*)(const thread_datum &, const Timestamp &);

/**
 * Pointer to the wait function that corresponds to the configuration wait_method.
 */
p_wait_function_t p_wait_function;
}  // end anonymous namespace

//==============================================================================

namespace {
/**
 * Input data.
 */
struct configuration {
  Milliseconds looping_period{Milliseconds::zero()};
  Milliseconds wait_period{Milliseconds::zero()};

  bool affinity;
  std::string wait_method;  // refer to kWaitMutex, etc.
};

JSON_READER(configuration) {
  JSON_REQVAR(looping_period);
  JSON_REQVAR(wait_period);

  JSON_REQVAR(affinity);
  JSON_REQVAR(wait_method);
}
}  // end anonymous namespace

//==============================================================================

namespace {
/**
 * Report first trace, corresponding to the runtime parameters (affinity, wait method).
 */
struct runtime_datum {
  std::string wait_method;
  bool affinity;
};

void WriteDatum(report_writers::Struct w, const runtime_datum &r) {
  ADD_DATUM_MEMBER(w, r, wait_method);
  ADD_DATUM_MEMBER(w, r, affinity);
}

/**
 * Report regular data, corresponding to a time interval and how many iterations a given thread
 * performed during it.
 * Unlike other operation report structs, this one has some functions to better encapsulate its
 * internals (like when the interval start time should be set, when the finish time, etc.)
 */
struct thread_datum {
  Timestamp t0;
  Timestamp t1;
  uint64_t iterations;

  const bool designated_thread;

  thread_datum(const configuration &config, const bool designated_thread) :
      _config{config}, designated_thread{designated_thread},
      _k{1.0}, _denominator{1.0}, _accumulator{1.0} {
    ResetForNextLoopingPeriod();
  }

  /**
   * Sets back some critic struct internals (t0, iteration counter, etc.) for the next series of Pi
   * calculation loops.
   */
  void ResetForNextLoopingPeriod() {
    t0 = SteadyClock::now();
    iterations = 0;
    _next_wait = t0 + _config.looping_period;
  }

  /**
   * Checks whether is time to pause the Pi calculation loop and wait. If so, waits per wait method.
   *
   * @param sync_point a coordination point for all other threads in case the wait method is mutex.
   * @return true if the thread paused to wait. False if it wasn't time for that yet.
   */
  bool WaitIfAboutTime(ThreadSyncPoint &sync_point) {
    t1 = SteadyClock::now();
    bool wait_time_arrived{t1 >= _next_wait};  // Wait/throttle

    if (wait_time_arrived) {
      (*p_wait_function)(*this, t1 + _config.wait_period);
    }

    return wait_time_arrived;
  }

  /**
   * Adds to the Pi progressive accumulator another term based on Leibniz' series
   * (1 - 1/3 + 1/5 - 1/7 ... = Pi/4).
   */
  void IterateCalculus() {
    _k *= -1.0;         // -1, 1, -1, 1, ...
    _denominator += 2;  // 3, 5, 7, ...
    _accumulator += _k/_denominator;  // 1, 1 -1/3, 1 -1/3 +1/5, 1 -1/3 +1/5 -1/7 ...
    volatile double value = _accumulator*4.0;  // Pi = 4 * Leibniz' series sum
    ++iterations;

    Log::V(TAG, "Pi = %.10f", value);
  }

 private:
  const configuration &_config;

  Timestamp _next_wait;

  double _k;
  double _denominator;
  double _accumulator;
};

void WriteDatum(report_writers::Struct w, const thread_datum &p) {
  ADD_DATUM_MEMBER(w, p, t0);
  ADD_DATUM_MEMBER(w, p, t1);
  ADD_DATUM_MEMBER(w, p, iterations);
}
}  // end anonymous namespace

//==============================================================================

namespace {
/**
 * Asks the OS to stop the thread calling this function until certain time.
 *
 * @param until earliest time when the thread can be awaken by the OS.
 */
void WaitSleeping(const thread_datum &thread_datum, const Timestamp &until) {
  std::this_thread::sleep_until(until);
}

/**
 * Rather than sleeping via OS, a thread that waits spinning just checks whether is time to resume
 * and, if not, checks again and again and again until such time arrives.
 *
 * @param until when to resume.
 */
void WaitSpinning(const thread_datum &thread_datum, const Timestamp &until) {
  // Like an anxious child asking dad how long else the ride will take, we stall this thread asking
  // "have we arrived already? No? Now, how we arrived already?"
  while (SteadyClock::now() < until);
}

/**
 * Make all threads wait together. They'll resume looping together as well.
 *
 * @param until the thread at core 0 will wait until the time indicated by this parameter. All the
 *        other threads will stall until that wait is over.
 */
void WaitMutex(const thread_datum &thread_datum, const Timestamp &until) {
  auto thread_id = std::this_thread::get_id();

  // All threads will stop here until the last of them comes.
  sync_point.Sync(thread_id);

  // As long as one thread stays spinning and doesn't reach the next sync point, all the other
  // threads will stay waiting for it at that sync point. That way, we just need one thread to wait
  // to stall them all.
  if (thread_datum.designated_thread) {
    WaitSpinning(thread_datum, until);
  }

  // All threads will stop here again, to resume looping all together.
  sync_point.Sync(thread_id);
}
}  // end anonymous namespace

//==============================================================================

/**
 * This operation spawns threads that approximate Pi (with some dead times in between each) with the
 * goal to heat up the CPU.
 */
class CalculateWaitPIOperation : public BaseOperation {
 public:
  CalculateWaitPIOperation() = default;

  void Start() override {
    BaseOperation::Start();

    const auto config = GetConfiguration<configuration>();
    SetWaitMethodBasedOnConfiguration(config);
    Report(runtime_datum{config.wait_method, config.affinity});

    const int num_cores = std::thread::hardware_concurrency();
    sync_point.Reset(num_cores);

    // This loop creates working threads. The first one will be designated to wait for all if wait
    // method is mutex
    for (int index = 0; index < num_cores; ++index) {
      _threads.emplace_back([this, index, config]() {
        if (config.affinity) SetThreadAffinity(index);
        bool is_designated_thread{index==0};
        PiLooper(config, is_designated_thread);
      });
    }
  }

  void Wait() override {
    for (std::thread &t : _threads) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

 private:
  /**
   * Starts a loop that alternates computing iterations with wait times. Performs an iterative
   * calculation of PI, not with the goal of accuracy but to heat up the CPU.
   *
   * @param config a reference to the original operation configuration.
   * @param designated_thread when wait method is mutex, one thread is designated to wait, stalling
   *                          the others until ready to resume.
   */
  void PiLooper(const configuration &config, const bool designated_thread) {
    ANCER_SCOPED_TRACE("CalculatePIOperation::PiLooper");

    thread_datum thread_data{config, designated_thread};

    while (!IsStopped()) {
      if (thread_data.WaitIfAboutTime(sync_point)) {
        Report(thread_data);
        thread_data.ResetForNextLoopingPeriod();
        continue;
      }

      thread_data.IterateCalculus();
    }
  }

  void SetWaitMethodBasedOnConfiguration(const configuration &config) const {
    if (kMutexName==config.wait_method) {
      p_wait_function = &WaitMutex;
    } else if (kSleepName==config.wait_method) {
      p_wait_function = &WaitSleeping;
    } else if (kSpinlockName==config.wait_method) {
      p_wait_function = &WaitSpinning;
    } else {
      FatalError(TAG, "unrecognized value for config.wait_method_name");
    }
  }

 private:
  std::vector<std::thread> _threads;
};

EXPORT_ANCER_OPERATION(CalculateWaitPIOperation)