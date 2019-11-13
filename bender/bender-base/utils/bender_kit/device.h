//
// Created by chingtangyu on 11/15/2019.
//

#ifndef BENDER_BASE_DEVICE_H
#define BENDER_BASE_DEVICE_H

#include <vector>
#include <string>
#include <array>

#include "vulkan_wrapper.h"
#include "bender_kit.h"

namespace BenderKit {
    class Device {
    public:
        Device(ANativeWindow *window);

        ~Device();

        bool isInitialized() { return initialized_; }

        void CreateImageView();

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer &buffer,
                          VkDeviceMemory &bufferMemory,
                          VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VkDevice getDevice() const { return device_; }

        VkPhysicalDevice getPhysicalDevice() const { return gpuDevice_; }

        uint32_t getQueueFamilyIndex() const { return queueFamilyIndex_; }

        VkSurfaceKHR getSurface() const { return surface_; }

        VkQueue getQueue() const { return queue_; }

        VkSwapchainKHR getSwapchain() const { return swapchain_; }

        uint getSwapchainLength() const { return swapchainLength_; }

        VkSurfaceTransformFlagBitsKHR getPretransformFlag() const { return pretransformFlag_; }

        VkExtent2D getDisplaySize() const { return displaySize_; }

        VkFormat getDisplayFormat() const { return displayFormat_; }

        VkPhysicalDeviceMemoryProperties getGpuMemProperties() { return gpuMemoryProperties_; }

        const std::vector<VkImage> &getDisplayImages() { return displayImages_; }

        VkImage getCurrentDisplayImage() const { return displayImages_[current_frame_index_]; }

        uint getCurrentFrameIndex() const { return current_frame_index_; }

        void present(VkSemaphore* wait_semaphores);

        void setObjectName(uint64_t object,
                           VkDebugReportObjectTypeEXT objectType,
                           const char *name);

        void setObjectTag(uint64_t object,
                          VkDebugReportObjectTypeEXT objectType,
                          uint64_t name,
                          size_t tagSize,
                          const void *tag);

        void beginDebugRegion(VkCommandBuffer cmdbuffer, const char *markerName,
                              std::array<float, 4> color = {
                                      1.0f, 1.0f, 1.0f, 1.0f});
        void insertDebugMarker(VkCommandBuffer cmdbuffer, const char *markerName,
                               std::array<float, 4> color = {
                                       1.0f, 1.0f, 1.0f, 1.0f});

        void endDebugRegion(VkCommandBuffer cmdBuffer);

        void createSwapChain(VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);

    private:
        bool initialized_;

        VkInstance instance_;
        VkPhysicalDevice gpuDevice_;
        VkPhysicalDeviceMemoryProperties gpuMemoryProperties_;
        VkDevice device_;
        uint32_t queueFamilyIndex_;
        VkSurfaceKHR surface_;
        VkQueue queue_;

        VkSwapchainKHR swapchain_;
        uint32_t swapchainLength_;
        VkSurfaceTransformFlagBitsKHR pretransformFlag_;

        VkExtent2D displaySize_;
        VkFormat displayFormat_;

        uint current_frame_index_ = 0;

        std::vector<VkImage> displayImages_;

        void CreateVulkanDevice(ANativeWindow *platformWindow,
                                VkApplicationInfo *appInfo);

    };
}

#endif //BENDER_BASE_DEVICE_H
