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

#include "JobQueue.hpp"

namespace marching_cubes {

JobQueue::JobQueue(ancer::ThreadAffinity affinity,
                   bool pinned,
                   int max_thread_count)
    : _affinity(affinity),
      _pin_threads(pinned),
      _max_thread_count(max_thread_count) {
  _thread_pool = std::make_unique<ancer::ThreadPool>(_affinity,
                                                     _pin_threads,
                                                     _max_thread_count);
  _executing_jobs = &_jobs_0;
  _queueing_jobs = &_jobs_1;
}

void JobQueue::AddJob(WorkUnit* work) {
  std::lock_guard guard(_lock);
  _queueing_jobs->push_back(work);
}

void JobQueue::RunAllReadiedJobs() {

  {
    // swap queueing and executing
    std::lock_guard guard(_lock);
    std::swap(_executing_jobs, _queueing_jobs);
  }

  for (auto job : *_executing_jobs) {
    _thread_pool->Enqueue([this, job](int thread_idx) {
      job->DoWork(thread_idx);
    });
  }

  _thread_pool->Wait();

  for (auto job : *_executing_jobs) {
    delete job;
  }

  _executing_jobs->clear();
}

} // namespace marching_cubes


