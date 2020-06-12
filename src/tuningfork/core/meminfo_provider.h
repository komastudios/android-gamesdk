/*
 * Copyright 2020 The Android Open Source Project
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

#include <cstdint>

namespace tuningfork {

// Provider of system memory information.
class IMemInfoProvider {
   public:
    virtual uint64_t GetNativeHeapAllocatedSize() = 0;
    virtual void SetEnabled(bool enable) = 0;
    virtual bool GetEnabled() const = 0;
    virtual void SetDeviceMemoryBytes(uint64_t bytesize) = 0;
    virtual uint64_t GetDeviceMemoryBytes() const = 0;
};

}  // namespace tuningfork