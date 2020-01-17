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
#define GL_GLEXT_PROTOTYPES
#include <GLES/glext.h>
#include <GLES2/gl2.h>     // TODO: Remove if possible
#include <GLES2/gl2ext.h>  // TODO: Remove if possible

#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <condition_variable>
#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

#include <dlfcn.h>
#include <android/hardware_buffer.h>
#include <android/hardware_buffer_jni.h>

using namespace ancer;

// PURPOSE:

//==============================================================================

namespace {
constexpr Log::Tag TAG{"ExternalBufferGLES3Operation"};
}  // anonymous namespace

namespace {
struct configuration {
  bool test;
};

JSON_CONVERTER(configuration) { JSON_OPTVAR(test); }

struct datum {
  bool success;
  int red;
  int green;
  int blue;
  int alpha;
};

JSON_WRITER(datum) {
  JSON_REQVAR(success);
  JSON_REQVAR(red);
  JSON_REQVAR(green);
  JSON_REQVAR(blue);
  JSON_REQVAR(alpha);
}

} // anonymous namespace

//==============================================================================

namespace {
  void *LoadLib(const char *lib_name) {
    void *lib = dlopen(lib_name, RTLD_NOW | RTLD_LOCAL);
    if (lib == nullptr) {
      Log::D(TAG, "Failed to load %s", lib_name);
    }
    return lib;
  }

  void *GetAndroidLib() {
    static void *lib;
    if (lib == nullptr) {
      lib = LoadLib("libandroid.so");
    }
    return lib;
  }

  void *GetEGLLib() {
    static void *lib;
    if (lib == nullptr) {
      lib = LoadLib("libEGL.so");
    }
    return lib;
  }

  void *GetPFN(void *lib, const char *function_name) {
    if (lib == nullptr) return nullptr;

    void *pfn = dlsym(lib, function_name);
    if (pfn == nullptr) {
      Log::D(TAG, "Failed to locate %s", function_name);
    }

    return pfn;
  }

  typedef int (*PFN_AHB_ALLOCATE)(const AHardwareBuffer_Desc*, AHardwareBuffer**);
  typedef void (*PFN_AHB_RELEASE)(AHardwareBuffer*);
  typedef int (*PFN_AHB_LOCK)(AHardwareBuffer*, uint64_t, int32_t, const ARect*, void**);
  typedef int (*PFN_AHB_UNLOCK)(AHardwareBuffer*, int32_t*);

  PFN_AHB_ALLOCATE GetAHardwareBuffer_Allocate() {
    static PFN_AHB_ALLOCATE pfn;
    if (pfn != nullptr) return pfn;
    pfn = reinterpret_cast<PFN_AHB_ALLOCATE>(GetPFN(GetAndroidLib(), "AHardwareBuffer_allocate"));
    return pfn;
  }

  PFN_AHB_RELEASE GetAHardwareBuffer_Release() {
    static PFN_AHB_RELEASE pfn;
    if (pfn != nullptr) return pfn;
    pfn = reinterpret_cast<PFN_AHB_RELEASE>(GetPFN(GetAndroidLib(), "AHardwareBuffer_release"));
    return pfn;
  }

  PFN_AHB_LOCK GetPFN_AHardwareBuffer_Lock() {
    static PFN_AHB_LOCK pfn;
    if (pfn != nullptr) return pfn;
    pfn = reinterpret_cast<PFN_AHB_LOCK>(GetPFN(GetAndroidLib(), "AHardwareBuffer_lock"));
    return pfn;
  }

  PFN_AHB_UNLOCK GetPFN_AHardwareBuffer_Unlock() {
    static PFN_AHB_UNLOCK pfn;
    if (pfn != nullptr) return pfn;
    pfn = reinterpret_cast<PFN_AHB_UNLOCK>(GetPFN(GetAndroidLib(), "AHardwareBuffer_unlock"));
    return pfn;
  }

  typedef PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC PFN_GET_EGLBUFFER;

  PFN_GET_EGLBUFFER GetCreateEGLBuffer() {
    static PFN_GET_EGLBUFFER pfn;
    if (pfn != nullptr) return pfn;
    pfn = reinterpret_cast<PFN_GET_EGLBUFFER>(GetPFN(GetEGLLib(), "eglGetNativeClientBufferANDROID"));
    return pfn;
  }

