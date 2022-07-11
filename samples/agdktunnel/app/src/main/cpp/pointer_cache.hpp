/*
 * Copyright 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef agdktunnel_pointer_cache_hpp
#define agdktunnel_pointer_cache_hpp

#include "common.hpp"

#define INVALID_POINTER_ID -1
#define OLDEST_POINTER_INDEX 0

enum PointerState {
    POINTER_STATE_INACTIVE = 0,
    POINTER_STATE_DOWN,
    POINTER_STATE_MOVE,
    POINTER_STATE_UP
};

struct PointerCacheEntry {
    int32_t pointerId;
    // order pointers were touched, with 0 being the first (oldest) touch
    // and the highest number being the most recent (newest)
    uint32_t pointerOrder;
    int32_t pointerSource;
    PointerState pointerState;
    float lastMotionX;
    float lastMotionY;
};

class PointerCache {
public:
    PointerCache();

    void ResetPointerCache();

    void PointerEvent(const uint32_t pointerId, const int32_t pointerSource,
                      const float motionX, const float motionY, const PointerState pointerState);

    // Release any pointers in an 'up' state
    void ReleaseUpPointers();

    uint32_t GetActivePointerCount() const { return mActivePointerCount; }

    // Returns true if the oldest original pointer from a sequence of pointer down events
    // is still active. This is used to filter for single-touch behavior. Once this returns false
    // it will not return true again until all active pointers have gone 'up' and a new
    // 'down' pointer becomes a new original touch.
    bool IsOldestOriginalPointerStillActive() const { return mOldestActive; };

    // pointerIndex must be between 0 and GetActivePointerCount() - 1
    // 0 is the oldest active pointer, GetActivePointerCount() - 1 is the newest active pointer
    const PointerCacheEntry &GetPointerCacheEntry(const uint32_t pointerIndex);
    int32_t GetPointerId(const uint32_t pointerIndex);
    float GetPointerMotionX(const uint32_t pointerIndex);
    float GetPointerMotionY(const uint32_t pointerIndex);
    int32_t GetPointerSource(const uint32_t pointerIndex);
    PointerState GetPointerState(const uint32_t pointerIndex);

private:
    void PointerDown(const int32_t pointerId, const int32_t pointerSource,
                     const float motionX, const float motionY);
    void PointerMove(const int32_t pointerId, const int32_t pointerSource,
                     const float motionX, const float motionY);
    void PointerUp(const int32_t pointerId, const int32_t pointerSource,
                   const float motionX, const float motionY);

    uint32_t GetIndexForId(const int32_t pointerId);
    uint32_t GetFreeIndex();

    uint32_t mActivePointerCount;
    PointerCacheEntry mPointers[GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT];
    bool mOldestActive;
};

#endif // agdktunnel_pointer_cache_hpp
