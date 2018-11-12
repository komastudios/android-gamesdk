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

#include "SwappyVk.h"
#include <map>
#include <dlfcn.h>

#ifdef ANDROID
#include <mutex>
#include <pthread.h>
#include <android/looper.h>
#include <android/choreographer.h>
#include <android/log.h>
#include <android/trace.h>

#define ATRACE_NAME(name) ScopedTrace ___tracer(name)

// ATRACE_CALL is an ATRACE_NAME that uses the current function name.
#define ATRACE_CALL() ATRACE_NAME(__FUNCTION__)

class ScopedTrace {
public:
    inline ScopedTrace(const char *name) {
        ATrace_beginSection(name);
    }

    inline ~ScopedTrace() {
        ATrace_endSection();
    }
};

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, "SwappyVk", __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, "SwappyVk", __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, "SwappyVk", __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "SwappyVk", __VA_ARGS__)
#ifdef ANGLE_FEATURE_UTIL_LOG_VERBOSE
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "SwappyVk", __VA_ARGS__)
#else
#define ALOGV(...) ((void)0)
#endif
#else
#define ATRACE_CALL()
#define ALOGE(...)
#define ALOGW(...)
#define ALOGI(...)
#define ALOGD(...)
#define ALOGV(...)
#endif

#define CHOREOGRAPHER_THRESH 1000


constexpr uint32_t kThousand = 1000;
constexpr uint32_t kMillion  = 1000000;
constexpr uint32_t kBillion  = 1000000000;
constexpr uint32_t k16_6msec = 16666666;



// Note: The API functions is at the botton of the file.  Those functions call methods of the
// singleton SwappyVk class.  Those methods call virtual methods of the abstract SwappyVkBase
// class, which is actually implemented by one of the derived/concrete classes:
//
// - SwappyVkGoogleDisplayTiming
// - SwappyVkVulkanFallback
// - SwappyVkAndroidFallback

// Forward declarations:
class SwappyVk;


/***************************************************************************************************
 *
 * Per-Device abstract base class.
 *
 ***************************************************************************************************/

/**
 * Abstract base class that calls the Vulkan API.
 *
 * It is expected that one concrete class will be instantiated per VkDevice, and that all
 * VkSwapchainKHR's for a given VkDevice will share the same instance.
 */
class SwappyVkBase
{
public:
    SwappyVkBase(VkPhysicalDevice physicalDevice,
                 VkDevice         device,
                 uint64_t         refreshDur,
                 uint32_t         interval,
                 SwappyVk         &swappyVk,
                 void             *libVulkan) :
            mPhysicalDevice(physicalDevice), mDevice(device), mRefreshDur(refreshDur),
            mInterval(interval), mSwappyVk(swappyVk), mLibVulkan(libVulkan), mInitialized(false)
    {
        mpfnGetDeviceProcAddr =
                reinterpret_cast<PFN_vkGetDeviceProcAddr>(
                    dlsym(mLibVulkan, "vkGetDeviceProcAddr"));
        mpfnQueuePresentKHR =
                reinterpret_cast<PFN_vkQueuePresentKHR>(
                    mpfnGetDeviceProcAddr(mDevice, "vkQueuePresentKHR"));
    }
    virtual bool doGetRefreshCycleDuration(VkSwapchainKHR swapchain,
                                           uint64_t*      pRefreshDuration) = 0;
    void doSetSwapInterval(VkSwapchainKHR swapchain,
                           uint32_t       interval)
    {
        mInterval = interval;
    }
    virtual VkResult doQueuePresent(VkQueue                 queue,
                                    const VkPresentInfoKHR* pPresentInfo) = 0;
public:
    VkPhysicalDevice mPhysicalDevice;
    VkDevice         mDevice;
    uint64_t         mRefreshDur;
    uint32_t         mInterval;
    SwappyVk         &mSwappyVk;
    void             *mLibVulkan;
    bool             mInitialized;

