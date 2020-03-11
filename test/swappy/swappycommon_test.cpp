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

#include "gtest/gtest.h"

#include <queue>
#include <functional>
#include <thread>
#include <ostream>
#include <vector>

#include "swappy/common/SwappyCommon.h"

#define LOG_TAG "SCTest"
#include "Log.h"

using namespace swappy;

using time_point = std::chrono::steady_clock::time_point;
using duration = std::chrono::steady_clock::duration;

// Spoof these - we shouldn't be doing any Java loading in the tests.
extern "C" {
    char _binary_classes_dex_start;
    char _binary_classes_dex_end;
}

namespace swappycommon_test {

class SwappyCommonTest: public SwappyCommon {
  public:
    SwappyCommonTest(const SwappyCommonSettings& settings) : SwappyCommon(settings) {}
};

struct Workload {
  std::function<duration()> cpuWorkTime;
  std::function<duration()> gpuWorkTime;
};

struct Parameters {
   bool autoModeEnabled;
   bool pipelineModeEnabled;
   duration fenceTimeout;
   duration maxAutoSwapInterval;
   duration swapInterval;
};
struct ParametersChangeEvent {
    Parameters parameters;
    time_point time;
};
typedef std::vector<ParametersChangeEvent> ParametersSchedule;

class GpuThread {
 public:
    enum Fence {
        NO_SYNC,
        SIGNALLED,
        UNSIGNALLED
    };
 private:
    Fence fence_;
    duration fenceTimeout_;
    std::mutex fenceMutex_;
    std::mutex threadMutex_;
    std::thread thread_;
    std::condition_variable fenceCondition_;
    std::condition_variable threadCondition_;
    duration waitTime_;
    bool running_;
 public:
    GpuThread(duration fenceTimeout) : fence_(NO_SYNC), fenceTimeout_(fenceTimeout), waitTime_(0),
                                       running_(true) {
        thread_ = std::thread(&GpuThread::run, this);
    }
    ~GpuThread() {
        {
            std::lock_guard<std::mutex> lock(threadMutex_);
            fence_ = UNSIGNALLED;
            waitTime_ = duration(0);
            running_ = false;
        }
        threadCondition_.notify_all();
        thread_.join();
    }
    Fence fenceStatus() const { return fence_;}
    void waitForFence() {
        std::unique_lock<std::mutex> lock(fenceMutex_);
        if (!fenceCondition_.wait_for(lock, fenceTimeout_, [this](){ return fence_!=UNSIGNALLED;})) {
            ALOGW("Fence Timeout");
        }
    }
    void createFence(duration gpuFrameTime) {
        std::lock_guard<std::mutex> lock(threadMutex_);
        fence_ = UNSIGNALLED;
        waitTime_ = gpuFrameTime;
        threadCondition_.notify_all();
    }
    void run() {
        while(running_) {
            {
                std::unique_lock<std::mutex> lock(threadMutex_);
                threadCondition_.wait(lock,
                    [this]() { return fence_ == UNSIGNALLED; });
            }
            std::this_thread::sleep_for(waitTime_);
            {
                std::lock_guard<std::mutex> lock(fenceMutex_);
                fence_ = SIGNALLED;
            }
            fenceCondition_.notify_all();
        }
    }
    duration gpuWorkTime() {
        return waitTime_;
    }
    void setFenceTimeout(duration t) {
        fenceTimeout_ = t;
    }
};

class Simulator {
  public:
    struct Result {
        duration averageCpuTime;
        duration averageGpuTime;
        uint64_t numFrames;
    };
    Simulator(const SwappyCommonSettings& settings, const Workload& workload,
              const ParametersSchedule& schedule) :
            commonBase_(new SwappyCommonTest(settings)),
            workload_(workload),
            schedule_(schedule),
            tracers_ {
                preWaitTracer,
                postWaitTracer,
                preSwapBuffersTracer,
                postSwapBuffersTracer,
                startFrameTracer,
                (void*)this,
                swapIntervalChangedTracer
            },
            gpuThread_(commonBase_->getFenceTimeout()),
            cpuTimeSum_(0),
            gpuTimeSum_(0),
            cpuTimeCount_(0)
    {
        commonBase_->addTracerCallbacks(tracers_);
    }
    void setParameters(const Parameters& p) {
        commonBase_->setAutoSwapInterval(p.autoModeEnabled);
        commonBase_->setAutoPipelineMode(p.pipelineModeEnabled);
        if (p.fenceTimeout.count()!=0) {
            // This needs to be propagated to the gpuThread
            commonBase_->setFenceTimeout(p.fenceTimeout);
            gpuThread_.setFenceTimeout(p.fenceTimeout);
        }
        if (p.maxAutoSwapInterval.count()!=0)
            commonBase_->setMaxAutoSwapIntervalNS(p.maxAutoSwapInterval);
        Settings::getInstance()->setSwapIntervalNS(p.swapInterval.count());
    }
    void run_for(duration run_time) {
        int scheduleIndex = 0;
        bool running = true;
        std::thread choreographer([this,&running](){
            while(running) {
                auto t = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()).count();
                commonBase_->onChoreographer(t);
                std::this_thread::sleep_for(16666666ns);
            }
        });
        auto end_time = std::chrono::steady_clock::now() + run_time;

