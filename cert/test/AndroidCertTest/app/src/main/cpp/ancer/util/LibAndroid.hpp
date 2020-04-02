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

#include <android/trace.h>
#include <android/choreographer.h>
#include <android/hardware_buffer.h>
#include <android/hardware_buffer_jni.h>

namespace libandroid {
void *GetLib();

// -----------------------------------------------------------------------------
// ATrace

typedef void (*FP_ATRACE_BEGINSECTION)(const char* sectionName);
typedef void (*FP_ATRACE_ENDSECTION)(void);

// Returns a function pointer to ATrace_beginSection.
FP_ATRACE_BEGINSECTION GetFP_ATrace_beginSection();

// Returns a function pointer to ATrace_endSection.
FP_ATRACE_ENDSECTION GetFP_ATrace_endSection();

// -----------------------------------------------------------------------------
// AChoreographer

typedef AChoreographer* (*FP_ACHOREOGRAPHER_GET_INSTANCE)();
typedef void (*FP_AChoreographer_postFrameCallback)(AChoreographer* choreographer,
        AChoreographer_frameCallback callback, void* data);

// Returns a function pointer to AChoreographer_getInstance.
FP_ACHOREOGRAPHER_GET_INSTANCE GetFP_AChoreographer_getInstance();

// Returns a function pointer to AChoreographer_postFrameCallback.
FP_AChoreographer_postFrameCallback GetFP_AChoreographer_postFrameCallback();

// -----------------------------------------------------------------------------
// AHardwareBuffer

typedef int (*FP_AHB_ALLOCATE)(const AHardwareBuffer_Desc *,
                               AHardwareBuffer **);
typedef void (*FP_AHB_RELEASE)(AHardwareBuffer *);
typedef int (*FP_AHB_LOCK)(AHardwareBuffer *, uint64_t, int32_t, const ARect *,
                           void **);
typedef int (*FP_AHB_UNLOCK)(AHardwareBuffer *, int32_t *);

// Returns a function pointer to AHardwareBuffer_allocate.
FP_AHB_ALLOCATE GetFP_AHardwareBuffer_Allocate();

// Returns a function pointer to AHardwareBuffer_release.
FP_AHB_RELEASE GetFP_AHardwareBuffer_Release();

// Returns a function pointer to AHardwareBuffer_lock.
FP_AHB_LOCK GetFP_AHardwareBuffer_Lock();

// Returns a function pointer to AHardwareBuffer_unlock.
FP_AHB_UNLOCK GetFP_AHardwareBuffer_Unlock();
}  // namespace libandroid
