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

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/util/Json.hpp>

#include "./buffer_storage/GLESBufferObject.inl"

using namespace ancer;

//==================================================================================================

namespace {
constexpr Log::Tag TAG{"BufferStorageGLES3Operation"};
}

//==================================================================================================

namespace {

enum BufferStorageStatus : int;

struct buffer_storage_info {
  bool available;
  BufferStorageStatus status;
};

void WriteDatum(report_writers::Struct w, const buffer_storage_info& i) {
  ADD_DATUM_MEMBER(w, i, available);
  ADD_DATUM_MEMBER(w, i, status);
}


struct datum {
  buffer_storage_info buffer_storage;
};

void WriteDatum(report_writers::Struct w, const datum& d) {
  ADD_DATUM_MEMBER(w, d, buffer_storage);
}

/**
 * This enum models the success / failure statuses that this test reports.
 * AVAILABLE means "PASSED". All the others indicate the test step in which the failure occurred.
 */
enum BufferStorageStatus : int {
  /**
   * Feature is available and works (i.e., PASS)
   */
      AVAILABLE,

  /**
   * Feature isn't available
   * It's missing in the OpenGL ES extensions list
   */
      MISSING_IN_EXTENSIONS_LIST,

  /**
   * Feature isn't available
   * It's missing in the OpenGL ES shared library
   */
      MISSING_IN_LIBRARY,

  /**
   * Feature is available but doesn't work
   * Test unexpectedly failed on a mutable store allocation
   */
      FAILURE_ALLOCATING_MUTABLE_STORE,

  /**
   * Feature is available but doesn't work
   * Test unexpectedly failed on a mutable store deallocation / reallocation
   */
      FAILURE_DEALLOCATING_MUTABLE_STORE,

  /**
   * Feature is available but doesn't work
   * Test unexpectedly failed on an immutable store allocation
   */
      FAILURE_ALLOCATING_IMMUTABLE_STORE,

  /**
   * Feature is available but doesn't work
   * Test unexpectedly succeeded on an immutable store deallocation / reallocation
   */
      UNEXPECTED_SUCCESS_DEALLOCATING_IMMUTABLE_STORE
};
}

//==================================================================================================

/**
 * OpenGL ES extension Buffer Storage test. This operation first creates a buffer object. Then it
 * allocates mutable and immutable GPU storage to it in order to verify that buffer storage
 * allocation and deallocation work as the OpenGL ES 3.x specification mandates.
 */
class BufferStorageGLES3Operation : public BaseGLES3Operation {
 public:

  void OnGlContextReady(const GLContextConfig &ctx_config) override {
    TestBufferAllocation();
  }

 private:

  /**
   * Allocates a buffer and sequentially tries to allocate, deallocate different kind of storages
   * for it. Goal is to verify that once an immutable store got allocated, it can't be deallocated
   * until the entire buffer object is destroyed.
   */
  void TestBufferAllocation() {
    auto gl_buffer_object = GLESBufferObject<GL_ARRAY_BUFFER, sizeof(float_t)*10>();

    // 1st stop: is the feature listed in OpenGL ES extensions?
    if (not gl_buffer_object.IsBufferStorageEXT_InGLESExtensionList()) {
      Report(datum{{false, MISSING_IN_EXTENSIONS_LIST}});
      return;
    }

    // 2nd stop: is the storage function available in the driver library?
    if (not gl_buffer_object.IsBufferStorageEXT_InGLESSharedLibrary()) {
      Report(datum{{false, MISSING_IN_LIBRARY}});
      return;
    }

    // 3rd stop: it should be possible to allocate a mutable store for the buffer
    if (not gl_buffer_object.AllocateMutableStorage()) {
      Report(datum{{false, FAILURE_ALLOCATING_MUTABLE_STORE}});
      return;
    }

    // 4th stop: it should be possible to deallocate/reallocate another mutable store for the buffer
    if (not gl_buffer_object.AllocateMutableStorage()) {
      Report(datum{{false, FAILURE_DEALLOCATING_MUTABLE_STORE}});
      return;
    }

    // Up to here, all fine as we were allocating / deallocating mutable buffer storage.
    // As long as mutable, we can do that multiple times.
    // Now follows to confirm that immutable storage can be allocated only once per buffer.

    // 5th stop: it should be possible to deallocate the second mutable store, allocate an immutable
    // one.
    if (not gl_buffer_object.AllocateImmutableStorage()) {
      Report(datum{{false, FAILURE_ALLOCATING_IMMUTABLE_STORE}});
      return;
    }

    // 6th stop: it shouldn't be possible to deallocate the immutable store, allocating a brand new
    // mutable one.
    if (gl_buffer_object.AllocateMutableStorage()) {
      Report(datum{{false, UNEXPECTED_SUCCESS_DEALLOCATING_IMMUTABLE_STORE}});
      return;
    }

    // 7th and last stop: it shouldn't be possible to deallocate the immutable store, allocating a
    // brand new immutable one.
    if (gl_buffer_object.AllocateImmutableStorage()) {
      Report(datum{{false, UNEXPECTED_SUCCESS_DEALLOCATING_IMMUTABLE_STORE}});
      return;
    }

    Report(datum{{true, AVAILABLE}});
  }
};

EXPORT_ANCER_OPERATION(BufferStorageGLES3Operation)