    PFN_vkGetDeviceProcAddr mpfnGetDeviceProcAddr = nullptr;
    PFN_vkQueuePresentKHR   mpfnQueuePresentKHR = nullptr;
};



/***************************************************************************************************
 *
 * Per-Device concrete/derived class for using VK_GOOGLE_display_timing.
 *
 ***************************************************************************************************/

#ifdef NOT_NEEDED
class SwappyVkGoogleDisplayTimingSwapchain;
#endif // NOT_NEEDED

/**
 * Concrete/derived class that sits on top of VK_GOOGLE_display_timing
 */
class SwappyVkGoogleDisplayTiming : public SwappyVkBase
{
public:
    SwappyVkGoogleDisplayTiming(VkPhysicalDevice physicalDevice,
                                VkDevice         device,
                                SwappyVk         &swappyVk,
                                void             *libVulkan) :
            SwappyVkBase(physicalDevice, device, k16_6msec, 1, swappyVk, libVulkan),
            mNextPresentID(0), mNextDesiredPresentTime(0)
    {
        initialize();
    }
    virtual bool doGetRefreshCycleDuration(VkSwapchainKHR swapchain,
                                           uint64_t*      pRefreshDuration) override
    {
        // FIXME/TODO: NEED TO CACHE THIS SWAPCHAIN, SO THAT WE KNOW ALL SWAPCHAINS FOR THIS DEVICE

        VkRefreshCycleDurationGOOGLE refreshCycleDuration;
        VkResult res = mpfnGetRefreshCycleDurationGOOGLE(mDevice, swapchain, &refreshCycleDuration);
        if (res != VK_SUCCESS) {
            // This should never occur, but in case it does, return 16,666,666ns:
            mRefreshDur = k16_6msec;
        } else {
            mRefreshDur = refreshCycleDuration.refreshDuration;
        }

        // TEMP CODE: LOG REFRESH DURATION AND RATE:
        double refreshRate = mRefreshDur;
        refreshRate = 1.0 / (refreshRate / 1000000000.0);
        ALOGI("Returning refresh duration of %llu nsec (approx %f Hz)", mRefreshDur, refreshRate);

        *pRefreshDuration = mRefreshDur;
        return true;
    }
    virtual VkResult doQueuePresent(VkQueue                 queue,
                                    const VkPresentInfoKHR* pPresentInfo) override;

private:
    void initialize()
    {
        mpfnGetRefreshCycleDurationGOOGLE =
                reinterpret_cast<PFN_vkGetRefreshCycleDurationGOOGLE>(
                    mpfnGetDeviceProcAddr(mDevice, "vkGetRefreshCycleDurationGOOGLE"));
        mpfnGetPastPresentationTimingGOOGLE =
                reinterpret_cast<PFN_vkGetPastPresentationTimingGOOGLE>(
                    mpfnGetDeviceProcAddr(mDevice, "vkGetPastPresentationTimingGOOGLE"));
    }

private:
    uint32_t mNextPresentID;
    uint64_t mNextDesiredPresentTime;

    PFN_vkGetRefreshCycleDurationGOOGLE mpfnGetRefreshCycleDurationGOOGLE = nullptr;
    PFN_vkGetPastPresentationTimingGOOGLE mpfnGetPastPresentationTimingGOOGLE = nullptr;

#ifdef NOT_NEEDED
    std::map<VkSwapchainKHR, SwappyVkGoogleDisplayTimingSwapchain> swapchains;
#endif // NOT_NEEDED
};

