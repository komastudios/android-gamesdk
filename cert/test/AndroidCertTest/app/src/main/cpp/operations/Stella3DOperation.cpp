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

#include <cmath>
#include <thread>
#include <mutex>
#include <sstream>

#include <ancer/util/ThreadPool.hpp>
#include <ancer/BaseOperation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>
#include <unistd.h>
#include <ancer/util/JobQueue.hpp>

using namespace ancer;
using ancer::ThreadPool;


//==================================================================================================

namespace {
    constexpr Log::Tag TAG{"RandomWork"};
}

//==================================================================================================

namespace {
    struct configuration {
        int vector_count_;
        bool use_job_system{};
        int range;
    };

    JSON_READER(configuration) {
        JSON_REQVAR(use_job_system);
        JSON_REQVAR(vector_count_);
        JSON_REQVAR(range);
    }
}

//==================================================================================================

struct Vec3
{
    Vec3()
    {
        x = (float(rand() % 200) / 100.0f) - 1;
        y = (float(rand() % 200) / 100.0f) - 1;
        z = (float(rand() % 200) / 100.0f) - 1;

    }
    Vec3& operator+=(const Vec3& rhs){
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    float x;
    float y;
    float z;
};

class StellaWork : public Work
{
 public:
    Vec3* positions;
    Vec3* velocities;
    Vec3* accelerations;
    int startIndex;
    int range;
    void DoWork()
    {
        for (int i = 0; i < range; ++i) {
            velocities[i + startIndex] += accelerations[i + startIndex];
            positions[i+ startIndex] += velocities[i + startIndex];
        }
    }
};

class Stella3DOperation : public BaseOperation {
    Vec3* positions;
    Vec3* velocities;
    Vec3* accelerations;

    JobQueue job_queue_;

    configuration configuration_;


 public:
    Stella3DOperation() {

    }

    ~Stella3DOperation() {

    }

    void Start() override {
        BaseOperation::Start();

        configuration_ = GetConfiguration<configuration>();
        positions = new Vec3[configuration_.vector_count_];
        velocities = new Vec3[configuration_.vector_count_];
        accelerations = new Vec3[configuration_.vector_count_];
    }

    void Wait() override {
        if (!IsStopped()) {
            std::unique_lock<std::mutex> lock(_stop_mutex);
            _stop_signal.wait(lock);
        }
    }

    void Stop() override {
        BaseOperation::Stop();
        std::lock_guard<std::mutex> guard(_stop_mutex);
        _stop_signal.notify_one();
    }



    void Draw(double delta_seconds) override {
        BaseOperation::Draw(delta_seconds);

        if (configuration_.use_job_system) {
            for (int j = 0; j < 1; ++j) {
                for (int i = 0; i < configuration_.vector_count_ / configuration_.range; i += configuration_.range) {
                    StellaWork *work = new StellaWork();
                    work->startIndex = i;
                    work->range = configuration_.range;
                    work->positions = positions;
                    work->accelerations = accelerations;
                    work->velocities = velocities;
                    job_queue_.AddJob(work);
                }
            }


            job_queue_.RunAllReadiedJobs();
        }
        else {
            for (int j = 0; j < 1; ++j) {
                for (int i = 0; i < configuration_.vector_count_; ++i) {
                    velocities[i] += accelerations[i];
                    positions[i] += velocities[i];
                }
            }
        }
    }

private:
    std::mutex _stop_mutex;
    std::condition_variable _stop_signal;



};

EXPORT_ANCER_OPERATION(Stella3DOperation)