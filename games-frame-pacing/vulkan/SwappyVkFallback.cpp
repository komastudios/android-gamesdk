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

#include "SwappyVkFallback.h"

#define LOG_TAG "SwappyVkFallback"

namespace swappy {

SwappyVkFallback::SwappyVkFallback(JNIEnv* env, jobject jactivity,
                                   VkPhysicalDevice physicalDevice,
                                   VkDevice device,
                                   const SwappyVkFunctionProvider* provider)
    : SwappyVkBase(env, jactivity, physicalDevice, device, provider) {}

bool SwappyVkFallback::doGetRefreshCycleDuration(VkSwapchainKHR swapchain,
                                                 uint64_t* pRefreshDuration) {
    if (!isEnabled()) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Swappy is disabled.");
#endif
        return false;
    }

    // Since we don't have presentation timing, we cannot achieve pipelining.
    mCommonBase.setAutoPipelineMode(false);

    *pRefreshDuration = mCommonBase.getRefreshPeriod().count();

#if SWAPPY_VERBOSE_LOGGING
    double refreshRate = 1000000000.0 / *pRefreshDuration;

    ALOGI("Returning refresh duration of %" PRIu64 " nsec (approx %f Hz)",
          *pRefreshDuration, refreshRate);
#endif

    return true;
}

VkResult SwappyVkFallback::doQueuePresent(
    VkQueue queue, uint32_t queueFamilyIndex,
    const VkPresentInfoKHR* pPresentInfo) {
    if (!isEnabled()) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Swappy is disabled.");
#endif
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkResult result = initializeVkSyncObjects(queue, queueFamilyIndex);
    if (result) {
        return result;
    }

    const SwappyCommon::SwapHandlers handlers = {
        .lastFrameIsComplete =
            std::bind(&SwappyVkFallback::lastFrameIsCompleted, this, queue),
        .getPrevFrameGpuTime =
            std::bind(&SwappyVkFallback::getLastFenceTime, this, queue),
    };

    // Inject the fence first and wait for it in onPreSwap() as we don't want to
    // submit a frame before rendering is completed.
    VkSemaphore semaphore;
    result = injectFence(queue, pPresentInfo, &semaphore);
    if (result) {
#if SWAPPY_VERBOSE_LOGGING
        ALOGE("Failed to vkQueueSubmit %d", result);
#endif
        return result;
    }

    uint32_t waitSemaphoreCount;
    const VkSemaphore* pWaitSemaphores;
    if (semaphore != VK_NULL_HANDLE) {
        waitSemaphoreCount = 1;
        pWaitSemaphores = &semaphore;
    } else {
        waitSemaphoreCount = pPresentInfo->waitSemaphoreCount;
        pWaitSemaphores = pPresentInfo->pWaitSemaphores;
    }

    mCommonBase.onPreSwap(handlers);

    VkPresentInfoKHR replacementPresentInfo = {
        pPresentInfo->sType,          nullptr,
        waitSemaphoreCount,           pWaitSemaphores,
        pPresentInfo->swapchainCount, pPresentInfo->pSwapchains,
        pPresentInfo->pImageIndices,  pPresentInfo->pResults};

    result = mpfnQueuePresentKHR(queue, &replacementPresentInfo);

    mCommonBase.onPostSwap(handlers);

    return result;
}

void SwappyVkFallback::enableStats(bool enabled) {
#if SWAPPY_VERBOSE_LOGGING
    ALOGE("Frame Statistics Unsupported - API ignored");
#endif
}

void SwappyVkFallback::getStats(SwappyStats* swappyStats) {
#if SWAPPY_VERBOSE_LOGGING
    ALOGE("Frame Statistics Unsupported - API ignored");
#endif
}

void SwappyVkFallback::recordFrameStart(VkQueue queue, uint32_t image) {
#if SWAPPY_VERBOSE_LOGGING
    ALOGE("Frame Statistics Unsupported - API ignored");
#endif
}

void SwappyVkFallback::clearStats() {
#if SWAPPY_VERBOSE_LOGGING
    ALOGE("Frame Statistics Unsupported - API ignored");
#endif
}

}  // namespace swappy
