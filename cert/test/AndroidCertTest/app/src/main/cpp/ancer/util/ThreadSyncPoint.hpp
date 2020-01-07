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


namespace ancer {
    /**
     * Helper to synchronizing the "real work" portion of multi-threaded operations.
     * A good example of this kind of work is the I/O and memory tests where
     * the different cores' simultaneous performance is part of the test.
     *
     * While we can't *guarantee* the device won't still stall a thread, we can
     * at least avoid having one thread charge ahead while others are still
     * being created or still doing pre-operation setup.
     *
     * TODO(tmillican@google.com): A mechanism like this should probably a core
     * part of BaseOperation so we have this kind of "setup/work/shutdown"
     * paradigm across all operations.
     *
     *
     * EXAMPLE
     * void SpawnWorkers() {
     *     // Example: If thread spawning is slow or we're spawning a lot of
     *     // threads, early threads may start their work before later threads
     *     // have even been created.
     *     const int num_threads = 100;
     *     ThreadSyncPoint sync_point{num_threads};
     *
     *     for (int i = 0 ; i < num_threads ; ++i) {
     *         _threads.push_back(std::thread([&sync_point] {
     *             // Example: There's a decent amount of setup work that can
     *             // lead to desync due to factors we don't care about in this
     *             // test. (e.g., we're loading example data from disk, but the
     *             // test is actually about processing that data with different
     *             // multi-threaded setups: The I/O side of things doesn't
     *             // matter for this test.)
     *             InitializeStuff();
     *
     *             // Sync up...
     *             sync_point.Sync();
     *             // ...then continue with everyone at the same known point.
     *             DoThingWeCareAbout();
     *         });
     *     }
     * }
     */
     class ThreadSyncPoint {
     public:
         ThreadSyncPoint(int num_threads);
         ThreadSyncPoint(const ThreadSyncPoint&) = delete;
         ThreadSyncPoint(ThreadSyncPoint&&) = delete;
         ~ThreadSyncPoint();

         // Resets the sync point for another run.
         void Reset(int num_threads);
         void Reset();
         void Sync();
     private:
         void ForceDone();

         int _num_threads;
         int _sync_count = 0;

         std::mutex _mutex;
         std::condition_variable _cond_var;
     };
}
