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

#include "BaseVulkanOperation.hpp"

namespace {
    constexpr Log::Tag TAG{"BaseVulkanOperation"};
}

namespace ancer {

static void DrawThread(BaseVulkanOperation *op) {
  Log::E(TAG, "draw thread started");
  Timestamp last_now = SteadyClock::now();
  while(!op->IsStopped()) {
    const auto now = SteadyClock::now();
    const auto delta0 = last_now - now;
    const auto delta1 = std::chrono::duration_cast<SecondsAs<double>>(delta0);
    //Log::E(TAG, "draw thread draw");
    op->Draw(delta1.count());
    std::this_thread::sleep_until(now + op->GetDrawPeriod());
  }
}

BaseVulkanOperation::BaseVulkanOperation(const Log::Tag testTag) :
  _vk{}, _testTag{testTag}, _vulkan_initialized(false) {
}

void BaseVulkanOperation::Start() {
  BaseOperation::Start();

  VulkanRequirements requirements;

  FillVulkanRequirements(requirements);

  if(!_vk.Initialize(requirements).is_success()) {
    Log::E(TAG, "Unable to initialize Vulkan");
    Stop();
    return;
  }

  _vulkan_initialized = true;

  _draw_thread = std::thread(DrawThread, this);
}

BaseVulkanOperation::~BaseVulkanOperation() {
  if (_vulkan_initialized) {
    _vk.Shutdown();

    _draw_thread.join();
  }
}

}
