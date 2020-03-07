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

#include "OpQueue.hpp"

namespace marching_cubes {

ancer::unowned_ptr<OperationQueue> MainThreadQueue() {
  static std::mutex _lock;
  static std::unique_ptr<OperationQueue> _queue = nullptr;

  std::lock_guard lock(_lock);
  if (!_queue) {
    _queue = std::make_unique<OperationQueue>();
  }

  return _queue.get();
}

}
