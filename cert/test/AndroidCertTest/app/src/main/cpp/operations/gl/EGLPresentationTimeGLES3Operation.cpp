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

// -----------------------------------------------------------------------------

struct configuration {
  int first_frame_id;
  int last_frame_id;
};

JSON_CONVERTER(configuration) {
  JSON_OPTVAR(first_frame_id);
  JSON_OPTVAR(last_frame_id);
}

// -----------------------------------------------------------------------------

struct extension_availability_datum {
  bool did_enable_surface_egl_timestamps;
  bool has_ext_egl_android_presentation_time;
  bool has_fp_presentation_time_android;
  bool has_ext_egl_android_get_frame_timestamps;
  bool has_fp_get_next_frame_id;
  bool has_fp_get_frame_timestamps;
  bool has_fp_get_frame_timestamp_supported;
  bool request_present_time_supported;
  bool display_present_time_supported;
};

struct frame_presentation_datum {
  uint64_t frame_id;

  int64_t specified_present_time;
  bool did_set_present_time;

  int64_t reported_request_time;  // TODO: organize
  int64_t reported_present_time;
  bool captured_reported_request_time;
  bool captured_reported_present_time;
  bool timestamp_collection_finished;
};

// -----------------------------------------------------------------------------

void WriteDatum(report_writers::Struct w,
                const extension_availability_datum& d) {
  ADD_DATUM_MEMBER(w, d, did_enable_surface_egl_timestamps);
  ADD_DATUM_MEMBER(w, d, has_ext_egl_android_presentation_time);
  ADD_DATUM_MEMBER(w, d, has_fp_presentation_time_android);
  ADD_DATUM_MEMBER(w, d, has_ext_egl_android_get_frame_timestamps);
  ADD_DATUM_MEMBER(w, d, has_fp_get_next_frame_id);
  ADD_DATUM_MEMBER(w, d, has_fp_get_frame_timestamps);
  ADD_DATUM_MEMBER(w, d, has_fp_get_frame_timestamp_supported);
  ADD_DATUM_MEMBER(w, d, request_present_time_supported);
  ADD_DATUM_MEMBER(w, d, display_present_time_supported);
}

void WriteDatum(report_writers::Struct w, const frame_presentation_datum& d) {
  ADD_DATUM_MEMBER(w, d, frame_id);
  ADD_DATUM_MEMBER(w, d, specified_present_time);
  ADD_DATUM_MEMBER(w, d, did_set_present_time);
  ADD_DATUM_MEMBER(w, d, reported_request_time);
  ADD_DATUM_MEMBER(w, d, reported_present_time);
  ADD_DATUM_MEMBER(w, d, captured_reported_request_time);
  ADD_DATUM_MEMBER(w, d, captured_reported_present_time);
}

}  // namespace

// =============================================================================

/**
 * https://www.khronos.org/registry/EGL/extensions/ANDROID/EGL_ANDROID_presentation_time.txt
 *
 * Our plan is to check if the extension is listed as available.
 * We also want to check if the function pointer can be loaded from the library.
 * We assume that the pointer might be available even if the extension report
 * is negative, so we don't want to short-circuit too early.
 * We will report the status of both, and then continue on with the test only
 * if the function pointer is available (*unless* during development we
 * determine that we don't need the FP.)
 *
 * There's actually another intermediate step we need here, because we need
 * the other extension so we can get the display timing. So we have to be able
 * to load the FP from that and verify that the display present timing is
 * available. (Should we still attempt to measure present values anyway, even
 * if we can't load the other extension? I suppose it's possible that this might
 * be the only way to determine if that function hard-crashes in this edge case)
 *
 * The second phase is to actually test the extension. We must get the next
 * frame id, choose some arbitrary point before which the frame must not be
 * presented, submit the frame with that present info, and then check to see
 * when the frame was presented. We *could* interpret that value from the app,
 * but regardless, we need to send the values back in the report.json.
 *
 * We could argue that this test can only be validated if the get frame
 * timestamps extension works. Actually, that's not entirely true. We could try
 * to depend on the Draw() calls and interleaving to perform some sort of upper-
 * bound checking. Perhaps we could just leave a note about that and proceed
 * with in a single fashion (which might even cover all cases)?
 *
 * Do test precheck
 *   A. Check "presentation time" extension.
 *     - Extension reported?
 *     - Function pointer found?
 *     - Ready?
 *   B. Check "get frame timestamps" extension.
 *     - Extension reported?
 *     - Get next frame id function pointer found?
 *     - Get frame timestamp supported function pointer found?
 *     - Get frame timestamp function pointer found?
 *     - Is EGL_REQUESTED_PRESENT_TIME_ANDROID supported?
 *     - Is EGL_DISPLAY_PRESENT_TIME_ANDROID supported?
 *     - Ready?
 *   C. Set EGL surface to collect timestamps.
 *     - Ready?
 * Determine if we can proceed
 *   - A && B && C
 * Perform test
 *   - Wait for a few frames
 *   - Test a frame
 *     - Get next frame id
 *     - Get system time
 *     - Set time at least two frames later
 *     - Get time for that frame id
 *       - Log result of set time
 *       - Log next frame id
 *       - Log set time
 *       - Log expected time
 *       - Log actual time
 */

