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

// Returns function pointer tp eglGetNativeClientBufferANDROID().
PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC PfnGetNativeClientBuffer();

// Returns function pointer to eglPresentationTimeANDROID().
PFNEGLPRESENTATIONTIMEANDROIDPROC PfnPresentationTime();

// Returns function pointer to eglGetCompositorTimingSupportedANDROID().
PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC
PfnGetCompositorTimingSupported();

// Returns function pointer to eglGetCompositorTimingANDROID().
PFNEGLGETCOMPOSITORTIMINGANDROIDPROC PfnGetCompositorTiming();

// Returns function pointer to eglGetNextFrameIdANDROID().
PFNEGLGETNEXTFRAMEIDANDROIDPROC PfnGetNextFrameId();

// Returns function pointer to eglGetFrameTimestampSupportedANDROID().
PFNEGLGETFRAMETIMESTAMPSUPPORTEDANDROIDPROC PfnGetFrameTimestampSupported();

// Returns function pointer to eglGetFrameTimestampsANDROID().
PFNEGLGETFRAMETIMESTAMPSANDROIDPROC PfnGetFrameTimestamps();

}  // namespace libegl
