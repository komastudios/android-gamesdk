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

#include <android/log.h>
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, "SwappyVk", __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, "SwappyVk", __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, "SwappyVk", __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "SwappyVk", __VA_ARGS__)
#ifdef ANGLE_FEATURE_UTIL_LOG_VERBOSE
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "SwappyVk", __VA_ARGS__)
#else
#define ALOGV(...) ((void)0)
#endif



// Note: The API functions is at the botton of the file.  Those functions call methods of the
// singleton SwappyVk class.  Those methods call virtual methods of the abstract SwappyVkBase
// class, which is actually implemented by one of the derived/concrete classes:
//
// - SwappyVkGoogle
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
            mInterval(interval), mSwappyVk(swappyVk), mLibVulkan(libVulkan) {};
    virtual uint64_t doGetRefreshCycleDuration(VkSwapchainKHR swapchain) = 0;
    virtual void doSetSwapInterval(VkSwapchainKHR swapchain,
                                   uint32_t       interval) = 0;
    virtual VkResult doQueuePresent(VkQueue                 queue,
                                    const VkPresentInfoKHR* pPresentInfo) = 0;
public:
    VkPhysicalDevice mPhysicalDevice;
    VkDevice         mDevice;
    uint64_t         mRefreshDur;
    uint32_t         mInterval;
    SwappyVk         &mSwappyVk;
    void             *mLibVulkan;
};



/***************************************************************************************************
 *
 * Per-Device concrete/derived class for using VK_GOOGLE_display_timing.
 *
 ***************************************************************************************************/

#ifdef NOT_NEEDED
class SwappyVkGoogleSwapchain;
#endif // NOT_NEEDED

/**
 * Concrete/derived class that sits on top of VK_GOOGLE_display_timing
 */
class SwappyVkGoogle : public SwappyVkBase
{
public:
    SwappyVkGoogle(VkPhysicalDevice physicalDevice,
                   VkDevice         device,
                   SwappyVk         &swappyVk,
                   void             *libVulkan) :
            SwappyVkBase(physicalDevice, device, 16666666, 1, swappyVk, libVulkan),
            mInitialized(false), mNextPresentID(0), mNextDesiredPresentTime(0)
    {
        initialize();
    }
    void initialize()
    {
        PFN_vkGetDeviceProcAddr pfnGetDeviceProcAddr =
                reinterpret_cast<PFN_vkGetDeviceProcAddr>(
                    dlsym(mLibVulkan, "vkGetDeviceProcAddr"));

        mpfnGetRefreshCycleDurationGOOGLE =
                reinterpret_cast<PFN_vkGetRefreshCycleDurationGOOGLE>(
                    pfnGetDeviceProcAddr(mDevice, "vkGetRefreshCycleDurationGOOGLE"));
        mpfnGetPastPresentationTimingGOOGLE =
                reinterpret_cast<PFN_vkGetPastPresentationTimingGOOGLE>(
                    pfnGetDeviceProcAddr(mDevice, "vkGetPastPresentationTimingGOOGLE"));
        if (mpfnGetRefreshCycleDurationGOOGLE && mpfnGetPastPresentationTimingGOOGLE) {
            mInitialized = true;
        }
    }
    virtual uint64_t doGetRefreshCycleDuration(VkSwapchainKHR swapchain)
    {
        // FIXME/TODO: NEED TO CACHE THIS SWAPCHAIN, SO THAT WE KNOW ALL SWAPCHAINS FOR THIS DEVICE

        VkRefreshCycleDurationGOOGLE rcDur;
        if (!mInitialized) {
            initialize();
        }
        if (!mInitialized) {
            // This should never happen.  If so, bail-out early:
            return 0;
        }
        VkResult res = mpfnGetRefreshCycleDurationGOOGLE(mDevice, swapchain, &rcDur);
        if (res != VK_SUCCESS) {
            // This is a rare/unexpected error; so continue to go with a 16,666,666ns duration
            mRefreshDur = 16666666;
        } else {
            mRefreshDur = rcDur.refreshDuration;
        }
        return mRefreshDur;
    }
    virtual void doSetSwapInterval(VkSwapchainKHR swapchain,
                                   uint32_t       interval)
    {
        // TODO: replace this with a better implementation.  While an
        // application will likely call this once, before presenting any images,
        // in case it calls it later, will need to adjust other values
        mInterval = interval;
    }
    virtual VkResult doQueuePresent(VkQueue                 queue,
                                    const VkPresentInfoKHR* pPresentInfo);

private:
    bool mInitialized;
    uint32_t mNextPresentID;
    uint64_t mNextDesiredPresentTime;

