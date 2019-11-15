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

#ifndef BENDER_BASE_UTILS_BENDER_KIT_BENDER_KIT_H_
#define BENDER_BASE_UTILS_BENDER_KIT_BENDER_KIT_H_

#include <android/log.h>
#include "device.h"

static const char *kTAG = "BenderKit";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

#define CALL_VK(func)                                                     \
    if (VK_SUCCESS != (func)) {                                           \
        __android_log_print(ANDROID_LOG_ERROR, "Bender ",                 \
                            "Vulkan error. File[%s], line[%d]", __FILE__, \
                            __LINE__);                                    \
        assert(false);                                                    \
    }

#endif //BENDER_BASE_BENDER_KIT_HPP
