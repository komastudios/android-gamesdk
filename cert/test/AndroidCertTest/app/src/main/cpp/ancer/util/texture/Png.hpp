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

#ifndef _SYSTEM_PNG_HPP
#define _SYSTEM_PNG_HPP

#include <string>

#include <ancer/util/Texture.hpp>

namespace ancer {

/**
 * PNG encoded texture.
 */
class PngTexture : public EncodedTexture {
 public:
  PngTexture(const std::string &relative_path, const std::string &filename_stem);

  const std::string &GetFilenameExtension() const override;

 protected:
  void _Load() override;
  void _ApplyBitmap() override;
};

}

#endif  // _SYSTEM_PNG_HPP
