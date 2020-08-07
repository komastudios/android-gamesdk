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

// Wind forward over any arrays ... [...] ...
template<typename Iterator>
bool WindForwardOverArrays(Iterator& i, Iterator end) {
    int level = 0;
    if (*i!='[') return false;
    for(;i!=end;++i) {
        if (*i=='[') ++level;
        if (*i==']') {
            --level;
            if (level==0) return true;
        }
    }
    return false;
}

bool CompareIgnoringWhitespace(std::string s0, std::string s1, bool ignoring_starred_arrays) {
    // Ignore all whitespace, except when in strings.
    // '[**]' is a wildcard for any array if ignoring_starred_arrays is true.
    bool in_string = false;
    bool in_wildcard = false;
    auto a = s0.begin();
    auto b = s1.begin();
    while (true) {
        if (!in_string) {
            while (a != s0.end() && isspace(*a)) a++;
            while (b != s1.end() && isspace(*b)) b++;
        }
        if (a == s0.end()) break;
        if (b == s1.end()) return false;
        if (ignoring_starred_arrays) {
            if ((s1.end() - b)>=4 && std::string(b,b+4) == "[**]") {
                if (!WindForwardOverArrays(a, s0.end())) break;
                b += 3;
                continue;
            }
        }
        if (*a != *b) return false;
        if (*a == '"') in_string = !in_string;
        ++a;
        ++b;
    }
    return b == s1.end();
}

}  // namespace test