        // Main game loop
        while(true) {
            // Check if we need to update parameters
            auto now = std::chrono::steady_clock::now();
            if (now > end_time) break;
            if (scheduleIndex<schedule_.size() && now >= schedule_[scheduleIndex].time) {
                setParameters(schedule_[scheduleIndex].parameters);
                ++scheduleIndex;
            }
            // Simulate CPU work
            std::this_thread::sleep_for(workload_.cpuWorkTime());
            // Swap
            swapInternal();
        }
        ALOGI("Done: %s", resultsString().c_str());
        running = false; // This avoids the choreographer ticking during the SwappyCommon
                         // destructor but we need fixes to make sure the choreographer filter
                         // shuts down correctly then.
        destroy();
        choreographer.join();
    }
    bool swapInternal() {
        const SwappyCommon::SwapHandlers handlers = {
            .lastFrameIsComplete = [&]() { return lastFrameIsComplete(); },
            .getPrevFrameGpuTime = [&]() { return getFencePendingTime(); },
        };

        commonBase_->onPreSwap(handlers);
        if (commonBase_->needToSetPresentationTime()) {
            bool setPresentationTimeResult = setPresentationTime();
            if (!setPresentationTimeResult) {
                return setPresentationTimeResult;
            }
        }
        resetSyncFence();

        // Actual call to swapBuffers would be here

        commonBase_->onPostSwap(handlers);

        return true; // swapBuffers result
    }

    void destroy() {
        commonBase_.reset();
    }

    // GPU interaction logic

    bool lastFrameIsComplete() {
      // In OpenGL, this returns
      //   eglGetSyncAttribKHR(display, mSyncFence, EGL_SYNC_STATUS_KHR, &status);
      switch (gpuThread_.fenceStatus()) {
          case GpuThread::NO_SYNC: return true; // First time
          case GpuThread::SIGNALLED: return true;
          case GpuThread::UNSIGNALLED: return false;
      }
      return true;
    }
    void resetSyncFence() {
        gpuThread_.waitForFence();
        gpuThread_.createFence(workload_.gpuWorkTime());
    }
    std::chrono::nanoseconds getFencePendingTime() {
        // How long the fence was waiting
        return gpuThread_.gpuWorkTime();
    }
    bool setPresentationTime() {
      // In openGL, this calls
      //   eglPresentationTimeANDROID(display, surface, time.time_since_epoch().count());
      return true;
    }

    // Tracer functions
    void preWait() {}
    void postWait(duration cpuTime, duration gpuTime) {
        // Discard the first frame
        static bool first = true;
        if (first) first = false;
        else {
            cpuTimeSum_ += cpuTime;
            gpuTimeSum_ += gpuTime;
            ++cpuTimeCount_;
        }
    }
    void preSwapBuffers() {}
    void postSwapBuffers(duration desiredPresentationTime) {}
    void startFrame(int currentFrame, duration desiredPresentationTime) {}
    void swapIntervalChanged() {}

      // Static tracer callbacks
    static void preWaitTracer(void *userdata) {
          Simulator* test = (Simulator*)userdata;
        test->preWait();
    }
    static void postWaitTracer(void *userdata, long cpuTime_ns, long gpuTime_ns) {
        Simulator* test = (Simulator*)userdata;
        test->postWait(std::chrono::nanoseconds(cpuTime_ns), std::chrono::nanoseconds(gpuTime_ns));
    }
    static void preSwapBuffersTracer(void *userdata) {
        Simulator* test = (Simulator*)userdata;
        test->preSwapBuffers();
    }
    static void postSwapBuffersTracer(void *userdata, long desiredPresentationTimeMillis) {
        Simulator* test = (Simulator*)userdata;
        test->postSwapBuffers(std::chrono::milliseconds(desiredPresentationTimeMillis));
    }
    static void startFrameTracer(void *userdata, int currentFrame,
                                 long desiredPresentationTimeMillis) {
        Simulator* test = (Simulator*)userdata;
        test->startFrame(currentFrame, std::chrono::milliseconds(desiredPresentationTimeMillis));
    }
    static void swapIntervalChangedTracer(void *userdata) {
        Simulator* test = (Simulator*)userdata;
        test->swapIntervalChanged();
    }

