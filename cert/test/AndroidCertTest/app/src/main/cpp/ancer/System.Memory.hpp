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

#ifndef _SYSTEM_MEMORY_HPP
#define _SYSTEM_MEMORY_HPP

namespace ancer {

/**
 * A data structure to convey fine-grained aspects about the device memory.
 * @see ancer::GetMemoryInfo()
 */
struct MemoryInfo {
  /**
   * Current out-of-memory score (higher is worse).
   */
  int _oom_score = 0;

  /**
   * Current heap allocation size in bytes.
   */
  long native_heap_allocation_size = 0;

  /**
   * Current commit limit in bytes.
   */
  long commit_limit = 0;

  /**
   * Total memory in bytes "accessible to kernel"
   */
  long total_memory;

  /**
   * Available memory in-system.
   */
  long available_memory;

  /**
   * Point where the system would enter a low-memory condition.
   */
  long low_memory_threshold;

  /**
   * True if we're currently in a low-memory condition.
   */
  bool low_memory;
};

/**
 * Get current memory information about the system. This is sourced from the host activity, via
 * BaseHostActivity.getMemoryHelper().getMemoryInfo().
 */
MemoryInfo GetMemoryInfo();

/**
 * Run the java vm garbage collector
 */
void RunSystemGc();
}

#endif  // _SYSTEM_MEMORY_HPP
