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

#include <condition_variable>

#include <GLES/gl.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES/glext.h>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>
#include <ancer/util/LibAndroid.hpp>
#include <ancer/util/LibEGL.hpp>

using namespace ancer;

// PURPOSE: To determine if a native hardware buffer can be created and used
// as a render target, and if the CPU can read directly from the buffer
// with native function calls (that is, not using glReadPixel or the like).

//==============================================================================

namespace {
constexpr Log::Tag TAG{"ExternalBufferGLES3Operation"};
}  // anonymous namespace

namespace {

struct datum {
  bool success;
  int measured_red;
  int measured_green;
  int measured_blue;
  int measured_alpha;
};

void WriteDatum(report_writers::Struct w, const datum &d) {
  ADD_DATUM_MEMBER(w, d, success);
  ADD_DATUM_MEMBER(w, d, measured_red);
  ADD_DATUM_MEMBER(w, d, measured_green);
  ADD_DATUM_MEMBER(w, d, measured_blue);
  ADD_DATUM_MEMBER(w, d, measured_alpha);
}

struct error_message_datum {
  std::string error_message;
};

void WriteDatum(report_writers::Struct w, const error_message_datum &d) {
  ADD_DATUM_MEMBER(w, d, error_message);
}

}  // anonymous namespace

//==============================================================================

class ExternalBufferGLES3Operation : public BaseGLES3Operation {
 public:
  ExternalBufferGLES3Operation() = default;

  ~ExternalBufferGLES3Operation() {}

  void OnGlContextReady(const GLContextConfig &ctx_config) override {
    Log::D(TAG, "GlContextReady");

    SetHeartbeatPeriod(500ms);

    _egl_context = eglGetCurrentContext();
    if (_egl_context == EGL_NO_CONTEXT) {
      FatalError(TAG, "No EGL context available");
    }

    if (!Init()) {
      Stop();
    }
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);

    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Wait for at least one frame to elapse,
    // otherwise the AHardwareBuffer will be empty.
    // TODO (baxtermichael): does this affect the validity of the test?
    if (_frames_drawn == 1) {
      ReadAHardwareBuffer();
      Stop();
    }

