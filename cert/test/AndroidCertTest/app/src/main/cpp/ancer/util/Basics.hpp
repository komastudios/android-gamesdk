/*
 * Copyright 2019 The Android Open Source Project
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

#include "Bitmath.hpp"


namespace ancer {
    // Forces the compiler to make sure the given variables' states are calculated at a given point.
    // For CPU stress operations where the compiler can tell that the result is actually ignored.
    template <typename... Args>
    void ForceCompute(Args&&... args);


    template<typename T>
    [[nodiscard]] constexpr auto NextAlignedValue(T value, T align);


    void RandomizeMemory(void* mem, bitmath::Bytes mem_sz);
}

#include "Basics.inl"
