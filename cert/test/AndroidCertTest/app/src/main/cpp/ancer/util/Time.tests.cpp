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

#include "Time.hpp"
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

using namespace ancer;


TEST(ancer_time, json_read) {
    auto j = R"({
        "a" : "500ms",
        "b" : "0.5 seconds"
    })"_json;

    Milliseconds ms;
    j["a"].get_to(ms);
    EXPECT_EQ(ms, 500ms);
    j["b"].get_to(ms);
    EXPECT_EQ(ms, 500ms);
}

TEST(ancer_time, json_write) {
    auto j = nlohmann::json{};
    j["a"] = 500ms;
    j["b"] = 5s;

    EXPECT_EQ(j["a"].get<Milliseconds>(), 500ms);
    EXPECT_EQ(j["b"].get<Seconds>(), 5s);
}
