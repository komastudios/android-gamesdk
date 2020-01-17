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

#include <android/hardware_buffer.h>
#include <android/hardware_buffer_jni.h>

#include "LibLoader.hpp"

namespace {
void *GetAndroidLibrary() { return LoadLibrary("libandroid.so"); }

typedef int (*PFN_AHB_ALLOCATE)(const AHardwareBuffer_Desc *,
                                AHardwareBuffer **);
typedef void (*PFN_AHB_RELEASE)(AHardwareBuffer *);
typedef int (*PFN_AHB_LOCK)(AHardwareBuffer *, uint64_t, int32_t, const ARect *,
                            void **);
typedef int (*PFN_AHB_UNLOCK)(AHardwareBuffer *, int32_t *);

PFN_AHB_ALLOCATE GetPFN_AHardwareBuffer_Allocate() {
  return reinterpret_cast<PFN_AHB_ALLOCATE>(
      LoadSymbol(GetAndroidLibrary(), "AHardwareBuffer_allocate"));
}

PFN_AHB_RELEASE GetPFN_AHardwareBuffer_Release() {
  return reinterpret_cast<PFN_AHB_RELEASE>(
      LoadSymbol(GetAndroidLibrary(), "AHardwareBuffer_release"));
}

PFN_AHB_LOCK GetPFN_AHardwareBuffer_Lock() {
  return reinterpret_cast<PFN_AHB_LOCK>(
      LoadSymbol(GetAndroidLibrary(), "AHardwareBuffer_lock"));
}

PFN_AHB_UNLOCK GetPFN_AHardwareBuffer_Unlock() {
  return reinterpret_cast<PFN_AHB_UNLOCK>(
      LoadSymbol(GetAndroidLibrary(), "AHardwareBuffer_unlock"));
}
}
