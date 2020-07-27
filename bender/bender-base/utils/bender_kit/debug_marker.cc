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
//https://github.com/SaschaWillems/Vulkan/blob/default/examples/debugmarker/debugmarker.cpp
namespace debugmarker {
bool active = false;
bool extension_present = false;

void Setup(VkDevice device, VkPhysicalDevice physical_device) {
  // Check if the debug marker extension is present (which is the case if run from a graphics debugger)
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, extensions.data());
  for (auto extension : extensions) {
    if (strcmp(extension.extensionName, VK_EXT_DEBUG_MARKER_EXTENSION_NAME) == 0) {
      extension_present = true;
      break;
    }
  }

  if (extension_present) {
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
void SetObjectName(VkDevice device,
                   uint64_t object,
                   VkDebugReportObjectTypeEXT object_type,
                   const char *name) {
  // Check for valid function pointer (may not be present if not running in a debugging application)
  if (!active)
    return;

  VkDebugMarkerObjectNameInfoEXT name_info = {};
  name_info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
  name_info.objectType = object_type;
  name_info.object = object;
  name_info.pObjectName = name;
  vkDebugMarkerSetObjectNameEXT(device, &name_info);
}

void SetObjectTag(VkDevice device,
                  uint64_t object,
                  VkDebugReportObjectTypeEXT object_type,
                  uint64_t name,
                  size_t tag_size,
                  const void *tag) {
  if (!active)
    return;

  VkDebugMarkerObjectTagInfoEXT tag_info = {};
  tag_info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT;
  tag_info.objectType = object_type;
  tag_info.object = object;
  tag_info.tagName = name;
  tag_info.tagSize = tag_size;
  tag_info.pTag = tag;
  vkDebugMarkerSetObjectTagEXT(device, &tag_info);
}

// Start a new debug marker region
void BeginRegion(VkCommandBuffer cmd_buffer, const char marker_name[], std::array<float, 4> color) {
    if (!active)
      return;

    VkDebugMarkerMarkerInfoEXT marker_info = {};
    marker_info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
    memcpy(marker_info.color, &color[0], sizeof(float) * 4);
    marker_info.pMarkerName = marker_name;
    vkCmdDebugMarkerBeginEXT(cmd_buffer, &marker_info);
}

void Insert(VkCommandBuffer cmd_buffer, const char marker_name[], std::array<float, 4> color) {
  if (!active)
    return;

  VkDebugMarkerMarkerInfoEXT marker_info = {};
  marker_info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
  memcpy(marker_info.color, &color[0], sizeof(float) * 4);
  marker_info.pMarkerName = marker_name;
  vkCmdDebugMarkerInsertEXT(cmd_buffer, &marker_info);
}

void EndRegion(VkCommandBuffer cmd_buffer) {
  // Check for valid function (may not be present if not running in a debugging application)
  if (vkCmdDebugMarkerEndEXT) {
    vkCmdDebugMarkerEndEXT(cmd_buffer);
  }
}
};