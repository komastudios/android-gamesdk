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

#ifndef thread_pool_h
#define thread_pool_h

#include <sched.h>

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <limits>
#include <mutex>
#include <random>
#include <thread>

#include <ancer/util/Time.hpp>
#include <ancer/util/Log.hpp>
#include <ancer/System.hpp>

namespace ancer {

/*
 * A simple thread pool with a Wait() function to enable blocking until
 * queued jobs have completed
 */
class ThreadPool {
 private:
  static constexpr ancer::Log::Tag TAG{"ThreadPool"};

 public:

  /*
   * SleepConfig is a specification for if/how to periodically sleep
   * worker threads to mitigate core(s) overheating and being throttled
   * by the OS. By default, ThreadPool uses SleepConfig::None(), which is
   * does nothing and allows threads to run at full bore.
   * But! It's been observed that a little periodic sleeping of threads
   * can actually result in increased performance for long running tasks.
   */
  struct SleepConfig {
    enum class Method {
      None, Sleep, Spinlock
    };

    // how often each thread will be slept
    const Duration period{0};

    // how long the thread will sleep for
    const Duration duration{0};

    // the technique used to sleep the thread
    const Method method{Method::None};

    SleepConfig(Duration period, Duration duration, Method method) :
        period(period), duration(duration), method(method) {}

    SleepConfig() = default;

    SleepConfig(const SleepConfig&) = default;

    SleepConfig(SleepConfig&&) = default;

    static SleepConfig None() {
      return SleepConfig{Duration{0}, Duration{0}, Method::None};
    }
  };

  typedef std::function<void(int)> WorkFn;

  /*
   * Create a ThreadPool
   * affinity: The thread affinity (big core, little core, etc)
   * pin_threads: if true, pin threads to the CPU they're on
   * max_thread_count: allow setting # cpus to a smaller value than the
   *   number of cores in the affinity group
   * sleep_config: A sleep configuration to use to periodically sleep
   *   each thread, to mitigate core overheating/throttling
   */
  ThreadPool(ThreadAffinity affinity,
             bool pin_threads,
             int max_thread_count = std::numeric_limits<int>::max(),
             SleepConfig sleep_config = SleepConfig::None())
      : _sleep_config(sleep_config) {
    auto count = std::max(std::min(NumCores(affinity), max_thread_count), 1);

    for (auto i = 0; i < count; ++i) {
      auto cpu_idx = pin_threads ? i : -1;
      _elapsed_work_times.push_back(Duration{0});
      _workers.emplace_back([this, i, affinity, cpu_idx]() {
        ThreadLoop(affinity, cpu_idx, i);
      });
    }
  }

  ~ThreadPool() {
    // set stop-condition
    std::unique_lock<std::mutex> latch(_queue_mutex);
    _stop_requested = true;
    _cv_task.notify_all();
    latch.unlock();

    // all threads terminate, then we're done.
    for (auto& t : _workers)
      t.join();
  }

  size_t NumThreads() const { return _workers.size(); }

  /*
   * Enqueue a job to run; job signature is:
   * void fn(int thread_idx) where thread_idx is an int
   * from [0,NumThreads-1] stably identifying the thread
   * executing the job
   */
  template<class F>
  void Enqueue(F&& f) {
    std::unique_lock<std::mutex> lock(_queue_mutex);
    _tasks.emplace_back(std::forward<F>(f));
    _cv_task.notify_one();
  }

  void EnqueueAll(const std::vector<WorkFn>& fns) {
    std::unique_lock<std::mutex> lock(_queue_mutex);
    std::copy(fns.begin(), fns.end(), std::back_inserter(_tasks));
    _cv_task.notify_all();
  }

  /*
   * Wait for all queued jobs to complete
   */
  void Wait() {
    std::unique_lock<std::mutex> lock(_queue_mutex);
    _cv_finished.wait(lock,
                      [this]() { return _tasks.empty() && (_busy == 0); });
  }

 private:

  void ThreadLoop(ancer::ThreadAffinity affinity, int cpu_idx, int thread_idx) {
    ancer::SetThreadAffinity(cpu_idx, affinity);

    while (true) {
      std::unique_lock<std::mutex> latch(_queue_mutex);
      _cv_task.wait(latch,
                    [this]() { return _stop_requested || !_tasks.empty(); });

      if (!_tasks.empty()) {
        ++_busy;

        auto fn = _tasks.front();
        _tasks.pop_front();

        // release lock. run async
        latch.unlock();

        // run function outside context
        const auto start_time = SteadyClock::now();

        fn(thread_idx);

        const auto end_time = SteadyClock::now();
        _elapsed_work_times[thread_idx] += (end_time - start_time);

        if (_sleep_config.method != SleepConfig::Method::None &&
          _sleep_config.duration > Duration{0} &&
          !_sleeping) {
          _sleeping = true;

          // work out how long to sleep; we sleep for SleepConfig::duration
          // per SleepConfig::period of work; if a thread di something really
          // time intensive, we don't want to sleep forever, so max out at 3
          // sleep periods (this is arbitrary)
          auto ref =_elapsed_work_times[thread_idx];
          auto duration = (ref / _sleep_config.period) * _sleep_config.duration;
          duration = std::min<Duration>(duration, 3 * duration);

          Sleep(duration);
          _elapsed_work_times[thread_idx] = Duration{0};

          _sleeping = false;
        }

        latch.lock();
        --_busy;
        _cv_finished.notify_one();
      } else if (_stop_requested) {
        break;
      }
    }
  }

 private:

  void Sleep(Duration duration)
  {
    switch(_sleep_config.method){

      case SleepConfig::Method::None:break;
      case SleepConfig::Method::Sleep:
        std::this_thread::sleep_for(duration);
        break;
      case SleepConfig::Method::Spinlock: {
        auto now = SteadyClock::now();
        while ((SteadyClock::now() - now) < duration) {
          std::this_thread::yield();
        }
        break;
      }
    }
  }

 private:

  std::vector<std::thread> _workers;
  std::vector<Duration> _elapsed_work_times;
  std::deque<WorkFn> _tasks;
  std::mutex _queue_mutex;
  std::condition_variable _cv_task;
  std::condition_variable _cv_finished;
  SleepConfig _sleep_config;
  std::atomic_bool _sleeping;
  unsigned int _busy = 0;
  bool _stop_requested = false;

};

} // namespace ancer

#endif /* thread_pool_h */
