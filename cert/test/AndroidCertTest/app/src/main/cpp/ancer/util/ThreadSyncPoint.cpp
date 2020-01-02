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
    // Just in case we've somehow aborted mid-setup and might have threads
    // waiting on us.
    if (_sync_count < _num_threads)
    {
        {
            std::lock_guard _guard{_mutex};
            ++_sync_count;
        }
        _cond_var.notify_all();
    }
}

//==============================================================================

void ThreadSyncPoint::Sync() {
    {
        std::lock_guard _guard{_mutex};
        ++_sync_count;
    }
    std::unique_lock lock{_mutex};
    _cond_var.wait(lock, [this] { return _num_threads <= _sync_count; });
    _cond_var.notify_all();
}