VkResult SwappyVkGoogleDisplayTiming::doQueuePresent(VkQueue                 queue,
                                                     const VkPresentInfoKHR* pPresentInfo)
{
#if 1
    // FIXME/TODO: ALLOCATE (CHUNK ALLOC?) MEMORY

    // FIXME/TODO: Look at any previous presents and come up with values that pPresentTimes points to.
    // FIXME/TODO: Look at any previous presents and come up with values that pPresentTimes points to.
    // FIXME/TODO: Look at any previous presents and come up with values that pPresentTimes points to.


    // Ideas:
    //
    // - At first (before any feedback), get the current time and pick a
    //   desiredPresentTime that's in the future.
    //
    // - Keep track of the next presentID to use.
    //
    // - When we get feedback, we do calibration.  We want desiredPresentTime to
    //   be between 1 and 3 msec before when we think vsync will be.  There is
    //   variability in the timestamps that come back.  If we tried to have
    //   desiredPresentTime be when we think vsync will occur, we will sometimes
    //   be early and sometimes be late (i.e. not truly be at the desired
    //   framerate).  It's better to be a little early than late.  Since the
    //   highest display refresh-rate immaginable is 120Hz (or vsync every 4+
    //   msec), I'll arbitrarily consider an acceptable desiredPresentTime to be
    //   between 1 and 3 msec earlier than when I think vsync will be.
    //
    // - When we do calibration, if there's more than one frame's worth of
    //   feedback, look at the highest presentID that comes back.
    //
    // - Rather than do malloc/free for getting feedback (when we do
    //   calibration), let's just have an array on the stack (i.e. a local
    //   variable) that gets passed to vkGetPastPresentationTimingGOOGLE().
    //
    // - We shouldn't do calibration too often.  In particular, an application
    //   may be several frames ahead of getting feedback.  Therefore, once we do
    //   calibration, we should not do calibration again until after the next
    //   presentID to use.
    //
    // - Oh yeah, all swapchains in this call are on the same VkDevice.  In most
    //   cases, there will only be one swapchain.  In most cases, a swapchain
    //   won't be re-created.  Perhaps it's best to assume that it's best to use
    //   the first swapchain for doing calibration, and then apply the
    //   desiredPresentTime to all swapchains.  That way, I won't need the
    //   SwappyVkGoogleDisplayTimingSwapchain class.
    //
    // - There's an outside chance that an application won't be able to keep up
    //   with its swap interval (e.g. it's designed for 30FPS on a premium
    //   device and is running on a slow device, or it's running on a 120Hz
    //   display).
#else
#endif
    VkResult ret = VK_SUCCESS;

    // Determine the desiredPresentTime:
    if (!mNextDesiredPresentTime) {
        struct timespec currTime;
        clock_gettime(CLOCK_MONOTONIC, &currTime);
        uint64_t currentTime =
                ((uint64_t)currTime.tv_sec * kBillion) + (uint64_t)currTime.tv_nsec;
        mNextDesiredPresentTime = currentTime + mRefreshDur;
    } else {
        mNextDesiredPresentTime += mRefreshDur * mInterval;
    }

    // Setup the new structures to pass:
    VkPresentTimeGOOGLE *pPresentTimes =
            reinterpret_cast<VkPresentTimeGOOGLE*>(malloc(sizeof(VkPresentTimeGOOGLE) *
                                                          pPresentInfo->swapchainCount));
    for (uint32_t i = 0 ; i < pPresentInfo->swapchainCount ; i++) {
        pPresentTimes[i].presentID = mNextPresentID;
        pPresentTimes[i].desiredPresentTime = mNextDesiredPresentTime;
    }
    mNextPresentID++;

    VkPresentTimesInfoGOOGLE presentTimesInfo = {VK_STRUCTURE_TYPE_PRESENT_TIMES_INFO_GOOGLE,
                                                 pPresentInfo->pNext, pPresentInfo->swapchainCount,
                                                 pPresentTimes};
    VkPresentInfoKHR replacementPresentInfo = {pPresentInfo->sType, &presentTimesInfo,
                                               pPresentInfo->waitSemaphoreCount,
                                               pPresentInfo->pWaitSemaphores,
                                               pPresentInfo->swapchainCount,
                                               pPresentInfo->pSwapchains,
                                               pPresentInfo->pImageIndices, pPresentInfo->pResults};
    ret = mpfnQueuePresentKHR(queue, &replacementPresentInfo);
    free(pPresentTimes);
    return ret;
};


