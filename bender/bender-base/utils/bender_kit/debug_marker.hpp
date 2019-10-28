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

#ifndef DEBUG_MARKER
#define DEBUG_MARKER

#include "vulkan_wrapper.h"
#include <string>

namespace DebugMarker {
void setup(VkDevice device, VkPhysicalDevice physicalDevice);
void setObjectName(VkDevice device,
                   uint64_t object,
                   VkDebugReportObjectTypeEXT objectType,
                   const char *name);
void setObjectTag(VkDevice device,
                  uint64_t object,
                  VkDebugReportObjectTypeEXT objectType,
                  uint64_t name,
                  size_t tagSize,
                  const void *tag);
void beginRegion(VkCommandBuffer cmdbuffer, const char *pMarkerName, float color[4]);
void insert(VkCommandBuffer cmdbuffer, std::string markerName, float color[4]);
void endRegion(VkCommandBuffer cmdBuffer);
}

#endif