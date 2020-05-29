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

/**
 * This integration test performs a slideshow of textures grouped in families. Each family is headed
 * by a texture in a traditional encoding format (like PNG, JPG, etc.) which are great for storage
 * but too bad for memory. The decoding process for these traditional formats is also lengthy.
 * The rest of the family completes with alternative, so-called "compressed" formats. Examples like
 * ASTC, ETC2 and BASIS fall in this category.
 * This test compares, for a given family of textures, a regular JPG or PNG with ASTC compressed at
 * 2, 4 and 8bpp, ETC2 and BASIS (this one still in progress).
 * The comparison completes with post-compressed choices like deflating (gzip) or lz4.
 * This test measures loading times, space on disk vs. space in memory and bitmap transfer to GPU
 * times for these different formats. Developers will be able to use this info in order to determine
 * sweet spots, edge scenarios for some low end devices.
 *
 * Input configuration:
 * - texture_families_dir: path, relative to the assets directory, where texture families are
 *      located.
 * - texture_families: the list of families to be considered for this test.
 * - slideshow_interval: minimum time a slide is rendered. Default: one second.
 *
 * Output report: for now, we are reporting
 * - event_type: a value from the following:
 *   - LOAD_START: a texture loading just began.
 *   - LOAD_END: that texture loading completed successfully.
 *   - LOAD_FAILURE: that texture loading finished abnormally.
 *   - GPU_TRX: that loaded texture bitmap was transferred to the GPU for rendering.
 *   - UNSUPPORTED_TEXTURE_FORMAT: an indication that the device hardware doesn't support a given
 *      texture format.
 *
 * Depending on the event type, there are other fields:
 * - texture: all events but UNSUPPORTED_TEXTURE_FORMAT report this element, which is composed by:
 *   - name: the texture file name (without including the relative path inside assets).
 *   - format: encoding type. Could be "JPG" or "ASTC 4BPP", etc.
 *   - channels: RGB or RGBA.
 *
 * In the case of LOAD_END, it's also reported:
 * - size_at_rest: asset size on disk (impacts the .apk or .aapt size).
 * - size_in_memory: RAM occupied by the asset (either in the GPU or in the CPU), once loaded.
 * - loading_time: in milliseconds.
 *
 * In the case of GPU_TRX, it's also reported:
 * - gpu_trx_time: elapsed time to copy the bitmap from the CPU to the GPU.
 *
 * In the case of UNSUPPORTED_TEXTURE_FORMAT, it's also reported:
 * - texture_format: format that the hardware doesn't support.
 */

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/System.Asset.hpp>
#include <ancer/util/Error.hpp>
#include <ancer/util/GLTexImage2DRenderer.hpp>
#include <ancer/util/Json.hpp>
#include <ancer/util/StatusMessage.inl>
#include <ancer/util/Texture.hpp>
#include <ancer/util/texture/Astc.hpp>
#include <ancer/util/texture/Etc2.hpp>
#include <ancer/util/texture/Jpg.hpp>
#include <ancer/util/texture/Png.hpp>
#include <ancer/util/Time.hpp>

using namespace ancer;

//==================================================================================================

namespace {
constexpr Log::Tag TAG{"TextureLoadingGLES3"};
}

//==================================================================================================

namespace {
struct configuration {
  std::vector<std::string> texture_families;
  std::string texture_families_dir{"Textures/TextureCompression"};
  Seconds slideshow_interval = 1s;
};

JSON_CONVERTER(configuration) {
  JSON_REQVAR(texture_families);
  JSON_OPTVAR(texture_families_dir);
  JSON_OPTVAR(slideshow_interval);
}

//--------------------------------------------------------------------------------------------------

struct texture {
  std::string name;
  const std::string format;
  const std::string channels;

  explicit texture(const Texture &tx) :
      name{tx.GetFilenameStem()},
      format{tx.GetFormat()},
      channels{ToString(tx.GetChannels())} {
    name.append(".").append(tx.GetFilenameExtension());
  }
};

void WriteDatum(report_writers::Struct writer, const texture &texture) {
  ADD_DATUM_MEMBER(writer, texture, name);
  ADD_DATUM_MEMBER(writer, texture, format);
  ADD_DATUM_MEMBER(writer, texture, channels);
}

enum EventType {
  LOAD_START,
  LOAD_END,
  LOAD_FAILURE,
  GPU_TRX
};

struct datum {
  const EventType event_type;
  const texture texture;
  const size_t size_at_rest;
  const size_t size_in_memory;
  const Milliseconds::rep loading_time;
  const Milliseconds::rep gpu_trx_time;

