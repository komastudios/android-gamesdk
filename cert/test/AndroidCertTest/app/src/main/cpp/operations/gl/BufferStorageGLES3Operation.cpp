/*
* Copyright 2019 The Android Open Source Project
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

#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/util/Json.hpp>

using namespace ancer;


//==================================================================================================

namespace {
constexpr Log::Tag TAG{"BufferStorageGLES3Operation"};
}

//==================================================================================================

static const auto *GL_EXT_BUFFER_STORAGE = "GL_EXT_buffer_storage";
static const auto *GL_BUFFER_STORAGE_EXT_FUNCTION_NAME = "glBufferStorageEXT";

enum OneTimeCheckStatus {
  DontKnow,
  No,
  Yes
};

static OneTimeCheckStatus _buffer_storage_is_in_gl_extension_list{DontKnow};

static OneTimeCheckStatus _buffer_storage_function_in_driver{DontKnow};

static PFNGLBUFFERSTORAGEEXTPROC _glBufferStorageEXT{nullptr};

namespace {
/**
 * Verifies whether the buffer storage feature is in the GL list of extensions.
 */
bool IsBufferStorageInGLExtensionList() {
  if (_buffer_storage_is_in_gl_extension_list==DontKnow) {
    _buffer_storage_is_in_gl_extension_list =
        glh::IsExtensionSupported(GL_EXT_BUFFER_STORAGE) ? Yes : No;
  }
  return _buffer_storage_is_in_gl_extension_list==Yes;
}

/**
 * Obtains, from the Open GL ES driver, the address to the function glBufferStorageEXT
 * @return the function address if available in the driver; nullptr otherwise.
 */
PFNGLBUFFERSTORAGEEXTPROC ObtainBufferStorageFunctionPtr() {
  if (_buffer_storage_function_in_driver==DontKnow) {
    _glBufferStorageEXT = reinterpret_cast<PFNGLBUFFERSTORAGEEXTPROC>(
        eglGetProcAddress(GL_BUFFER_STORAGE_EXT_FUNCTION_NAME)
    );
    _buffer_storage_function_in_driver = _glBufferStorageEXT ? Yes : No;
  }

  return _glBufferStorageEXT;
}

}
//==================================================================================================

namespace {

enum BufferStorageStatus : int;

struct buffer_storage_info {
  bool available;
  BufferStorageStatus status;
};

struct datum {
  buffer_storage_info buffer_storage;
};

JSON_WRITER(datum) {
  JSON_REQVAR(buffer_storage);
}

JSON_WRITER(buffer_storage_info) {
  JSON_REQVAR(available);
  JSON_REQVAR(status);
}

enum BufferStorageStatus : int {
  // Feature ready to be used
      AVAILABLE,

  // The GLES extensions list doesn't include Buffer Storage.
      MISSING_GLES_EXTENSION,

  // OpenGL ES driver doesn't have an entry for this function
      MISSING_FUNCTION_IN_DRIVER,

  // A regular call to BufferData() failed when it wasn't supposed to
      FAILURE_SETTING_MUTABLE_STORE,

  // An attempt to deallocate a mutable store unexpectedly failed
      FAILURE_CHANGING_MUTABLE_STORE,

  // A regular call to BufferStorageEXT() failed when it wasn't supposed to
      FAILURE_SETTING_IMMUTABLE_STORE,

  // A call to BufferStorageEXT or BufferData succeeded when a previous call to BufferStorageEXT
  // was supposed to have locked down mutability
      UNEXPECTED_SUCCESS_MUTATING_IMMUTABLE_STORE
};
}

//==================================================================================================

template<GLenum TARGET, GLsizeiptr SIZE>
class GLES3BufferObject {
 public:
  GLES3BufferObject() : _buffer_id{0} {
    glGenBuffers(1, &_buffer_id);
    glh::CheckGlError("creating a buffer object");
  }

  virtual ~GLES3BufferObject() {
    if (_buffer_id > 0) {
      glDeleteBuffers(1, &_buffer_id);
      glh::CheckGlError("destroying a buffer object");
    }
  }

  bool SetMutableDataStore() {
    if (SetBufferAsCurrent()) {
      glBufferData(TARGET, SIZE, nullptr, GL_MAP_READ_BIT);

      return not glh::CheckGlError("setting mutable data storage to buffer object");
    }

    return false;
  }

  bool SetImmutableDataStore() {
    if (SetBufferAsCurrent()) {
      auto glBufferStorageEXT{::ObtainBufferStorageFunctionPtr()};
      if (glBufferStorageEXT) {
        glBufferStorageEXT(TARGET, SIZE, nullptr, GL_MAP_READ_BIT);
      }

      return not glh::CheckGlError("setting immutable data storage to buffer object");
    }

    return false;
  }

 private:
  bool SetBufferAsCurrent() {
    glBindBuffer(TARGET, _buffer_id);
    return not glh::CheckGlError("binding a buffer object");
  }

  uint _buffer_id;
};

//==================================================================================================

class BufferStorageGLES3Operation : public BaseGLES3Operation {
 public:
  void OnGlContextReady(const GLContextConfig &ctx_config) override {
    // 1st stop: is the feature listed in OpenGL ES extensions?
    if (not::IsBufferStorageInGLExtensionList()) {
      Report(datum{{false, MISSING_GLES_EXTENSION}});
      return;
    }

    // 2nd stop: is the storage function available in the driver library?
    if (::ObtainBufferStorageFunctionPtr()==nullptr) {
      Report(datum{{false, MISSING_FUNCTION_IN_DRIVER}});
      return;
    }

    // So far, so good. Let's spare up a new OpenGL ES buffer object.
    auto buffer = GLES3BufferObject<GL_ARRAY_BUFFER, sizeof(float_t)*10>();

    // 3rd stop: it should be possible to allocate a mutable store for the buffer
    if (not buffer.SetMutableDataStore()) {
      Report(datum{{false, FAILURE_SETTING_MUTABLE_STORE}});
      return;
    }

    // 4th stop: it should be possible to deallocate/reallocate another mutable store for the buffer
    if (not buffer.SetMutableDataStore()) {
      Report(datum{{false, FAILURE_CHANGING_MUTABLE_STORE}});
      return;
    }

    // 5th stop: it should be possible to deallocate the second mutable store, allocate an immutable
    // one.
    if (not buffer.SetImmutableDataStore()) {
      Report(datum{{false, FAILURE_SETTING_IMMUTABLE_STORE}});
      return;
    }

    // 6th stop: it shouldn't be possible to deallocate the immutable store, allocating a brand new
    // mutable one.
    if (buffer.SetMutableDataStore()) {
      Report(datum{{false, UNEXPECTED_SUCCESS_MUTATING_IMMUTABLE_STORE}});
      return;
    }

    // 7th and last stop: it shouldn't be possible to deallocate the immutable store, allocating a
    // brand new immutable one.
    if (buffer.SetImmutableDataStore()) {
      Report(datum{{false, UNEXPECTED_SUCCESS_MUTATING_IMMUTABLE_STORE}});
      return;
    }

    Report(datum{{true, AVAILABLE}});
  }
};

EXPORT_ANCER_OPERATION(BufferStorageGLES3Operation)
