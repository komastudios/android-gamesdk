/*
 * Copyright 2018 The Android Open Source Project
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

#include "SwappyCommon.h"

#include <cmath>
#include <thread>
#include <cstdlib>

#include "Settings.h"
#include "Thread.h"
#include "Log.h"
#include "Trace.h"

#define LOG_TAG "SwappyCommon"

namespace swappy {

using std::chrono::milliseconds;
using std::chrono::nanoseconds;

// NB These are only needed for C++14
constexpr nanoseconds SwappyCommon::FrameDuration::MAX_DURATION;
constexpr nanoseconds SwappyCommon::FRAME_HYSTERESIS;
constexpr nanoseconds SwappyCommon::REFRESH_RATE_MARGIN;

SwappyCommon::SwappyCommon(JNIEnv *env, jobject jactivity)
        : mSwapDuration(nanoseconds(0)),
          mAutoSwapInterval(1),
          mValid(false) {
    jclass activityClass = env->FindClass("android/app/NativeActivity");
    jclass windowManagerClass = env->FindClass("android/view/WindowManager");
    jclass displayClass = env->FindClass("android/view/Display");

    jmethodID getWindowManager = env->GetMethodID(
            activityClass,
            "getWindowManager",
            "()Landroid/view/WindowManager;");

    jmethodID getDefaultDisplay = env->GetMethodID(
            windowManagerClass,
            "getDefaultDisplay",
            "()Landroid/view/Display;");

    jobject wm = env->CallObjectMethod(jactivity, getWindowManager);
    jobject display = env->CallObjectMethod(wm, getDefaultDisplay);

    jmethodID getRefreshRate = env->GetMethodID(
            displayClass,
            "getRefreshRate",
            "()F");

    const float refreshRateHz = env->CallFloatMethod(display, getRefreshRate);

    jmethodID getAppVsyncOffsetNanos = env->GetMethodID(
            displayClass,
            "getAppVsyncOffsetNanos", "()J");

    // getAppVsyncOffsetNanos was only added in API 21.
    // Return gracefully if this device doesn't support it.
    if (getAppVsyncOffsetNanos == 0 || env->ExceptionOccurred()) {
        ALOGE("Error while getting method: getAppVsyncOffsetNanos");
        env->ExceptionClear();
        return;
    }
    const long appVsyncOffsetNanos = env->CallLongMethod(display, getAppVsyncOffsetNanos);

    jmethodID getPresentationDeadlineNanos = env->GetMethodID(
        displayClass,
        "getPresentationDeadlineNanos",
        "()J");


    if (getPresentationDeadlineNanos == 0 || env->ExceptionOccurred()) {
        ALOGE("Error while getting method: getPresentationDeadlineNanos");
        return;
    }

    const long vsyncPresentationDeadlineNanos = env->CallLongMethod(
        display, getPresentationDeadlineNanos);

    const long ONE_MS_IN_NS = 1000 * 1000;
    const long ONE_S_IN_NS = ONE_MS_IN_NS * 1000;

    const long vsyncPeriodNanos = static_cast<long>(ONE_S_IN_NS / refreshRateHz);
    const long sfVsyncOffsetNanos =
        vsyncPeriodNanos - (vsyncPresentationDeadlineNanos - ONE_MS_IN_NS);

    JavaVM* vm;
    env->GetJavaVM(&vm);

    using std::chrono::nanoseconds;
    mRefreshPeriod = nanoseconds(vsyncPeriodNanos);
    nanoseconds appVsyncOffset = nanoseconds(appVsyncOffsetNanos);
    nanoseconds sfVsyncOffset  = nanoseconds(sfVsyncOffsetNanos);

    mChoreographerFilter = std::make_unique<ChoreographerFilter>(mRefreshPeriod,
                                                                 sfVsyncOffset - appVsyncOffset,
                                                                 [this]() { return wakeClient(); });

    mChoreographerThread = ChoreographerThread::createChoreographerThread(
                                   ChoreographerThread::Type::Swappy,
                                   vm,
                                   [this]{ mChoreographerFilter->onChoreographer(); });
    if (!mChoreographerThread->isInitialized()) {
        ALOGE("failed to initialize ChoreographerThread");
        return;
    }

    if (USE_DISPLAY_MANAGER) {
        mDisplayManager = std::make_unique<SwappyDisplayManager>(vm, jactivity);
        if (!mDisplayManager->isInitialized()) {
            ALOGE("failed to initialize DisplayManager");
            return;
        }
    }

    Settings::getInstance()->addListener([this]() { onSettingsChanged(); });
    Settings::getInstance()->setDisplayTimings({mRefreshPeriod, appVsyncOffset, sfVsyncOffset});

    std::lock_guard<std::mutex> lock(mFrameDurationsMutex);
    mAutoSwapIntervalThreshold = (1e9f / vsyncPeriodNanos) / 20; // 20FPS
    mFrameDurations.reserve(mFrameDurationSamples);

    ALOGI("Initialized Swappy with vsyncPeriod=%lld, appOffset=%lld, sfOffset=%lld",
          (long long)vsyncPeriodNanos,
          (long long)appVsyncOffset.count(),
          (long long)sfVsyncOffset.count()
    );

    mValid = true;
}

SwappyCommon::~SwappyCommon() {
    // destroy all threads first before the other members of this class
    mChoreographerFilter.reset();
    mChoreographerThread.reset();

    Settings::reset();
}

nanoseconds SwappyCommon::wakeClient() {
    std::lock_guard<std::mutex> lock(mWaitingMutex);
    ++mCurrentFrame;

    // We're attempting to align with SurfaceFlinger's vsync, but it's always better to be a little
    // late than a little early (since a little early could cause our frame to be picked up
    // prematurely), so we pad by an additional millisecond.
    mCurrentFrameTimestamp = std::chrono::steady_clock::now() + mSwapDuration.load() + 1ms;
    mWaitingCondition.notify_all();
    return mSwapDuration;
}

void SwappyCommon::onChoreographer(int64_t frameTimeNanos) {
    TRACE_CALL();

    if (!mUsingExternalChoreographer) {
        mUsingExternalChoreographer = true;
        mChoreographerThread =
                ChoreographerThread::createChoreographerThread(
                        ChoreographerThread::Type::App,
                        nullptr,
                        [this] { mChoreographerFilter->onChoreographer(); });
    }

    mChoreographerThread->postFrameCallbacks();
}

bool SwappyCommon::waitForNextFrame(const SwapHandlers& h) {
    int lateFrames = 0;
    bool presentationTimeIsNeeded;

    const nanoseconds cpuTime = std::chrono::steady_clock::now() - mStartFrameTime;

    preWaitCallbacks();

    // if we are running slower than the threshold there is no point to sleep, just let the
    // app run as fast as it can
    if (mAutoSwapInterval <= mAutoSwapIntervalThreshold) {
        waitUntilTargetFrame();

        // wait for the previous frame to be rendered
        while (!h.lastFrameIsComplete()) {
            lateFrames++;
            waitOneFrame();
        }

        mPresentationTime += lateFrames * mRefreshPeriod;
        presentationTimeIsNeeded = true;
    } else {
        presentationTimeIsNeeded = false;
    }

    const nanoseconds gpuTime = h.getPrevFrameGpuTime();
    addFrameDuration({cpuTime, gpuTime});
    postWaitCallbacks();

    return presentationTimeIsNeeded;
}

void SwappyCommon::updateDisplayTimings() {
    // grab a pointer to the latest supported refresh rates
    if (mDisplayManager) {
        mSupportedRefreshRates = mDisplayManager->getSupportedRefreshRates();
    }

    std::lock_guard<std::mutex> lock(mFrameDurationsMutex);
    if (!mTimingSettingsNeedUpdate) {
        return;
    }

    mTimingSettingsNeedUpdate = false;

    const int newSwapInterval = calculateSwapInterval(mNextTimingSettings.swapIntervalNS,
                                                      mNextTimingSettings.refreshPeriod);
    if (mRefreshPeriod != mNextTimingSettings.refreshPeriod ||
        mAutoSwapInterval != newSwapInterval ||
        mSwapIntervalNS != mNextTimingSettings.swapIntervalNS) {
        mRefreshPeriod = mNextTimingSettings.refreshPeriod;
        mAutoSwapInterval = newSwapInterval;
        mSwapIntervalNS = mNextTimingSettings.swapIntervalNS;

        // Auto mode takes care of setting the preferred refresh rate. If it is not enabled
        // we should just set it now.
        if (!mAutoSwapIntervalEnabled || mNextModeId == -1) {
            setPreferredRefreshRate(mNextTimingSettings.swapIntervalNS);
        }

        mFrameDurations.clear();
        mFrameDurationsSum = {};
    }
    TRACE_INT("mSwapIntervalNS", int(mSwapIntervalNS.count()));
    TRACE_INT("mAutoSwapInterval", mAutoSwapInterval);
    TRACE_INT("mRefreshPeriod", mRefreshPeriod.count());
}

void SwappyCommon::onPreSwap(const SwapHandlers& h) {
    if (!mUsingExternalChoreographer) {
        mChoreographerThread->postFrameCallbacks();
    }

    updateDisplayTimings();

    // for non pipeline mode where both cpu and gpu work is done at the same stage
    // wait for next frame will happen after swap
    if (mPipelineMode == FrameDuration::PipelineMode::On) {
        mPresentationTimeNeeded = waitForNextFrame(h);
    } else {
        mPresentationTimeNeeded = mAutoSwapInterval <= mAutoSwapIntervalThreshold;
    }

    mSwapTime = std::chrono::steady_clock::now();
    preSwapBuffersCallbacks();
}

void SwappyCommon::onPostSwap(const SwapHandlers& h) {
    postSwapBuffersCallbacks();

    if (updateSwapInterval()) {
        swapIntervalChangedCallbacks();
        TRACE_INT("mPipelineMode", static_cast<int>(mPipelineMode));
        TRACE_INT("mAutoSwapInterval", mAutoSwapInterval);
    }

    updateSwapDuration(std::chrono::steady_clock::now() - mSwapTime);

    if (mPipelineMode == FrameDuration::PipelineMode::Off) {
        waitForNextFrame(h);
    }

    startFrame();
}

void SwappyCommon::updateSwapDuration(nanoseconds duration) {
    // TODO: The exponential smoothing factor here is arbitrary
    mSwapDuration = (mSwapDuration.load() * 4 / 5) + duration / 5;

    // Clamp the swap duration to half the refresh period
    //
    // We do this since the swap duration can be a bit noisy during periods such as app startup,
    // which can cause some stuttering as the smoothing catches up with the actual duration. By
    // clamping, we reduce the maximum error which reduces the calibration time.
    if (mSwapDuration.load() > (mRefreshPeriod / 2)) mSwapDuration = mRefreshPeriod / 2;
}

uint64_t SwappyCommon::getSwapIntervalNS() {
    std::lock_guard<std::mutex> lock(mFrameDurationsMutex);
    return mAutoSwapInterval * mRefreshPeriod.count();
};

void SwappyCommon::addFrameDuration(FrameDuration duration) {
    ALOGV("cpuTime = %.2f", duration.getCpuTime().count() / 1e6f);
    ALOGV("gpuTime = %.2f", duration.getGpuTime().count() / 1e6f);

    std::lock_guard<std::mutex> lock(mFrameDurationsMutex);
    // keep a sliding window of mFrameDurationSamples
    if (mFrameDurations.size() == mFrameDurationSamples) {
        mFrameDurationsSum -= mFrameDurations.front();
        mFrameDurations.erase(mFrameDurations.begin());
    }

    mFrameDurations.push_back(duration);
    mFrameDurationsSum += duration;
}

bool SwappyCommon::pipelineModeNotNeeded(const nanoseconds& averageFrameTime,
                                         const nanoseconds& upperBound) {
    // We want to be extra cautious when we switch to pipeline mode so we add the hysteresis twice
    return (mPipelineModeAutoMode && averageFrameTime < upperBound - (2 * FRAME_HYSTERESIS));
}

void SwappyCommon::swapSlower(const FrameDuration& averageFrameTime,
                        const nanoseconds& upperBound,
                        const int32_t& newSwapInterval) {
    ALOGV("Rendering takes too much time for the given config");

    if (mPipelineMode == FrameDuration::PipelineMode::Off &&
            averageFrameTime.getTime(FrameDuration::PipelineMode::On) <= upperBound) {
        ALOGV("turning on pipelining");
        mPipelineMode = FrameDuration::PipelineMode::On;
    } else {
        mAutoSwapInterval = newSwapInterval;
        ALOGV("Changing Swap interval to %d", mAutoSwapInterval);

        // since we changed the swap interval, we may be able to turn off pipeline mode
        nanoseconds newUpperBound = mRefreshPeriod * mAutoSwapInterval;
        newUpperBound -= FRAME_HYSTERESIS;
        if (pipelineModeNotNeeded(averageFrameTime.getTime(FrameDuration::PipelineMode::Off),
                                  newUpperBound)) {
            ALOGV("Turning off pipelining");
            mPipelineMode = FrameDuration::PipelineMode::Off;
        } else {
            ALOGV("Turning on pipelining");
            mPipelineMode = FrameDuration::PipelineMode::On;
        }
    }
}

void SwappyCommon::swapFaster(const FrameDuration& averageFrameTime,
                        const nanoseconds& lowerBound,
                        const int32_t& newSwapInterval) {


    ALOGV("Rendering is much shorter for the given config");
    mAutoSwapInterval = newSwapInterval;
    ALOGV("Changing Swap interval to %d", mAutoSwapInterval);

    // since we changed the swap interval, we may need to turn on pipeline mode
    nanoseconds newUpperBound = mRefreshPeriod * mAutoSwapInterval;
    newUpperBound -= FRAME_HYSTERESIS;
    if (pipelineModeNotNeeded(averageFrameTime.getTime(FrameDuration::PipelineMode::Off),
                                                       newUpperBound)) {
        ALOGV("Turning off pipelining");
        mPipelineMode = FrameDuration::PipelineMode::Off;
    } else {
        ALOGV("Turning on pipelining");
        mPipelineMode = FrameDuration::PipelineMode::On;
    }
}

bool SwappyCommon::updateSwapInterval() {
    std::lock_guard<std::mutex> lock(mFrameDurationsMutex);
    if (!mAutoSwapIntervalEnabled)
        return false;

    if (mFrameDurations.size() < mFrameDurationSamples)
        return false;

    const auto averageFrameTime = mFrameDurationsSum / mFrameDurations.size();

    const auto pipelineFrameTime = averageFrameTime.getTime(FrameDuration::PipelineMode::On);
    const auto nonPipelineFrameTime = averageFrameTime.getTime(FrameDuration::PipelineMode::Off);
    const auto currentConfigFrameTime = mPipelineMode == FrameDuration::PipelineMode::On ?
                                        pipelineFrameTime :
                                        nonPipelineFrameTime;

    // define upper and lower bounds based on the swap duration
    nanoseconds upperBoundForThisRefresh = mRefreshPeriod * mAutoSwapInterval;
    nanoseconds lowerBoundForThisRefresh = mRefreshPeriod * (mAutoSwapInterval - 1);

    // the lower bound is the next swap duration across all available refresh rates
    nanoseconds lowerBoundForAllRefresh = 0ms;
    if (mSupportedRefreshRates) {
        for (auto i : *mSupportedRefreshRates) {
            const auto period = i.first;
            const int swapIntervalForPeriod =
                    calculateSwapInterval(upperBoundForThisRefresh, period);
            const nanoseconds lowerBoundForPeriod = period * (swapIntervalForPeriod - 1);
            lowerBoundForAllRefresh = std::max(lowerBoundForAllRefresh, lowerBoundForPeriod);
        }
    } else {
        lowerBoundForAllRefresh = lowerBoundForThisRefresh;
    }

    // to be on the conservative side, lower bounds by FRAME_HYSTERESIS
    upperBoundForThisRefresh -= FRAME_HYSTERESIS;
    lowerBoundForThisRefresh -= FRAME_HYSTERESIS;
    lowerBoundForAllRefresh -= FRAME_HYSTERESIS;

    // add the hysteresis to one of the bounds to avoid going back and forth when frames
    // are exactly at the edge.
    lowerBoundForThisRefresh -= FRAME_HYSTERESIS;
    lowerBoundForAllRefresh -= FRAME_HYSTERESIS;

    // calculate the new swap interval based on average frame time assume we are in pipeline mode
    // (prefer higher swap interval rather than turning off pipeline mode)
    const int newSwapInterval = calculateSwapInterval(pipelineFrameTime + FRAME_HYSTERESIS,
                                                      mRefreshPeriod);

    ALOGV("mPipelineMode = %d", static_cast<int>(mPipelineMode));
    ALOGV("Average cpu frame time = %.2f", (averageFrameTime.getCpuTime().count()) / 1e6f);
    ALOGV("Average gpu frame time = %.2f", (averageFrameTime.getGpuTime().count()) / 1e6f);
    ALOGV("upperBound = %.2f", upperBoundForThisRefresh.count() / 1e6f);
    ALOGV("lowerBound = %.2f", lowerBoundForThisRefresh.count() / 1e6f);
    ALOGV("lowerBoundForAllRefresh = %.2f", lowerBoundForAllRefresh.count() / 1e6f);

    bool configChanged = false;
    bool mayNeedRefreshChange = false;

    // Make sure the frame time fits in the current config to avoid missing frames
    if (currentConfigFrameTime > upperBoundForThisRefresh) {
        swapSlower(averageFrameTime, upperBoundForThisRefresh, newSwapInterval);
        configChanged = true;
    }

    // So we shouldn't miss any frames with this config but maybe we can go faster ?
    // we check the pipeline frame time here as we prefer lower swap interval than no pipelining
    else if (mSwapIntervalNS <= mRefreshPeriod * (mAutoSwapInterval - 1) &&
            pipelineFrameTime < lowerBoundForThisRefresh) {
        swapFaster(averageFrameTime, lowerBoundForThisRefresh, newSwapInterval);
        configChanged = true;
    }

    // If we reached to this condition it means that we fit into the boundaries.
    // However we might be in pipeline mode and we could turn it off if we still fit.
    else if (pipelineModeNotNeeded(nonPipelineFrameTime, upperBoundForThisRefresh)) {
        ALOGV("Rendering time fits the current swap interval without pipelining");
        mPipelineMode = FrameDuration::PipelineMode::Off;
        configChanged = true;
    }

    // Check if we there is a potential better refresh rate
    if (pipelineFrameTime < lowerBoundForAllRefresh) {
        mayNeedRefreshChange = true;
    }

    if (configChanged || mayNeedRefreshChange) {
        mFrameDurationsSum = {};
        mFrameDurations.clear();
    }

    if (mayNeedRefreshChange) {
        setPreferredRefreshRate(pipelineFrameTime);
    }

    return configChanged;
}

template<typename Tracers, typename Func> void addToTracers(Tracers& tracers, Func func, void *userData) {
    if (func != nullptr) {
        tracers.push_back([func, userData](auto... params) {
            func(userData, params...);
        });
    }
}

void SwappyCommon::addTracerCallbacks(SwappyTracer tracer) {
    addToTracers(mInjectedTracers.preWait, tracer.preWait, tracer.userData);
    addToTracers(mInjectedTracers.postWait, tracer.postWait, tracer.userData);
    addToTracers(mInjectedTracers.preSwapBuffers, tracer.preSwapBuffers, tracer.userData);
    addToTracers(mInjectedTracers.postSwapBuffers, tracer.postSwapBuffers, tracer.userData);
    addToTracers(mInjectedTracers.startFrame, tracer.startFrame, tracer.userData);
    addToTracers(mInjectedTracers.swapIntervalChanged, tracer.swapIntervalChanged, tracer.userData);
}

template<typename T, typename ...Args> void executeTracers(T& tracers, Args... args) {
    for (const auto& tracer : tracers) {
        tracer(std::forward<Args>(args)...);
    }
}

void SwappyCommon::preSwapBuffersCallbacks() {
    executeTracers(mInjectedTracers.preSwapBuffers);
}

void SwappyCommon::postSwapBuffersCallbacks() {
    executeTracers(mInjectedTracers.postSwapBuffers,
                   (long) mPresentationTime.time_since_epoch().count());
}

void SwappyCommon::preWaitCallbacks() {
    executeTracers(mInjectedTracers.preWait);
}

void SwappyCommon::postWaitCallbacks() {
    executeTracers(mInjectedTracers.postWait);
}

void SwappyCommon::startFrameCallbacks() {
    executeTracers(mInjectedTracers.startFrame,
                   mCurrentFrame,
                   (long) mCurrentFrameTimestamp.time_since_epoch().count());
}

void SwappyCommon::swapIntervalChangedCallbacks() {
    executeTracers(mInjectedTracers.swapIntervalChanged);
}

void SwappyCommon::setAutoSwapInterval(bool enabled) {
    std::lock_guard<std::mutex> lock(mFrameDurationsMutex);
    mAutoSwapIntervalEnabled = enabled;

    // non pipeline mode is not supported when auto mode is disabled
    if (!enabled) {
        mPipelineMode = FrameDuration::PipelineMode::On;
        TRACE_INT("mPipelineMode", static_cast<int>(mPipelineMode));
    }
}

void SwappyCommon::setAutoPipelineMode(bool enabled) {
    std::lock_guard<std::mutex> lock(mFrameDurationsMutex);
    mPipelineModeAutoMode = enabled;
    TRACE_INT("mPipelineModeAutoMode", mPipelineModeAutoMode);
    if (!enabled) {
        mPipelineMode = FrameDuration::PipelineMode::On;
        TRACE_INT("mPipelineMode", static_cast<int>(mPipelineMode));
    }
}

void SwappyCommon::setPreferredRefreshRate(int modeId) {
    if (!mDisplayManager || modeId < 0 || mNextModeId == modeId) {
        return;
    }

    mNextModeId = modeId;
    mDisplayManager->setPreferredRefreshRate(modeId);
}

int SwappyCommon::calculateSwapInterval(nanoseconds frameTime, nanoseconds refreshPeriod) {

    if (frameTime < refreshPeriod) {
        return 1;
    }

    auto div_result = div(frameTime.count(), refreshPeriod.count());
    auto framesPerRefresh = div_result.quot;
    auto framesPerRefreshRemainder = div_result.rem;

    return (framesPerRefresh + (framesPerRefreshRemainder > REFRESH_RATE_MARGIN.count() ? 1 : 0));
}

void SwappyCommon::setPreferredRefreshRate(nanoseconds swapIntervalNS) {
    if (!mDisplayManager) {
        return;
    }

    int bestModeId = -1;
    nanoseconds swapIntervalNSMin = 100ms;
    for (auto i = mSupportedRefreshRates->crbegin(); i != mSupportedRefreshRates->crend(); ++i) {
        const auto period = i->first;
        const int modeId = i->second;

        nanoseconds frameTime = swapIntervalNS;
        // Make sure we don't cross the swap interval set by the app
        if (frameTime < mSwapIntervalNS) {
            frameTime = mSwapIntervalNS;
        }

        int swapIntervalForPeriod = calculateSwapInterval(frameTime, period);
        const auto swapIntervalNS = (period * swapIntervalForPeriod);
        if (swapIntervalNS < swapIntervalNSMin) {
            swapIntervalNSMin = swapIntervalNS;
            bestModeId = modeId;
        }
    }
    setPreferredRefreshRate(bestModeId);
}

void SwappyCommon::onSettingsChanged() {
    std::lock_guard<std::mutex> lock(mFrameDurationsMutex);

    TimingSettings timingSettings = TimingSettings::from(*Settings::getInstance());

    // If display timings has changed, cache the update and apply them on the next frame
    if (timingSettings != mNextTimingSettings) {
        mNextTimingSettings = timingSettings;
        mTimingSettingsNeedUpdate = true;
    }

}

void SwappyCommon::startFrame() {
    TRACE_CALL();

    int32_t currentFrame;
    std::chrono::steady_clock::time_point currentFrameTimestamp;
    {
        std::unique_lock<std::mutex> lock(mWaitingMutex);
        currentFrame = mCurrentFrame;
        currentFrameTimestamp = mCurrentFrameTimestamp;
    }

    startFrameCallbacks();

    mTargetFrame = currentFrame + mAutoSwapInterval;

    const int intervals = (mPipelineMode == FrameDuration::PipelineMode::On) ? 2 : 1;

    // We compute the target time as now
    //   + the time the buffer will be on the GPU and in the queue to the compositor (1 swap period)
    mPresentationTime = currentFrameTimestamp + (mAutoSwapInterval * intervals) * mRefreshPeriod;

    mStartFrameTime = std::chrono::steady_clock::now();
}

void SwappyCommon::waitUntilTargetFrame() {
    TRACE_CALL();
    std::unique_lock<std::mutex> lock(mWaitingMutex);
    mWaitingCondition.wait(lock, [&]() { return mCurrentFrame >= mTargetFrame; });
}

void SwappyCommon::waitOneFrame() {
    TRACE_CALL();
    std::unique_lock<std::mutex> lock(mWaitingMutex);
    const int32_t target = mCurrentFrame + 1;
    mWaitingCondition.wait(lock, [&]() { return mCurrentFrame >= target; });
}

} // namespace swappy
