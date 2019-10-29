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

#include <algorithm>
#include <chrono>
#include <functional>

#include "Time.hpp"


namespace ancer {
    class FpsCalculator {
    public:

        FpsCalculator() {
            Reset();
        }

        void Reset() {
            _last_timestamp = SteadyClock::now();
            _fps_sum = 0;
            _fps_count = 0;
            _average_fps = 0;
            _warm_up = true;
            _min_frame_time = Duration::max();
            _max_frame_time = Duration::zero();
        }

        /*
         * Execute an operation while stopping time recording; this allows
         * an act (such as loading a resource, rebuilding VBOs, etc) to
         * not affect fps stats.
         */
        template <typename Func>
        void Ignore(Func&& operation) {
            auto start = SteadyClock::now();
            operation();
            auto elapsed = SteadyClock::now() - start;
            _last_timestamp += elapsed;
        }

        /*
         * Update Fps tally, returning the time in seconds since the last call to UpdateFps.
         * To get the current fps average call GetAverageFps()
         */
        float UpdateFps() {
            // Arbitrary count of samples to take before updating
            // FPS stats. Set low to update more frequently, higher to
            // "smooth out" results.
            static constexpr int FPS_SAMPLES = 60;

            auto now = SteadyClock::now();
            auto delta = now - _last_timestamp;


            _fps_sum += 1.0f / (delta.count() / 1e9f);
            ++_fps_count;

            if ( _fps_count == FPS_SAMPLES ) {
                _average_fps = _fps_sum / _fps_count;
                _fps_sum = 0;
                _fps_count = 0;
                _min_frame_time = std::min(_min_frame_time, delta);
                _max_frame_time = std::max(_max_frame_time, delta);
                _warm_up = false;
            } else if ( _warm_up ) {
                _average_fps = _fps_sum / _fps_count;
            }

            _last_timestamp = now;
            return delta.count() / 1e9F;
        }

        float GetAverageFps() const { return _average_fps; }

        Duration GetMinFrameTime() const { return _warm_up ? Duration::zero() : _min_frame_time; }

        Duration GetMaxFrameTime() const { return _warm_up ? Duration::zero() : _max_frame_time; }

    private:
        bool _warm_up = true;
        Timestamp _last_timestamp;
        float _fps_sum = 0;
        int _fps_count = 0;
        float _average_fps = 0;
        Duration _min_frame_time = Duration::max();
        Duration _max_frame_time = Duration::zero();
    };
} // namespace ancer