  GLuint CompileShader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
      GLint len = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

      if (len > 1) {
        char *log = (char*)malloc(sizeof(char) * len);
        glGetShaderInfoLog(shader, len, NULL, log);
        free(log);
        FatalError(TAG, "shader compile error: %s", log);
      }

      glDeleteShader(shader);

      FatalError(TAG, "Failed to compiled shader");
    }

    return shader;
  }
} // anonymous namespace

//==============================================================================

// - [x] Check for extension
// - [x] Get function pointer
// - [x] Create EGLClientBuffer
// - [x] Create EGLImageKHR from EGLClientBuffer
// - [x] Create framebuffer from EGLClientBuffer
// - [x] Draw to framebuffer
// - [x] CPU read from buffer-backed memory
// - [ ] Try to use #define's instead of dynamic loading, if possible

bool initialized = false;
GLuint framebuffer = 0;
GLuint default_framebuffer = 0;
GLuint texture = 0;
GLuint program = 0;
AHardwareBuffer *h_buffer = nullptr;
EGLClientBuffer client_buffer = nullptr;
EGLImageKHR egl_image = nullptr;
int frames_drawn = 0;

class ExternalBufferGLES3Operation : public BaseGLES3Operation {
 public:
  ExternalBufferGLES3Operation() = default;

  ~ExternalBufferGLES3Operation() {}

  void OnGlContextReady(const GLContextConfig& ctx_config) override {
    Log::D(TAG, "GlContextReady");
    _configuration = GetConfiguration<configuration>();

    SetHeartbeatPeriod(500ms);

    _egl_context = eglGetCurrentContext();
    if (_egl_context == EGL_NO_CONTEXT) {
      FatalError(TAG, "No EGL context available");
    }

    CreateShader();
//    DestroyNativeClientBuffer();
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);

    glClearColor(1.0f, 0.5f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!initialized) {
      initialized = true;
      CreateNativeClientBuffer();
      CreateEGLImage();
      GetDefaultFramebuffer();
      CreateFramebuffer();
      DrawToFramebuffer();

      glUseProgram(program);
      glBindTexture(GL_TEXTURE_2D, texture);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    if (frames_drawn == 1) {
      ReadAHardwareBuffer();
    }

    ++frames_drawn;
  }

  void OnHeartbeat(Duration elapsed) override {}

 private:

  void CreateShader() {
    const char *vert_source = "#version 310 es\n"
                               "const vec2 points[6] = vec2[](\n"
                               "  vec2(-1.0, 1.0),\n"
                               "  vec2(-1.0, -1.0),\n"
                               "  vec2(1.0, -1.0),\n"
                               "  vec2(-1.0, 1.0),\n"
                               "  vec2(1.0, -1.0),\n"
                               "  vec2(1.0, 1.0)\n"
                               ");\n"
                               "const vec2 texcoords[6] = vec2[](\n"
                               "  vec2(0.0, 1.0),\n"
                               "  vec2(0.0, 0.0),\n"
                               "  vec2(1.0, 0.0),\n"
                               "  vec2(0.0, 1.0),\n"
                               "  vec2(1.0, 0.0),\n"
                               "  vec2(1.0, 1.0)\n"
                               ");\n"
                               "out vec2 tex_coord;\n"
                               "void main() {\n"
                               "  tex_coord = texcoords[gl_VertexID];\n"
                               "  gl_Position = vec4(0.5 * points[gl_VertexID], 0.0, 1.0);\n"
                               "}\n";
    GLuint vsh = CompileShader(GL_VERTEX_SHADER, vert_source);
    if (vsh == 0) return;

    const char *frag_source = "#version 310 es\n"
                              "precision mediump float;\n"
                              "uniform sampler2D s_texture;\n"
                              "in vec2 tex_coord;\n"
                              "out vec4 frag_color;\n"
                              "void main() {\n"
                              "  frag_color = texture(s_texture, tex_coord);\n"
                              "}\n";
    GLuint fsh = CompileShader(GL_FRAGMENT_SHADER, frag_source);
    if (fsh == 0) return;

    GLuint _program = glCreateProgram();
    glAttachShader(_program, vsh);
    glAttachShader(_program, fsh);
    glLinkProgram(_program);

    GLint linked = 0;
    glGetProgramiv(_program, GL_LINK_STATUS, &linked);
    if (!linked) {
      glDeleteShader(vsh);
      glDeleteShader(fsh);
      glDeleteProgram(_program);
      FatalError(TAG, "Failed to link program");
    }

    glDeleteShader(vsh);
    glDeleteShader(fsh);

    program = _program;
  }