    PFN_vkGetRefreshCycleDurationGOOGLE mpfnGetRefreshCycleDurationGOOGLE = nullptr;
    PFN_vkGetPastPresentationTimingGOOGLE mpfnGetPastPresentationTimingGOOGLE = nullptr;

#ifdef NOT_NEEDED
    std::map<VkSwapchainKHR, SwappyVkGoogleSwapchain> swapchains;
#endif // NOT_NEEDED
};

VkResult SwappyVkGoogle::doQueuePresent(VkQueue                 queue,
                                        const VkPresentInfoKHR* pPresentInfo)
{
    // FIXME/TODO: ALLOCATE (CHUNK ALLOC?) MEMORY
    VkPresentTimeGOOGLE *pPresentTimes = nullptr;

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
    //   SwappyVkGoogleSwapchain class.
    //
    // - There's an outside chance that an application won't be able to keep up
    //   with its swap interval (e.g. it's designed for 30FPS on a premium
    //   device and is running on a slow device, or it's running on a 120Hz
    //   display).



    VkPresentTimesInfoGOOGLE presentTimesInfo = {VK_STRUCTURE_TYPE_PRESENT_TIMES_INFO_GOOGLE,
                                                 pPresentInfo->pNext, pPresentInfo->swapchainCount,
                                                 pPresentTimes};
    VkPresentInfoKHR replacementPresentInfo = {pPresentInfo->sType, &presentTimesInfo,
                                               pPresentInfo->waitSemaphoreCount,
                                               pPresentInfo->pWaitSemaphores,
                                               pPresentInfo->swapchainCount,
                                               pPresentInfo->pSwapchains,
                                               pPresentInfo->pImageIndices, pPresentInfo->pResults};
    return vkQueuePresentKHR(queue, &replacementPresentInfo);
};


#ifdef NOT_NEEDED
/**
 * Per-VkSwapchainKHR helper class for using VK_GOOGLE_display_timing
 */
