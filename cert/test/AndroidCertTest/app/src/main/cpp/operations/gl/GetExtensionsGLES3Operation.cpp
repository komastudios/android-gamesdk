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

#include <GLES/gl.h>       // TODO: Remove if possible
#include <GLES2/gl2.h>     // TODO: Remove if possible
#include <GLES2/gl2ext.h>  // TODO: Remove if possible
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <condition_variable>
#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;

// PURPOSE: Gets a list of GLES and EGL extensions available on the device.

//==============================================================================

namespace {
  constexpr Log::Tag TAG{"GetExtensionsGLES3Operation"};
}  // anonymous namespace

namespace {

  struct datum {
    std::vector<std::string> gl_extensions;
    std::vector<std::string> egl_extensions;
  };

  JSON_WRITER(datum) {
    JSON_REQVAR(gl_extensions);
    JSON_REQVAR(egl_extensions);
  }

}  // anonymous namespace

//==============================================================================

class GetExtensionsGLES3Operation : public BaseGLES3Operation {
public:
  GetExtensionsGLES3Operation() = default;

  ~GetExtensionsGLES3Operation() {}

  void OnGlContextReady(const GLContextConfig& ctx_config) override {
    Log::D(TAG, "GlContextReady");

    SetHeartbeatPeriod(500ms);

    _egl_context = eglGetCurrentContext();
    if (_egl_context == EGL_NO_CONTEXT) {
      FatalError(TAG, "No EGL context available");
    }

    LogExtensions();
    Stop();
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);
  }

  void OnHeartbeat(Duration elapsed) override {}

private:
  void LogExtensions() {
    const auto gl_extensions = glh::GetGlExtensions();
    const auto egl_extensions = glh::GetEglExtensions();
    datum d = {};
    d.gl_extensions = gl_extensions;
    d.egl_extensions = egl_extensions;
    Report(d);
  }

private:
  EGLContext _egl_context = EGL_NO_CONTEXT;
};

EXPORT_ANCER_OPERATION(GetExtensionsGLES3Operation)