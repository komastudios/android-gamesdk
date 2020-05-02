/*
 * Copyright 2020 The Android Open Source Project
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

#ifndef _SYSTEM_ASSET_HPP
#define _SYSTEM_ASSET_HPP

#include <string>

#include <android/asset_manager.h>

namespace ancer {
class Asset {
 public:
  Asset(const std::string &filename, const int opening_mode);
  virtual ~Asset();

  explicit operator bool() const;

  const void *GetBuffer() const;

  int64_t GetLength() const;

  int64_t GetRemainingLength() const;

  bool IsAllocated() const;

  int OpenFileDescriptor(int64_t *outStart, int64_t *outLength) const;

  int Read(void *buf, const size_t count) const;

  int64_t Seek(const int64_t offset, const int whence) const;

 private:
  // Can't use a more convenient std::unique_ptr<AAsset> because AAsset is an incomplete type.
  AAsset *_asset_ptr;
};
}

#endif  // _SYSTEM_ASSET_HPP
