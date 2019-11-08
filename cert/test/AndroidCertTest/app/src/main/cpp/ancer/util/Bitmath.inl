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

#include <nlohmann/json_fwd.hpp>


namespace ancer::bitmath {
    template <typename Rep, typename Period>
    template <typename Rep2, typename Period2>
    constexpr Size<Rep, Period>::Size(const Size<Rep2, Period2> &rhs)
    : Size(BitmathCast<Size<Rep, Period>>(rhs)) {}
}

namespace nlohmann {
    template <>
    struct adl_serializer<ancer::bitmath::Bits> {
        static void to_json(json& j, const ancer::bitmath::Bits& t);
        static void from_json(const json& j, ancer::bitmath::Bits& t);
    };

    template <>
    struct adl_serializer<ancer::bitmath::Bytes> {
        static void to_json(json& j, const ancer::bitmath::Bytes& t);
        static void from_json(const json& j, ancer::bitmath::Bytes& t);
    };

    template <>
    struct adl_serializer<ancer::bitmath::Kilobytes> {
        static void to_json(json& j, const ancer::bitmath::Kilobytes& t);
        static void from_json(const json& j, ancer::bitmath::Kilobytes& t);
    };

    template <>
    struct adl_serializer<ancer::bitmath::Megabytes> {
        static void to_json(json& j, const ancer::bitmath::Megabytes& t);
        static void from_json(const json& j, ancer::bitmath::Megabytes& t);
    };

    template <>
    struct adl_serializer<ancer::bitmath::Gigabytes> {
        static void to_json(json& j, const ancer::bitmath::Gigabytes& t);
        static void from_json(const json& j, ancer::bitmath::Gigabytes& t);
    };
}
