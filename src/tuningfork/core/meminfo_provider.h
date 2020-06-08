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
    virtual void UpdateMemInfo() = 0;
    virtual uint64_t GetNativeHeapAllocatedSize() = 0;
    virtual void SetEnabled(bool enable) = 0;
    virtual bool GetEnabled() const = 0;
    virtual void SetDeviceMemoryBytes(uint64_t bytesize) = 0;
    virtual uint64_t GetDeviceMemoryBytes() const = 0;
    virtual uint64_t GetMeminfoOomScore() const = 0;
    virtual bool IsMeminfoActiveAvailable() const = 0;
    virtual bool IsMeminfoActiveAnonAvailable() const = 0;
    virtual bool IsMeminfoActiveFileAvailable() const = 0;
    virtual bool IsMeminfoAnonPagesAvailable() const = 0;
    virtual bool IsMeminfoCommitLimitAvailable() const = 0;
    virtual bool IsMeminfoHighTotalAvailable() const = 0;
    virtual bool IsMeminfoLowTotalAvailable() const = 0;
    virtual bool IsMeminfoMemAvailableAvailable() const = 0;
    virtual bool IsMeminfoMemFreeAvailable() const = 0;
    virtual bool IsMeminfoMemTotalAvailable() const = 0;
    virtual bool IsMeminfoVmDataAvailable() const = 0;
    virtual bool IsMeminfoVmRssAvailable() const = 0;
    virtual bool IsMeminfoVmSizeAvailable() const = 0;
    virtual uint64_t GetMeminfoActiveBytes() const = 0;
    virtual uint64_t GetMeminfoActiveAnonBytes() const = 0;
    virtual uint64_t GetMeminfoActiveFileBytes() const = 0;
    virtual uint64_t GetMeminfoAnonPagesBytes() const = 0;
    virtual uint64_t GetMeminfoCommitLimitBytes() const = 0;
    virtual uint64_t GetMeminfoHighTotalBytes() const = 0;
    virtual uint64_t GetMeminfoLowTotalBytes() const = 0;
    virtual uint64_t GetMeminfoMemAvailableBytes() const = 0;
    virtual uint64_t GetMeminfoMemFreeBytes() const = 0;
    virtual uint64_t GetMeminfoMemTotalBytes() const = 0;
    virtual uint64_t GetMeminfoVmDataBytes() const = 0;
    virtual uint64_t GetMeminfoVmRssBytes() const = 0;
    virtual uint64_t GetMeminfoVmSizeBytes() const = 0;
};

}  // namespace tuningfork
