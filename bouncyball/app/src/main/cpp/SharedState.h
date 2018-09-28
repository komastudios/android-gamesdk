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

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <vector>

#include "swappy-utils/Thread.h"
#include "Circle.h"

class SharedState {
  public:
    struct Frame {
        int64_t number;
        std::vector<Circle> circles;
    };

    void update(int64_t frameNumber, std::vector<Circle> circles);
    Frame getLatest() const;

  private:
    mutable std::mutex mMutex;
    mutable bool mNewDataPending GUARDED_BY(mMutex) = false;
    int64_t mFrameNumber GUARDED_BY(mMutex) = 0;
    std::vector<Circle> mCircles GUARDED_BY(mMutex);
    mutable std::condition_variable_any mCondition;
};
