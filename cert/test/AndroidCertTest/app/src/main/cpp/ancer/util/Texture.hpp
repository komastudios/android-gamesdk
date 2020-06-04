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

#ifndef _TEXTURE_HPP
#define _TEXTURE_HPP

#include <memory>
#include <string>

#include <GLES3/gl32.h>

#include <ancer/util/Time.hpp>

namespace ancer {

/**
 * Certain image formats don't handle transparency (the case of BMP or JPG, for example) while
 * others may or may not handle (like PNG). This information is relevant at the time of defining the
 * bitmap buffer to be posted to the GPU (e.g., glTexImage2D() or glCompressedTexImage2D()) because
 * that's how the GPU knows how to render the data.
 */
enum TextureChannels { RGB, RGBA };

/**
 * Returns a human readable version of a TextureChannels value.
 */
std::string ToString(const TextureChannels);

/**
 * Some texture assets are delivered as they are (e.g., JPG, ASTC, PKM, ...) whereas others are
 * post compressed (e.g., deflated, etc.) for minimal storage impact). This enumeration captures all
 * the possible considered options.
 */
enum class TexturePostCompressionFormat {
  /**
   * No post-compression. Just as is.
   */
      NONE,

  /**
   * Deflated via gzip.
   */
      GZIP,

  /**
   * Post-compressed via Lempel-Ziv variation LZ4.
   */
      LZ4
};

//--------------------------------------------------------------------------------------------------

/**
 * Abstract, base class for all texture types.
 */
class Texture {
 public:
  /**
   * Constructor. Doesn't load the texture from storage into memory. Invoke method Load() to that
   * effect.
   *
   * @param relative_path path relative to the texture in the assets folder.
   * @param filename_stem file name without extension (the subclass determines the extension).
   */
  Texture(std::string relative_path, std::string filename_stem, const TextureChannels channels);

  Texture(Texture &&) = default;

  virtual ~Texture() = default;

  /**
   * Route to the texture, relative to some root. The root could be the assets folder if using the
   * AssetManager or the app storage folder. This depends more on the component using a Texture.
   */
  const std::string &GetRelativePath() const;

  /**
   * Texture filename without the extension.
   */
  const std::string &GetFilenameStem() const;

  /**
   * Texture extension.
   */
  virtual const std::string &GetFilenameExtension() const = 0;

  /**
   * Readable texture format to categorize in groups like "ASTC 4bpp", etc.
   */
  virtual std::string GetFormat() const = 0;

  /**
   * Channels that are present (RGB or RGBA).
   */
  TextureChannels GetChannels() const;

  /**
   * Format passed to OpenGL ES when invoking glTexImage2D() or glCompressedImage2D().
   */
  GLenum GetInternalFormat() const;

  /**
   * Texture width in pixels.
   */
  int32_t GetWidth() const;

  /**
   * Texture height in pixels.
   */
  int32_t GetHeight() const;

  /**
   * Whether an alpha channel is present in the bitmap.
   */
  bool HasAlpha() const;

  /**
   * Texture size as an asset stored in the device.
   */
  size_t GetSizeAtRest() const;

  /**
   * Texture size once loaded and decoded in memory.
   */
  size_t GetSizeInMemory() const;

  /**
   * Elapsed time between texture asset loading from storage to full decoding, uncompressing in
   * memory.
   */
  Milliseconds::rep GetLoadTimeInMillis() const;

  /**
   * Elapsed bitmap transfer time from CPU to GPU.
   */
  Milliseconds::rep GetGpuTransferTimeInMillis() const;

  /**
   * Loads the texture into memory, decoding and decompressing depending on the texture type.
   * This loading should happen anytime in any CPU thread. Ideally, this function shouldn't be
   * invoked either from UIThread or GLThread. Instead, a background thread should be used to load.
   *
   * @return true if the load finished successfully, meaning that the texture could be rendered at
   *         anytime from now.
   */
  bool Load();

