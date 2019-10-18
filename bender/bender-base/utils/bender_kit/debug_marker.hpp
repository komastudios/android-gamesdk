//
// Created by theoking on 10/22/2019.
//

#ifndef DEBUG_MARKER
#define DEBUG_MARKER

#include "vulkan_wrapper.h"
#include <string>

namespace DebugMarker {
    void setup(VkDevice device, VkPhysicalDevice physicalDevice);
    void setObjectName(VkDevice device, uint64_t object, VkDebugReportObjectTypeEXT objectType, const char *name);
    void setObjectTag(VkDevice device, uint64_t object, VkDebugReportObjectTypeEXT objectType, uint64_t name, size_t tagSize, const void* tag);
    void beginRegion(VkCommandBuffer cmdbuffer, const char* pMarkerName, float color[4]);
    void insert(VkCommandBuffer cmdbuffer, std::string markerName, float color[4]);
    void endRegion(VkCommandBuffer cmdBuffer);
}

#endif