    std::vector<Result> results() const {
        return { { cpuTimeSum_/cpuTimeCount_, gpuTimeSum_/cpuTimeCount_, cpuTimeCount_  } };
    }

    template<typename T>
    static bool check(T actual, T expected, T error) {
        return (actual <= expected + error && actual >= expected - error);
    }
    bool resultsCheck(const std::vector<Result>& expected, const std::vector<Result>& error) const {
        auto r = results();
        if (r.size()!=expected.size() || r.size()!=error.size()) return false;
        for(int i = 0; i < r.size(); ++i) {
            if (!check(r[i].averageCpuTime, expected[i].averageCpuTime, error[i].averageCpuTime))
                return false;
            if (!check(r[i].averageGpuTime, expected[i].averageGpuTime, error[i].averageGpuTime))
                return false;
            if (!check(r[i].numFrames, expected[i].numFrames, error[i].numFrames))
                return false;
        }
        return true;
    }
    static std::string DisplayDuration(duration d) {
        std::stringstream str;
        str << (double(d.count())/1000000) << "ms";
        return str.str();
    }
    friend std::ostream& operator<<(std::ostream& o, const std::vector<Result>& rs) {
        bool first = true;
        for(auto& r: rs) {
            if (first) first = false;
            else o << ", ";
            o << "{ avg cpu/ms: " << DisplayDuration(r.averageCpuTime) << ", avg gpu/ms: "
              << DisplayDuration(r.averageGpuTime) << ", num frames: " << r.numFrames << "}";
        }
        return o;
    }

    std::string resultsString() const {
        std::stringstream str;
        str << results();
        return str.str();
    }
 private:
    std::unique_ptr<SwappyCommonTest> commonBase_;
    Workload workload_;
    ParametersSchedule schedule_;
    SwappyTracer tracers_;
    GpuThread gpuThread_;
    duration cpuTimeSum_;
    duration gpuTimeSum_;
    uint64_t cpuTimeCount_;
};

}

using namespace swappycommon_test;
using Result = Simulator::Result;

TEST(SwappyCommonTest, AutoModeOff) {
    SwappyCommonSettings settings {
        0, // SDK version
        std::chrono::nanoseconds(16666667), // refresh period
        std::chrono::nanoseconds(0), // app vsync offset
        std::chrono::nanoseconds(0) // sf vsync offset
    };
    Parameters parameters {
        false, // auto-mode
        false, // pipeline mode
        std::chrono::nanoseconds(50ms), // fence timeout
        std::chrono::nanoseconds(50ms + 1us),// maxAutoSwapInterval
        16666667ns // swap interval
    };
    auto single_test = [&](duration time, const Workload& workload,
                           const std::vector<Result>& expected,
                           const std::vector<Result>& error) {
        auto now = std::chrono::steady_clock::now();
        Simulator test(settings,
                       workload,
                       ParametersSchedule{
                           ParametersChangeEvent{parameters, now}});
        test.run_for(time);
        EXPECT_TRUE(test.resultsCheck(expected, error))
                        << "Bad test result" << test.resultsString();
    };
    Workload light_workload {
        [&](){ return 10000000ns;}, // cpu
        [&](){ return 10000000ns;}, // gpu
    };
    single_test( 1s, light_workload, {Result{10ms, 10ms, 60}}, {Result{1ms, 1ms, 1}});

    Workload medium_cpu_workload {
        [&](){ return 33000000ns;}, // cpu
        [&](){ return 10000000ns;}, // gpu
    };
    single_test(1s, medium_cpu_workload, {Result{33ms, 10ms, 30}}, {Result{1ms, 1ms, 1}});

    Workload medium_gpu_workload {
        [&](){ return 10000000ns;}, // cpu
        [&](){ return 33000000ns;}, // gpu
    };
    single_test(1s, medium_gpu_workload, {Result{10ms, 33ms, 24}}, {Result{1ms, 3ms, 2}});
}