  /**
   * When loading finished successfully, this function posts this image to OpenGL ES for rendering.
   * Unlike Load(), this function must be called on the GLThread() (typically inside the scope of an
   * operation Draw() method.
   *
   * @param texture_id any valid OpenGL ES texture ID (see ancer::BindNewTextureID()).
   */
  void ApplyBitmap(const GLuint texture_id);

  /**
   * Formats the fully qualified texture asset file, relative to the assets folder. Beware that this
   * function is expensive and idempotent. This means that, for a given texture instance, calling
   * this function n times will always return the same result.
   */
  std::string Str() const;

 protected:
  /**
   * Subclasses override this function to implement the loading logic inherent to the texture type.
   * Decompression, decoding, etc., should happen here.
   */
  virtual void _Load() = 0;

  /**
   * Subclasses override this function to post the bitmap to OpenGL ES with the parameters that
   * correspond to the texture type (e.g., internal format, etc.)
   */
  virtual void _ApplyBitmap() = 0;

  std::string _relative_path;
  std::string _filename_stem;
  TextureChannels _channels;
  GLenum _internal_gl_format;

  int32_t _width;
  int32_t _height;
  bool _has_alpha;
  std::unique_ptr<u_char> _bitmap_data;
  size_t _mem_size;
  size_t _file_size;
  Milliseconds _load_time_in_millis;
  Milliseconds _gpu_trx_time_in_millis;
};

//--------------------------------------------------------------------------------------------------

/**
 * In few words, the traditional image formats (JPG, PNG, etc.) belong to this range.
 * Encoded textures are typically compressed at storage. However, during loading, they get decoded
 * into a fully decompressed bitmap (RGB88 or RGBA8888). 24 or 32 bits per pixel.
 */
class EncodedTexture : public Texture {
 public:
  EncodedTexture(const std::string &relative_path,
                 const std::string &filename_stem,
                 const TextureChannels channels);
};

//--------------------------------------------------------------------------------------------------

/**
 * Beside encoded textures, there's a range of so-called compressed textures. These are textures
 * that are compressed both on disk and in-memory once loaded (without any decoding involved).
 * These are meant to be cheaper (faster, lighter) that their EncodedTextureMetadata cousins. This
 * might come at the expense of some quality loss, that is frequently imperceptible.
 *
 * These compressed images may come further compressed via gzip or lz4.
 */
class CompressedTexture : public Texture {
 public:
  /**
   * Constructor.
   *
   * @param relative_path
   * @param filename_stem
   * @param post_compression_format (see TexturePostCompressionFormat for a discussion on this).
   */
  CompressedTexture(const std::string &relative_path,
                    const std::string &filename_stem, const TextureChannels channels,
                    const TexturePostCompressionFormat post_compression_format
                    = TexturePostCompressionFormat::NONE);

  const std::string &GetFilenameExtension() const override;

  std::string GetFormat() const override;

  TexturePostCompressionFormat GetPostCompressionFormat() const;

 protected:
  /**
   * Subclasses of this sub-category typically get the asset loading for free. Once the bitmap is
   * loaded in memory, _OnBitmapLoaded() is invoked.
   */
  void _Load() override;

  /**
   * Subclass implementers can post process the bitmap once loaded here (typically, to gather some
   * info about the texture like memory blocks, etc).
   */
  virtual void _OnBitmapLoaded() = 0;

  /**
   * A post-compressed texture like .gz, typically holds a compressed texture like .astc or .pkm.
   * Calling GetExtension to this type will return "gzip" or "lz4" if post-compressed. But this
   * function rescues the original compressed extension (like "astc", "basis" or "pkm").
   */
  virtual const std::string &_GetInnerExtension() const = 0;

  /**
   * A post-compressed texture formatted as GZIP, typically holds an inner format like ASTC or ETC2.
   * Calling GetFormat to a post-compressed texture returns, namely, "Deflated" if GZIP'ped, with
   * appended ASTC or the corresponding inner format that this function returns.
   */
  virtual std::string _GetInnerFormat() const = 0;

  TexturePostCompressionFormat _post_compression_format;
  GLsizei _image_size;
};

}

#endif  // _TEXTURE_HPP