#ifdef NOT_NEEDED
/**
 * Per-VkSwapchainKHR helper class for using VK_GOOGLE_display_timing
 */
class SwappyVkGoogleDisplayTimingSwapchain
{
public:
    SwappyVkGoogleDisplayTimingSwapchain(SwappyVkGoogleDisplayTiming *pGoogle,
                                         VkSwapchainKHR swapchain) :
            mpGoogle(pGoogle), mSwapchain(swapchain), mNextPresentID(0), mNextDesiredPresentTime(0),
            mReceivedPastData(false) {}
public:
    SwappyVkGoogleDisplayTiming *mpGoogle;
    VkSwapchainKHR mSwapchain;
    uint32_t mNextPresentID;
    uint64_t mNextDesiredPresentTime;
    bool mReceivedPastData;
};
#endif // NOT_NEEDED


/***************************************************************************************************
 *
 * Per-Device concrete/derived class for the "fallback" path (no timing support; just generic Vulkan)
 *
 ***************************************************************************************************/

/**
 * Concrete/derived class that sits on top of the Vulkan API
 */
#ifdef ANDROID
class SwappyVkAndroidFallback : public SwappyVkBase
{
public:
    SwappyVkAndroidFallback(VkPhysicalDevice physicalDevice,
                            VkDevice         device,
                            SwappyVk         &swappyVk,
                            void             *libVulkan) :
            SwappyVkBase(physicalDevice, device, 0, 1, swappyVk, libVulkan) {

        std::unique_lock<std::mutex> lock(mWaitingMutex);
        // create a new ALooper thread to get Choreographer events
        mTreadRunning = true;
        pthread_create(&mThread, NULL, looperThreadWrapper, this);
        mWaitingCondition.wait(lock, [&]() { return mChoreographer != nullptr; });
    }

        ~SwappyVkAndroidFallback() {
        if (mLooper) {
            ALooper_acquire(mLooper);
            mTreadRunning = false;
            ALooper_wake(mLooper);
            ALooper_release(mLooper);
            pthread_join(mThread, NULL);
        }
    }

    virtual bool doGetRefreshCycleDuration(VkSwapchainKHR swapchain,
                                               uint64_t*      pRefreshDuration) override
    {
        std::unique_lock<std::mutex> lock(mWaitingMutex);
        mWaitingCondition.wait(lock, [&]() {
            if (mRefreshDur == 0) {
                AChoreographer_postFrameCallbackDelayed(mChoreographer, frameCallback, this, 1);
                return false;
            }
            return true;
        });

        *pRefreshDuration = mRefreshDur;
        ALOGI("Returning refresh duration of %llu nsec", mRefreshDur);
        return true;
    }

    virtual VkResult doQueuePresent(VkQueue                 queue,
                                    const VkPresentInfoKHR* pPresentInfo) override
    {
        {
            const long target = mFrameID + mInterval;
            std::unique_lock<std::mutex> lock(mWaitingMutex);


            mWaitingCondition.wait(lock, [&]() {
                if (mFrameID < target) {
                    // wait for the next frame as this frame is too soon
                    AChoreographer_postFrameCallbackDelayed(mChoreographer, frameCallback, this, 1);
                    return false;
                }
                return true;
            });
        }
        return mpfnQueuePresentKHR(queue, pPresentInfo);
    }

private:
    static void *looperThreadWrapper(void *data) {
        SwappyVkAndroidFallback *me = reinterpret_cast<SwappyVkAndroidFallback *>(data);
        return me->looperThread();
    }

    void *looperThread() {
        int outFd, outEvents;
        void *outData;

        mLooper = ALooper_prepare(0);
        if (!mLooper) {
            ALOGE("ALooper_prepare failed");
            return NULL;
        }

        mChoreographer = AChoreographer_getInstance();
        if (!mChoreographer) {
            ALOGE("AChoreographer_getInstance failed");
            return NULL;
        }
        mWaitingCondition.notify_all();

        while (mTreadRunning) {
            ALooper_pollAll(-1, &outFd, &outEvents, &outData);
        }

        return NULL;
    }

