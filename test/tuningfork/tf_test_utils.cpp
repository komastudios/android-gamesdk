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

#include "tf_test_utils.h"

namespace test {

bool CompareIgnoringWhitespace(std::string s0, std::string s1) {
    // Ignore all whitespace, except when in strings
    bool in_string = false;
    auto a = s0.begin();
    auto b = s1.begin();
    while(true) {
        if (!in_string) {
            while( a!=s0.end() && isspace(*a)) a++;
            while( b!=s1.end() && isspace(*b)) b++;
        }
        if (a==s0.end()) break;
        if (b==s1.end()) return false;
        if (*a!=*b) return false;
        if (*a=='"')
            in_string = !in_string;
        ++a;
        ++b;
    }
    return b==s1.end();
}

}
