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

#ifndef SWAPPYVK_H
#define SWAPPYVK_H

#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize Swappy-for-Vulkan and obtain the approximate number of nanoseconds
 * between vertical-blanking periods, or intervals, of the swapchain’s physical
 * display.
 *
 * If the application converts this number to a fraction (e.g. 16,666,666 nsec
 * to 0.016666666) and divides one by that fraction, it will be the approximate
 * refresh rate of the display (e.g. 16,666,666 nanoseconds corresponds to a
 * 60Hz display, 11,111,111 nsec corresponds to a 90Hz display).
 *
 * If your application presents to more than one swapchain at a time, you must
 * call this for each swapchain before calling swappyVkSetSwapInterval() for it.
 *
 * Parameters:
 *
 *  (IN)  physicalDevice - The VkPhysicalDevice associated with the swapchain
 *  (IN)  device    - The VkDevice associated with the swapchain
 *  (IN)  swapchain - The VkSwapchainKHR the application wants Swappy to swap
 */
uint64_t swappyVkGetRefreshCycleDuration(
        VkPhysicalDevice physicalDevice,
        VkDevice         device,
        VkSwapchainKHR   swapchain);


// TODO/TBD: DECIDE HOW TO HANDLE SWAPCHAIN RE-CREATION.  IT WOULD BE NICE FOR
// SWAPPY TO KNOW/TRACK SWAPCHAINS GOING AWAY.


/**
 * Tell Swappy the number of vertical-blanking intervals that each presented
 * image should be visible for.
 *
 * For example, if the refresh duration is approximately 16,666,666 nses (60Hz
 * display), and if the application wants to present at 30 frames-per-second
 * (FPS), the application would set the swap interval to 2.  For 30FPS on a 90Hz
 * display, set the swap interval to 3.
 *
 * If your application presents to more than one swapchain at a time, you must
 * call this for each swapchain before presenting to it.
 *
 * Parameters:
 *
 *  (IN)  device    - The VkDevice associated with the swapchain
 *  (IN)  swapchain - The VkSwapchainKHR the application wants Swappy to swap
 *  (IN)  interval  - The number of vertical-blanking intervals each image
 *                    should be visible
 */
void swappyVkSetSwapInterval(
        VkDevice       device,
        VkSwapchainKHR swapchain,
        uint32_t       interval);

/**
 * Tell Swappy to present one or more images to corresponding swapchains.
 *
 * Swappy will call vkQueuePresentKHR for your application.  Swappy may insert a
 * struct to the pNext-chain of VkPresentInfoKHR, or it may insert other Vulkan
 * commands in order to attempt to honor the desired swap interval.
 *
 * Note: If your application presents to more than one swapchain at a time, and
 * if you use a different swap interval for each swapchain, Swappy will attempt
 * to honor the swap interval for each swapchain (being more successful on
 * devices that support an underlying presentation-timing extension, such as
 * VK_GOOGLE_display_timing).
 *
 * Parameters:
 *
 *  (IN)  queue     - The VkQueue associated with the device and swapchain
 *  (IN)  pPresentInfo - A pointer to the VkPresentInfoKHR containing the
 *                    information about what image(s) to present on which
 *                    swapchain(s).
 */
VkResult swappyVkQueuePresent(
        VkQueue                 queue,
        const VkPresentInfoKHR* pPresentInfo);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //SWAPPYVK_H
