/*
 * Copyright 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * TODO(baxtermichael@google.com): Test description.
 */

#include <time.h>

#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/LibEGL.hpp>

using namespace ancer;
using std::string;
using std::vector;

// =============================================================================

namespace {
constexpr Log::Tag TAG{"EGLPresentationTimeGLES3Operation"};

const string kExtPresentationTime = "EGL_ANDROID_presentation_time";
const string kExtGetFrameTimestamps = "EGL_ANDROID_get_frame_timestamps";

const int64_t kSecToNano = 1000 * 1000 * 1000;
const int64_t kMillisecToNano = 1000 * 1000;

// -----------------------------------------------------------------------------

struct configuration {
  int first_frame_id;
  int last_frame_id;
  int64_t presentation_delay_ms;
};

JSON_CONVERTER(configuration) {
  JSON_OPTVAR(first_frame_id);
  JSON_OPTVAR(last_frame_id);
  JSON_OPTVAR(presentation_delay_ms);
}

// -----------------------------------------------------------------------------

struct extension_availability_datum {
  bool did_enable_surface_egl_timestamps;
  bool ext_egl_android_presentation_time;
  bool pfn_presentation_time_android;
  bool ext_egl_android_get_frame_timestamps;
  bool pfn_get_next_frame_id;
  bool pfn_get_frame_timestamps;
  bool pfn_get_frame_timestamp_supported;
  bool request_timestamp_supported;
  bool present_timestamp_supported;
};

void WriteDatum(report_writers::Struct w,
                const extension_availability_datum& d) {
  ADD_DATUM_MEMBER(w, d, did_enable_surface_egl_timestamps);
  ADD_DATUM_MEMBER(w, d, ext_egl_android_presentation_time);
  ADD_DATUM_MEMBER(w, d, pfn_presentation_time_android);
  ADD_DATUM_MEMBER(w, d, ext_egl_android_get_frame_timestamps);
  ADD_DATUM_MEMBER(w, d, pfn_get_next_frame_id);
  ADD_DATUM_MEMBER(w, d, pfn_get_frame_timestamps);
  ADD_DATUM_MEMBER(w, d, pfn_get_frame_timestamp_supported);
  ADD_DATUM_MEMBER(w, d, request_timestamp_supported);
  ADD_DATUM_MEMBER(w, d, present_timestamp_supported);
}

struct time_datum {
  int64_t time;
  bool set;
};

void WriteDatum(report_writers::Struct w, const time_datum& d) {
  ADD_DATUM_MEMBER(w, d, time);
  ADD_DATUM_MEMBER(w, d, set);
}

struct reported_time_datum {
  time_datum requested;
  time_datum actual;
};

void WriteDatum(report_writers::Struct w, const reported_time_datum& d) {
  ADD_DATUM_MEMBER(w, d, requested);
  ADD_DATUM_MEMBER(w, d, actual);
}

struct frame_presentation_datum {
  uint64_t frame_id;
  time_datum requested;
  reported_time_datum reported;
  bool timestamp_collection_finished;
};

void WriteDatum(report_writers::Struct w, const frame_presentation_datum& d) {
  ADD_DATUM_MEMBER(w, d, frame_id);
  ADD_DATUM_MEMBER(w, d, requested);
  ADD_DATUM_MEMBER(w, d, reported);
}

}  // namespace

// =============================================================================

namespace {
PFNEGLPRESENTATIONTIMEANDROIDPROC pfnPresentationTime =
    libegl::PfnPresentationTime();

PFNEGLGETNEXTFRAMEIDANDROIDPROC pfnGetNextFrameId = libegl::PfnGetNextFrameId();

PFNEGLGETFRAMETIMESTAMPSUPPORTEDANDROIDPROC pfnGetFrameTimestampSupported =
    libegl::PfnGetFrameTimestampSupported();

PFNEGLGETFRAMETIMESTAMPSANDROIDPROC pfnGetFrameTimestamps =
    libegl::PfnGetFrameTimestamps();
}  // namespace

class EGLPresentationTimeGLES3Operation : public BaseGLES3Operation {
 public:
  EGLPresentationTimeGLES3Operation() = default;

