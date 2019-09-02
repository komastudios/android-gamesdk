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
#include <stdio.h>

namespace androidgamesdk_deviceinfo {

namespace stream_util {

/**
 * Implementation of getdelim(3)
 */
ssize_t getdelim(char **buf, size_t *bufsiz, int delimiter, FILE *fp);

/**
 * Implementation of getline(3)
 */
ssize_t getline(char **buf, size_t *bufsiz, FILE *fp);

}  // namespace stream_util

}  // namespace androidgamesdk_deviceinfo
