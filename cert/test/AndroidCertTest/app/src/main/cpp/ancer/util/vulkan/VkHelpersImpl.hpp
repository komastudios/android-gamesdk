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

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

// Include files for Android
#include <unistd.h>
#include <android/log.h>
#include <vulkan_wrapper.h>


#include <vulkan/vulkan.h>

/* Number of descriptor sets needs to be the same at alloc,       */
/* pipeline layout creation, and descriptor set layout creation   */
#define NUM_DESCRIPTOR_SETS 1

/* Number of samples needs to be the same at image creation,      */
/* renderpass creation and pipeline creation.                     */
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

/* Number of viewports and number of scissors have to be the same */
/* at pipeline creation and in any call to set them dynamically   */
/* They also have to be the same as each other                    */
#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS

/* Amount of time, in nanoseconds, to wait for a command buffer to complete */
#define FENCE_TIMEOUT 100000000

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                               \
    {                                                                          \
        info.fp##entrypoint =                                                  \
            (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint); \
        if (info.fp##entrypoint == NULL) {                                     \
            std::cout << "vkGetDeviceProcAddr failed to find vk" #entrypoint;  \
            exit(-1);                                                          \
        }                                                                      \
    }

#if defined(NDEBUG) && defined(__GNUC__)
#define U_ASSERT_ONLY __attribute__((unused))
#else
#define U_ASSERT_ONLY
#endif

std::string GetBaseDataDir();

std::string GetDataDir(const std::string &filename);

/*
 * structure to track all objects related to a texture.
 */
struct TextureObject {
    VkSampler sampler;

    VkImage image;
    VkImageLayout imageLayout;

    VkDeviceMemory mem;
    VkImageView view;
    int32_t texWidth, texHeight;
};

/*
 * Keep each of our swap chain buffers' image, command buffer and view in one
 * spot
 */
typedef struct _SwapChainBuffers {
    VkImage image;
    VkImageView view;
} SwapChainBuffer;

/*
 * A layer can expose extensions, keep track of those
 * extensions here.
 */
typedef struct {
    VkLayerProperties properties;
    std::vector<VkExtensionProperties> instanceExtensions;
    std::vector<VkExtensionProperties> deviceExtensions;
} LayerProperties;

/*
 * Structure for tracking information used / created / modified
 * by utility functions.
 */
struct VulkanInfo {
    PFN_vkCreateAndroidSurfaceKHR fpCreateAndroidSurfaceKHR;

    VkSurfaceKHR surface;
    bool prepared;
    bool useStagingBuffer;
    bool saveImages;

    std::vector<const char *> instanceLayerNames;
    std::vector<const char *> instanceExtensionNames;
    std::vector<LayerProperties> instanceLayerProperties;
    std::vector<VkExtensionProperties> instanceExtensionProperties;
    VkInstance inst;

    std::vector<const char *> deviceExtensionNames;
    std::vector<VkExtensionProperties> deviceExtensionProperties;
    std::vector<VkPhysicalDevice> gpus;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    uint32_t graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;
    VkPhysicalDeviceProperties gpuProps;
    std::vector<VkQueueFamilyProperties> queueProps;
    VkPhysicalDeviceMemoryProperties memoryProperties;

    VkFramebuffer *framebuffers;
    int width, height;
    VkFormat format;

    uint32_t swapchainImageCount;
    VkSwapchainKHR swapChain;
    std::vector<SwapChainBuffer> buffers;
    VkSemaphore imageAcquiredSemaphore;

    VkCommandPool cmdPool;

    struct {
        VkFormat format;

        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } depth;

    std::vector<struct TextureObject> textures;

    struct {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo bufferInfo;
    } uniformData;

    struct {
        VkDescriptorImageInfo imageInfo;
    } textureData;
    VkDeviceMemory stagingMemory;
    VkImage stagingImage;

    struct {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo bufferInfo;
    } vertexBuffer;
    VkVertexInputBindingDescription viBinding;
    VkVertexInputAttributeDescription viAttribs[2];

    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 Model;
    glm::mat4 Clip;
    glm::mat4 MVP;

    VkCommandBuffer cmd; // Buffer for initialization commands
    VkPipelineLayout pipelineLayout;
    std::vector<VkDescriptorSetLayout> descLayout;
    VkPipelineCache pipelineCache;
    VkRenderPass renderPass;
    VkPipeline pipeline;

    VkPipelineShaderStageCreateInfo shaderStages[2];

    VkDescriptorPool descPool;
    std::vector<VkDescriptorSet> descSet;

    PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback;
    PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback;
    PFN_vkDebugReportMessageEXT dbgBreakCallback;
    std::vector<VkDebugReportCallbackEXT> debugReportCallbacks;

    uint32_t currentBuffer;
    uint32_t queueFamilyCount;

    VkViewport viewport;
    VkRect2D scissor;
};

void ProcessCommandLineArgs(struct VulkanInfo &info, int argc,
                            char **argv);

bool MemoryTypeFromProperties(struct VulkanInfo &info, uint32_t typeBits,
                              VkFlags requirementsMask,
                              uint32_t *typeIndex);

void SetImageLayout(struct VulkanInfo &demo, VkImage image,
                    VkImageAspectFlags aspectMask,
                    VkImageLayout oldImageLayout,
                    VkImageLayout newImageLayout,
                    VkPipelineStageFlags srcStages,
                    VkPipelineStageFlags destStages);

bool ReadPpm(char const *const filename, int &width, int &height,
             uint64_t rowPitch, unsigned char *dataPtr);

void WritePpm(struct VulkanInfo &info, const char *basename);

void ExtractVersion(uint32_t version, uint32_t &major, uint32_t &minor,
                    uint32_t &patch);

bool GLSLtoSPV(const VkShaderStageFlagBits shaderType, const char *pShader,
               std::vector<unsigned int> &spirv);

void InitGlslang();

void FinalizeGlslang();

void WaitSeconds(int seconds);

void PrintUUID(uint8_t *pipelineCacheUUID);

std::string GetFileDirectory();

typedef unsigned long long timestamp_t;

timestamp_t GetMilliseconds();

// Android specific definitions & helpers.
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "VK-SAMPLE",    \
                                             __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "VK-SAMPLE",   \
                                             __VA_ARGS__))
// Replace printf to logcat output.
#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, "VK-SAMPLE",        \
                                        __VA_ARGS__);

bool AndroidProcessCommand();

ANativeWindow *AndroidGetApplicationWindow();

FILE *AndroidFopen(const char *fname, const char *mode);

void AndroidGetWindowSize(int32_t *width, int32_t *height);

bool AndroidLoadFile(const char *filePath, std::string *data);

#ifndef VK_API_VERSION_1_0
// On Android, NDK would include slightly older version of headers that is
// missing the definition.
#define VK_API_VERSION_1_0 VK_API_VERSION
#endif

#pragma clang diagnostic pop