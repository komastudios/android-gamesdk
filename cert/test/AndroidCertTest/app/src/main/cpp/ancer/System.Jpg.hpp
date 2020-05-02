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

#ifndef _SYSTEM_JPG_HPP
#define _SYSTEM_JPG_HPP

#include <string>

#include <android/asset_manager.h>

#include <basis_universal/jpgd.h>

namespace ancer {

class JpegDecoderAssetStream : public jpgd::jpeg_decoder_stream {
 public:
  JpegDecoderAssetStream();
  JpegDecoderAssetStream(const JpegDecoderAssetStream &) = delete;
  JpegDecoderAssetStream &operator=(const JpegDecoderAssetStream &) = delete;
  virtual ~JpegDecoderAssetStream();

  bool open(const std::string &asset_path);
  void close();

  virtual int read(jpgd::uint8 *pBuf, int max_bytes_to_read, bool *pEOF_flag);

 private:
  AAsset *_asset_ptr;
  bool _is_eof, _is_error;
};

}

#endif  // _SYSTEM_JPG_HPP