  void OnGlContextReady(const GLContextConfig& ctx_config) override {
    Log::D(TAG, "GlContextReady");
    _configuration = GetConfiguration<configuration>();

    SetHeartbeatPeriod(0ms);

    _egl_context = eglGetCurrentContext();
    if (_egl_context == EGL_NO_CONTEXT) {
      FatalError(TAG, "No EGL context available");
    }

    EnableSurfaceTimestamps();  // Step 1
    CheckAvailability();        // Step 2
    CheckCanPerformTest();      // Step 3
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);
    FrameTick();  // Step 4
  }

 private:
  void EnableSurfaceTimestamps() {
    const EGLDisplay display = eglGetCurrentDisplay();
    const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    const EGLBoolean did_set_surface =
        eglSurfaceAttrib(display, surface, EGL_TIMESTAMPS_ANDROID, EGL_TRUE);

    _extension_availability.did_enable_surface_egl_timestamps =
        static_cast<bool>(did_set_surface);
  }

  // ---------------------------------------------------------------------------

  void CheckAvailability() {
    // Check extensions
    _extension_availability.ext_egl_android_get_frame_timestamps =
        glh::CheckEglExtension(kExtGetFrameTimestamps);
    _extension_availability.ext_egl_android_presentation_time =
        glh::CheckEglExtension(kExtPresentationTime);

    // Check extension's function pointers
    _extension_availability.pfn_presentation_time_android =
        nullptr != pfnPresentationTime;
    _extension_availability.pfn_get_next_frame_id =
        nullptr != pfnGetNextFrameId;
    _extension_availability.pfn_get_frame_timestamps =
        nullptr != pfnGetFrameTimestamps;
    _extension_availability.pfn_get_frame_timestamp_supported =
        nullptr != pfnGetFrameTimestampSupported;

    // Check for "request" and "present" timestamp support.
    if (_extension_availability.pfn_get_frame_timestamp_supported) {
      const EGLDisplay display = eglGetCurrentDisplay();
      const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

      const EGLBoolean request_supported = pfnGetFrameTimestampSupported(
          display, surface, EGL_REQUESTED_PRESENT_TIME_ANDROID);
      _extension_availability.request_timestamp_supported =
          static_cast<bool>(request_supported);

      const EGLBoolean present_supported = pfnGetFrameTimestampSupported(
          display, surface, EGL_DISPLAY_PRESENT_TIME_ANDROID);
      _extension_availability.present_timestamp_supported =
          static_cast<bool>(present_supported);
    }

    Report(_extension_availability);
  }

  // ---------------------------------------------------------------------------

  void CheckCanPerformTest() {
    const bool can_perform_test =
        _extension_availability.did_enable_surface_egl_timestamps &&
        _extension_availability.ext_egl_android_presentation_time &&
        _extension_availability.pfn_presentation_time_android &&
        _extension_availability.ext_egl_android_get_frame_timestamps &&
        _extension_availability.pfn_get_next_frame_id &&
        _extension_availability.pfn_get_frame_timestamps &&
        _extension_availability.pfn_get_frame_timestamp_supported &&
        _extension_availability.request_timestamp_supported &&
        _extension_availability.present_timestamp_supported;

    if (!can_perform_test) {
      Stop();
    }
  }

  // ---------------------------------------------------------------------------

  void FrameTick() {
    CheckFrames();

    if (_absolute_frame_id >= _configuration.first_frame_id &&
        _absolute_frame_id <= _configuration.last_frame_id) {
      AddFrame();
    }

    ++_absolute_frame_id;
  }

  // ---------------------------------------------------------------------------

  void AddFrame() {
    EGLuint64KHR next_frame_id;
    if (!GetNextFrameId(&next_frame_id)) {
      FatalError(TAG, "Failed to get next frame id");
    }

    frame_presentation_datum frame = {};
    frame.frame_id = static_cast<uint64_t>(next_frame_id);
    frame.requested.time = GetNextFramePresentTime();
    frame.requested.set =
        SetPresentationTime(frame.frame_id, frame.requested.time);

    _frames.push_back(frame);
  }

  bool GetNextFrameId(EGLuint64KHR* next_frame_id) const {
    const EGLDisplay display = eglGetCurrentDisplay();
    const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
    const EGLBoolean ok = pfnGetNextFrameId(display, surface, next_frame_id);
    return static_cast<bool>(ok);
  }

  int64_t GetNextFramePresentTime() {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    const int64_t current_time_ns = t.tv_sec * kSecToNano + t.tv_nsec;
    const int64_t frame_offset_ns =
        _configuration.presentation_delay_ms * kMillisecToNano;
    return current_time_ns + frame_offset_ns;
  }

  bool SetPresentationTime(uint64_t frame_id, int64_t scheduled_time) {
    const EGLDisplay display = eglGetCurrentDisplay();
    const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
    const EGLBoolean ok = pfnPresentationTime(
        display, surface, static_cast<EGLnsecsANDROID>(scheduled_time));
    return static_cast<bool>(ok);
  }

  // ---------------------------------------------------------------------------

  void CheckFrames() {
    for (auto& frame : _frames) {
      if (!frame.timestamp_collection_finished) {
        UpdateFrameTimestamps(frame);
      }
    }
  }

  enum class TimestampResult { OK, PENDING, INVALID, FAIL };

  void UpdateFrameTimestamps(frame_presentation_datum& frame) {
    if (frame.timestamp_collection_finished) return;

    const uint64_t id = frame.frame_id;

    bool failed = false;
    if (!frame.reported.requested.set) {
      const TimestampResult result = GetTimestamp(
          id, EGL_REQUESTED_PRESENT_TIME_ANDROID, frame.reported.requested.time,
          frame.reported.requested.set);
      if (result == TimestampResult::FAIL) failed = true;
    }

    if (!frame.reported.actual.set) {
      const TimestampResult result =
          GetTimestamp(id, EGL_DISPLAY_PRESENT_TIME_ANDROID,
                       frame.reported.actual.time, frame.reported.actual.set);
      if (result == TimestampResult::FAIL) failed = true;
    }

    if (failed || (frame.reported.requested.set && frame.reported.actual.set)) {
      frame.timestamp_collection_finished = true;
      Report(frame);
    }
  }

  TimestampResult GetTimestamp(uint64_t frame_id, EGLint name,
                               EGLnsecsANDROID& timestamp, bool& set) {
    const EGLDisplay display = eglGetCurrentDisplay();
    const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    EGLnsecsANDROID t;
    const EGLuint64KHR id = static_cast<EGLuint64KHR>(frame_id);
    const EGLBoolean ok =
        pfnGetFrameTimestamps(display, surface, id, 1, &name, &t);
    if (!ok) return TimestampResult::FAIL;

    if (EGL_TIMESTAMP_INVALID_ANDROID == t) return TimestampResult::INVALID;
    if (EGL_TIMESTAMP_PENDING_ANDROID == t) return TimestampResult::PENDING;

    timestamp = static_cast<uint64_t>(t);
    set = true;
    return TimestampResult::OK;
  }

 private:
  configuration _configuration;
  EGLContext _egl_context = EGL_NO_CONTEXT;

  extension_availability_datum _extension_availability = {};
  int _absolute_frame_id = 0;
  std::vector<frame_presentation_datum> _frames;
};

EXPORT_ANCER_OPERATION(EGLPresentationTimeGLES3Operation)
