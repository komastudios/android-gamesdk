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

#ifndef materials_hpp
#define materials_hpp

#include <ancer/util/GLHelpers.hpp>

namespace marching_cubes {

struct SkydomeMaterial {
 private:
  GLuint _program = 0;
  GLint _uProjectionInverse = -1;
  GLint _uModelViewInverse = -1;
  GLint _uSkyboxSampler = -1;
  ancer::glh::TextureHandleRef _skyboxTex;

 public:
  SkydomeMaterial(ancer::glh::TextureHandleRef skybox)
      : _skyboxTex(skybox) {
    _program =
        ancer::glh::CreateProgramFromFiles("Shaders/MarchingCubesGLES3Operation/skydome_vert.glsl",
                                           "Shaders/MarchingCubesGLES3Operation/skydome_frag.glsl");
    _uProjectionInverse = glGetUniformLocation(_program, "uProjectionInverse");
    _uModelViewInverse = glGetUniformLocation(_program, "uModelViewInverse");
    _uSkyboxSampler = glGetUniformLocation(_program, "uSkyboxSampler");
  }

  SkydomeMaterial(const SkydomeMaterial &other) = delete;
  SkydomeMaterial(const SkydomeMaterial &&other) = delete;

  ~SkydomeMaterial() {
    if (_program > 0) {
      glDeleteProgram(_program);
    }
  }

  void bind(const mat4 &projection, const mat4 &modelview) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _skyboxTex->id());

    glUseProgram(_program);
    glUniformMatrix4fv(_uProjectionInverse, 1, GL_FALSE, value_ptr(glm::inverse(projection)));
    glUniformMatrix4fv(_uModelViewInverse, 1, GL_FALSE, value_ptr(glm::inverse(modelview)));
    glUniform1i(_uSkyboxSampler, 0);
  }
};

struct TerrainMaterial {
 private:
  GLuint _program = 0;
  GLint _uMVP = -1;
  GLint _uModel = -1;
  GLint _uCameraPos = -1;
  GLint _uAmbientLight = -1;
  GLint _uRenderDistance = -1;
  GLint _uTexture0Sampler = -1;
  GLint _uTexture0Scale = -1;
  GLint _uTexture1Sampler = -1;
  GLint _uTexture1Scale = -1;
  GLint _uLightProbeSampler = -1;
  GLint _uReflectionMapSampler = -1;
  GLint _uReflectionMapMipLevels = -1;

  ancer::glh::TextureHandleRef _lightProbe;
  ancer::glh::TextureHandleRef _reflectionMap;
  ancer::glh::TextureHandleRef _texture0;
  ancer::glh::TextureHandleRef _texture1;

  vec3 _ambientLight;
  float _texture0Scale = 1;
  float _texture1Scale = 1;
  float _renderDistance = 0;

 public:
  TerrainMaterial(vec3 ambientLight,
                  ancer::glh::TextureHandleRef lightProbe,
                  ancer::glh::TextureHandleRef reflectionMap,
                  ancer::glh::TextureHandleRef texture0, float tex0Scale,
                  ancer::glh::TextureHandleRef texture1, float tex1Scale,
                  float renderDistance)
      : _lightProbe(lightProbe),
        _reflectionMap(reflectionMap),
        _texture0(texture0),
        _texture1(texture1),
        _ambientLight(ambientLight),
        _texture0Scale(tex0Scale),
        _texture1Scale(tex1Scale),
        _renderDistance(renderDistance) {
    _program =
        ancer::glh::CreateProgramFromFiles("Shaders/MarchingCubesGLES3Operation/terrain_vert.glsl",
                                           "Shaders/MarchingCubesGLES3Operation/terrain_frag.glsl");
    _uMVP = glGetUniformLocation(_program, "uMVP");
    _uModel = glGetUniformLocation(_program, "uModel");
    _uCameraPos = glGetUniformLocation(_program, "uCameraPosition");
    _uAmbientLight = glGetUniformLocation(_program, "uAmbientLight");
    _uRenderDistance = glGetUniformLocation(_program, "uRenderDistance");
    _uTexture0Sampler = glGetUniformLocation(_program, "uTexture0Sampler");
    _uTexture0Scale = glGetUniformLocation(_program, "uTexture0Scale");
    _uTexture1Sampler = glGetUniformLocation(_program, "uTexture1Sampler");
    _uTexture1Scale = glGetUniformLocation(_program, "uTexture1Scale");
    _uLightProbeSampler = glGetUniformLocation(_program, "uLightProbeSampler");
    _uReflectionMapSampler = glGetUniformLocation(_program, "uReflectionMapSampler");
    _uReflectionMapMipLevels = glGetUniformLocation(_program, "uReflectionMapMipLevels");
  }

  TerrainMaterial(const TerrainMaterial &other) = delete;
  TerrainMaterial(const TerrainMaterial &&other) = delete;

  ~TerrainMaterial() {
    if (_program > 0) {
      glDeleteProgram(_program);
    }
  }

  void bind(const mat4 &model,
            const mat4 &view,
            const mat4 &projection,
            const vec3 &cameraPosition) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(_texture0->target(), _texture0->id());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(_texture1->target(), _texture1->id());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(_reflectionMap->target(), _reflectionMap->id());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(_lightProbe->target(), _lightProbe->id());

    glUseProgram(_program);
    glUniformMatrix4fv(_uMVP, 1, GL_FALSE, value_ptr(projection * view * model));
    glUniformMatrix4fv(_uModel, 1, GL_FALSE, value_ptr(model));
    glUniform3fv(_uCameraPos, 1, value_ptr(cameraPosition));
    glUniform3fv(_uAmbientLight, 1, value_ptr(_ambientLight));
    glUniform1f(_uRenderDistance, _renderDistance);
    glUniform1i(_uTexture0Sampler, 0);
    glUniform1f(_uTexture0Scale, _texture0Scale);
    glUniform1i(_uTexture1Sampler, 1);
    glUniform1f(_uTexture1Scale, _texture1Scale);
    glUniform1i(_uReflectionMapSampler, 2);
    glUniform1f(_uReflectionMapMipLevels, static_cast<float>(_reflectionMap->mipLevels()));
    glUniform1i(_uLightProbeSampler, 3);
  }
};

struct LineMaterial {
 private:
  GLuint _program = 0;
  GLint _uMVP = -1;

 public:
  LineMaterial() {
    _program =
        ancer::glh::CreateProgramFromFiles("Shaders/MarchingCubesGLES3Operation/line_vert.glsl",
                                           "Shaders/MarchingCubesGLES3Operation/line_frag.glsl");
    _uMVP = glGetUniformLocation(_program, "uMVP");
  }
  LineMaterial(const LineMaterial &other) = delete;
  LineMaterial(const LineMaterial &&other) = delete;

  ~LineMaterial() {
    if (_program > 0) {
      glDeleteProgram(_program);
    }
  }
  void bind(const mat4 &mvp) {
    glUseProgram(_program);
    glUniformMatrix4fv(_uMVP, 1, GL_FALSE, value_ptr(mvp));
  }
};

}

#endif