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

#include "pointer_cache.hpp"

PointerCache::PointerCache() {
    ResetPointerCache();
}

void PointerCache::ResetPointerCache() {
    mActivePointerCount = 0;
    mOldestActive = false;
    for (size_t i = 0; i < GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT; ++i) {
        mPointers[i].pointerId = INVALID_POINTER_ID;
        mPointers[i].pointerOrder = GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT;
        mPointers[i].pointerSource = 0;
        mPointers[i].pointerState = POINTER_STATE_INACTIVE;
        mPointers[i].lastMotionX = 0.0f;
        mPointers[i].lastMotionY = 0.f;
    }
}

void PointerCache::PointerEvent(const uint32_t pointerId, const int32_t pointerSource,
                  const float motionX, const float motionY, const PointerState pointerState) {
    if (pointerState == POINTER_STATE_DOWN) {
        PointerDown(pointerId, pointerSource, motionX, motionY);
    } else if (pointerState == POINTER_STATE_UP) {
        PointerUp(pointerId, pointerSource, motionX, motionY);
    } else if (pointerState == POINTER_STATE_MOVE) {
        PointerMove(pointerId, pointerSource, motionX, motionY);
    }
}

void PointerCache::PointerDown(const int32_t pointerId, const int32_t pointerSource,
                               const float motionX, const float motionY) {
    const uint32_t checkIndex = GetIndexForId(pointerId);
    if (checkIndex == GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT) {
        const uint32_t freeIndex = GetFreeIndex();
        if (freeIndex != GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT) {
            mPointers[freeIndex].pointerId = pointerId;
            mPointers[freeIndex].pointerOrder = mActivePointerCount;
            mPointers[freeIndex].pointerSource = pointerSource;
            mPointers[freeIndex].pointerState = POINTER_STATE_DOWN;
            mPointers[freeIndex].lastMotionX = motionX;
            mPointers[freeIndex].lastMotionY = motionY;
            if (mActivePointerCount == 0) {
                // First down, mark the oldest active, which is now this pointer
                mOldestActive = true;
            }
            mActivePointerCount += 1;
        }
    } else {
        ALOGE("PointerDown, but pointerId %d is already in the cache!", pointerId);
    }
}

void PointerCache::PointerMove(const int32_t pointerId, const int32_t pointerSource,
                               const float motionX, const float motionY) {
    const uint32_t i = GetIndexForId(pointerId);
    if (i != GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT) {
        mPointers[i].lastMotionX = motionX;
        mPointers[i].lastMotionY = motionY;
        mPointers[i].pointerSource = pointerSource;
        mPointers[i].pointerState = POINTER_STATE_MOVE;
    } else {
        ALOGE("PointerMove, pointerId %d was not found in the cache!", pointerId);
    }
}

void PointerCache::PointerUp(const int32_t pointerId, const int32_t pointerSource,
                             const float motionX, const float motionY) {
    const uint32_t i = GetIndexForId(pointerId);
    if (i != GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT) {
        mPointers[i].lastMotionX = motionX;
        mPointers[i].lastMotionY = motionY;
        mPointers[i].pointerSource = pointerSource;
        mPointers[i].pointerState = POINTER_STATE_UP;
    } else {
        ALOGE("PointerMove, pointerId %d was not found in the cache!", pointerId);
    }
}

void PointerCache::ReleaseUpPointers() {
    for (uint32_t i = 0; i < GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT; ++i) {
        if (mPointers[i].pointerId != INVALID_POINTER_ID) {
            if (mPointers[i].pointerState == POINTER_STATE_UP) {
                const uint32_t oldOrder = mPointers[i].pointerOrder;
                if (oldOrder == 0) {
                    // The oldest active pointer was released, set this to false
                    // until we got a new down once all the other pointers have
                    // gone up
                    mOldestActive = false;
                }
                mPointers[i].pointerId = INVALID_POINTER_ID;
                mPointers[i].pointerOrder = GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT;
                mPointers[i].pointerSource = 0;
                mPointers[i].pointerState = POINTER_STATE_INACTIVE;
                mPointers[i].lastMotionX = 0.0f;
                mPointers[i].lastMotionY = 0.f;
                mActivePointerCount -= 1;

                // Decrement order of any higher order active pointers to fill the hole left
                // by the departing pointer
                for (uint32_t j = 0; j < GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT; ++j) {
                    if (mPointers[j].pointerId != INVALID_POINTER_ID) {
                        if (mPointers[j].pointerOrder > oldOrder) {
                            mPointers[j].pointerOrder -= 1;
                        }
                    }
                }
            }
        }
    }
}
const PointerCacheEntry &PointerCache::GetPointerCacheEntry(const uint32_t pointerIndex) {
    for (uint32_t i = 0; i < GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT; ++i) {
        if (mPointers[i].pointerId != INVALID_POINTER_ID) {
            if (mPointers[i].pointerOrder == pointerIndex) {
                return mPointers[i];
            }
        }
    }
    return mPointers[0];
}

int32_t PointerCache::GetPointerId(const uint32_t pointerIndex) {
    for (uint32_t i = 0; i < GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT; ++i) {
        if (mPointers[i].pointerId != INVALID_POINTER_ID) {
            if (mPointers[i].pointerOrder == pointerIndex) {
                return mPointers[i].pointerId;
            }
        }
    }
    return INVALID_POINTER_ID;
}

float PointerCache::GetPointerMotionX(const uint32_t pointerIndex) {
    for (uint32_t i = 0; i < GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT; ++i) {
        if (mPointers[i].pointerId != INVALID_POINTER_ID) {
            if (mPointers[i].pointerOrder == pointerIndex) {
                return mPointers[i].lastMotionX;
            }
        }
    }
    return 0.0f;
}

float PointerCache::GetPointerMotionY(const uint32_t pointerIndex) {
    for (uint32_t i = 0; i < GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT; ++i) {
        if (mPointers[i].pointerId != INVALID_POINTER_ID) {
            if (mPointers[i].pointerOrder == pointerIndex) {
                return mPointers[i].lastMotionY;
            }
        }
    }
    return 0.0f;
}

int32_t PointerCache::GetPointerSource(const uint32_t pointerIndex) {
    for (uint32_t i = 0; i < GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT; ++i) {
        if (mPointers[i].pointerId != INVALID_POINTER_ID) {
            if (mPointers[i].pointerOrder == pointerIndex) {
                return mPointers[i].pointerSource;
            }
        }
    }
    return 0;
}

PointerState PointerCache::GetPointerState(const uint32_t pointerIndex) {
    for (uint32_t i = 0; i < GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT; ++i) {
        if (mPointers[i].pointerId != INVALID_POINTER_ID) {
            if (mPointers[i].pointerOrder == pointerIndex) {
                return mPointers[i].pointerState;
            }
        }
    }
    return POINTER_STATE_INACTIVE;
}

uint32_t PointerCache::GetIndexForId(const int32_t pointerId) {
    // Default to no match
    uint32_t index = GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT;
    for (uint32_t i = 0; i < index; ++i) {
        if (mPointers[i].pointerId == pointerId) {
            index = i;
            break;
        }
    }
    return index;
}

uint32_t PointerCache::GetFreeIndex() {
    // Default to no free index
    uint32_t index = GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT;

    for (uint32_t i = 0; i < index; ++i) {
        if (mPointers[i].pointerId == INVALID_POINTER_ID) {
            index = i;
            break;
        }
    }
    return index;
}
