#include <list>

#include "vulkan_wrapper/vulkan_wrapper.h"

namespace istresser_testvulkanrenderer {

// A manager object that allocates memory via the Vulkan API.
class TestVulkanRenderer {
 private:
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device = VK_NULL_HANDLE;
  std::list<VkDeviceMemory> deviceMemoryHandles;

  bool MapMemoryTypeToIndex(uint32_t typeBits, VkFlags requirements_mask,
                            uint32_t *typeIndex);

 public:
  TestVulkanRenderer();

  ~TestVulkanRenderer() = default;

  int64_t Allocate(int64_t to_allocate);

  void Release();
};
}  // namespace istresser_testvulkanrenderer