// =============================================================================

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

    _extension_datum.did_enable_surface_egl_timestamps =
        static_cast<bool>(did_set_surface);
  }

  void CheckAvailability() {
    // Check extensions
    _extension_datum.has_ext_egl_android_get_frame_timestamps =
        glh::CheckEglExtension(kExtGetFrameTimestamps);
    _extension_datum.has_ext_egl_android_presentation_time =
        glh::CheckEglExtension(kExtPresentationTime);

    // Check function pointers for extensions
    _extension_datum.has_fp_presentation_time_android =
        libegl::GetFP_PresentationTime() != nullptr;
    _extension_datum.has_fp_get_next_frame_id =
        libegl::GetFP_GetNextFrameId() != nullptr;
    _extension_datum.has_fp_get_frame_timestamps =
        libegl::GetFP_GetFrameTimestamps() != nullptr;
    _extension_datum.has_fp_get_frame_timestamp_supported =
        libegl::GetFP_GetFrameTimestampSupported() != nullptr;

    // Check for "request" and "present" timestamp support.
    if (_extension_datum.has_fp_get_frame_timestamp_supported) {
      const auto fp = libegl::GetFP_GetFrameTimestampSupported();
      const EGLDisplay display = eglGetCurrentDisplay();
      const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

      _extension_datum.request_present_time_supported = static_cast<bool>(
          fp(display, surface, EGL_REQUESTED_PRESENT_TIME_ANDROID));
      _extension_datum.display_present_time_supported = static_cast<bool>(
          fp(display, surface, EGL_DISPLAY_PRESENT_TIME_ANDROID));
    }

    Report(_extension_datum);
  }

  void CheckCanPerformTest() {
    bool can_perform_test =
        _extension_datum.did_enable_surface_egl_timestamps &&
        _extension_datum.has_ext_egl_android_presentation_time &&
        _extension_datum.has_fp_presentation_time_android &&
        _extension_datum.has_ext_egl_android_get_frame_timestamps &&
        _extension_datum.has_fp_get_next_frame_id &&
        _extension_datum.has_fp_get_frame_timestamps &&
        _extension_datum.has_fp_get_frame_timestamp_supported &&
        _extension_datum.request_present_time_supported &&
        _extension_datum.display_present_time_supported;

    if (!can_perform_test) {
      // TODO(baxtermichael@google.com): Consider signaling stop in results.json
      Stop();
    }
  }

  void FrameTick() {
    CheckFrames();

    // TODO(baxtermichael@google.com): Get values from config.
    if (_absolute_frame_id == 20 || _absolute_frame_id == 40) {
      AddFrame();
    }

    ++_absolute_frame_id;
  }

  void AddFrame() {
    EGLuint64KHR next_frame_id;
    if (!GetNextFrameId(&next_frame_id)) {
      FatalError(TAG, "Failed to get next frame id");
    }

    frame_presentation_datum frame = {};
    frame.frame_id = static_cast<uint64_t>(next_frame_id);
    frame.specified_present_time = GetNextFramePresentTime();
    frame.did_set_present_time =
        SetPresentationTime(frame.frame_id, frame.specified_present_time);

    if (!frame.did_set_present_time) {
      Report(frame);
      // TODO: Add message
      Stop();
      return;
    }

    _frames.push_back(frame);
  }

  bool GetNextFrameId(EGLuint64KHR* next_frame_id) const {
    const auto fp = libegl::GetFP_GetNextFrameId();
    if (fp == nullptr || next_frame_id == nullptr) return false;

    const EGLDisplay display = eglGetCurrentDisplay();
    const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    return static_cast<bool>(fp(display, surface, next_frame_id));
  }

  int64_t GetNextFramePresentTime() {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);

    // TODO(baxtermichael@google.com):
    const int64_t current_time = t.tv_sec * 1000 * 1000 * 1000 + t.tv_nsec;
    const int64_t kFrameOffset = 1 * 16 * 1000 * 1000;  // ms
    return current_time + kFrameOffset;
    // return 1000;
  }

  bool SetPresentationTime(uint64_t frame_id, int64_t scheduled_time) {
    const auto fp = libegl::GetFP_PresentationTime();
    if (fp == nullptr) return false;

    const EGLDisplay display = eglGetCurrentDisplay();
    const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    const EGLBoolean result =
        fp(display, surface, static_cast<EGLnsecsANDROID>(scheduled_time));
    return static_cast<bool>(result);
  }

  void CheckFrames() {
    for (auto& frame : _frames) {
      if (!frame.timestamp_collection_finished) {
        UpdateFrameTimestamps(frame);
      }
    }
  }

  enum class TimestampResult { OK, PENDING, INVALID, FAIL, INVALID_POINTER };

  TimestampResult GetTimestamp(uint64_t frame_id, EGLint name,
                               EGLnsecsANDROID& timestamp) {
    const auto fp = libegl::GetFP_GetFrameTimestamps();
    if (fp == nullptr) return TimestampResult::INVALID_POINTER;

    const EGLDisplay display = eglGetCurrentDisplay();
    const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    EGLnsecsANDROID t;
    const EGLuint64KHR id = static_cast<EGLuint64KHR>(frame_id);
    const EGLBoolean ok = fp(display, surface, id, 1, &name, &t);
    if (!ok) {
      EGLint err = eglGetError();
      Log::D(TAG, "Fail : EGL error: %d", err);
      return TimestampResult::FAIL;
    }

    timestamp = static_cast<uint64_t>(t);
    if (EGL_TIMESTAMP_INVALID_ANDROID == timestamp)
      return TimestampResult::INVALID;
    if (EGL_TIMESTAMP_PENDING_ANDROID == timestamp)
      return TimestampResult::PENDING;

    return TimestampResult::OK;
  }

  // Somewhere in here there's a mix of data and logic...
  // If it's OK, just set the timestamp, set the bool > ok, move on.
  // If it's FAIL, log, set the bool > ok, move on.
  // If PENDING, just move on.
  // If INVALID, set the book > ok and move on.
  void UpdateFrameTimestamps(frame_presentation_datum& frame) {
    if (frame.timestamp_collection_finished) return;

    // frame.captured_reported_request_time = true;
    // if (frame.captured_reported_present_time &&
    // !frame.captured_reported_request_time) {
    if (!frame.captured_reported_request_time) {
      const uint64_t id = frame.frame_id + 0;
      const TimestampResult result = GetTimestamp(
          id, EGL_REQUESTED_PRESENT_TIME_ANDROID, frame.reported_request_time);
      frame.captured_reported_request_time = result != TimestampResult::PENDING;

      switch (result) {
        case TimestampResult::OK:
          Log::D(TAG, "Frame %zu, requested OK", id);
          break;
        case TimestampResult::PENDING:
          Log::D(TAG, "Frame %zu, requested PENDING", id);
          break;
        case TimestampResult::FAIL:
          Log::D(TAG, "Frame %zu, requested FAIL", id);
          break;
        case TimestampResult::INVALID:
          Log::D(TAG, "Frame %zu, requested INVALID", id);
          break;
        case TimestampResult::INVALID_POINTER:
          Log::D(TAG, "Frame %zu, requested INVALID_POINTER", id);
          break;
      }
    }

    if (!frame.captured_reported_present_time) {
      const uint64_t id = frame.frame_id - 0;
      const TimestampResult result = GetTimestamp(
          id, EGL_DISPLAY_PRESENT_TIME_ANDROID, frame.reported_present_time);
      frame.captured_reported_present_time = result != TimestampResult::PENDING;

      switch (result) {
        case TimestampResult::OK:
          Log::D(TAG, "Frame %zu, actual OK", id);
          break;
        case TimestampResult::PENDING:
          Log::D(TAG, "Frame %zu, actual PENDING", id);
          break;
        case TimestampResult::FAIL:
          Log::D(TAG, "Frame %zu, actual FAIL", id);
          break;
        case TimestampResult::INVALID:
          Log::D(TAG, "Frame %zu, actual INVALID", id);
          break;
        case TimestampResult::INVALID_POINTER:
          Log::D(TAG, "Frame %zu, actual INVALID_POINTER", id);
          break;
      }
    }

    if (frame.captured_reported_request_time &&
        frame.captured_reported_present_time) {
      frame.timestamp_collection_finished = true;
      Report(frame);
    }
  }

 private:
  configuration _configuration;
  EGLContext _egl_context = EGL_NO_CONTEXT;

  extension_availability_datum _extension_datum = {};

  int _absolute_frame_id = 0;
  std::vector<frame_presentation_datum> _frames;

  // TODO(baxtermichael@google.com): Cache the function pointers, could help
  // clean up code.
};

EXPORT_ANCER_OPERATION(EGLPresentationTimeGLES3Operation)
