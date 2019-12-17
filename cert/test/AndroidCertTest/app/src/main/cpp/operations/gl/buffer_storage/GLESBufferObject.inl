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

#include <ancer/util/GLHelpers.hpp>

namespace {

/**
 * This enumeration models the possible states of data gathering whose answer isn't expected to
 * change either if checked it once or n times. Namely, let's imaging that a test run checks once
 * whether OpenGL ES extension function BufferStorageEXT is available, and the answer is yes. Such
 * answer would be the same if the test checked it N more times.
 * The goal is to avoid re-checking when such action is expensive (e.g., looking up in a shared
 * dynamic library, etc).
 */
enum OneTimeCheckStatus {
  // We are yet to check at least once.
      DontKnow,

  // We've already checked and the result was negative.
      No,

  // We've already checked and the result was positive.
      Yes
};

//==================================================================================================

/**
 * This class encapsulates all OpenGL ES buffer object access. From creation to deletion, including
 * storage allocation and deallocation.
 * It's recommended a reading to chapter 6 of OpenGL ES 3.1 specification to better understand this
 * class internals.
 *
 * @tparam TARGET any buffer storage target (see table 6.1 in OpenGL ES specification)
 * @tparam SIZE the storage size in bytes.
 */
template<GLenum TARGET, GLsizeiptr SIZE>
class GLESBufferObject {

  //------------------------------------------------------------------------------------------------
  // Static (class-level) members.

 public:
  /**
   * Verifies whether the buffer storage allocation function is in the list of OpenGL ES extensions.
   * @return Yes or No.
   */
  static bool IsBufferStorageEXT_InGLESExtensionList() {
    if (_buffer_storage_in_gl_extension_list==DontKnow) {
      _buffer_storage_in_gl_extension_list =
          ancer::glh::IsExtensionSupported("GL_EXT_buffer_storage") ? Yes : No;
    }
    return _buffer_storage_in_gl_extension_list==Yes;
  }

  /**
   * Verifies whether the OpenGL ES library contains an entry for the buffer storage allocation
   * function.
   * @return Yes or No.
   */
  static bool IsBufferStorageEXT_InGLESSharedLibrary() {
    if (_buffer_storage_function_in_library==DontKnow) {
      glBufferStorageEXT = reinterpret_cast<PFNGLBUFFERSTORAGEEXTPROC>(
          eglGetProcAddress("glBufferStorageEXT")
      );
      _buffer_storage_function_in_library = glBufferStorageEXT ? Yes : No;
    }

    return _buffer_storage_function_in_library;
  }

 private:

  static OneTimeCheckStatus _buffer_storage_in_gl_extension_list;

  static OneTimeCheckStatus _buffer_storage_function_in_library;

  static PFNGLBUFFERSTORAGEEXTPROC glBufferStorageEXT;

  //------------------------------------------------------------------------------------------------
  // Instance members.

 public:
  GLESBufferObject() : _buffer_id{0} {
    glGenBuffers(1, &_buffer_id);
    ancer::glh::CheckGlError("creating a buffer object");
  }

  virtual ~GLESBufferObject() {
    if (_buffer_id > 0) {
      glDeleteBuffers(1, &_buffer_id);
      ancer::glh::CheckGlError("destroying a buffer object");
    }
  }

  /**
   * Allocates server storage for this buffer object. Such storage "lives" until this buffer object
   * is destroyed, or until either this function or AllocateImmutableStorage() are invoked.
   * Any storage previously allocated through this function is deallocated every time this function
   * is called again to allocate new buffer storage.
   * @return true if the allocation succeeded.
   */
  bool AllocateMutableStorage() const {
    if (SetBufferAsCurrent()) {
      glBufferData(TARGET, SIZE, nullptr, GL_DYNAMIC_DRAW);

      return not ancer::glh::CheckGlError("setting mutable data storage to buffer object");
    }

    return false;
  }

  /**
   * Allocates server storage for this buffer object. Such storage "lives" until this buffer object
   * is destroyed. Invoking this function twice or more for a same instance should not succeed if
   * OpenGL ES extension BufferStorageEXT is available and fully working.
   * Any storage previously allocated via AllocateMutableStorage() is deallocated when this function
   * is invoked.
   * @return true if the allocation succeeded.
   */
  bool AllocateImmutableStorage() const {
    if (SetBufferAsCurrent()) {
      if (IsBufferStorageEXT_InGLESSharedLibrary()) {
        glBufferStorageEXT(TARGET, SIZE, nullptr, GL_MAP_READ_BIT);
      }

      return not ancer::glh::CheckGlError("setting immutable data storage to buffer object");
    }

    return false;
  }

 private:
  bool SetBufferAsCurrent() const {
    glBindBuffer(TARGET, _buffer_id);
    return not ancer::glh::CheckGlError("binding a buffer object");
  }

  uint _buffer_id;
};

template<GLenum TARGET, GLsizeiptr SIZE>
OneTimeCheckStatus GLESBufferObject<TARGET, SIZE>::_buffer_storage_in_gl_extension_list = DontKnow;

template<GLenum TARGET, GLsizeiptr SIZE>
OneTimeCheckStatus GLESBufferObject<TARGET, SIZE>::_buffer_storage_function_in_library = DontKnow;

template<GLenum TARGET, GLsizeiptr SIZE>
PFNGLBUFFERSTORAGEEXTPROC GLESBufferObject<TARGET, SIZE>::glBufferStorageEXT = nullptr;
}