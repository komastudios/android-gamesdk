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

#pragma once

#ifdef ANDROID
#define SWAPPYVK_USE_WRAPPER
#endif
#include <swappyVk/SwappyVk.h>

#include <map>
#include <condition_variable>
#include <cstring>
#include <unistd.h>

#include <dlfcn.h>
#include <cstdlib>

#include <inttypes.h>

#ifdef ANDROID
#include <mutex>
#include <pthread.h>
#include <list>
#include <android/looper.h>
#include <android/log.h>
#include "Trace.h"
#include "ChoreographerShim.h"

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, "SwappyVk", __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, "SwappyVk", __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, "SwappyVk", __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "SwappyVk", __VA_ARGS__)
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "SwappyVk", __VA_ARGS__)
#else
#define ATRACE_CALL() ((void)0)
#define ALOGE(...)    ((void)0)
#define ALOGW(...)    ((void)0)
#define ALOGD(...)    ((void)0)
#define ALOGV(...)    ((void)0)
#endif


constexpr uint32_t kThousand = 1000;
constexpr uint32_t kMillion  = 1000000;
constexpr uint32_t kBillion  = 1000000000;
constexpr uint32_t k16_6msec = 16666666;

constexpr uint32_t kTooCloseToVsyncBoundary     = 3000000;
constexpr uint32_t kTooFarAwayFromVsyncBoundary = 7000000;
constexpr uint32_t kNudgeWithinVsyncBoundaries  = 2000000;

// Note: The API functions is at the botton of the file.  Those functions call methods of the
// singleton SwappyVk class.  Those methods call virtual methods of the abstract SwappyVkBase
// class, which is actually implemented by one of the derived/concrete classes:
//
// - SwappyVkGoogleDisplayTiming
// - SwappyVkVulkanFallback
// - SwappyVkAndroidFallback

// Forward declarations:
class SwappyVk;

// AChoreographer is supported from API 24. To allow compilation for minSDK < 24
// and still use AChoreographer for SDK >= 24 we need runtime support to call
// AChoreographer APIs.

using PFN_AChoreographer_getInstance = AChoreographer* (*)();

using PFN_AChoreographer_postFrameCallback = void (*)(AChoreographer* choreographer,
                                                  AChoreographer_frameCallback callback,
                                                  void* data);

using PFN_AChoreographer_postFrameCallbackDelayed = void (*)(AChoreographer* choreographer,
                                                        AChoreographer_frameCallback callback,
                                                        void* data,
                                                        long delayMillis);

// Forward declarations:
class SwappyVk;

#include "SwappyVkBase.h"
#include "SwappyVkGoogleDisplayTiming.h"
#include "SwappyVkGoogleDisplayTimingAndroid.h"
#include "SwappyVkAndroidFallback.h"
#include "SwappyVkVulkanFallback.h"