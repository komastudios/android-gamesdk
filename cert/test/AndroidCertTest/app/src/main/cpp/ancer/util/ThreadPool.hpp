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
#include <deque>
#include <functional>
#include <limits>
#include <mutex>
#include <random>
#include <thread>

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

  typedef std::function<void(int)> WorkFn;

  /*
   * Create a ThreadPool
   * affinity: The thread affinity (big core, little core, etc)
   * pin_threads: if true, pin threads to the CPU they're on
   * max_thread_count: allow setting # cpus to a smaller value than the
   *   number of cores in the affinity group
   */
  ThreadPool(ThreadAffinity affinity,
             bool pin_threads,
             int max_thread_count = std::numeric_limits<int>::max()) {
    auto count = std::max(std::min(
        NumCores(affinity), max_thread_count), 1);

    for (auto i = 0; i < count; ++i) {
      auto cpu_idx = pin_threads ? i : -1;
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
        fn(thread_idx);

        latch.lock();
        --_busy;
        _cv_finished.notify_one();
      } else if (_stop_requested) {
        break;
      }
    }
  }

 private:

  std::vector<std::thread> _workers;
  std::deque<WorkFn> _tasks;
  std::mutex _queue_mutex;
  std::condition_variable _cv_task;
  std::condition_variable _cv_finished;
  unsigned int _busy = 0;
  bool _stop_requested = false;

};

} // namespace ancer

#endif /* thread_pool_h */
