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
 * TODO(dagum): document this operation
 */

#include <algorithm>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Error.hpp>
#include <ancer/util/GLTexImage2DRenderer.hpp>
#include <ancer/util/Json.hpp>
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
           "%s.%s: %db at rest; %db in memory; loaded in %dms",
           texture_name,
           texture_type,
           size_at_rest,
           size_in_memory,
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
        texture_metadata->GetRelativePath(),
        filename_stem,
        TexturePostCompressionFormat::GZIP
    });
    PopulateSlideshowWithCompressedTexture(new T{
        texture_metadata->GetRelativePath(),
        filename_stem,
        TexturePostCompressionFormat::LZ4
    });
  }
}

void PopulateSlideshowWithUncompressedTexture(UncompressedTextureMetadata *const texture_metadata) {
  PushTextureToSlideshow(texture_metadata);

  std::string filename_stem(texture_metadata->GetFilenameStem());
  filename_stem.append("_").append(texture_metadata->GetChannels());

  PopulateSlideshowWithCompressedTexture(new AstcTextureMetadata{
      texture_metadata->GetRelativePath() + "/astc/2bpp", filename_stem});
  PopulateSlideshowWithCompressedTexture(new AstcTextureMetadata{
      texture_metadata->GetRelativePath() + "/astc/4bpp", filename_stem});
  PopulateSlideshowWithCompressedTexture(new AstcTextureMetadata{
      texture_metadata->GetRelativePath() + "/astc/8bpp", filename_stem});
//  PopulateSlideshowWithCompressedTexture(new Etc2TextureMetadata{
//      texture_metadata->GetRelativePath() + "/etc2", filename_stem});
}

void PopulateSlideshowTexturesFromFamilies() {
  static const std::string kOperationTexturePath{"Textures/TextureCompression"};
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

    _texture_id = ancer::SetupTexture();
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

    if (_new_slide) {
      ::slideshow_textures[_slideshow_index]->ApplyBitmap(_texture_id);
      _new_slide = false;
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
        _new_slide = texture_metadata->Load();
        Report(load_stats{*texture_metadata});

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
  bool _new_slide{false};

  bool _context_ready{false};

  std::unique_ptr<GLTexImage2DRenderer> _renderer;
};

EXPORT_ANCER_OPERATION(TextureLoadingGLES3Operation)
