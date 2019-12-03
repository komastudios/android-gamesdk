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
    BaseVulkanOperation::BaseVulkanOperation(const Log::Tag testTag) :
            _operationInfo{}, _testTag{testTag}, _vulkan_initialized(false) {

        if (InitGlobalLayerProperties(this->_operationInfo) != VK_SUCCESS) {
            Log::E(TAG, "Unable to initialize Vulkan");
            Stop();
            return;
        }

        InitInstanceExtensionNames(this->_operationInfo);
        InitDeviceExtensionNames(this->_operationInfo);
        Log::D(this->_testTag, "Initializing Vulkan instance");
        if (InitInstance(this->_operationInfo, testTag.tag) != VK_SUCCESS){
            Log::E(TAG, "Unable to initialize Vulkan instance");
            Stop();
            return;
        }

        if (InitEnumerateDevice(this->_operationInfo) != VK_SUCCESS) {
            Log::E(TAG, "Unable to enumerate Vulkan devices");
            Stop();
            return;
        }

        _vulkan_initialized = true;
    }

    BaseVulkanOperation::~BaseVulkanOperation() {
        if (_vulkan_initialized) {
          if (this->_operationInfo.device != VK_NULL_HANDLE) {
            Log::D(this->_testTag, "Releasing Vulkan device");
            DestroyDevice(this->_operationInfo);
          }
          Log::D(this->_testTag, "Releasing Vulkan instance");
          DestroyInstance(this->_operationInfo);
        }
    }

    VulkanInfo &BaseVulkanOperation::GetInfo() {
        return this->_operationInfo;
    }

    const VkDevice &BaseVulkanOperation::GetDevice() {
        if (this->_operationInfo.device == VK_NULL_HANDLE) {
            Log::D(this->_testTag, "Initializing Vulkan device");
            InitDevice(this->_operationInfo);
        }

        return this->_operationInfo.device;
    }
}