  void CreateNativeClientBuffer() const {
    // Android hardware buffer
    PFN_AHB_ALLOCATE pfn_alloc = GetAHardwareBuffer_Allocate();
    if (pfn_alloc == nullptr) return;

    AHardwareBuffer_Desc h_desc = {};
    h_desc.width = 400;
    h_desc.height = 400;
    h_desc.layers = 1;
    h_desc.format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM;
    h_desc.usage = AHARDWAREBUFFER_USAGE_GPU_FRAMEBUFFER | AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN | AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN; // TODO: Consider trying this without CPU access

    pfn_alloc(&h_desc, &h_buffer);
    if (h_buffer == nullptr) {
      FatalError(TAG, "Failed to allocate AHardwareBuffer");
    }

    // EGL
    PFN_GET_EGLBUFFER pfn_get_egl_buffer = GetCreateEGLBuffer();
    if (pfn_get_egl_buffer == nullptr) {
      FatalError(TAG, "Failed to locate EGL Create Buffer");
    }

    client_buffer = pfn_get_egl_buffer(h_buffer);
    if (client_buffer == nullptr) {
      DestroyNativeClientBuffer();
      FatalError(TAG, "Failed to get client buffer");
    }

    Log::D(TAG, "CreateNativeClientBuffer() exited without error");
  }

  void DestroyNativeClientBuffer() const {
    PFN_AHB_RELEASE pfn_release = GetAHardwareBuffer_Release();
    if (pfn_release == nullptr) return;

    pfn_release(h_buffer);
    h_buffer = nullptr;
  }

  void CreateEGLImage() {
    EGLDisplay display = eglGetCurrentDisplay();
    EGLContext ctx = EGL_NO_CONTEXT; // eglGetCurrentContext();
//    EGLenum target = EGL_GL_TEXTURE_2D;
    EGLenum target = EGL_NATIVE_BUFFER_ANDROID;
    const EGLint attribs[] = { EGL_NONE };
    EGLImageKHR image = eglCreateImageKHR(display, ctx, target, client_buffer, attribs);
    if (image == EGL_NO_IMAGE_KHR) {
      EGLint err = eglGetError();
      switch (err) {
        case EGL_BAD_DISPLAY:
          Log::D(TAG, "EGL_BAD_DISPLAY");
          break;
        case EGL_BAD_CONTEXT:
          Log::D(TAG, "EGL_BAD_CONTEXT");
          break;
        case EGL_BAD_PARAMETER:
          Log::D(TAG, "EGL_BAD_PARAMETER");
          break;
        case EGL_BAD_MATCH:
          Log::D(TAG, "EGL_BAD_MATCH");
          break;
        case EGL_BAD_ACCESS:
          Log::D(TAG, "EGL_BAD_ACCESS");
          break;
        case EGL_BAD_ALLOC:
          Log::D(TAG, "EGL_BAD_ALLOC");
          break;
        default:
          Log::D(TAG, "unrecognized error");
          break;
      }
      FatalError(TAG, "Failed to create EGLImageKHR");
    }

    egl_image = image;
  }

