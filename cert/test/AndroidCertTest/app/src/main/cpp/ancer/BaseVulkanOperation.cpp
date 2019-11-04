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
        _operation_info{}, _test_tag{testTag}
    {
        init_global_layer_properties(this->_operation_info);
        init_instance_extension_names(this->_operation_info);
        init_device_extension_names(this->_operation_info);
        Log::I(this->_test_tag, "Initializing Vulkan instance");
        init_instance(this->_operation_info, testTag.tag);
        init_enumerate_device(this->_operation_info);
    }

    BaseVulkanOperation::~BaseVulkanOperation() {
        if (this->_operation_info.device != VK_NULL_HANDLE) {
            Log::I(this->_test_tag, "Releasing Vulkan device");
            destroy_device(this->_operation_info);
        }
        Log::I(this->_test_tag, "Releasing Vulkan instance");
        destroy_instance(this->_operation_info);
    }

    vulkan_info& BaseVulkanOperation::GetInfo() {
        return this->_operation_info;
    }

    const VkDevice& BaseVulkanOperation::GetDevice() {
        if (this->_operation_info.device == VK_NULL_HANDLE) {
            Log::I(this->_test_tag, "Initializing Vulkan device");
            init_device(this->_operation_info);
        }

        return this->_operation_info.device;
    }
}
