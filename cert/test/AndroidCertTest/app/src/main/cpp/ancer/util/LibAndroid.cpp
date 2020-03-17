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

#include "LibAndroid.hpp"

#include "LibLoader.hpp"

namespace libandroid {
void *GetLib() { return LoadLibrary("libandroid.so"); }

PFN_AHB_ALLOCATE PfnAHardwareBuffer_Allocate() {
  return reinterpret_cast<PFN_AHB_ALLOCATE>(
      LoadSymbol(GetLib(), "AHardwareBuffer_allocate"));
}

PFN_AHB_RELEASE PfnAHardwareBuffer_Release() {
  return reinterpret_cast<PFN_AHB_RELEASE>(
      LoadSymbol(GetLib(), "AHardwareBuffer_release"));
}

PFN_AHB_LOCK PfnAHardwareBuffer_Lock() {
  return reinterpret_cast<PFN_AHB_LOCK>(
      LoadSymbol(GetLib(), "AHardwareBuffer_lock"));
}

PFN_AHB_UNLOCK PfnAHardwareBuffer_Unlock() {
  return reinterpret_cast<PFN_AHB_UNLOCK>(
      LoadSymbol(GetLib(), "AHardwareBuffer_unlock"));
}
}  // namespace libandroid