  explicit datum(const EventType event_type, const Texture &tx)
      : event_type{event_type},
        texture{tx},
        size_at_rest{tx.GetSizeAtRest()},
        size_in_memory{tx.GetSizeInMemory()},
        loading_time{tx.GetLoadTimeInMillis()},
        gpu_trx_time{tx.GetGpuTransferTimeInMillis()} {}
};

void WriteDatum(report_writers::Struct writer, const datum &event) {
  switch (event.event_type) {
    case EventType::LOAD_START: {
      writer.AddItem("event_type", "TEXTURE_LOAD_START");
      ADD_DATUM_MEMBER(writer, event, texture);
    }
      break;

    case EventType::LOAD_FAILURE: {
      writer.AddItem("event_type", "TEXTURE_LOAD_FAILURE");
      ADD_DATUM_MEMBER(writer, event, texture);

      Log::E(TAG, "%s: loading failed", event.texture.name.c_str());
    }
      break;

    case EventType::LOAD_END: {
      writer.AddItem("event_type", "TEXTURE_LOAD_END");
      ADD_DATUM_MEMBER(writer, event, texture);
      ADD_DATUM_MEMBER(writer, event, size_at_rest);
      ADD_DATUM_MEMBER(writer, event, size_in_memory);
      ADD_DATUM_MEMBER(writer, event, loading_time);

      Log::I(TAG,
             "%s: %.1f KB at rest; %.1f KB in memory; decoded from storage to memory in %d ms",
             event.texture.name.c_str(),
             event.size_at_rest / 1000.0f,
             event.size_in_memory / 1000.0f,
             event.loading_time);
    }
      break;

    case EventType::GPU_TRX: {
      writer.AddItem("event_type", "TEXTURE_GPU_TRANSFER");
      ADD_DATUM_MEMBER(writer, event, texture);
      ADD_DATUM_MEMBER(writer, event, gpu_trx_time);

      Log::I(TAG,
             "%s: transferred from CPU to GPU in %d ms",
             event.texture.name.c_str(),
             event.gpu_trx_time);
    }
      break;
  }
}

struct unsupported_format {
  std::string texture_format;
};

void WriteDatum(report_writers::Struct writer, const unsupported_format &event) {
  writer.AddItem("event_type", "UNSUPPORTED_TEXTURE_FORMAT");
  ADD_DATUM_MEMBER(writer, event, texture_format);

  Log::I(TAG, "Texture format \"%s\" unsupported", event.texture_format.c_str());
}
}

//==================================================================================================

class TextureLoadingGLES3Operation : public BaseGLES3Operation {
 public:
  TextureLoadingGLES3Operation() = default;

  ~TextureLoadingGLES3Operation() override {
    glDeleteProgram(_program);
    glDeleteTextures(1, &_texture_id);
  }

  void Start() override {
    _configuration = GetConfiguration<configuration>();
    PopulateSlideshowTexturesFromFamilies();

    BaseGLES3Operation::Start();
    _slideshow_thread = TriggerSlideshowThread();
  }

  void Wait() override {
    _slideshow_thread.join();
  }

  void OnGlContextReady(const GLContextConfig &ctx_config) override {
    _egl_context = eglGetCurrentContext();
    if (_egl_context == EGL_NO_CONTEXT) {
      FatalError(TAG, "No EGL context available");
    }

    _texture_id = ancer::BindNewTexture2D();
    _program = SetupProgram();

    _context_ready = true;
  }

  // Depending on screen sizes, tries to fit the projection as a 1:1 square right in the middle.
  void OnGlContextResized(int width, int height) override {
    BaseGLES3Operation::OnGlContextResized(width, height);
    int left{0}, top{0}, right{0}, bottom{0};

    _projection = glh::Ortho2d(0, 0, static_cast<float>(width), static_cast<float>(height));

    if (_context_ready) {
      SetupRenderer();
    }
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);
    glh::CheckGlError("TextureLoadingGLES3Operation::Draw(delta_seconds) - superclass");

    if (_new_slide_available) {
      PostTextureToGpu();
      _new_slide_available = false;
    }

    glUseProgram(_program);
    glh::CheckGlError("TextureLoadingGLES3Operation::Draw(delta_seconds) - glUseProgram");

