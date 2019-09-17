/*
 * Copyright 2018 The Android Open Source Project
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

/** @file
 * OpenGL part of Swappy
 */

#pragma once

#include "swappy_common.h"

#include <stdint.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

// Internal init function. Do not call directly.
bool SwappyGL_init_internal(JNIEnv *env, jobject jactivity);

/**
 * @brief Initialize Swappy, getting the required Android parameters from the display subsystem via JNI.
 * @param env The JNI environment where Swappy is used
 * @param jactivity The activity where Swappy is used
 * @return false if Swappy failed to initialize.
 * @see SwappyGL_destroy
 */
static inline bool SwappyGL_init(JNIEnv *env, jobject jactivity)  {
    // This call ensures that the header and the linked library are from the same version
    // (if not, a linker error will be triggered because of an undefined symbolP).
    SWAPPY_VERSION_SYMBOL();
    return SwappyGL_init_internal(env, jactivity);
}

/**
 * @brief Check if Swappy was successfully initialized.
 * @return false if either the 'swappy.disable' system property is not 'false'
 * or the required OpenGL extensions are not available for Swappy to work.
 */
bool SwappyGL_isEnabled();

/**
 * @brief Destroy resources and stop all threads that Swappy has created.
 * @see SwappyGL_init
 */
void SwappyGL_destroy();

/**
 * @brief Replace calls to eglSwapBuffers with this. Swappy will wait for the previous frame's
 * buffer to be processed by the GPU before actually calling eglSwapBuffers.
 */
bool SwappyGL_swap(EGLDisplay display, EGLSurface surface);

// Parameter setters
void SwappyGL_setUseAffinity(bool tf);
void SwappyGL_setSwapIntervalNS(uint64_t swap_ns);
void SwappyGL_setFenceTimeoutNS(uint64_t fence_timeout_ns);

// Parameter getters
uint64_t SwappyGL_getRefreshPeriodNanos();
uint64_t SwappyGL_getSwapIntervalNS();
bool SwappyGL_getUseAffinity();
uint64_t SwappyGL_getFenceTimeoutNS();

#ifdef __cplusplus
};
#endif
