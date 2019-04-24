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
 *
 * Base class members are used by the derived classes to unify the behavior across implementaitons:
 *  @mThread - Thread used for getting Choreographer events.
 *  @mTreadRunning - Used to signal the tread to exit
 *  @mNextPresentID - unique ID for frame presentation.
 *  @mNextDesiredPresentTime - Holds the time in nanoseconds for the next frame to be presented.
 *  @mNextPresentIDToCheck - Used to determine whether presentation time needs to be adjusted.
 *  @mFrameID - Keeps track of how many Choreographer callbacks received.
 *  @mLastframeTimeNanos - Holds the last frame time reported by Choreographer.
 *  @mSumRefreshTime - Used together with @mSamples to calculate refresh rate based on Choreographer.
 */

#pragma once
#include "SwappyVkIncludeAll.h"

class SwappyVkBase
{
public:
    SwappyVkBase(VkPhysicalDevice physicalDevice,
                 VkDevice         device,
                 uint64_t         refreshDur,
                 uint32_t         interval,
                 SwappyVk         &swappyVk,
                 void             *libVulkan);

    virtual ~SwappyVkBase();

    virtual bool doGetRefreshCycleDuration(VkSwapchainKHR swapchain,
                                           uint64_t*      pRefreshDuration) = 0;

    virtual VkResult doQueuePresent(VkQueue                 queue,
                                    uint32_t                queueFamilyIndex,
                                    const VkPresentInfoKHR* pPresentInfo) = 0;

    void doSetSwapInterval(VkSwapchainKHR swapchain,
                           uint32_t       interval);
protected:
    VkPhysicalDevice mPhysicalDevice;
    VkDevice         mDevice;
    uint64_t         mRefreshDur;
    uint32_t         mInterval;
    SwappyVk         &mSwappyVk;
    void             *mLibVulkan;
    bool             mInitialized;
    pthread_t mThread = 0;
    ALooper *mLooper = nullptr;
    bool mTreadRunning = false;
    AChoreographer *mChoreographer = nullptr;
    std::mutex mWaitingMutex;
    std::condition_variable mWaitingCondition;
    uint32_t mNextPresentID = 0;
    uint64_t mNextDesiredPresentTime = 0;
    uint32_t mNextPresentIDToCheck = 2;

    PFN_vkGetDeviceProcAddr mpfnGetDeviceProcAddr = nullptr;
    PFN_vkQueuePresentKHR   mpfnQueuePresentKHR = nullptr;
    PFN_vkGetRefreshCycleDurationGOOGLE mpfnGetRefreshCycleDurationGOOGLE = nullptr;
    PFN_vkGetPastPresentationTimingGOOGLE mpfnGetPastPresentationTimingGOOGLE = nullptr;

    void *mLibAndroid = nullptr;
    PFN_AChoreographer_getInstance mAChoreographer_getInstance = nullptr;
    PFN_AChoreographer_postFrameCallback mAChoreographer_postFrameCallback = nullptr;
    PFN_AChoreographer_postFrameCallbackDelayed mAChoreographer_postFrameCallbackDelayed = nullptr;

    long mFrameID = 0;
    long mTargetFrameID = 0;
    uint64_t mLastframeTimeNanos = 0;
    long mSumRefreshTime = 0;
    long mSamples = 0;
    long mCallbacksBeforeIdle = 0;

    static constexpr int MAX_SAMPLES = 5;
    static constexpr int MAX_CALLBACKS_BEFORE_IDLE = 10;

    void initGoogExtention();
    void startChoreographerThread();
    void stopChoreographerThread();
    static void *looperThreadWrapper(void *data);
    void *looperThread();
    static void frameCallback(long frameTimeNanos, void *data);
    void onDisplayRefresh(long frameTimeNanos);
    void calcRefreshRate(uint64_t currentTime);
    void postChoreographerCallback();
};