    ++_frames_drawn;
  }

  void OnHeartbeat(Duration elapsed) override {}

 private:
  void ReportError(const std::string &msg) {
    Log::D(TAG, msg);
    error_message_datum d{msg};
    Report(d);
  }

  // Logging error messages (via `ReportError`) is handled by the functions
  // called by `Init`, so in the event of an error we can just return early
  // and stop execution of the test.
  bool Init() {
    GetDefaultFramebuffer();
    if (!CreateNativeClientBuffer()) return false;
    if (!CreateEGLImage()) return false;
    if (!CreateFramebuffer()) return false;
    DrawFramebuffer();

    return true;
  }

  bool CreateNativeClientBuffer() {
    libandroid::FP_AHB_ALLOCATE fp_alloc =
        libandroid::GetFP_AHardwareBuffer_Allocate();
    if (fp_alloc == nullptr) {
      ReportError("Failed to locate symbol for AHardwareBuffer_allocate");
      return false;
    }

    AHardwareBuffer_Desc h_desc = {};
    h_desc.width = _framebuffer_width;
    h_desc.height = _framebuffer_height;
    h_desc.layers = 1;
    h_desc.format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM;
    h_desc.usage = AHARDWAREBUFFER_USAGE_GPU_FRAMEBUFFER |
                   AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN |
                   AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN;

    fp_alloc(&h_desc, &_hardware_buffer);
    if (_hardware_buffer == nullptr) {
      ReportError("Failed to allocate AHardwareBuffer");
      return false;
    }

    libegl::FP_GET_NATIVE_CLIENT_BUFFER fp_get_native_client_buffer =
        libegl::GetFP_GetNativeClientBuffer();
    if (fp_get_native_client_buffer == nullptr) {
      ReportError(
          "Failed to locate symbol for eglGetNativeClientBufferANDROID");
      return false;
    }

    _egl_buffer = fp_get_native_client_buffer(_hardware_buffer);
    if (_egl_buffer == nullptr) {
      DestroyNativeClientBuffer();
      ReportError("Failed to get native client buffer");
      return false;
    }

    return true;
  }

  bool DestroyNativeClientBuffer() {
    libandroid::FP_AHB_RELEASE fp_release =
        libandroid::GetFP_AHardwareBuffer_Release();
    if (fp_release == nullptr) {
      ReportError("Failed to locate symbol for AHardwareBuffer_release");
      return false;
    }

    fp_release(_hardware_buffer);
    _hardware_buffer = nullptr;

    return true;
  }

  bool CreateEGLImage() {
    EGLDisplay display = eglGetCurrentDisplay();
    EGLContext ctx = EGL_NO_CONTEXT;
    EGLenum target = EGL_NATIVE_BUFFER_ANDROID;
    const EGLint attribs[] = {EGL_NONE};
    _egl_image = eglCreateImageKHR(display, ctx, target, _egl_buffer, attribs);
    if (_egl_image == EGL_NO_IMAGE_KHR) {
      EGLint err = eglGetError();
      switch (err) {
        case EGL_BAD_DISPLAY:
          ReportError("Failed to create EGLImageKHR : EGL_BAD_DISPLAY");
          break;
        case EGL_BAD_CONTEXT:
          ReportError("Failed to create EGLImageKHR : EGL_BAD_CONTEXT");
          break;
        case EGL_BAD_PARAMETER:
          ReportError("Failed to create EGLImageKHR : EGL_BAD_PARAMETER");
          break;
        case EGL_BAD_MATCH:
          ReportError("Failed to create EGLImageKHR : EGL_BAD_MATCH");
          break;
        case EGL_BAD_ACCESS:
          ReportError("Failed to create EGLImageKHR : EGL_BAD_ACCESS");
          break;
        case EGL_BAD_ALLOC:
          ReportError("Failed to create EGLImageKHR : EGL_BAD_ALLOC");
          break;
        default:
          ReportError("Failed to create EGLImageKHR : unrecognized error");
          break;
      }
      return false;
    }

    return true;
  }

  void GetDefaultFramebuffer() {
    GLint fbo = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
    _default_framebuffer = static_cast<GLuint>(fbo);
  }

  bool CreateFramebuffer() {
    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 static_cast<GLsizei>(_framebuffer_width),
                 static_cast<GLsizei>(_framebuffer_height), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, _egl_image);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      ReportError("Failed to bind EGLImage to texture");
      return false;
    }

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, _texture, 0);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
      ReportError("Incomplete framebuffer");
      return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, _default_framebuffer);

    return true;
  }

  void DrawFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    GLenum attachment = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &attachment);

    // Since our goal is to draw to the framebuffer and see if we can read
    // it back through the AHardwareBuffer API, we choose to just clear to
    // an arbitrary color that is easy to identify.
    const float color_max = 255.0F;
    const float r = (float)_red / color_max;
    const float g = (float)_green / color_max;
    const float b = (float)_blue / color_max;
    const float a = (float)_alpha / color_max;
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, _default_framebuffer);
  }

  bool ReadAHardwareBuffer() {
    libandroid::FP_AHB_LOCK fp_lock = libandroid::GetFP_AHardwareBuffer_Lock();
    if (fp_lock == nullptr) {
      ReportError("Failed to load symbol for AHardwareBuffer_lock");
      return false;
    }

    libandroid::FP_AHB_UNLOCK fp_unlock =
        libandroid::GetFP_AHardwareBuffer_Unlock();
    if (fp_unlock == nullptr) {
      ReportError("Failed to load symbol for AHardwareBuffer_unlock");
      return false;
    }

    void *buffer;
    const int lock_result =
        fp_lock(_hardware_buffer, AHARDWAREBUFFER_USAGE_CPU_READ_MASK, -1,
                nullptr, &buffer);
    if (lock_result != 0) {
      ReportError("Failed to lock AHardwareBuffer : " +
                  std::to_string(lock_result));
      return false;
    }

    if (buffer == nullptr) {
      ReportError("Failed to map buffer memory");
      return false;
    }

    // We attempt to read back the color set during the call to `glClearColor()`
    // for the framebuffer attached to the AHardwareBuffer. We just need to
    // check one pixel since the whole framebuffer is cleared to the same color.
    const uint8_t *p_buffer = reinterpret_cast<uint8_t *>(buffer);
    const uint8_t r = p_buffer[0];
    const uint8_t g = p_buffer[1];
    const uint8_t b = p_buffer[2];
    const uint8_t a = p_buffer[3];

    const bool success =
        (r == _red) && (g == _green) && (b == _blue) && (a == _alpha);

    datum d = {};
    d.success = success;
    d.measured_red = static_cast<int>(r);
    d.measured_green = static_cast<int>(g);
    d.measured_blue = static_cast<int>(b);
    d.measured_alpha = static_cast<int>(a);
    Report(d);

    const int unlock_result = fp_unlock(_hardware_buffer, nullptr);
    if (unlock_result != 0) {
      ReportError("Failed to unlock AHardwareBuffer : " +
                  std::to_string(unlock_result));
      return false;
    }

    return true;
  }

 private:
  EGLContext _egl_context = EGL_NO_CONTEXT;

  int _frames_drawn = 0;
  GLuint _framebuffer = 0;
  GLuint _default_framebuffer = 0;
  GLuint _texture = 0;
  AHardwareBuffer *_hardware_buffer = nullptr;
  EGLClientBuffer _egl_buffer = nullptr;
  EGLImageKHR _egl_image = nullptr;

  // Values used to clear the framebuffer. We expect to read
  // these values back from the associated AHardwareBuffer.
  const uint8_t _red = 255;
  const uint8_t _green = 128;
  const uint8_t _blue = 64;
  const uint8_t _alpha = 255;

  // Arbitrary width and height for our framebuffer (AHardwareBuffer)
  const uint32_t _framebuffer_width = 512;
  const uint32_t _framebuffer_height = 512;
};

EXPORT_ANCER_OPERATION(ExternalBufferGLES3Operation)