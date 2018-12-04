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

#include <sched.h>

#include <vector>
#include <map>
#include <string>


class Cpu {
public:
    struct Core {
        enum class Type {
            Little,
            Big
        };

        int id;
        int package_id;
        long frequency;

        Type type;
    };

    Cpu();

    unsigned int getNumberOfCores() const;

    const std::vector<Core>& cores() const;
    const std::string hardware() const;

    cpu_set_t littleCoresMask() const;
    cpu_set_t bigCoresMask() const;

private:
    std::vector<Core> mCores;
    std::string mHardware;

    long mMaxFrequency;
    long mMinFrequency;

    cpu_set_t mLittleCoresMask;
    cpu_set_t mBigCoresMask;
};

unsigned int to_mask(cpu_set_t cpu_set);