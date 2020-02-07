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

#include <ancer/util/ThreadPool.hpp>
#include <mutex>
#include <vector>

namespace marching_cubes {


  class JobQueue {
   public:

    class WorkUnit {
     public:
      virtual ~WorkUnit() = default;
      virtual void DoWork(int thread_idx) = 0;
    };


   public:
    JobQueue(ancer::ThreadAffinity affinity, bool pinned, int max_thread_count);
    JobQueue() = delete;
    JobQueue(const JobQueue&) = delete;
    JobQueue(JobQueue &&) = delete;
    ~JobQueue() = default;

    void RunAllReadiedJobs();
    void AddJob(WorkUnit*);

    size_t NumThreads() const { return _thread_pool->NumThreads(); }

   private:
    ancer::ThreadAffinity _affinity;
    bool _pin_threads;
    int _max_thread_count;

    std::mutex _lock;
    std::unique_ptr<ancer::ThreadPool> _thread_pool;
    std::vector<WorkUnit*> _jobs_0;
    std::vector<WorkUnit*> _jobs_1;

    std::vector<WorkUnit*>* _executing_jobs;
    std::vector<WorkUnit*>* _queueing_jobs;
  };

}
