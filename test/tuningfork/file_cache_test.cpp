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

#include <string>
#include <gtest/gtest.h>
#include "tuningfork/file_cache.h"
#include "tuningfork/tuningfork_utils.h"
#include "tuningfork/protobuf_util.h"


namespace test {

using namespace tuningfork;

constexpr char kBasePath[] = "/data/local/tmp/tuningfork_file_test";

class FileCacheTest {
    FileCache cache_;
  public:
    FileCacheTest() : cache_(kBasePath) {
        EXPECT_EQ(cache_.Clear(), TFERROR_OK);
    }
    void Save(uint64_t key, const ProtobufSerialization& value) {
        CProtobufSerialization cvalue;
        ToCProtobufSerialization(value, cvalue);
        cache_.Set(key, &cvalue);
        CProtobufSerialization_Free(&cvalue);
    }
    ProtobufSerialization Load(uint64_t key) {
        CProtobufSerialization cvalue;
        if (cache_.Get(key, &cvalue)==TFERROR_OK) {
            auto value = ToProtobufSerialization(cvalue);
            CProtobufSerialization_Free(&cvalue);
            return value;
        }
        else
            return {};
    }
    void Remove(uint64_t key) {
        cache_.Remove(key);
    }
};

std::string LocalFileName(uint64_t key) {
    std::stringstream str;
    str << kBasePath << "/local_cache_" << key;
    return str.str();
}

std::vector<uint64_t> keys = {0, 1, 24523, 0xffffff};

TEST(FileCacheTest, FileSaveLoadOp) {
    FileCacheTest ft;
    ProtobufSerialization saved = {1, 2, 3};
    for(auto k: keys) {
        EXPECT_FALSE(file_utils::FileExists(LocalFileName(k)));
        ft.Save(k, saved);
        EXPECT_TRUE(file_utils::FileExists(LocalFileName(k)));
        EXPECT_EQ(ft.Load(k), saved) << "Save+Load 1";
    }
}

TEST(FileCacheTest, FileRemoveOp) {
    FileCacheTest ft;
    ProtobufSerialization saved = {1, 2, 3};
    for(auto k: keys) {
        ft.Save(k, saved);
        EXPECT_TRUE(file_utils::FileExists(LocalFileName(k)));
        EXPECT_EQ(ft.Load(k), saved) << "Save+Load 2";
        ft.Remove(k);
        EXPECT_FALSE(file_utils::FileExists(LocalFileName(k)));
        EXPECT_EQ(ft.Load(k), ProtobufSerialization {}) << "Remove";
    }
}

} // namespace test
