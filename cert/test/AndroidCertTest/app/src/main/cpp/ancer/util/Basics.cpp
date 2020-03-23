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
#include "Basics.hpp"

#include <random>

using namespace ancer;

//==============================================================================

void util_detail::ForceCompute(const void*) {}

//==============================================================================

namespace {
    template <typename Generator>
    void RandomizeImpl(void* memory, bitmath::Bytes size) {
        Generator random_generator{};
        using T = typename Generator::result_type;
        for (int i = 0; i < size.count(); i += sizeof(T)) {
            auto* ptr = reinterpret_cast<T*>((char*)memory + i);
            *ptr = static_cast<T>(random_generator());
        }
    }
}

void ancer::RandomizeMemory(void* memory, bitmath::Bytes size) {
    // TODO(tmillican@google.com): More variety in generator implementations.
    using Generator64 = std::mt19937_64;
    using Generator32 = std::mt19937;
    using Generator16 = std::mt19937;
    using Generator8 = std::mt19937;

    if (size % sizeof(Generator64::result_type)) {
        RandomizeImpl<Generator64>(memory, size);
    } else if (size % sizeof(Generator32::result_type)) {
        RandomizeImpl<Generator32>(memory, size);
    } else if (size % sizeof(Generator16::result_type)) {
        RandomizeImpl<Generator16>(memory, size);
    } else {
        RandomizeImpl<Generator8>(memory, size);
    }
}