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

#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_set>

namespace ancer {
    /**
     * Helps synchronize an arbitrary set of threads so they can resume
     * execution at the same time.
     * 
     * Any thread may participate in synchronization by calling this class's
     * `Sync` method. However, if the total number of threads that call `Sync`
     * does not *exactly* match `_num_threads`, the waiting threads will be
     * blocked indefinitely.
     *
     * `Sync` can be called multiple times by a thread, but care must be taken
     * to ensure that all threads call `Sync` the same number of times, or else
     * some threads may become blocked indefinitely. (Note: if sync is used in
     * this way, it is *not* necessary to call `Reset`; one can simply call
     * `Sync` as many times as needed, as long as _num_threads is invariant.)
     *
     * A typical use case might involve each thread doing its own initialization
     * routine, then calling `Sync` to wait until all threads have initialized,
     * allowing them to start their "real work" (post-initialization) in unison.
     * This can be useful when measuring and comparing performance across cores,
     * such as during I/O and memory tests.
     *
     * (While we can't *guarantee* the device won't subsequently stall a thread,
     * we can at least avoid having one thread charging ahead while others are
     * still being created or still doing pre-operation setup.)
     *
     * EXAMPLE 1:
     * void SpawnWorkers() {
     *     const int num_threads = 100;
     *     ThreadSyncPoint sync_point{num_threads};
     *
     *     for (int i = 0 ; i < num_threads ; ++i) {
     *         _threads.push_back(std::thread([&sync_point] {
     *             InitializeStuff();
     *
     *             // Wait for other threads to finish spawning/initializing.
     *             sync_point.Sync(std::this_thread::get_id());
     *
     *             // Everyone continues at the same known point.
     *             DoThingWeCareAbout();
     *         });
     *     }
     * }
     *
     * EXAMPLE 2:
     * void SpawnWorkers() {
     *     const int num_threads = 8;
     *     ThreadSyncPoint sync_point{num_threads};
     *
     *     for (int i = 0 ; i < num_threads ; ++i) {
     *         _threads.push_back(std::thread([&sync_point] {
     *             // Wait for all threads to spawn
     *             sync_point.Sync(std::this_thread::get_id());
     *             FirstWorkBatch();
     *
     *             // Sync between batches
     *             sync_point.Sync(std::this_thread::get_id());
     *             SecondWorkBatch();
     *
     *             // Sync again
     *             sync_point.Sync(std::this_thread::get_id());
     *             ThirdWorkBatch();
     *         });
     *     }
     * }
     *
     * TODO(tmillican@google.com): A mechanism like this should probably a core
     * part of BaseOperation so we have this kind of "setup/work/shutdown"
     * paradigm across all operations.
     */
     class ThreadSyncPoint {
     public:
         ThreadSyncPoint(int num_threads);
         ThreadSyncPoint(const ThreadSyncPoint&) = delete;
         ThreadSyncPoint(ThreadSyncPoint&&) = delete;
         ~ThreadSyncPoint();

         // Resets the sync point for reuse.
         void Reset(int num_threads);
         void Reset();
         void Sync(std::thread::id id);
     private:
         void ForceDone();

         int _num_threads;
         std::unordered_set<std::thread::id> _waiting_threads;

         std::mutex _mutex;
         std::condition_variable _cond_var;
     };
}
