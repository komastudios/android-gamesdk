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

/**
 * Models an Android asset (elements that get typically stored in app/src/main/assets folder.
 * Instances of this class are purely C++ and only cross the JNI bridge to get an AAsset pointer.
 * However, this pointer doesn't point to any JNI jobject; nor that performing Asset methods like
 * Read() or Seek() require any sort of Java-side involvement under the hood.
 * Prefer this type rather than crossing the JNI bridge and manipulating assets in the Java side of
 * things.
 * These instances ensure the release of all asset resources when destroyed. So you should prefer
 * instances of these classes rather than dealing with error prone NDK AAsset pointers.
 */
class Asset {
 public:
  /**
   * Constructor. The asset instance is opened and ready to be read.
   *
   * @param relative_path path to the asset, relative to the assets folder. If no asset is available
   *        at that path, the instance is void (see bool operator).
   * @param opening_mode read NDK AAsset type for a complete discussion.
   */
  Asset(const std::string &relative_path, const int opening_mode);
  virtual ~Asset();

  /**
   * False if no asset was available at the path used during construction.
   */
  explicit operator bool() const;

  // For the rest of the methods, check NDK AAsset.
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
