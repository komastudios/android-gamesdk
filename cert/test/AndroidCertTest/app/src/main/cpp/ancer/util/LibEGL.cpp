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

#include "LibEGL.hpp"

#include "LibLoader.hpp"

namespace libegl {
void *GetLib() { return LoadLibrary("libEGL.so"); }

PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC PfnGetNativeClientBuffer() {
  return reinterpret_cast<PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC>(
      LoadSymbol(GetLib(), "eglGetNativeClientBufferANDROID"));
}

// -----------------------------------------------------------------------------

PFNEGLPRESENTATIONTIMEANDROIDPROC PfnPresentationTime() {
  return reinterpret_cast<PFNEGLPRESENTATIONTIMEANDROIDPROC>(
      LoadSymbol(GetLib(), "eglPresentationTimeANDROID"));
}

// -----------------------------------------------------------------------------

PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC PfnGetCompositorTimingSupported() {
  return reinterpret_cast<PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC>(
      LoadSymbol(GetLib(), "eglGetCompositorTimingSupportedANDROID"));
}

PFNEGLGETCOMPOSITORTIMINGANDROIDPROC PfnGetCompositorTiming() {
  return reinterpret_cast<PFNEGLGETCOMPOSITORTIMINGANDROIDPROC>(
      LoadSymbol(GetLib(), "eglGetCompositorTimingANDROID"));
}

PFNEGLGETNEXTFRAMEIDANDROIDPROC PfnGetNextFrameId() {
  return reinterpret_cast<PFNEGLGETNEXTFRAMEIDANDROIDPROC>(
      LoadSymbol(GetLib(), "eglGetNextFrameIdANDROID"));
}

PFNEGLGETFRAMETIMESTAMPSUPPORTEDANDROIDPROC PfnGetFrameTimestampSupported() {
  return reinterpret_cast<PFNEGLGETFRAMETIMESTAMPSUPPORTEDANDROIDPROC>(
      LoadSymbol(GetLib(), "eglGetFrameTimestampSupportedANDROID"));
}

PFNEGLGETFRAMETIMESTAMPSANDROIDPROC PfnGetFrameTimestamps() {
  return reinterpret_cast<PFNEGLGETFRAMETIMESTAMPSANDROIDPROC>(
      LoadSymbol(GetLib(), "eglGetFrameTimestampsANDROID"));
}

}  // namespace libegl
