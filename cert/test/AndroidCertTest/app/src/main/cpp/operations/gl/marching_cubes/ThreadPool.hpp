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

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

#include <ancer/util/Time.hpp>
#include <ancer/util/Log.hpp>
#include <ancer/System.hpp>

namespace marching_cubes {

/**
 * ThreadPool
 * A minimalist thread pool allowing addition of work items, returning a handle
 * for each which may be used to wait on the work's completion.
 * Does not do anything fancy like marshalling arguments or return values.
 */
class ThreadPool {
 public:
  using Duration = ancer::Duration;
  using ThreadAffinity = ancer::ThreadAffinity;

/*
   * SleepConfig is a specification for if/how to periodically sleep
   * worker threads to mitigate core(s) overheating and being throttled
   * by the OS. By default, ThreadPool uses SleepConfig::None(), which is
   * does nothing and allows threads to run at full bore.
   * But! It's been observed that a little periodic sleeping of threads
   * can, *in some cases* result in improved performance for long running tasks.
   * By making sleep optional/configurable for ThreadPool we can experiment
   * with different sleep scenarios, to plot best practices.
   */
  struct SleepConfig {
    enum class Method {
      None, Sleep, Spinlock
    };

    // how often each thread will be slept; each thread will sleep
    // after it's done this much work.
    const Duration period{0};

    // when a thread has worked for `period`, it will be slept for
    // this long
    const Duration duration{0};

    // the technique used to sleep the thread; if Method::None,
    // sleep is a no-op
    const Method method{Method::None};

    SleepConfig(Duration period, Duration duration, Method method) :
        period(period), duration(duration), method(method) {}

    SleepConfig(const SleepConfig &) = default;

    SleepConfig(SleepConfig &&) = default;

    // convenience function to create a default no-op sleep config
    static SleepConfig None() {
      return SleepConfig{Duration{0}, Duration{0}, Method::None};
    }
  };

  /**
   * Work function definition. Receives the stable index of the thread
   * executing the job. This will be a value from [0, size())
   */
  typedef std::function<void(int)> WorkFn;

 public:
  /**
   * Create a ThreadPool that will use a specified number of threads,
   * and which will optionally pin those threads to specific CPUs.
   */
  ThreadPool(ThreadAffinity affinity,
             bool pin_threads,
             int max_thread_count = std::numeric_limits<int>::max(),
             SleepConfig sleep_config = SleepConfig::None());

  ~ThreadPool();

  /**
   * Enqueue a job to execute on the pool.
   * see `WorkFn`; job receives index of thread in pool
   * executing its work.
   * Returns a std::future which can be waited on to block
   * until job is complete.
   */
  template<class F>
  std::future<void> enqueue(F &&f);

  /**
   * Return number of threads being used
   */
  size_t size() const { return _workers.size(); }

 private:

  void Sleep(Duration duration);

 private:
  std::vector<std::thread> _workers;
  std::queue<std::function<void(int)>> _tasks;

  // synchronization
  std::mutex _queue_mutex;
  std::condition_variable _condition;
  bool _stop;

  SleepConfig _sleep_config;
  std::vector<Duration> _elapsed_work_times;
  std::atomic_bool _sleeping;
  unsigned int _busy = 0;

};

inline ThreadPool::ThreadPool(ThreadAffinity affinity,
                              bool pin_threads,
                              int max_thread_count,
                              SleepConfig sleep_config)
    : _sleep_config(sleep_config), _stop(false) {
  auto count = std::max(std::min(NumCores(affinity), max_thread_count), 1);
  _elapsed_work_times.resize(count);
  for (size_t i = 0; i < count; ++i)
    _workers.emplace_back(
        [this, i, pin_threads, affinity]() -> void {
          auto cpu_idx = pin_threads ? i : -1;
          _elapsed_work_times[i] = Duration{0};
          ancer::SetThreadAffinity(cpu_idx, affinity);

          for (;;) {
            std::function<void(int)> task;

            {
              std::unique_lock<std::mutex> lock(this->_queue_mutex);
              this->_condition.wait(lock,
                                    [this] { return this->_stop || !this->_tasks.empty(); });

              if (this->_stop && this->_tasks.empty())
                return;

              task = std::move(this->_tasks.front());
              this->_tasks.pop();
            }

            // run function outside context
            const auto start_time = ancer::SteadyClock::now();

            task(i);

            const auto end_time = ancer::SteadyClock::now();
            _elapsed_work_times[i] += (end_time - start_time);

            if (_sleep_config.method != SleepConfig::Method::None &&
                _sleep_config.duration > Duration{0} &&
                !_sleeping) {
              _sleeping = true;

              // work out how long to sleep; we sleep for SleepConfig::duration
              // per SleepConfig::period of work; if a thread di something really
              // time intensive, we don't want to sleep forever, so max out at 3
              // sleep periods (this is arbitrary)
              auto ref = _elapsed_work_times[i];
              auto duration = (ref / _sleep_config.period) * _sleep_config.duration;
              duration = std::min<ancer::Duration>(duration, 3 * duration);

              Sleep(duration);
              _elapsed_work_times[i] = ancer::Duration{0};

              _sleeping = false;
            }
          }
        });
}

template<class F>
std::future<void> ThreadPool::enqueue(F &&f) {
  auto task = std::make_shared<
      std::packaged_task<void(int) >>(std::bind(std::forward<F>(f), std::placeholders::_1));

  std::future<void> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(_queue_mutex);

    // don't allow enqueueing after stopping the pool
    if (_stop) {
      throw std::runtime_error("[ThreadPool::enqueue] - enqueue on stopped ThreadPool");
    }

    _tasks.emplace([task](int idx) { (*task)(idx); });
  }
  _condition.notify_one();
  return res;
}

inline ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(_queue_mutex);
    _stop = true;
  }
  _condition.notify_all();
  for (std::thread &worker : _workers)
    worker.join();
}

inline void ThreadPool::Sleep(Duration duration) {
  switch (_sleep_config.method) {

    case SleepConfig::Method::None:break;
    case SleepConfig::Method::Sleep:std::this_thread::sleep_for(duration);
      break;
    case SleepConfig::Method::Spinlock: {
      auto now = ancer::SteadyClock::now();
      while ((ancer::SteadyClock::now() - now) < duration) {
        std::this_thread::yield();
      }
      break;
    }
  }
}

} // namespace marching_cubes

#endif /* thread_pool_h */
