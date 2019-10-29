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

#include "Time.hpp"

// Hiding away the JSON ugliness to avoid cluttering up the API.
#include <nlohmann/json_fwd.hpp>


namespace nlohmann {
    template <>
    struct adl_serializer<ancer::Timestamp> {
        static void to_json(json& j, const ancer::Timestamp& t);
    };

    template <>
    struct adl_serializer<ancer::Seconds> {
        static void to_json(json& j, const ancer::Seconds& t);
        static void from_json(const json& j, ancer::Seconds& t);
    };

    template <>
    struct adl_serializer<ancer::Milliseconds> {
        static void to_json(json& j, const ancer::Milliseconds& t);
        static void from_json(const json& j, ancer::Milliseconds& t);
    };

    template <>
    struct adl_serializer<ancer::Nanoseconds> {
        static void to_json(json& j, const ancer::Nanoseconds& t);
        static void from_json(const json& j, ancer::Nanoseconds& t);
    };
}
