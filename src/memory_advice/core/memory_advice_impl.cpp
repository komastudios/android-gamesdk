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

#include "memory_advice_impl.h"

namespace memory_advice {

MemoryAdviceImpl::MemoryAdviceImpl() {
    metrics_provider_ = std::make_unique<MetricsProvider>();
    device_profiler_ = std::make_unique<DeviceProfiler>();

    initialization_error_code_ = device_profiler_->Init();
}

}  // namespace memory_advice
