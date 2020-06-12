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

#include <deque>
#include <memory>

#include "core/runnable.h"
#include "core/time_provider.h"

namespace tuningfork {

class Session;

class WorkOp {
   public:
    WorkOp(Duration min_work_duration_in)
        : min_work_duration(min_work_duration_in) {}
    virtual void DoWork(Session* session) = 0;
    Duration min_work_duration;
    TimePoint next_time;
    Duration work_duration;
};

// Scheduler of metric recordings.
class AsyncTelemetry : public Runnable {
    double time_occupancy_fraction_;
    // This is maintained as a min-heap using the above comparator.
    std::deque<std::shared_ptr<WorkOp>> metrics_;
    Session* session_;

   public:
    AsyncTelemetry(ITimeProvider* time_provider,
                   double time_occupancy_fraction);
    void AddMetric(const std::shared_ptr<WorkOp>& m);
    virtual Duration DoWork() override;
    void SetSession(Session* session) { session_ = session; }

    static const Duration kNoWorkPollPeriod;

   private:
    double TotalSquaredNanosecondDuration() const;
};

}  // namespace tuningfork
