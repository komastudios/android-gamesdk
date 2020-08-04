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

/**
 * This operation queries the GLES and EGL libraries for the list of extensions
 * available on the device. The initial use case was to query a set of devices
 * in order to locate one that supported a specific extension.
 *
 * Input configuration:
 * - None
 *
 * Output report:
 * - gl_extensions:  the list of supported GLES extensions
 * - egl_extensions: the list of supported EGl extensions
 *
 * (Note: No manipulation is done to the strings in the report lists of
 * extensions; therefore, they should match those found in online documentation
 * and specifications.)
 */

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/System.Cpu.hpp>

using namespace ancer;

//==============================================================================

namespace {
constexpr Log::Tag TAG{"BigLittleCoreTest"};
}  // anonymous namespace

namespace {

struct datum {
    int coreNumber;
    std::chrono::duration<double> cpuTime;
    std::chrono::duration<double> l1Time;
    std::chrono::duration<double> l2Time;
    std::chrono::duration<double> missTime;
};

void WriteDatum(report_writers::Struct w, const datum &d) {
    ADD_DATUM_MEMBER(w, d, coreNumber);
    ADD_DATUM_MEMBER(w, d, cpuTime);
    ADD_DATUM_MEMBER(w, d, l1Time);
    ADD_DATUM_MEMBER(w, d, l2Time);
    ADD_DATUM_MEMBER(w, d, missTime);
}
}




//==============================================================================

class BigLittleCoreTest : public BaseGLES3Operation {
 public:
    BigLittleCoreTest() = default;

    ~BigLittleCoreTest() {}

    void Start() override
    {
        BaseGLES3Operation::Start();
        Log();
    }

    void Draw(double delta_seconds) override {
        BaseGLES3Operation::Draw(delta_seconds);
        if (readyToAdvance)
        {
            Advance();
        }
    }

    void OnHeartbeat(Duration elapsed) override {}

 private:
    void Log() {

    }

    void Advance()
    {

        readyToAdvance = false;
        if (currentCore == std::thread::hardware_concurrency())
        {
            Stop();
        }
        else
        {
            std::thread test = std::thread([this]() {
                SetThreadAffinity(currentCore);
                WorkThread(readyToAdvance, currentCore);
            });
            test.join();
            readyToAdvance = true;
            ++currentCore;
        }
    }

    void WorkThread(bool& ready, int threadNumber)
    {
        char memory[1024*500];
        int cpuRegisters[] = {0,1};
        int l1Cache[] = { 0, 64, 64 * 2, 64 * 3, 64 * 4, 64 * 5, 64 * 6 };
        int l2Cache[] = { 0, 1024, 1024 * 2 };
        int cacheMiss[] = { 0, 1024 *100 , 1024 * 200, 1024 * 300, 1024*400 };
        datum datum;
        datum.coreNumber = threadNumber;
        auto curTime = SteadyClock::now();

        for(int i = 0; i < 10000; ++i)
        {
            memory[cpuRegisters[i % 2]] += memory[cpuRegisters[i % 2]];
        }

        datum.cpuTime = SteadyClock::now() - curTime;
        curTime = SteadyClock::now();

        for(int i = 0; i < 10000; ++i)
        {
            memory[l1Cache[i % 7]] += memory[l1Cache[i % 7]];
        }

        datum.l1Time = SteadyClock::now() - curTime;
        curTime = SteadyClock::now();

        for(int i = 0; i < 10000; ++i)
        {
            memory[l2Cache[i % 3]] += memory[l2Cache[i % 3]];
        }

        datum.l2Time = SteadyClock::now() - curTime;
        curTime = SteadyClock::now();

        for(int i = 0; i < 10000; ++i)
        {
            memory[cacheMiss[i % 5]] += memory[cacheMiss[i % 5]];
        }

        datum.missTime = SteadyClock::now() - curTime;

        Report(datum);
    }

 private:
    int currentCore = 0;
    bool readyToAdvance = true;
};

EXPORT_ANCER_OPERATION(BigLittleCoreTest)