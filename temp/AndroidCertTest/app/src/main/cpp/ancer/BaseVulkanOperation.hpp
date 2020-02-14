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

#pragma once

#include "ancer/vulkan/vulkan_base.h"

#include "BaseOperation.hpp"

using namespace ancer;
using namespace ancer::vulkan;

namespace ancer {

/**
 * BaseVulkanOperation
 * Base class for operations which intend to render Vulkan output
 */
class BaseVulkanOperation : public BaseOperation {
 public:
  BaseVulkanOperation(const Log::Tag testTag);

  virtual ~BaseVulkanOperation();

  void Wait() override {}

 protected:
  /**
   * Returns true iff Vulkan was successfully initialized
   */
  bool IsVulkanAvailable() const { return _vulkan_initialized; }

  /**
   * This is protected so operations can access the vulkan functions
   */
  Vulkan _vk;

  /**
   * Allow an operation to specify requirements (extensions, physical device
   * features)
   */
  virtual void FillVulkanRequirements(VulkanRequirements & requirements) { }

 private:
  bool _vulkan_initialized = false;
  const Log::Tag _testTag;
};

}
