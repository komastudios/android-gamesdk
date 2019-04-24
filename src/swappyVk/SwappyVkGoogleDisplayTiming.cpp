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

#include "SwappyVkIncludeAll.h"

SwappyVkGoogleDisplayTiming::SwappyVkGoogleDisplayTiming(VkPhysicalDevice physicalDevice,
                                                            VkDevice         device,
                                                            SwappyVk         &swappyVk,
                                                            void             *libVulkan) :
SwappyVkBase(physicalDevice, device, k16_6msec, 1, swappyVk, libVulkan)
{
    initGoogExtention();
}

bool SwappyVkGoogleDisplayTiming::doGetRefreshCycleDuration(VkSwapchainKHR swapchain,
                                           uint64_t*      pRefreshDuration)
{
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

    ALOGD("Returning refresh duration of %" PRIu64 " nsec (approx %f Hz)", mRefreshDur, refreshRate);

    *pRefreshDuration = mRefreshDur;
    return true;
}

VkResult SwappyVkGoogleDisplayTiming::doQueuePresent(VkQueue                 queue,
                                                     uint32_t                queueFamilyIndex,
                                                     const VkPresentInfoKHR* pPresentInfo)
{
    VkResult ret = VK_SUCCESS;

    calculateNextDesiredPresentTime(pPresentInfo->pSwapchains[0]);

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
}

void SwappyVkGoogleDisplayTiming::calculateNextDesiredPresentTime(VkSwapchainKHR swapchain)
{
    struct timespec currTime;
    clock_gettime(CLOCK_MONOTONIC, &currTime);
    uint64_t currentTime =
            ((uint64_t) currTime.tv_sec * kBillion) + (uint64_t) currTime.tv_nsec;


    // Determine the desiredPresentTime:
    if (!mNextDesiredPresentTime) {
        mNextDesiredPresentTime = currentTime + mRefreshDur;
    } else {
        // Look at the timing of past presents, and potentially adjust mNextDesiredPresentTime:
        checkPastPresentTiming(swapchain);
        mNextDesiredPresentTime += mRefreshDur * mInterval;

        // Make sure the calculated time is not before the current time to present
        if (mNextDesiredPresentTime < currentTime) {
            mNextDesiredPresentTime = currentTime + mRefreshDur;
        }
    }
}

void SwappyVkGoogleDisplayTiming::checkPastPresentTiming(VkSwapchainKHR swapchain)
{
    VkResult ret = VK_SUCCESS;

    if (mNextPresentID <= mNextPresentIDToCheck) {
        return;
    }
    // Check the timing of past presents to see if we need to adjust mNextDesiredPresentTime:
    uint32_t pastPresentationTimingCount = 0;
    VkResult err = mpfnGetPastPresentationTimingGOOGLE(mDevice, swapchain,
                                                       &pastPresentationTimingCount, NULL);
    if (!pastPresentationTimingCount) {
        return;
    }
    // TODO: don't allocate memory for the timestamps every time.
    VkPastPresentationTimingGOOGLE *past =
            reinterpret_cast<VkPastPresentationTimingGOOGLE*>(
                    malloc(sizeof(VkPastPresentationTimingGOOGLE) *
                           pastPresentationTimingCount));
    err = mpfnGetPastPresentationTimingGOOGLE(mDevice, swapchain,
                                              &pastPresentationTimingCount, past);
    for (uint32_t i = 0; i < pastPresentationTimingCount; i++) {
        // Note: On Android, actualPresentTime can actually be before desiredPresentTime
        // (which shouldn't be possible.  Therefore, this must be a signed integer.
        int64_t amountEarlyBy =
                (int64_t) past[i].actualPresentTime - (int64_t)past[i].desiredPresentTime;
        if (amountEarlyBy < kTooCloseToVsyncBoundary) {
            // We're getting too close to vsync.  Nudge the next present back
            // towards/in the boundaries, and check back after a few more presents:
            mNextDesiredPresentTime -= kNudgeWithinVsyncBoundaries;
            mNextPresentIDToCheck = mNextPresentID + 7;
            break;
        }
        if (amountEarlyBy > kTooFarAwayFromVsyncBoundary) {
            // We're getting too far away from vsync.  Nudge the next present back
            // towards/in the boundaries, and check back after a few more presents:
            mNextDesiredPresentTime += kNudgeWithinVsyncBoundaries;
            mNextPresentIDToCheck = mNextPresentID + 7;
            break;
        }
    }
    free(past);
}