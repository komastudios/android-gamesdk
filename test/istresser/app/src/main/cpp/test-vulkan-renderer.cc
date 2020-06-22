#include "test-vulkan-renderer.h"

#include <android/log.h>

#include <vector>

constexpr char kAppName[] = "istresser";

#define CALL_VK(func)                                                 \
  if (VK_SUCCESS != (func)) {                                         \
    __android_log_print(ANDROID_LOG_ERROR, kAppName,                  \
                        "Vulkan error. File[%s], line[%d]", __FILE__, \
                        __LINE__);                                    \
    assert(false);                                                    \
  }

namespace istresser_testvulkanrenderer {

TestVulkanRenderer::TestVulkanRenderer() {
  if (!InitVulkan()) {
    __android_log_print(ANDROID_LOG_INFO, kAppName, "Could not start Vulkan");
    return;
  }

  VkApplicationInfo appInfo = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .apiVersion = VK_MAKE_VERSION(1, 0, 0),
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .pApplicationName = kAppName,
      .pEngineName = kAppName,
  };

  VkInstanceCreateInfo instanceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .pApplicationInfo = &appInfo,
      .enabledExtensionCount = 0,
      .ppEnabledExtensionNames = nullptr,
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,
  };
  VkInstance instance;
  CALL_VK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

  uint32_t gpuCount = 0;
  CALL_VK(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr));
  VkPhysicalDevice tmpGpus[gpuCount];
  CALL_VK(vkEnumeratePhysicalDevices(instance, &gpuCount, tmpGpus));
  physicalDevice = tmpGpus[0];

  uint32_t queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           nullptr);
  assert(queueFamilyCount);
  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           queueFamilyProperties.data());

  uint32_t queueFamilyIndex;
  for (queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount;
       queueFamilyIndex++) {
    if (queueFamilyProperties[queueFamilyIndex].queueFlags &
        VK_QUEUE_GRAPHICS_BIT) {
      break;
    }
  }

  float priorities[] = {1.0f};

  VkDeviceQueueCreateInfo queueCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueCount = 1,
      .queueFamilyIndex = queueFamilyIndex,
      .pQueuePriorities = priorities,
  };

  VkDeviceCreateInfo deviceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = nullptr,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &queueCreateInfo,
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,
      .enabledExtensionCount = 0,
      .ppEnabledExtensionNames = nullptr,
      .pEnabledFeatures = nullptr,
  };

  CALL_VK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));
}

bool TestVulkanRenderer::MapMemoryTypeToIndex(uint32_t typeBits,
                                              VkFlags requirements_mask,
                                              uint32_t *typeIndex) {
  VkPhysicalDeviceMemoryProperties memoryProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
  for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
    if ((typeBits & 1) == 1) {
      if ((memoryProperties.memoryTypes[i].propertyFlags & requirements_mask) ==
          requirements_mask) {
        *typeIndex = i;
        return true;
      }
    }
    typeBits >>= 1;
  }
  return false;
}

void TestVulkanRenderer::Release() {
  for (std::list<VkDeviceMemory>::iterator it = deviceMemoryHandles.begin();
       it != deviceMemoryHandles.end(); it++) {
    vkFreeMemory(device, *it, nullptr);
  }
  deviceMemoryHandles.clear();
}

int64_t TestVulkanRenderer::Allocate(int64_t to_allocate) {
  if (device == VK_NULL_HANDLE) {
    return 0;
  }
  VkMemoryAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = nullptr,
      .allocationSize = (VkDeviceSize)to_allocate,
      .memoryTypeIndex = 0,
  };

  MapMemoryTypeToIndex((uint32_t)-1,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       &allocInfo.memoryTypeIndex);

  VkDeviceMemory deviceMemory;
  VkResult result =
      vkAllocateMemory(device, &allocInfo, nullptr, &deviceMemory);
  if (result != VK_SUCCESS) {
    return 0;
  }

  void *data;
  CALL_VK(
      vkMapMemory(device, deviceMemory, 0, allocInfo.allocationSize, 0, &data));
  memset(data, -1, (size_t)to_allocate);
  vkUnmapMemory(device, deviceMemory);

  deviceMemoryHandles.push_back(deviceMemory);

  return to_allocate;
}

}  // namespace istresser_testvulkanrenderer
