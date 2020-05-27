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

#include "Gzip.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <zlib.h>

#include <ancer/System.Asset.hpp>
#include <ancer/util/Log.hpp>

using namespace ancer;

namespace {
Log::Tag TAG{"Gzip"};

/**
 * As a deflated buffer is read, this is the size of the buffer to keep in memory at all times.
 */
static const size_t kGzipBufferSliceSize{32768};

static const uint8_t kCompressDeflated{8};  // Deflated

enum {  // Gzip header flags
  kFlagHCrc = 0x02,
  kFlagExtra = 0x04,
  kFlagName = 0x08,
  kFlagComment = 0x10,
};
}

namespace {
/**
 * Flips a number represented as a little endian 4-u_char (like the CRC32 or the Gzip payload size
 * if inflated.
 */
static inline uint32_t Get4LE(const u_char *buffer) {
  return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
}

/**
 * The name is self-descriptive. Given a deflated asset (e.g., a .gzip file in the assets directory)
 * inflates its content.
 */
class GzipAssetInflater {
 public:
  /**
   * Constructor.
   *
   * @param asset_path relative from the assets folder.
   */
  GzipAssetInflater(const std::string &asset_path) :
      _asset{asset_path, AASSET_MODE_RANDOM},
      _inflated_data_size{0},
      _header_size{0},
      _deflated_data_size{0},
      _epilogue_size{8},
      _compression_method{0},
      _crc_32{0},
      _is_eof{false},
      _is_error{false} {
    _InitializeInternals(asset_path.c_str());
  }

  /**
   * An instance of this class is non-null if it really has something to inflate.
   */
  explicit operator bool() const {
    return _inflated_data_size > 0;
  }

  /**
   * Inflates the asset content, returning a pointer to the inflated data.
   *
   * @param p_deflated_size pointer to a variable where the asset size in its compressed form is
   *                        to be stored.
   * @param p_inflated_size pointer to a variable where the asset size in its uncompressed form
   *                        is to be stored.
   * @return a std::unique_ptr to the uncompressed data (or null in case of failure).
   */
  std::unique_ptr<u_char> Inflate(size_t *const p_deflated_size,
                                  size_t *const p_inflated_size) const {
    assert(_asset);
    assert(_inflated_data_size >= 0);
    assert(_deflated_data_size >= 0);

    bool result{false};

    u_char *inflated_data_buffer = new u_char[_inflated_data_size];
    std::unique_ptr<u_char> p_inflated_data_buffer{inflated_data_buffer};

    u_char *deflated_data_slice = new u_char[::kGzipBufferSliceSize];
    std::unique_ptr<u_char> p_read_buffer_holder{deflated_data_slice};

    // Initialize the zlib stream.
    z_stream zlib_stream{_CreateZLibStream(inflated_data_buffer)};

    if (_InflateInit(&zlib_stream)) {
      result = _Inflate(&zlib_stream, deflated_data_slice);
      inflateEnd(&zlib_stream);        /* free up any allocated structures */
    }

    if (result) {
      *p_deflated_size = static_cast<size_t>(_asset.GetLength());
      *p_inflated_size = _inflated_data_size;
    } else {
      p_inflated_data_buffer = nullptr;
    }

    return p_inflated_data_buffer;
  }

 private:
  /**
   * Parses the asset header info in order to see if its a valid deflated resource. If so, internal
   * fields of this type get initialized.
   *
   * @return true if a valid deflated asset.
   */
  bool _InitializeInternals(const char *asset_name) {
    // Short circuit
    if (not _asset) { return false; }

    size_t characters_read{0};
    _asset.Seek(0, SEEK_SET);

    int current_char{_ReadNextCharacter()};
    if (current_char != 0x1f || _ReadNextCharacter() != 0x8b) {
      Log::E(TAG, "Asset %s isn't a .gz/.gzip file", asset_name);
      return false;
    }

    int compression_method{_ReadNextCharacter()};
    if (compression_method != ::kCompressDeflated) {
      Log::E(TAG, "Asset %s isn't deflated", asset_name);
      return false;
    }

    int gzip_flags{_ReadNextCharacter()};
    if (gzip_flags == EOF) {
      Log::E(TAG, "Asset %s misses header flags", asset_name);
      return false;
    }

    // skip over 4 bytes of mod time, 1 byte XFL, 1 byte OS
    for (auto i = 0; i < 6; i++) { _ReadNextCharacter(); }

    _SkipHeaderFlags(gzip_flags);

    if (_is_eof || _is_error) {
      Log::E(TAG, "Asset %s file seems corrupted", asset_name);
      return false;
    }

    // We just finished walking through the header
    // Now let's jump to the end; CRC and inflated length are at the end of the file
    _asset.Seek(-_epilogue_size, SEEK_END);

    u_char buf[_epilogue_size];
    if (_asset.Read(buf, _epilogue_size) != _epilogue_size) {
      Log::E(TAG, "Asset %s epilogue seems corrupted", asset_name);
      return false;
    }

    _compression_method = compression_method;
    _crc_32 = ::Get4LE(&buf[0]);

    _deflated_data_size = _asset.GetLength() - (_epilogue_size + _header_size);
    _inflated_data_size = Get4LE(&buf[4]);

    return true;
  }

