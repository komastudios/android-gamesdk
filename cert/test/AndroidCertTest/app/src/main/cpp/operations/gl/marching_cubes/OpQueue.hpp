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

#ifndef op_queue_hpp
#define op_queue_hpp

#include <functional>
#include <list>
#include <mutex>

#include <ancer/util/UnownedPtr.hpp>

namespace marching_cubes {

class OperationQueue {
 public:
  typedef std::function<void()> OperationFn;

  OperationQueue() = default;
  OperationQueue(const OperationQueue &) = delete;
  OperationQueue(OperationQueue &&) = delete;
  ~OperationQueue() = default;

  void add(OperationFn op) {
    std::lock_guard lock(_lock);
    _operations.push_back(op);
  }

  void drain() {
    std::lock_guard lock(_lock);
    for (const auto &op : _operations) {
      op();
    }
    _operations.clear();
  }

 private:
  std::mutex _lock;
  std::list<OperationFn> _operations;
};

/**
 * Get the singleton OperationQueue meant for
 * being processed by the app's main thread
 */
ancer::unowned_ptr<OperationQueue> MainThreadQueue();

}

#endif
