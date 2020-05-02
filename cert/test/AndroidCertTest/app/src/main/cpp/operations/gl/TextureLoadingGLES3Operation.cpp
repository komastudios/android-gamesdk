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
 * but too bad for memory. The decoding process is also lengthy.
 * The rest of the family completes with alternative formats, like ASTC compressed at different bits
 * per pixel rates. ETC2 is also considered (although it's in progress at this time), and BASIS
 * universal format (which is compressed in storage but can also get transcoded on the device to
 * wide range of memory efficient formats, assuming that either ASTC or ETC may not be available on
 * a given device.
 * The picture completes with post-compressed choices like deflating (gzip) or lz4.
 * This test measures for now, loading times, space on disk and space in memory of the different
 * formats. Developers will be able to use this info in order to determine sweet spots, or edge
 * scenarios for some low end devices.
 *
 * Input configuration:
 * - slideshow_interval: minimum time a slide is rendered. Default: one second.
 *
 * Output report: for now, we are reporting
 * - texture_name: so we can correlate textures that belong to a same family.
 * - texture_type: so we can distinguish texture formats within a family.
 * - size_at_rest: asset size on disk (impacts the .apk or .aapt size).
 * - size_in_memory: RAM occupied by the asset (either in the GPU or in the CPU), once loaded.
 * - loading_time: in milliseconds.
 *
 * Pending to report: skipped images because the device hardware (or OpenGL ES driver don't support
 * a given compressed format); other errors.
 */

#include <algorithm>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/util/Error.hpp>
#include <ancer/util/GLTexImage2DRenderer.hpp>
#include <ancer/util/Json.hpp>
#include <ancer/util/Texture.hpp>
#include <ancer/util/texture/Astc.hpp>
#include <ancer/util/texture/Etc2.hpp>
#include <ancer/util/texture/Jpg.hpp>
#include <ancer/util/Time.hpp>

using namespace ancer;

//==================================================================================================

namespace {
constexpr Log::Tag TAG{"TextureLoadingGLES3"};
}

//==================================================================================================

namespace {
struct configuration {
  Seconds slideshow_interval = 1s;
};

JSON_CONVERTER(configuration) {
  JSON_OPTVAR(slideshow_interval);
}

//--------------------------------------------------------------------------------------------------

struct load_stats {
  const char *texture_name;
  const char *texture_type;
  size_t size_at_rest;
  size_t size_in_memory;
  Milliseconds::rep loading_time;

  explicit load_stats(const TextureMetadata &metadata)
      : texture_name{metadata.GetFilenameStem().c_str()},
        texture_type{metadata.GetFilenameExtension().c_str()},
        size_at_rest{metadata.GetSizeAtRest()},
        size_in_memory{metadata.GetSizeInMemory()},
        loading_time{metadata.GetLoadTimeInMillis()} {
    Log::D(TAG,
           "%s: %.1f KB at rest; %.1f KB in memory; loaded in %d ms",
           metadata.ToString().c_str(),
           size_at_rest / 1000.0f,
           size_in_memory / 1000.0f,
           loading_time);
  }
};

void WriteDatum(report_writers::Struct writer, const load_stats &load_stats) {
  ADD_DATUM_MEMBER(writer, load_stats, texture_name);
  ADD_DATUM_MEMBER(writer, load_stats, texture_type);
  ADD_DATUM_MEMBER(writer, load_stats, size_at_rest);
  ADD_DATUM_MEMBER(writer, load_stats, size_in_memory);
  ADD_DATUM_MEMBER(writer, load_stats, loading_time);
}
}

//==================================================================================================

namespace {
std::vector<std::unique_ptr<TextureMetadata>> slideshow_textures;

inline void PushTextureToSlideshow(TextureMetadata *const texture_metadata) {
  slideshow_textures.emplace_back(texture_metadata);
  Log::D(TAG,
         "%s/%s.%s queued to slideshow",
         texture_metadata->GetRelativePath().c_str(),
         texture_metadata->GetFilenameStem().c_str(),
         texture_metadata->GetFilenameExtension().c_str());
}

template<typename T>
void PopulateSlideshowWithCompressedTexture(T *const texture_metadata) {
  PushTextureToSlideshow(texture_metadata);

  if (texture_metadata->GetPostCompressionFormat() != TexturePostCompressionFormat::NONE) {
    std::string filename_stem(texture_metadata->GetFilenameStem());
    filename_stem.append(".").append(texture_metadata->GetFilenameExtension());

    PopulateSlideshowWithCompressedTexture(new T{
        texture_metadata->MirrorPostCompressed(TexturePostCompressionFormat::GZIP)
    });
    PopulateSlideshowWithCompressedTexture(new T{
        texture_metadata->MirrorPostCompressed(TexturePostCompressionFormat::LZ4)
    });
  }
}

void PopulateSlideshowWithUncompressedTexture(EncodedTextureMetadata *const texture_metadata) {
  PushTextureToSlideshow(texture_metadata);

  std::string filename_stem(texture_metadata->GetFilenameStem());
  filename_stem.append("_").append(texture_metadata->GetChannels());

  static const auto astc_supported{
      ancer::glh::IsExtensionSupported("GL_KHR_texture_compression_astc_ldr")};
  if (astc_supported) {
    PopulateSlideshowWithCompressedTexture(new AstcTextureMetadata{
        texture_metadata->GetRelativePath() + "/astc/2bpp", filename_stem, 2});
    PopulateSlideshowWithCompressedTexture(new AstcTextureMetadata{
        texture_metadata->GetRelativePath() + "/astc/4bpp", filename_stem, 4});
    PopulateSlideshowWithCompressedTexture(new AstcTextureMetadata{
        texture_metadata->GetRelativePath() + "/astc/8bpp", filename_stem, 8});
  } else {
    Log::D(TAG, "Skipping ASTC from slideshow due to lack of hardware support");
    // TODO(dagum): report that hw doesn't support format
  }

  PopulateSlideshowWithCompressedTexture(new Etc2TextureMetadata{
      texture_metadata->GetRelativePath() + "/etc2", filename_stem});
}

void PopulateSlideshowTexturesFromFamilies() {
  static const std::string kOperationTexturePath{"Textures/TextureCompression"};
  // TODO(dagum): we must rework entirely the family names. Rather than hardcoded, they'll have to
  //              be discovered at run-time, in kOperationTexturePath (a family per sub-directory)
  static std::vector<std::string> texture_family_names({
                                                           "avocados",
                                                           "ballons",
                                                           "bananas",
                                                           "cards",
                                                           "tomatoes"
                                                       });

  std::for_each(std::cbegin(texture_family_names),
                std::cend(texture_family_names),
                [](const std::string &texture_family_name) {
                  PopulateSlideshowWithUncompressedTexture(
                      new JpgTextureMetadata{kOperationTexturePath, texture_family_name}
                  );
//                  PopulateSlideshowWithUncompressedTexture(
//                      new PngTextureMetadata{kOperationTexturePath, texture_family_name}
//                  );
                });
}
}

//==================================================================================================

class TextureLoadingGLES3Operation : public BaseGLES3Operation {
 public:
  TextureLoadingGLES3Operation() = default;

  ~TextureLoadingGLES3Operation() override {
    glDeleteProgram(_program);
    glDeleteTextures(1, &_texture_id);

    slideshow_textures.clear();
  }

  void Start() override {
    ::PopulateSlideshowTexturesFromFamilies();
    BaseOperation::Start();

    _slideshow_thread = TriggerSlideshowThread();
  }

  void Wait() override {
    _slideshow_thread.join();
  }

  void OnGlContextReady(const GLContextConfig &ctx_config) override {
    _configuration = GetConfiguration<configuration>();

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
      ::slideshow_textures[_slideshow_index]->ApplyBitmap(_texture_id);
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
      while (not this->IsStopped() && not this->_context_ready);

      while (not this->IsStopped() && this->_slideshow_index < ::slideshow_textures.size()) {
        auto *const texture_metadata = ::slideshow_textures[_slideshow_index].get();
        if ((_new_slide_available = texture_metadata->Load())) {
          Report(load_stats{*texture_metadata});
        } // TODO(dagum): else report that it couldn't be loaded

        if (not this->IsStopped()) {
          std::this_thread::sleep_for(this->_configuration.slideshow_interval);
          ++_slideshow_index;
        }
      }
    }};
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
  std::size_t _slideshow_index{0};
  bool _new_slide_available{false};

  bool _context_ready{false};

  std::unique_ptr<GLTexImage2DRenderer> _renderer;
};

EXPORT_ANCER_OPERATION(TextureLoadingGLES3Operation)