    static void frameCallback(long frameTimeNanos, void *data) {
        SwappyVkAndroidFallback *me = reinterpret_cast<SwappyVkAndroidFallback *>(data);
        me->onDisplayRefresh(frameTimeNanos);
    }

    void onDisplayRefresh(long frameTimeNanos) {
        std::lock_guard<std::mutex> lock(mWaitingMutex);

        // ignore back-to-back callbacks
        if (mLastframeTimeNanos != 0 && frameTimeNanos - mLastframeTimeNanos < CHOREOGRAPHER_THRESH)
            return;

        if (mRefreshDur == 0 && mLastframeTimeNanos != 0) {
            // prevent wraparound
            if (mLastframeTimeNanos < frameTimeNanos) {
                calcRefreshRate(frameTimeNanos);
            }
        }

        mLastframeTimeNanos = frameTimeNanos;
        mFrameID++;
        mWaitingCondition.notify_all();
    }

    void calcRefreshRate(long frameTimeNanos) {
        long refresh_nano = frameTimeNanos - mLastframeTimeNanos;
        const int step = 30;
        int fps = 0;
        long rate, delta;

        do {
            fps += step;
            rate = (double) 1.0 / fps * kBillion;
            delta = (rate > refresh_nano) ? rate - refresh_nano : refresh_nano - rate;
        } while (delta > 1000000);

        mRefreshDur = ((double)1.0 / fps) * kBillion;
    }

    pthread_t mThread = 0;
    ALooper *mLooper = nullptr;
    bool mTreadRunning = false;
    AChoreographer *mChoreographer = nullptr;
    std::mutex mWaitingMutex;
    std::condition_variable mWaitingCondition;
    long mFrameID = 0;
    long mLastframeTimeNanos = 0;
};
#endif

/***************************************************************************************************
 *
 * Per-Device concrete/derived class for the "Vulkan fallback" path (i.e. no API/OS timing support;
 * just generic Vulkan)
 *
 ***************************************************************************************************/

/**
 * Concrete/derived class that sits on top of the Vulkan API
 */
class SwappyVkVulkanFallback : public SwappyVkBase
{
public:
    SwappyVkVulkanFallback(VkPhysicalDevice physicalDevice,
                            VkDevice         device,
                            SwappyVk         &swappyVk,
                            void             *libVulkan) :
            SwappyVkBase(physicalDevice, device, k16_6msec, 1, swappyVk, libVulkan) {}
    virtual bool doGetRefreshCycleDuration(VkSwapchainKHR swapchain,
                                           uint64_t*      pRefreshDuration) override
    {
        *pRefreshDuration = mRefreshDur;
        return true;
    }
    virtual VkResult doQueuePresent(VkQueue                 queue,
                                    const VkPresentInfoKHR* pPresentInfo) override
    {
        return mpfnQueuePresentKHR(queue, pPresentInfo);
    }
};




/***************************************************************************************************
 *
 * Singleton class that provides the high-level implementation of the Swappy entrypoints.
 *
 ***************************************************************************************************/
/**
 * Singleton class that provides the high-level implementation of the Swappy entrypoints.
 *
 * This class determines which low-level implementation to use for each physical
 * device, and then calls that class's do-method for the entrypoint.
 */
class SwappyVk
{
public:
    static SwappyVk& getInstance()
    {
        static SwappyVk instance;
        return instance;
    }
    ~SwappyVk() {}

