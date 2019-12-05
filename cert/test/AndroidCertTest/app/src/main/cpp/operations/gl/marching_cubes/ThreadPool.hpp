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

class ThreadPool {
 private:
  ancer::Log::Tag TAG{"ThreadPool"};

 public:
  ThreadPool(ancer::ThreadAffinity affinity,
      bool pin_threads,
      int max_thread_count = std::numeric_limits<int>().max()) {
    auto count = max(min(ancer::NumCores(affinity), max_thread_count), 1);

    for (auto i = 0; i < count; ++i) {
      auto cpu_idx = pin_threads ? i : -1;
      _workers.emplace_back([this, i, affinity, cpu_idx]() {
        ThreadLoop(affinity, cpu_idx);
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

  template<class F>
  void Enqueue(F&& f) {
    std::unique_lock<std::mutex> lock(_queue_mutex);
    _tasks.emplace_back(std::forward<F>(f));
    _cv_task.notify_one();
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(_queue_mutex);
    _cv_finished.wait(lock, [this]() { return _tasks.empty() && (_busy == 0); });
  }

 private:

  void ThreadLoop(ancer::ThreadAffinity affinity, int cpu_idx) {
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
        fn();

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
  std::deque<std::function<void()>> _tasks;
  std::mutex _queue_mutex;
  std::condition_variable _cv_task;
  std::condition_variable _cv_finished;
  unsigned int _busy = 0;
  bool _stop_requested = false;

};

#endif /* thread_pool_h */
