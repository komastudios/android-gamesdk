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
 * Determine any Vulkan device extensions that must be enabled for a new
 * VkDevice.
 *
 * Swappy-for-Vulkan (SwappyVk) benefits from certain Vulkan device extensions
 * (e.g. VK_GOOGLE_display_timing).  Before the application calls
 * vkCreateDevice, SwappyVk needs to look at the list of available extensions
 * (returned by vkEnumerateDeviceExtensionProperties) and potentially identify
 * one or more extensions that the application must add to:
 *
 * - VkDeviceCreateInfo::enabledExtensionCount
 * - VkDeviceCreateInfo::ppEnabledExtensionNames
 *
 * before the application calls vkCreateDevice.  For each VkPhysicalDevice that
 * the application will call vkCreateDevice for, the application must call this
 * function, and then must add the identified extension(s) to the list that are
 * enabled for the VkDevice.  Similar to many Vulkan functions, this function
 * can be called twice, once to identify the number of required extensions, and
 * again with application-allocated memory that the function can write into.
 *
 * Parameters:
 *
 *  (IN)    physicalDevice          - The VkPhysicalDevice associated with the
 *                    available extensions.
 *  (IN)    availableExtensionCount - This is the returned value of
 *                    pPropertyCount from vkEnumerateDeviceExtensionProperties.
 *  (IN)    pAvailableExtensions    - This is the returned value of
 *                    pProperties from vkEnumerateDeviceExtensionProperties.
 *  (INOUT) pRequiredExtensionCount - If pRequiredExtensions is nullptr, the
 *                    function sets this to the number of extensions that are
 *                    required.  If pRequiredExtensions is non-nullptr, this
 *                    is the number of required extensions that the function
 *                    should write into pRequiredExtensions.
 *  (INOUT) pRequiredExtensions - If non-nullptr, this is application-allocated
 *                    memory into which the function will write the names of
 *                    required extensions.  It is a pointer to an array of
 *                    char* strings (i.e. the same as
 *                    VkDeviceCreateInfo::ppEnabledExtensionNames).
 */
void swappyVkDetermineDeviceExtensions(
    VkPhysicalDevice       physicalDevice,
    uint32_t               availableExtensionCount,
    VkExtensionProperties* pAvailableExtensions,
    uint32_t*              pRequiredExtensionCount,
    char**                 pRequiredExtensions);


// TBD: For now, SwappyVk assumes only one VkSwapchainKHR per VkDevice, and that
// applications don't re-create swapchains.  Is this long-term sufficient?

/**
 * Initialize SwappyVk for a given device and swapchain, and obtain the
 * approximate time duration between vertical-blanking periods.
 *
 * If your application presents to more than one swapchain at a time, you must
 * call this for each swapchain before calling swappyVkSetSwapInterval() for it.
 *
 * The duration between vertical-blanking periods (an interval) is expressed as
 * the approximate number of nanoseconds between vertical-blanking periods of
 * the swapchain’s physical display.
 *
 * If the application converts this number to a fraction (e.g. 16,666,666 nsec
 * to 0.016666666) and divides one by that fraction, it will be the approximate
 * refresh rate of the display (e.g. 16,666,666 nanoseconds corresponds to a
 * 60Hz display, 11,111,111 nsec corresponds to a 90Hz display).
 *
 * Parameters:
 *
 *  (IN)  physicalDevice   - The VkPhysicalDevice associated with the swapchain
 *  (IN)  device    - The VkDevice associated with the swapchain
 *  (IN)  swapchain - The VkSwapchainKHR the application wants Swappy to swap
 *  (OUT) pRefreshDuration - The returned refresh cycle duration
 *
 * Return value:
 *
 *  bool            - true if the value returned by pRefreshDuration is valid,
 *                    otherwise false if an error.
 */
bool swappyVkGetRefreshCycleDuration(
        VkPhysicalDevice physicalDevice,
        VkDevice         device,
        VkSwapchainKHR   swapchain,
        uint64_t*        pRefreshDuration);


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
 *  (IN) fenceCount        - Number of elements in fences array
 *  (IN) fences (optional) - Array of fences corresponding to previous frame
 */
VkResult swappyVkQueuePresent(
        VkQueue                 queue,
        const VkPresentInfoKHR* pPresentInfo,
        uint32_t                fenceCount,
        VkFence*                fences);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //SWAPPYVK_H