    void swappyVkDetermineDeviceExtensions(VkPhysicalDevice       physicalDevice,
                                           uint32_t               availableExtensionCount,
                                           VkExtensionProperties* pAvailableExtensions,
                                           uint32_t*              pRequiredExtensionCount,
                                           char**                 pRequiredExtensions);
    bool GetRefreshCycleDuration(VkPhysicalDevice physicalDevice,
                                 VkDevice         device,
                                 VkSwapchainKHR   swapchain,
                                 uint64_t*        pRefreshDuration);
    void SetSwapInterval(VkDevice       device,
                         VkSwapchainKHR swapchain,
                         uint32_t       interval);
    VkResult QueuePresent(VkQueue                 queue,
                          const VkPresentInfoKHR* pPresentInfo);

private:
    std::map<VkPhysicalDevice, bool> doesPhysicalDeviceHaveGoogleDisplayTiming;
    std::map<VkDevice, SwappyVkBase*> perDeviceImplementation;
    std::map<VkSwapchainKHR, SwappyVkBase*> perSwapchainImplementation;

    void *mLibVulkan     = nullptr;

private:
    SwappyVk() {} // Need to implement this constructor
//    SwappyVk(SwappyVk const&); // Don't implement a copy constructor--no copies
//    void operator=(SwappyVk const&); // Don't implement--no copies
public:
//    SwappyVk(SwappyVk const&)       = delete;
//    void operator=(SwappyVk const&) = delete;
};


/**
 * Generic/Singleton implementation of swappyVkDetermineDeviceExtensions.
 */
void SwappyVk::swappyVkDetermineDeviceExtensions(
    VkPhysicalDevice       physicalDevice,
    uint32_t               availableExtensionCount,
    VkExtensionProperties* pAvailableExtensions,
    uint32_t*              pRequiredExtensionCount,
    char**                 pRequiredExtensions)
{
    // TODO: Refactor this to be more concise:
    if (!pRequiredExtensions) {
        for (uint32_t i = 0; i < availableExtensionCount; i++) {
            if (!strcmp(VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME,
                        pAvailableExtensions[i].extensionName)) {
                (*pRequiredExtensionCount)++;
            }
        }
    } else {
        doesPhysicalDeviceHaveGoogleDisplayTiming[physicalDevice] = false;
        for (uint32_t i = 0, j = 0; i < availableExtensionCount; i++) {
            if (!strcmp(VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME,
                        pAvailableExtensions[i].extensionName)) {
                if (j < *pRequiredExtensionCount) {
                    strcpy(pRequiredExtensions[j++], VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME);
                    doesPhysicalDeviceHaveGoogleDisplayTiming[physicalDevice] = true;
                }
            }
        }
    }
}


/**
 * Generic/Singleton implementation of swappyVkGetRefreshCycleDuration.
 */
bool SwappyVk::GetRefreshCycleDuration(VkPhysicalDevice physicalDevice,
                                       VkDevice         device,
                                       VkSwapchainKHR   swapchain,
                                       uint64_t*        pRefreshDuration)
{
    SwappyVkBase *pImplementation = perDeviceImplementation[device];
    if (!pImplementation) {
        // We have not seen this device yet.
        if (!mLibVulkan) {
            // This is the first time we've been called--initialize function pointers:
            mLibVulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
            if (!mLibVulkan)
            {
                // If Vulkan doesn't exist, bail-out early:
                return false;
            }
        }

        // First, based on whether VK_GOOGLE_display_timing is available
        // (determined and cached by swappyVkDetermineDeviceExtensions),
        // determine which derived class to use to implement the rest of the API
        if (doesPhysicalDeviceHaveGoogleDisplayTiming[physicalDevice]) {
            // Instantiate the class that sits on top of VK_GOOGLE_display_timing
            pImplementation = new SwappyVkGoogleDisplayTiming(physicalDevice, device,
                                                              getInstance(), mLibVulkan);
            ALOGD("SwappyVk initialized for VkDevice %p using VK_GOOGLE_display_timing", device);
        } else {
            // Instantiate the class that sits on top of the basic Vulkan APIs
#ifdef ANDROID
            pImplementation = new SwappyVkAndroidFallback(physicalDevice, device, getInstance(),
                                                          mLibVulkan);
            ALOGD("SwappyVk initialized for VkDevice %p using Android fallback", device);
#else  // ANDROID
            pImplementation = new SwappyVkVulkanFallback(physicalDevice, device, getInstance(),
                                                         mLibVulkan);
            ALOGD("SwappyVk initialized for VkDevice %p using Vulkan-only fallback", device);
#endif // ANDROID
        }

        // Second, cache the per-device pointer to the derived class:
        if (pImplementation) {
            perDeviceImplementation[device] = pImplementation;
        } else {
            // This shouldn't happen, but if it does, something is really wrong.
            return false;
        }
    }

    // Cache the per-swapchain pointer to the derived class:
    perSwapchainImplementation[swapchain] = pImplementation;

    // Now, call that derived class to get the refresh duration to return
    return pImplementation->doGetRefreshCycleDuration(swapchain, pRefreshDuration);
}