  void GetDefaultFramebuffer() {
    GLint default_fbo = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &default_fbo);
    default_framebuffer = static_cast<GLuint>(default_fbo);
  }

  void CreateFramebuffer() {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 400, 400, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      FatalError(TAG, "some GL error before");
    }
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, egl_image); // TODO: How can well tell this works?
    err = glGetError();
    if (err != GL_NO_ERROR) {
      FatalError(TAG, "some GL error after");
    }

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
      FatalError(TAG, "Incomplete framebuffer");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, default_framebuffer);
  }

  void DrawToFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    GLenum attachment = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &attachment);

    glClearColor(1.0F, 0.5F, 0.25F, 1.0F); // expected value
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, default_framebuffer);
  }

  void ReadAHardwareBuffer() {
    PFN_AHB_LOCK pfn_lock = GetPFN_AHardwareBuffer_Lock();
    if (pfn_lock == nullptr) {
      FatalError(TAG, "Failed to locate AHardwareBuffer_lock");
    }

    PFN_AHB_UNLOCK pfn_unlock = GetPFN_AHardwareBuffer_Unlock();
    if (pfn_unlock == nullptr) {
      FatalError(TAG, "Failed to locate AHardwareBuffer_unlock");
    }

    void *buffer;
    const int lock_result = pfn_lock(h_buffer, AHARDWAREBUFFER_USAGE_CPU_READ_MASK, -1, nullptr, &buffer);
    if (lock_result != 0) {
      FatalError(TAG, "Failed to lock AHardwareBuffer");
    }

    if (buffer == nullptr) {
      FatalError(TAG, "Failed to map shared buffer memory");
    }

    // Let's try to read the first four bytes
    const char *p_buffer = reinterpret_cast<char*>(buffer);
    const char red = p_buffer[0];
    const char green = p_buffer[1];
    const char blue = p_buffer[2];
    const char alpha = p_buffer[3];

    const bool success = (red == 255) &&
        (green == 128) && (blue == 64) && (alpha == 255);

    datum d = {};
    d.success = success;
    d.red = static_cast<int>(red);
    d.green = static_cast<int>(green);
    d.blue = static_cast<int>(blue);
    d.alpha = static_cast<int>(alpha);
    Report(d);

    const int unlock_result = pfn_unlock(h_buffer, nullptr);
    if (unlock_result != 0) {
      FatalError(TAG, "Failed to unlock AHardwareBuffer");
    }
  }

 private:
//  void CheckExternalBufferExtension(
//      const std::vector<std::string>& extensions) {
//    const bool ext_available =
//        std::find(extensions.begin(), extensions.end(),
//                  "GL_EXT_external_buffer") != extensions.end();
//    std::string message = ext_available ? "true" : "false";
//    Log::D(TAG, "available: " + message);
//
//    PFNGLBUFFERSTORAGEEXTERNALEXTPROC pfn =
//        (PFNGLBUFFERSTORAGEEXTERNALEXTPROC)eglGetProcAddress(
//            "glBufferStorageExternalEXT");
//    const bool pfn_found = pfn != nullptr;
//    message = pfn_found ? "true" : "false";
//    Log::D(TAG, "pfn: " + message);
//
//    //    GLenum target = 0;
//    //    GLintptr offset = 0;
//    //    GLsizeiptr size = 0;
//    //    GLeglClientBufferEXT client_buf = 0;
//    //    GLbitfield flags = 0;
//    //    pfn(target, offset, size, client_buf, flags);
//
//    datum d{"GL_EXT_external_buffer", ext_available, pfn_found};
//    Report(d);
//  }
//
//  void CheckNativeBufferExtension(const std::vector<std::string>& extensions) {
//    const bool ext_available =
//        std::find(extensions.begin(), extensions.end(),
//                  "EGL_ANDROID_create_native_client_buffer") !=
//        extensions.end();
//    std::string message = ext_available ? "true" : "false";
//    Log::D(TAG, "available: " + message);
//
//    PFNEGLCREATENATIVECLIENTBUFFERANDROIDPROC pfn =
//        (PFNEGLCREATENATIVECLIENTBUFFERANDROIDPROC)eglGetProcAddress(
//            "eglCreateNativeClientBufferANDROID");
//    const bool pfn_found = pfn != nullptr;
//    message = pfn_found ? "true" : "false";
//    Log::D(TAG, "pfn: " + message);
//
//    datum d{"EGL_ANDROID_create_native_client_buffer", ext_available,
//            pfn_found};
//    Report(d);
//  }
//
//  void CheckLib(const std::string& lib_name, const std::string& ext_name) {
//    void *lib = dlopen(lib_name.c_str(), RTLD_NOW | RTLD_LOCAL);
//    if (lib == nullptr) {
//      Log::D(TAG, "cannot load " + lib_name);
//      return;
//    }
//
//    void *pfn = (void*)dlsym(lib, ext_name.c_str());
//    if (pfn == nullptr) {
//      Log::D(TAG, "cannot find symbol for %s", ext_name.c_str());
//      return;
//    }
//
//    Log::D(TAG, "found symbol for %s!", ext_name.c_str());
//  }

 private:
  configuration _configuration;
  EGLContext _egl_context = EGL_NO_CONTEXT;
};

EXPORT_ANCER_OPERATION(ExternalBufferGLES3Operation)