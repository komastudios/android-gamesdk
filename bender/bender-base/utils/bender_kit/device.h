//
// Created by chingtangyu on 11/15/2019.
//

#ifndef BENDER_BASE_DEVICE_H
#define BENDER_BASE_DEVICE_H

#include "vulkan_wrapper.h"
#include "bender_kit.h"

#include <vector>
#include <string>
#include <array>

namespace benderkit {
    class Device {
    public:
        Device(ANativeWindow *window);

        ~Device();

        bool IsInitialized() { return initialized_; }

        void CreateImageView();

        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer &buffer,
                          VkDeviceMemory &buffer_memory,
                          VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VkDevice GetDevice() const { return device_; }

        VkPhysicalDevice GetPhysicalDevice() const { return gpu_device_; }

        uint32_t GetQueueFamilyIndex() const { return queue_family_index_; }

        VkSurfaceKHR GetSurface() const { return surface_; }

        VkQueue GetMainQueue() const { return main_queue_; }
        VkQueue GetWorkerQueue() const { return worker_queue_; }

        VkSwapchainKHR GetSwapchain() const { return swapchain_; }

        uint GetSwapchainLength() const { return swapchain_length_; }

        VkSurfaceTransformFlagBitsKHR GetPretransformFlag() const { return pretransform_flag_; }

        VkExtent2D GetDisplaySize() const { return display_size_identity_; }

        VkExtent2D GetDisplaySizeOriented() const {
            VkExtent2D result = display_size_identity_;
            if (pretransform_flag_ == VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR ||
                pretransform_flag_ == VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
                result.width = display_size_identity_.height;
                result.height = display_size_identity_.width;
            }
            return result;
        }

        VkFormat GetDisplayFormat() const { return display_format_; }

        VkPhysicalDeviceMemoryProperties GetGpuMemProperties() { return gpu_memory_properties_; }

        const std::vector<VkImage> &GetDisplayImages() { return display_images_; }

        VkImage GetCurrentDisplayImage() const { return display_images_[current_frame_index_]; }

        uint GetCurrentFrameIndex() const { return current_frame_index_; }

        bool GetWindowResized() const { return window_resized_; }

        void SetWindowResized(bool resized) { window_resized_ = resized; }

        void Present(VkSemaphore* wait_semaphores);

        void SetObjectName(uint64_t object,
                           VkDebugReportObjectTypeEXT object_type,
                           const char *name);

        void SetObjectTag(uint64_t object,
                          VkDebugReportObjectTypeEXT object_type,
                          uint64_t name,
                          size_t tag_size,
                          const void *tag);

        void BeginDebugRegion(VkCommandBuffer cmd_buffer, const char *marker_name,
                              std::array<float, 4> color = {
                                      1.0f, 1.0f, 1.0f, 1.0f});

        void InsertDebugMarker(VkCommandBuffer cmd_buffer, const char *marker_name,
                               std::array<float, 4> color = {
                                       1.0f, 1.0f, 1.0f, 1.0f});

        void EndDebugRegion(VkCommandBuffer cmd_buffer);

        void CreateSwapChain(VkSwapchainKHR old_swapchain = VK_NULL_HANDLE);

    private:
        bool initialized_;

        VkInstance instance_;
        VkPhysicalDevice gpu_device_;
        VkPhysicalDeviceMemoryProperties gpu_memory_properties_;
        VkDevice device_;
        uint32_t queue_family_index_;
        VkSurfaceKHR surface_;
        VkQueue main_queue_;
        VkQueue worker_queue_;

        VkSwapchainKHR swapchain_;
        uint32_t swapchain_length_;
        VkSurfaceTransformFlagBitsKHR pretransform_flag_;

        VkExtent2D display_size_identity_;
        VkFormat display_format_;
        bool window_resized_ = false;

        uint current_frame_index_ = 0;

        std::vector<VkImage> display_images_;

        void CreateVulkanDevice(ANativeWindow *platform_window, VkApplicationInfo *app_info);
    };
}

#endif //BENDER_BASE_DEVICE_H
