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

#include "ThreadSyncPoint.hpp"


#include "Log.hpp"

using namespace ancer;


//==============================================================================

ThreadSyncPoint::ThreadSyncPoint(int num_threads)
: _num_threads(num_threads) {
}


ThreadSyncPoint::~ThreadSyncPoint() {
    ForceDone();
}

//==============================================================================

void ThreadSyncPoint::Reset(int num_threads) {
    ForceDone();
    {
        std::lock_guard _guard{_mutex};
        _num_threads = num_threads;
        _waiting_threads.clear();
    }
}

void ThreadSyncPoint::Reset() {
    ForceDone();
    {
        std::lock_guard _guard{_mutex};
        _waiting_threads.clear();
    }
}

//==============================================================================

void ThreadSyncPoint::Sync(std::thread::id id) {
    {
        std::lock_guard _guard{_mutex};
        _waiting_threads.insert(id);

        if (_waiting_threads.size() == _num_threads) {
            _waiting_threads.clear();
        }
    }
    std::unique_lock lock{_mutex};
    _cond_var.wait(lock, [this, id] {
        // Only block a thread as long as its thread_id is in the list.
        return _waiting_threads.find(id) == _waiting_threads.end();
    });
    _cond_var.notify_all();
}

//==============================================================================

void ThreadSyncPoint::ForceDone() {
    std::lock_guard _guard{_mutex};
    _waiting_threads.clear();
    _cond_var.notify_all();
}