class SwappyVkGoogleSwapchain
{
public:
    SwappyVkGoogleSwapchain(SwappyVkGoogle *pGoogle, VkSwapchainKHR swapchain) :
            mpGoogle(pGoogle), mSwapchain(swapchain), mNextPresentID(0), mNextDesiredPresentTime(0),
            mReceivedPastData(false) {}
public:
    SwappyVkGoogle *mpGoogle;
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
class SwappyVkAndroidFallback : public SwappyVkBase
{
public:
    SwappyVkAndroidFallback(VkPhysicalDevice physicalDevice,
                            VkDevice         device,
                            SwappyVk         &swappyVk,
                            void             *libVulkan) :
            SwappyVkBase(physicalDevice, device, 16666666, 1, swappyVk, libVulkan) {}
    virtual uint64_t doGetRefreshCycleDuration(VkSwapchainKHR swapchain)
    {
        // TODO: replace this with a better implementation, if there's another
        // Android API that can provide this value
        return mRefreshDur;
    }
    virtual void doSetSwapInterval(VkSwapchainKHR swapchain,
                                   uint32_t       interval)
    {
        // TODO: replace this with a real implementation
    }
    virtual VkResult doQueuePresent(VkQueue                 queue,
                                    const VkPresentInfoKHR* pPresentInfo)
    {
        // TODO: replace this with a better implementation that waits before
        // calling vkQueuePresentKHR
        return vkQueuePresentKHR(queue, pPresentInfo);
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

    uint64_t GetRefreshCycleDuration(VkPhysicalDevice physicalDevice,
                                     VkDevice         device,
                                     VkSwapchainKHR   swapchain);
    void SetSwapInterval(VkDevice       device,
                         VkSwapchainKHR swapchain,
                         uint32_t       interval);
    VkResult QueuePresent(VkQueue                 queue,
                          const VkPresentInfoKHR* pPresentInfo);

private:
    std::map<VkDevice, SwappyVkBase*> perDeviceImplementation;
    std::map<VkSwapchainKHR, SwappyVkBase*> perSwapchainImplementation;

    void *mLibVulkan     = nullptr;
public:
    PFN_vkEnumerateDeviceExtensionProperties mpfnEnumerateDeviceExtensionProperties = nullptr;
    PFN_vkQueuePresentKHR mpfnQueuePresentKHR = nullptr;

private:
    SwappyVk() {} // Need to implement this constructor
//    SwappyVk(SwappyVk const&); // Don't implement a copy constructor--no copies
//    void operator=(SwappyVk const&); // Don't implement--no copies
public:
//    SwappyVk(SwappyVk const&)       = delete;
//    void operator=(SwappyVk const&) = delete;
};


/**
 * Generic/Singleton implementation of swappyVkGetRefreshCycleDuration.
 */
uint64_t SwappyVk::GetRefreshCycleDuration(VkPhysicalDevice physicalDevice,
                                           VkDevice         device,
                                           VkSwapchainKHR   swapchain)
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
                return 0;
            }
            mpfnEnumerateDeviceExtensionProperties =
                    reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(
                        dlsym(mLibVulkan, "vkEnumerateDeviceExtensionProperties"));
            mpfnQueuePresentKHR =
                    reinterpret_cast<PFN_vkQueuePresentKHR>(
                        dlsym(mLibVulkan, "vkQueuePresentKHR"));
            if (!mpfnEnumerateDeviceExtensionProperties || !mpfnQueuePresentKHR) {
                // This should never happen.  Bail-out early:
                return 0;
            }
        }
        // First, determine whether VK_GOOGLE_display_timing is available
        bool have_VK_GOOGLE_display_timing = false;
        VkResult res;
        uint32_t count = 0;
        res = mpfnEnumerateDeviceExtensionProperties(physicalDevice, NULL, &count, NULL);
        VkExtensionProperties *pProperties = (VkExtensionProperties *) malloc(sizeof(VkExtensionProperties) * count);
        if (!pProperties) {
            // This is an unlikely error situation, and so return 0
            return 0;
        }
        res = mpfnEnumerateDeviceExtensionProperties(physicalDevice, NULL, &count, pProperties);
        for (uint32_t i = 0; i < count; i++) {
            if (!strcmp(VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME, pProperties[i].extensionName)) {
                have_VK_GOOGLE_display_timing = true;
                break;
            }
        }

        // Second, determine which derived class to use to implement the rest of the
        // API
        if (have_VK_GOOGLE_display_timing) {
            // Instantiate the class that sits on top of VK_GOOGLE_display_timing
            pImplementation = new SwappyVkGoogle(physicalDevice, device, getInstance(), mLibVulkan);
        } else {
            // Instantiate the class that sits on top of the basic Vulkan APIs
            pImplementation = new SwappyVkAndroidFallback(physicalDevice, device, getInstance(),
                                                          mLibVulkan);
        }

        // Third, cache the per-device pointer to the derived class:
        if (pImplementation) {
            perDeviceImplementation[device] = pImplementation;
        } else {
            // This shouldn't happen, but if it does, something is really wrong.
            return VK_ERROR_DEVICE_LOST;
        }
    }

    // Cache the per-swapchain pointer to the derived class:
    perSwapchainImplementation[swapchain] = pImplementation;

    // Now, call that derived class to get the refresh duration to return
    return pImplementation->doGetRefreshCycleDuration(swapchain);
}


/**
 * Generic/Singleton implementation of swappyVkSetSwapInterval.
 */
void SwappyVk::SetSwapInterval(VkDevice       device,
                               VkSwapchainKHR swapchain,
                               uint32_t       interval)
{
    SwappyVkBase *pImplementation = perDeviceImplementation[device];
    if (pImplementation) {
        pImplementation->doSetSwapInterval(swapchain, interval);
    }
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

uint64_t swappyVkGetRefreshCycleDuration(
        VkPhysicalDevice physicalDevice,
        VkDevice         device,
        VkSwapchainKHR   swapchain)
{
    SwappyVk swappy = SwappyVk::getInstance();
    return swappy.GetRefreshCycleDuration(physicalDevice, device, swapchain);
}

void swappyVkSetSwapInterval(
        VkDevice       device,
        VkSwapchainKHR swapchain,
        uint32_t       interval)
{
    SwappyVk swappy = SwappyVk::getInstance();
    swappy.SetSwapInterval(device, swapchain, interval);
}

VkResult swappyVkQueuePresent(
        VkQueue                 queue,
        const VkPresentInfoKHR* pPresentInfo)
{
    SwappyVk swappy = SwappyVk::getInstance();
    return swappy.QueuePresent(queue, pPresentInfo);
}

}  // extern "C"
