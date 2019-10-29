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
#include <queue>
#include <thread>

#include "Thread.h"


namespace ancer {
    template <class ThreadState>
    class WorkerThread {
    public:
        using Work = std::function<void(ThreadState*)>;

        WorkerThread(const char* name, samples::Affinity affinity)
                : _name(name), _affinity(affinity) {
            LaunchThread();
        }

        ~WorkerThread() {
            std::lock_guard<std::mutex> threadLock(_thread_mutex);
            TerminateThread();
        }

        void Run(Work work) {
            std::lock_guard<std::mutex> workLock(_work_mutex);
            _work_queue.emplace(std::move(work));
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
            samples::setAffinity(_affinity);
            pthread_setname_np(pthread_self(), _name.c_str());

            ThreadState threadState;

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
                    head(&threadState);
                    _work_mutex.lock();
                }
            }
        }

        const std::string _name;
        const samples::Affinity _affinity;

        std::mutex _thread_mutex;
        std::thread _thread GUARDED_BY(_thread_mutex);

        std::mutex _work_mutex;
        bool _isActive GUARDED_BY(_work_mutex) = true;
        std::queue<std::function<void(ThreadState*)>> _work_queue GUARDED_BY(_work_mutex);
        std::condition_variable_any _work_condition;
    };
}
