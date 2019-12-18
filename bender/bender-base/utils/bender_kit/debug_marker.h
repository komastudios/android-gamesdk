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

#ifndef BENDER_BASE_UTILS_BENDER_KIT_DEBUG_MARKER_H_
#define BENDER_BASE_UTILS_BENDER_KIT_DEBUG_MARKER_H_

#include "vulkan_wrapper.h"

#include <string>
#include <array>

namespace debugmarker {
  void Setup(VkDevice device, VkPhysicalDevice physical_device);

  void SetObjectName(VkDevice device,
                     uint64_t object,
                     VkDebugReportObjectTypeEXT object_type,
                     const char *name);

  void SetObjectTag(VkDevice device,
                    uint64_t object,
                    VkDebugReportObjectTypeEXT object_type,
                    uint64_t name,
                    size_t tag_size,
                    const void *tag);

  void BeginRegion(VkCommandBuffer cmd_buffer, const char marker_name[], std::array<float, 4> color = {1.0f, 1.0f, 1.0f, 1.0f });
  void Insert(VkCommandBuffer cmd_buffer, const char marker_name[], std::array<float, 4> color = {1.0f, 1.0f, 1.0f, 1.0f });
  void EndRegion(VkCommandBuffer cmd_buffer);
}

#endif