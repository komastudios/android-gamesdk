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

#include "device_info/core/stream_util.h"
#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

namespace androidgamesdk_deviceinfo {

namespace stream_util_test {

FILE* openFileStream(const char* content) {
  return fmemopen((void*)content, strlen(content), "r");
}

TEST(stream_util, get_delim_including_end) {
  char* line = NULL;
  size_t bufferSize = 0;
  FILE* in = openFileStream("123;456;789;");

  ssize_t result = stream_util::getdelim(&line, &bufferSize, ';', in);
  ASSERT_EQ(result, 4);
  ASSERT_STREQ(line, "123;");
  ASSERT_GE(bufferSize, 4);

  result = stream_util::getdelim(&line, &bufferSize, ';', in);
  ASSERT_EQ(result, 4);
  ASSERT_STREQ(line, "456;");
  ASSERT_GE(bufferSize, 4);

  result = stream_util::getdelim(&line, &bufferSize, ';', in);
  ASSERT_EQ(result, 4);
  ASSERT_STREQ(line, "789;");
  ASSERT_GE(bufferSize, 4);

  result = stream_util::getdelim(&line, &bufferSize, ';', in);
  ASSERT_EQ(result, -1);
  ASSERT_GE(bufferSize, 3);

  ASSERT_NE(feof(in), 0);
  fclose(in);
}

TEST(stream_util, get_delim_without_end) {
  char* line = NULL;
  size_t bufferSize = 0;
  FILE* in = openFileStream("123;456;789");

  ssize_t result = stream_util::getdelim(&line, &bufferSize, ';', in);
  ASSERT_EQ(result, 4);
  ASSERT_STREQ(line, "123;");
  ASSERT_GE(bufferSize, 4);

  result = stream_util::getdelim(&line, &bufferSize, ';', in);
  ASSERT_EQ(result, 4);
  ASSERT_STREQ(line, "456;");
  ASSERT_GE(bufferSize, 4);

  result = stream_util::getdelim(&line, &bufferSize, ';', in);
  ASSERT_EQ(result, 3);
  ASSERT_STREQ(line, "789");
  ASSERT_GE(bufferSize, 3);

  ASSERT_NE(feof(in), 0);
  fclose(in);
}

TEST(stream_util, get_delim_empty) {
  char* line = NULL;
  size_t bufferSize = 0;
  FILE* in = openFileStream("");

  ssize_t result = stream_util::getdelim(&line, &bufferSize, ';', in);
  ASSERT_EQ(result, -1);
  ASSERT_GE(bufferSize, 0);

  ASSERT_NE(feof(in), 0);
  fclose(in);
}

TEST(stream_util, get_delim_only_delimiters) {
  char* line = NULL;
  size_t bufferSize = 0;
  FILE* in = openFileStream("//");

  ssize_t result = stream_util::getdelim(&line, &bufferSize, '/', in);
  ASSERT_EQ(result, 1);
  ASSERT_STREQ(line, "/");
  ASSERT_GE(bufferSize, 1);

  result = stream_util::getdelim(&line, &bufferSize, '/', in);
  ASSERT_EQ(result, 1);
  ASSERT_STREQ(line, "/");
  ASSERT_GE(bufferSize, 1);

  result = stream_util::getdelim(&line, &bufferSize, '/', in);
  ASSERT_EQ(result, -1);
  ASSERT_GE(bufferSize, 1);

  ASSERT_NE(feof(in), 0);
  fclose(in);
}

TEST(stream_util, get_line) {
  char* line = NULL;
  size_t bufferSize = 0;
  FILE* in = openFileStream("Hello,\nworld!\n\nthe end.");

  ssize_t result = stream_util::getline(&line, &bufferSize, in);
  ASSERT_EQ(result, 7);
  ASSERT_STREQ(line, "Hello,\n");
  ASSERT_GE(bufferSize, 7);

  result = stream_util::getline(&line, &bufferSize, in);
  ASSERT_EQ(result, 7);
  ASSERT_STREQ(line, "world!\n");
  ASSERT_GE(bufferSize, 7);

  result = stream_util::getline(&line, &bufferSize, in);
  ASSERT_EQ(result, 1);
  ASSERT_STREQ(line, "\n");
  ASSERT_GE(bufferSize, 1);

  result = stream_util::getline(&line, &bufferSize, in);
  ASSERT_EQ(result, 8);
  ASSERT_STREQ(line, "the end.");
  ASSERT_GE(bufferSize, 8);

  ASSERT_NE(feof(in), 0);
  fclose(in);
}

}  // namespace stream_util_test
}  // namespace androidgamesdk_deviceinfo