  /**
   * A Gzip header size varies depending on its own info. This function just skips certain fields
   * based on flags.
   */
  void _SkipHeaderFlags(const int gzip_flags) {
    int current_char;

    /* consume "extra" field, if present */
    if (gzip_flags & kFlagExtra) {
      int len{_ReadNextCharacter()};
      len |= _ReadNextCharacter() << 8;
      while (len-- && _ReadNextCharacter() != EOF);
    }
    /* consume filename, if present */
    if (gzip_flags & kFlagName) {
      do {
        current_char = _ReadNextCharacter();
      } while (current_char != 0 && current_char != EOF);
    }
    /* consume comment, if present */
    if (gzip_flags & kFlagComment) {
      do {
        current_char = _ReadNextCharacter();
      } while (current_char != 0 && current_char != EOF);
    }
    /* consume 16-bit header CRC, if present */
    if (gzip_flags & kFlagHCrc) {
      _ReadNextCharacter();
      _ReadNextCharacter();
    }
  }

  /**
   * Imitates C stdlib getc(FILE*), reading one character at a time. We depend on it because the
   * gzip header doesn't have a fixed size. It's own content determines its length.
   *
   * @return the next character available for reading in the asset stream.
   */
  int _ReadNextCharacter() {
    u_char input;
    auto read_result = _asset.Read(&input, 1);
    switch (read_result) {
      case 0: {
        _is_eof = true;
      }
        break;

      case 1: {
        _header_size += 1;
        return static_cast<int>(input);
      }

      default: {
        _is_error = true;
      }
    }

    return EOF;
  }

  /**
   * Initializes a Zlib stream structure to be passed to Zlib functions (e.g., inflateInit(), etc.)
   *
   * @param inflated_data_buffer a pointer where Zlib will record the progressive inflate result.
   * @return the newly created stream.
   */
  z_stream _CreateZLibStream(u_char *inflated_data_buffer) const {
    z_stream zlib_stream;
    memset(&zlib_stream, 0, sizeof(zlib_stream));
    zlib_stream.zalloc = Z_NULL;
    zlib_stream.zfree = Z_NULL;
    zlib_stream.opaque = Z_NULL;
    zlib_stream.next_in = NULL;
    zlib_stream.avail_in = 0;
    zlib_stream.next_out = inflated_data_buffer;
    zlib_stream.avail_out = static_cast<uInt>(_inflated_data_size);
    zlib_stream.data_type = Z_UNKNOWN;

    return zlib_stream;
  }

  bool _InflateInit(z_stream *zlib_stream) const {
    _asset.Seek(_header_size, SEEK_SET);

    // Use the undocumented "negative window bits" feature to tell zlib that there's no zlib header
    // waiting for it.
    int zlib_err{inflateInit2(zlib_stream, -MAX_WBITS)};
    if (zlib_err != Z_OK) {
      if (zlib_err == Z_VERSION_ERROR) {
        Log::E(TAG, "Installed zlib is not compatible with linked version (%s)", ZLIB_VERSION);
      } else {
        Log::E(TAG, "Call to inflateInit2 failed (err=%d)", zlib_err);
      }
      return false;
    }

    return true;
  }

  /**
   * Progressive inflater. Reads deflated slices and inflates each onto the inflated buffer.
   *
   * @param zlib_stream the structure initialized via _CreateZLibStream.
   * @param deflated_data_slice pointer to the buffer that holds the iterative slices to inflate.
   * @return a boolean indicator of the success (true) or failure.
   */
  bool _Inflate(z_stream *zlib_stream, u_char *deflated_data_slice) const {
    size_t deflated_data_remaining{_deflated_data_size};
    int gzip_err;

    // Loop while we have data.
    do {
      size_t get_size;
      if (zlib_stream->avail_in == 0) {
        // Read an entire slice or the input remainder if lesser.
        get_size = std::min(deflated_data_remaining, ::kGzipBufferSliceSize);
        Log::V(TAG, "Reading %ld bytes (%ld left)", get_size, deflated_data_remaining);

        int read_count = _asset.Read(deflated_data_slice, get_size);
        if (read_count != get_size) {
          Log::D(TAG, "Inflate read failed (%d vs %ld)", read_count, get_size);
          return false;
        }

        deflated_data_remaining -= get_size;
        zlib_stream->next_in = deflated_data_slice;
        zlib_stream->avail_in = static_cast<uInt>(get_size);
      }
      // Uncompress the slice
      gzip_err = inflate(zlib_stream, Z_NO_FLUSH);
      if (gzip_err != Z_OK && gzip_err != Z_STREAM_END) {
        Log::D(TAG, "zlib inflate call failed (err=%d)", gzip_err);
        return false;
      }
      // The output buffer holds all; no need to write the output
    } while (gzip_err == Z_OK);
    assert(gzip_err == Z_STREAM_END);       /* other errors should've been caught */

    if ((long) zlib_stream->total_out != _inflated_data_size) {
      Log::W(TAG, "Size mismatch on inflated file (%ld vs %ld)",
             zlib_stream->total_out, _inflated_data_size);
      return false;
    }

    return true;
  }

 private:
  Asset _asset;
  size_t _inflated_data_size;

  // A gzip file consists on a header, the deflated data per-se and an epilogue (CRC32 plus the size
  // of the data when inflated back up).
  size_t _header_size;
  size_t _deflated_data_size;
  const size_t _epilogue_size;

  int _compression_method;
  uLong _crc_32;

  bool _is_eof;
  bool _is_error;
};
}

//--------------------------------------------------------------------------------------------------

std::unique_ptr<u_char> gzip::InflateAsset(const std::string &asset_path,
                                           size_t *p_deflated_size,
                                           size_t *p_inflated_size) {
  auto inflater = ::GzipAssetInflater{asset_path};
  return inflater ? inflater.Inflate(p_deflated_size, p_inflated_size) : nullptr;
}
