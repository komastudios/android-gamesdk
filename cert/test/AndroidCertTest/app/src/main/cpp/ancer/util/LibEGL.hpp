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

#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace libegl {
void *GetLib();

typedef PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC FP_GET_NATIVE_CLIENT_BUFFER;

FP_GET_NATIVE_CLIENT_BUFFER GetFP_GetNativeClientBuffer();

// typedef PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC
//     FP_GET_COMPOSITOR_TIMING_SUPPORTED;
// typedef PFNEGLGETCOMPOSITORTIMINGANDROIDPROC FP_GET_COMPOSITOR_TIMING;
typedef PFNEGLGETFRAMETIMESTAMPSUPPORTEDANDROIDPROC
    FP_GET_FRAME_TIMESTAMP_SUPPORTED;
typedef PFNEGLGETFRAMETIMESTAMPSANDROIDPROC FP_GET_FRAME_TIMESTAMPS;
typedef PFNEGLGETNEXTFRAMEIDANDROIDPROC FP_GET_NEXT_FRAME_ID;

// FP_GET_COMPOSITOR_TIMING_SUPPORTED GetFP_GetCompositorTimingSupported() {
//   return reinterpret_cast<FP_GET_COMPOSITOR_TIMING_SUPPORTED>(
//       LoadSymbol(GetLib(), "eglGetCompositorTimingSupportedANDROID"));
// }

// FP_GET_COMPOSITOR_TIMING GetFP_GetCompositorTiming() {
//   return reinterpret_cast<FP_GET_COMPOSITOR_TIMING>(
//       LoadSymbol(GetLib(), "eglGetCompositorTimingANDROID"));
// }

FP_GET_FRAME_TIMESTAMP_SUPPORTED GetFP_GetFrameTimestampSupported();

FP_GET_FRAME_TIMESTAMPS GetFP_GetFrameTimestamps();

FP_GET_NEXT_FRAME_ID GetFP_GetNextFrameId();

}  // namespace libegl