/**
 * Generic/Singleton implementation of swappyVkSetSwapInterval.
 */
void SwappyVk::SetSwapInterval(VkDevice       device,
                               VkSwapchainKHR swapchain,
                               uint32_t       interval)
{
    SwappyVkBase *pImplementation = perDeviceImplementation[device];
    if (!pImplementation) {
        return;
    }
    pImplementation->doSetSwapInterval(swapchain, interval);
}


/**
 * Generic/Singleton implementation of swappyVkQueuePresent.
 */
VkResult SwappyVk::QueuePresent(VkQueue                 queue,
                                const VkPresentInfoKHR* pPresentInfo)
{
    // This command doesn't have a VkDevice.  It should have at least one VkSwapchainKHR's.  For
    // this command, all VkSwapchainKHR's will have the same VkDevice and VkQueue.
    if ((pPresentInfo->swapchainCount == 0) || (!pPresentInfo->pSwapchains)) {
        // This shouldn't happen, but if it does, something is really wrong.
        return VK_ERROR_DEVICE_LOST;
    }
    SwappyVkBase *pImplementation = perSwapchainImplementation[*pPresentInfo->pSwapchains];
    if (pImplementation) {
        return pImplementation->doQueuePresent(queue, pPresentInfo);
    } else {
        // This should only happen if the API was used wrong (e.g. they never
        // called swappyVkGetRefreshCycleDuration).
        // NOTE: Technically, a Vulkan library shouldn't protect a user from
        // themselves, but we'll be friendlier
        return VK_ERROR_DEVICE_LOST;
    }
}


/***************************************************************************************************
 *
 * API ENTRYPOINTS
 *
 ***************************************************************************************************/

extern "C" {

void swappyVkDetermineDeviceExtensions(
    VkPhysicalDevice       physicalDevice,
    uint32_t               availableExtensionCount,
    VkExtensionProperties* pAvailableExtensions,
    uint32_t*              pRequiredExtensionCount,
    char**                 pRequiredExtensions)
{
    ATRACE_CALL();
    SwappyVk& swappy = SwappyVk::getInstance();
    swappy.swappyVkDetermineDeviceExtensions(physicalDevice,
                                             availableExtensionCount, pAvailableExtensions,
                                             pRequiredExtensionCount, pRequiredExtensions);
}

bool swappyVkGetRefreshCycleDuration(
        VkPhysicalDevice physicalDevice,
        VkDevice         device,
        VkSwapchainKHR   swapchain,
        uint64_t*        pRefreshDuration)
{
    ATRACE_CALL();
    SwappyVk& swappy = SwappyVk::getInstance();
    return swappy.GetRefreshCycleDuration(physicalDevice, device, swapchain, pRefreshDuration);
}

void swappyVkSetSwapInterval(
        VkDevice       device,
        VkSwapchainKHR swapchain,
        uint32_t       interval)
{
    ATRACE_CALL();
    SwappyVk& swappy = SwappyVk::getInstance();
    swappy.SetSwapInterval(device, swapchain, interval);
}

VkResult swappyVkQueuePresent(
        VkQueue                 queue,
        const VkPresentInfoKHR* pPresentInfo)
{
    ATRACE_CALL();
    SwappyVk& swappy = SwappyVk::getInstance();
    return swappy.QueuePresent(queue, pPresentInfo);
}

}  // extern "C"
