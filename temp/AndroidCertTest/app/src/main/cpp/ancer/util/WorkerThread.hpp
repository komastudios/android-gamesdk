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

#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <sched.h>
#include <unistd.h>
#include <cstdint>
#include <unordered_map>


//#include "Thread.h"

// Enable thread safety attributes only with clang.
// The attributes can be safely erased when compiling with other compilers.
#if defined(__clang__) && (!defined(SWIG))
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   // no-op
#endif

#define GUARDED_BY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define REQUIRES(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))


namespace ancer {


        enum class Affinity {
            None,
            Even,
            Odd
        };

    int32_t getNumCpus() {
        static int32_t sNumCpus = []() {
            pid_t pid = gettid();
            cpu_set_t cpuSet;
            CPU_ZERO(&cpuSet);
            sched_getaffinity(pid, sizeof(cpuSet), &cpuSet);

            int32_t numCpus = 0;
            while (CPU_ISSET(numCpus, &cpuSet)) {
                ++numCpus;
            }

            return numCpus;
        }();

        return sNumCpus;
    }

    void setAffinity(int32_t cpu) {
        cpu_set_t cpuSet;
        CPU_ZERO(&cpuSet);
        CPU_SET(cpu, &cpuSet);
        sched_setaffinity(gettid(), sizeof(cpuSet), &cpuSet);
    }

    void setAffinity(Affinity affinity) {
        const int32_t numCpus = getNumCpus();

        cpu_set_t cpuSet;
        CPU_ZERO(&cpuSet);
        for (int32_t cpu = 0; cpu < numCpus; ++cpu) {
            switch (affinity) {
                case Affinity::None:
                    CPU_SET(cpu, &cpuSet);
                    break;
                case Affinity::Even:
                    if (cpu % 2 == 0) CPU_SET(cpu, &cpuSet);
                    break;
                case Affinity::Odd:
                    if (cpu % 2 == 1) CPU_SET(cpu, &cpuSet);
                    break;
            }
        }

        sched_setaffinity(gettid(), sizeof(cpuSet), &cpuSet);
    }

    template <class ThreadState>
    class WorkerThread {
    public:
        using Work = std::function<void(ThreadState*)>;

        WorkerThread(const char* name, ancer::Affinity affinity,
            std::function<ThreadState()> thread_state_factory)
            : _name(name), _affinity(affinity),
              _thread_state_factory(thread_state_factory) {
            LaunchThread();
        }

        WorkerThread(const char* name,
                   ancer::Affinity affinity)
            : _name(name), _affinity(affinity),
            _thread_state_factory([](){ return ThreadState(); }) {
            LaunchThread();
        }

        ~WorkerThread() {
            std::lock_guard<std::mutex> threadLock(_thread_mutex);
            TerminateThread();
        }

        template <typename Func>
        void Run(Func&& work) {
            std::lock_guard<std::mutex> workLock(_work_mutex);
            _work_queue.emplace(std::forward<Func>(work));
            _work_condition.notify_all();
        }

        void Reset() {
            LaunchThread();
        }

    private:

        void LaunchThread() {
            std::lock_guard<std::mutex> threadLock(_thread_mutex);
            if ( _thread.joinable()) {
                TerminateThread();
            }
            _thread = std::thread([this]() { ThreadMain(); });
        }

        void TerminateThread() REQUIRES(_thread_mutex) {
            {
                std::lock_guard<std::mutex> workLock(_work_mutex);
                _isActive = false;
                _work_condition.notify_all();
            }
            _thread.join();
        }

        void ThreadMain() {
            ancer::setAffinity(_affinity);
            pthread_setname_np(pthread_self(), _name.c_str());

            ThreadState thread_state = _thread_state_factory();

            std::lock_guard<std::mutex> lock(_work_mutex);
            while ( _isActive ) {
                _work_condition.wait(
                        _work_mutex,
                        [this]() REQUIRES(_work_mutex) {
                            return !_work_queue.empty() || !_isActive;
                        });
                if ( !_work_queue.empty()) {
                    auto head = _work_queue.front();
                    _work_queue.pop();

                    // Drop the mutex while we execute
                    _work_mutex.unlock();
                    head(&thread_state);
                    _work_mutex.lock();
                }
            }
        }

        const std::string _name;
        const ancer::Affinity _affinity;
        std::function<ThreadState()> _thread_state_factory;

        std::mutex _thread_mutex;
        std::thread _thread GUARDED_BY(_thread_mutex);

        std::mutex _work_mutex;
        bool _isActive GUARDED_BY(_work_mutex) = true;
        std::queue<std::function<void(ThreadState*)>> _work_queue GUARDED_BY(_work_mutex);
        std::condition_variable_any _work_condition;
    };
} // namespace ancer