    glUniformMatrix4fv(_projection_uniform_loc, 1, GL_FALSE, glm::value_ptr(_projection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glUniform1i(_tex_id_uniform_loc, 0);

    if (_renderer) {
      _renderer->Draw();
    }
  }

//--------------------------------------------------------------------------------------------------

 private:
  inline void PushTextureToSlideshow(Texture *const texture) {
    _slideshow_textures.emplace_back(texture);
    Log::D(TAG, "%s queued to slideshow", ToString(*texture).c_str());
  }

  template<typename T>
  void PopulateSlideshowWithCompressedTexture(T *const texture) {
    PushTextureToSlideshow(texture);

    if (texture->GetPostCompressionFormat() == TexturePostCompressionFormat::NONE) {
      std::string filename_stem{texture->GetFilenameStem()};
      filename_stem.append(".").append(texture->GetFilenameExtension());

      PopulateSlideshowWithCompressedTexture(new T{
          texture->MirrorPostCompressed(TexturePostCompressionFormat::GZIP)
      });
      PopulateSlideshowWithCompressedTexture(new T{
          texture->MirrorPostCompressed(TexturePostCompressionFormat::LZ4)
      });
    }
  }

  void PopulateSlideshowWithUncompressedTexture(EncodedTexture *const texture) {
    PushTextureToSlideshow(texture);

    std::string filename_stem{texture->GetFilenameStem()};
    filename_stem.append("_").append(ToString(texture->GetChannels(), false));

    static const auto astc_supported{
        ancer::glh::IsExtensionSupported("GL_KHR_texture_compression_astc_ldr")};
    if (astc_supported) {
      uint bpp{1};  // ASTC at three different qualities: 2bpp, 4bpp & 8bpp
      for (uint iteration{0}; iteration < 3; ++iteration) {
        bpp *= 2;
        std::stringstream astc_stem;
        astc_stem << filename_stem << '_' << bpp << "bpp";
        PopulateSlideshowWithCompressedTexture(
            new AstcTexture{texture->GetRelativePath(), astc_stem.str(),
                            texture->GetChannels(), bpp});
      }
    } else {
      Report(unsupported_format{"ASTC"});
    }

    PopulateSlideshowWithCompressedTexture(new Etc2Texture{
        texture->GetRelativePath(), filename_stem, texture->GetChannels()});
  }

  void PopulateSlideshowTexturesFromFamilies() {
    std::for_each(std::cbegin(_configuration.texture_families),
                  std::cend(_configuration.texture_families),
                  [this](const std::string &texture_family) {
                    std::string texture_dir{_configuration.texture_families_dir};
                    texture_dir.append("/").append(texture_family);

                    PopulateSlideshowWithUncompressedTexture(
                        new JpgTexture{texture_dir, texture_family}
                    );
                    PopulateSlideshowWithUncompressedTexture(
                        new PngTexture{texture_dir, texture_family}
                    );
                  });
  }

  /**
   * Creates a shader program based on app assets.
   * @return the program id.
   */
  GLuint SetupProgram() {
    auto vertex_file = "Shaders/TextureLoadingGLES3Operation/pass_through.vsh";
    auto fragment_file = "Shaders/TextureLoadingGLES3Operation/pass_through.fsh";

    GLuint program_id = CreateProgram(vertex_file, fragment_file);

    if (!program_id) {
      FatalError(TAG, "Unable to create shader program");
    }

    _tex_id_uniform_loc = glGetUniformLocation(program_id, "uTex");
    glh::CheckGlError("looking up uTex");

    _projection_uniform_loc = glGetUniformLocation(program_id, "uProjection");
    glh::CheckGlError("looking up uProjection");

    return program_id;
  }

  /**
   * Depending on the total of renderers to build, it will set an NxN grid where N is the minimum
   * layout dimension that can hold all renderers.
   */
  void SetupRenderer() {
    auto size = GetGlContextSize();
    _renderer = std::make_unique<GLTexImage2DRenderer>(0, 0, size.x, size.y);
  }

  std::thread TriggerSlideshowThread() {
    return std::thread{[this] {
      while (not IsStopped() && not _context_ready);

      while (not IsStopped() && _slideshow_index < _slideshow_textures.size()) {
        auto *const p_texture = _slideshow_textures[_slideshow_index].get();
        Report(datum{EventType::LOAD_START, *p_texture});
        if ((_new_slide_available = p_texture->Load())) {
          Report(datum{EventType::LOAD_END, *p_texture});
          if (not IsStopped()) {
            std::this_thread::sleep_for(_configuration.slideshow_interval);
          }
        } else {
          Report(datum{EventType::LOAD_FAILURE, *p_texture});
        }
        ++_slideshow_index;
      }
    }};
  }

  void PostTextureToGpu() {
    auto *const p_texture = _slideshow_textures[_slideshow_index].get();
    p_texture->ApplyBitmap(_texture_id);
    datum new_texture_drawn{EventType::GPU_TRX, *p_texture};
    Report(new_texture_drawn);
    std::stringstream format;
    format << p_texture->GetFormat() << " (" << ancer::ToString(p_texture->GetChannels()) << ')';
    ancer::GetStatusMessageManager().SetValue(format.str());
  }

  //--------------------------------------------------------------------------------------------------

 private:
  EGLContext _egl_context = nullptr;
  GLuint _program = 0;

  GLuint _texture_id = 0;
  GLint _tex_id_uniform_loc = -1;

  mat4 _projection;
  GLint _projection_uniform_loc = -1;

//--------------------------------------------------------------------------------------------------

 private:
  configuration _configuration{};

  std::thread _slideshow_thread;
  std::vector<std::unique_ptr<Texture>> _slideshow_textures;
  std::size_t _slideshow_index{0};
  bool _new_slide_available{false};

  bool _context_ready{false};

  std::unique_ptr<GLTexImage2DRenderer> _renderer;
};

EXPORT_ANCER_OPERATION(TextureLoadingGLES3Operation)
