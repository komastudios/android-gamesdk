// Copyright 2019 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "debug_marker.h"
#include "bender_kit.h"

//Implementation based entirely on:
//https://github.com/SaschaWillems/Vulkan/blob/master/examples/debugmarker/debugmarker.cpp
namespace DebugMarker {
bool active = false;
bool extensionPresent = false;

// Get function pointers for the debug report extensions from the device
void setup(VkDevice device, VkPhysicalDevice physicalDevice) {
  // Check if the debug marker extension is present (which is the case if run from a graphics debugger)
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());
  for (auto extension : extensions) {
    if (strcmp(extension.extensionName, VK_EXT_DEBUG_MARKER_EXTENSION_NAME) == 0) {
      extensionPresent = true;
      break;
    }
  }

  if (extensionPresent) {
    // The debug marker extension is not part of the core, so function pointers need to be loaded manually
    vkDebugMarkerSetObjectTagEXT = (PFN_vkDebugMarkerSetObjectTagEXT) vkGetDeviceProcAddr(device,
                                                                                          "vkDebugMarkerSetObjectTagEXT");
    vkDebugMarkerSetObjectNameEXT = (PFN_vkDebugMarkerSetObjectNameEXT) vkGetDeviceProcAddr(device,
                                                                                            "vkDebugMarkerSetObjectNameEXT");
    vkCmdDebugMarkerBeginEXT =
        (PFN_vkCmdDebugMarkerBeginEXT) vkGetDeviceProcAddr(device, "vkCmdDebugMarkerBeginEXT");
    vkCmdDebugMarkerEndEXT =
        (PFN_vkCmdDebugMarkerEndEXT) vkGetDeviceProcAddr(device, "vkCmdDebugMarkerEndEXT");
    vkCmdDebugMarkerInsertEXT =
        (PFN_vkCmdDebugMarkerInsertEXT) vkGetDeviceProcAddr(device, "vkCmdDebugMarkerInsertEXT");
    // Set flag if at least one function pointer is present
    active = (vkDebugMarkerSetObjectNameEXT != VK_NULL_HANDLE);
  } else {
    LOGE("Warning: VK_EXT_debug_marker not present, debug markers are disabled.");
    LOGE("Try running from inside a Vulkan graphics debugger (e.g. RenderDoc)");
  }
}

// Sets the debug name of an object
// All Objects in Vulkan are represented by their 64-bit handles which are passed into this function
// along with the object type
void setObjectName(VkDevice device,
                   uint64_t object,
                   VkDebugReportObjectTypeEXT objectType,
                   const char *name) {
  // Check for valid function pointer (may not be present if not running in a debugging application)
  if (active) {
    VkDebugMarkerObjectNameInfoEXT nameInfo = {};
    nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
    nameInfo.objectType = objectType;
    nameInfo.object = object;
    nameInfo.pObjectName = name;
    vkDebugMarkerSetObjectNameEXT(device, &nameInfo);
  }
}

// Set the tag for an object
void setObjectTag(VkDevice device,
                  uint64_t object,
                  VkDebugReportObjectTypeEXT objectType,
                  uint64_t name,
                  size_t tagSize,
                  const void *tag) {
  // Check for valid function pointer (may not be present if not running in a debugging application)
  if (active) {
    VkDebugMarkerObjectTagInfoEXT tagInfo = {};
    tagInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT;
    tagInfo.objectType = objectType;
    tagInfo.object = object;
    tagInfo.tagName = name;
    tagInfo.tagSize = tagSize;
    tagInfo.pTag = tag;
    vkDebugMarkerSetObjectTagEXT(device, &tagInfo);
  }
}

// Start a new debug marker region
void beginRegion(VkCommandBuffer cmdbuffer, const char *pMarkerName, float color[4]) {
  // Check for valid function pointer (may not be present if not running in a debugging application)
  if (active) {
    VkDebugMarkerMarkerInfoEXT markerInfo = {};
    markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
    memcpy(markerInfo.color, &color[0], sizeof(float) * 4);
    markerInfo.pMarkerName = pMarkerName;
    vkCmdDebugMarkerBeginEXT(cmdbuffer, &markerInfo);
  }
}

// Insert a new debug marker into the command buffer
void insert(VkCommandBuffer cmdbuffer, std::string markerName, float color[4]) {
  // Check for valid function pointer (may not be present if not running in a debugging application)
  if (active) {
    VkDebugMarkerMarkerInfoEXT markerInfo = {};
    markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
    memcpy(markerInfo.color, &color[0], sizeof(float) * 4);
    markerInfo.pMarkerName = markerName.c_str();
    vkCmdDebugMarkerInsertEXT(cmdbuffer, &markerInfo);
  }
}

// End the current debug marker region
void endRegion(VkCommandBuffer cmdBuffer) {
  // Check for valid function (may not be present if not runnin in a debugging application)
  if (vkCmdDebugMarkerEndEXT) {
    vkCmdDebugMarkerEndEXT(cmdBuffer);
  }
}
};