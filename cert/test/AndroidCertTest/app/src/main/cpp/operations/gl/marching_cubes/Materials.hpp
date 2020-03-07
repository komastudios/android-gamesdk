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
    _program = ancer::glh::CreateProgramFromFiles("Shaders/MarchingCubesGLES3Operation/skydome_vert.glsl",
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
  GLint _uAmbientLight;
  GLint _uRenderDistance = -1;

  vec3 _ambientLight;
  float _renderDistance = 0;

 public:
  TerrainMaterial(vec3 ambientLight,float renderDistance)
      : _ambientLight(ambientLight),
        _renderDistance(renderDistance) {
    _program = ancer::glh::CreateProgramFromFiles("Shaders/MarchingCubesGLES3Operation/terrain_vert.glsl",
                                                  "Shaders/MarchingCubesGLES3Operation/terrain_frag.glsl");
    _uMVP = glGetUniformLocation(_program, "uMVP");
    _uModel = glGetUniformLocation(_program, "uModel");
    _uCameraPos = glGetUniformLocation(_program, "uCameraPosition");
    _uAmbientLight = glGetUniformLocation(_program, "uAmbientLight");
    _uRenderDistance = glGetUniformLocation(_program, "uRenderDistance");
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
    glUseProgram(_program);
    glUniformMatrix4fv(_uMVP, 1, GL_FALSE, value_ptr(projection * view * model));
    glUniformMatrix4fv(_uModel, 1, GL_FALSE, value_ptr(model));
    glUniform3fv(_uCameraPos, 1, value_ptr(cameraPosition));
    glUniform3fv(_uAmbientLight, 1, value_ptr(_ambientLight));
    glUniform1f(_uRenderDistance, _renderDistance);
  }
};

struct LineMaterial {
 private:
  GLuint _program = 0;
  GLint _uMVP = -1;

 public:
  LineMaterial() {
    _program = ancer::glh::CreateProgramFromFiles("Shaders/MarchingCubesGLES3Operation/line_vert.glsl",
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