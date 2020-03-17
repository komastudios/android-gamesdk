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
 * This test gathers data to determine if EGL_ANDROID_presentation_time
 * extension is present and works.
 *
 * The presentation time extension allows the application to set a minimum bound
 * on the time which a given frame will be presented to the display. This
 * minimum bound value is a "timestamp" in nanoseconds, relative to the native
 * monotonic clock.
 *
 * The test evaluates the extension on two fronts. First, it determines if the
 * extension is in EGL's list of available extensions, and if the extension's
 * function can be loaded from the library. Second, it sets the presentation
 * time for a series of frames and queries the actual presentation time via the
 * EGL_ANDROID_get_frame_timestamps extension. Therefore, this test also depends
 * on the EGL_ANDROID_get_frame_timestamps extension. If a test has the primary
 * extension we're testing (EGL_ANDROID_presentation_time) but not the other one
 * we depend on, the test will not proceed, since we would be unable to evaluate
 * the efficacy of setting a frame's presentation time. In such a case, the test
 * would be inconclusive.
 *
 * Input configuration:
 *
 * - first_frame_id: Integer id of first frame in the range of collected frames.
 *                   (Note, this is the "absolute" frame id, where the first
 *                   frame the application draws is 0. EGL may choose a
 *                   different starting point when generating its frame ids.)
 * - last_frame_id:  Integer id of last frame in the range of collected frames.
 * - presentation_delay_ms: The number of milliseconds between when a frame
 *                          begins to render and the earliest possible time it
 *                          may be displayed to the screen.
 *
 * Output report:
 *
 * - extension_availability_datum:
 *   - enabled_surface_timestamps:            True if surface timestamps were
 *                                            enabled.
 *   - ext_egl_android_presentation_time:     True if extension was available.
 *   - pfn_presentation_time_android:         True if function pointer to
 *                                            eglPresentationTimeAndroid
 *                                            was not null.
 *   - ext_egl_android_get_frame_timestamps:  True if extension was available.
 *   - pfn_get_next_frame_id:                 True if function pointer to
 *                                            eglGetNextFrameIdANDROID
 *                                            was not null.
 *   - pfn_get_frame_timestamps:              True if function pointer to
 *                                            eglGetFrameTimestampsANDROID
 *                                            was not null.
 *   - pfn_get_frame_timestamp_supported:     True if function pointer to
 *                                            eglGetFrameTimestampSupported-
 *                                            ANDROID was not null.
 *   - request_timestamp_supported:           True if timestamp querying for
 *                                            EGL_REQUESTED_PRESENT_TIME_ANDROID
 *                                            is available.
 *   - present_timestamp_supported:           True if timestamp querying for
 *                                            EGL_DISPLAY_PRESENT_TIME_ANDROID
 *                                            is available.
 *
 * - frame_presentation_datum:  Results from an individual frame.
 *   - frame_id:    Frame id from eglGetNextFrameIdANDROID.
 *   - requested:   The timestamp value passed to eglPresentationTimeAndroid.
 *     - time:          Requested time in nanoseconds.
 *     - set:           True if eglPresentationTimeAndroid succeeded.
 *   - reported:    Values reported by eglGetFrameTimestampsANDROID.
 *     - requested:   EGL's report of when we requested present time.
 *       - time:        EGL_REQUESTED_PRESENT_TIME_ANDROID timestamp.
 *       - set:         True if query to get timestamp succeeded.
 *     - actual:      EGL's report of when device actually presented the frame.
 *       - time:        EGL_DISPLAY_PRESENT_TIME_ANDROID timestamp.
 *       - set:         True if query to get timestamp succeeded.
 *     - requested_lte_actual:  True if the requested present time was less than
 *                              or equal to actual present time, indicating
 *                              success.
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

const int64_t kSecToNano = 1'000'000'000;
const int64_t kMillisecToNano = 1'000'000;

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
  bool enabled_surface_timestamps;

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
  ADD_DATUM_MEMBER(w, d, enabled_surface_timestamps);

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
  bool requested_lte_actual;
};

void WriteDatum(report_writers::Struct w, const reported_time_datum& d) {
  ADD_DATUM_MEMBER(w, d, requested);
  ADD_DATUM_MEMBER(w, d, actual);
  ADD_DATUM_MEMBER(w, d, requested_lte_actual);
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

    // Test evaluation and reporting begins here.
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

    _extension_availability.enabled_surface_timestamps =
        static_cast<bool>(did_set_surface);
  }

  // ---------------------------------------------------------------------------

  /**
   * Determines and reports availability of the functionality required to
   * perform the test, dependent on the EGL_ANDROID_presentation_time and
   * EGL_ANDROID_get_frame_timestamps extensions.
   */
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

  /**
   * Checks that all required extensions and library functions are available
   * for performing frame presentation queries. If not, the entire test stops.
   */
  void CheckCanPerformTest() {
    const bool can_perform_test =
        _extension_availability.enabled_surface_timestamps &&
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

  /**
   * Queues and processes frame presentation queries.
   */
  void FrameTick() {
    CheckFrames();

    if (_absolute_frame_id >= _configuration.first_frame_id &&
        _absolute_frame_id <= _configuration.last_frame_id) {
      AddFrame();
    }

    ++_absolute_frame_id;
  }

  // ---------------------------------------------------------------------------

  /**
   * Adds a frame to be processed, setting its presentation time and queueing
   * it for subsequent timestamp collection.
   */
  void AddFrame() {
    EGLuint64KHR next_frame_id;
    if (!GetNextFrameId(&next_frame_id)) {
      FatalError(TAG, "Failed to get next frame id");
    }

    frame_presentation_datum frame = {};
    frame.frame_id = static_cast<uint64_t>(next_frame_id);
    frame.requested.time = NanosecondsFromNow(
        _configuration.presentation_delay_ms * kMillisecToNano);
    frame.requested.set =
        SetPresentationTime(static_cast<EGLnsecsANDROID>(frame.requested.time));

    _frames.push_back(frame);
  }

  bool GetNextFrameId(EGLuint64KHR* next_frame_id) const {
    const EGLDisplay display = eglGetCurrentDisplay();
    const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
    const EGLBoolean ok = pfnGetNextFrameId(display, surface, next_frame_id);
    return static_cast<bool>(ok);
  }

  /**
   * Returns the time `offset_ns` nanoseconds from now.
   */
  int64_t NanosecondsFromNow(int64_t offset_ns) {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    const int64_t current_time_ns =
        static_cast<int64_t>(t.tv_sec) * kSecToNano +
        static_cast<int64_t>(t.tv_nsec);
    return current_time_ns + offset_ns;
  }

  /**
   * Sets the presentation time for the next frame to be swapped (i.e., the one
   * for which draw commands can currently be recorded.)
   * This specifies the earliest time a frame should be presented, not the exact
   * time it will be presented to the display.
   */
  bool SetPresentationTime(EGLnsecsANDROID scheduled_time) {
    const EGLDisplay display = eglGetCurrentDisplay();
    const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
    return static_cast<bool>(
        pfnPresentationTime(display, surface, scheduled_time));
  }

  // ---------------------------------------------------------------------------

  void CheckFrames() {
    for (auto& frame : _frames) {
      if (!frame.timestamp_collection_finished) {
        UpdateFrameTimestamps(frame);
      }
    }
  }

  /**
   * Attempts to get the timestamps for a given frame if they have not already
   * been collected.
   */
  void UpdateFrameTimestamps(frame_presentation_datum& frame) {
    if (frame.timestamp_collection_finished) return;

    bool failed = false;
    if (!frame.reported.requested.set) {
      const TimestampResult result =
          GetTimestamp(frame.frame_id, EGL_REQUESTED_PRESENT_TIME_ANDROID,
                       frame.reported.requested.time);
      frame.reported.requested.set = TimestampResult::OK == result;
      if (result == TimestampResult::FAIL) failed = true;
    }

    if (!frame.reported.actual.set) {
      const TimestampResult result =
          GetTimestamp(frame.frame_id, EGL_DISPLAY_PRESENT_TIME_ANDROID,
                       frame.reported.actual.time);
      frame.reported.actual.set = TimestampResult::OK == result;
      if (result == TimestampResult::FAIL) failed = true;
    }

    if (failed || (frame.reported.requested.set && frame.reported.actual.set)) {
      frame.timestamp_collection_finished = true;
      frame.reported.requested_lte_actual =
          frame.reported.requested.time < frame.reported.actual.time;
      Report(frame);
    }
  }

  /**
   * OK:      timestamp was successfully queried.
   * FAIL:    timestamp was unreadable or expired.
   * PENDING: timestamp not yet available, try later.
   * INVALID: the event being queried did not (or will not) occur.
   */
  enum class TimestampResult { OK, FAIL, PENDING, INVALID };

  /**
   * If the requested timestamp is available, sets `timestamp` to the resulting
   * value and returns OK. If PENDING is returned, the function can be called
   * again at a later time to attempt to retrieve the timestamp.
   * `event_name` may be EGL_REQUESTED_PRESENT_TIME_ANDROID,
   * EGL_DISPLAY_PRESENT_TIME_ANDROID, etc.
   */
  TimestampResult GetTimestamp(uint64_t frame_id, EGLint event_name,
                               int64_t& timestamp) {
    const EGLDisplay display = eglGetCurrentDisplay();
    const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    EGLnsecsANDROID t;
    const EGLuint64KHR id = static_cast<EGLuint64KHR>(frame_id);
    const EGLBoolean ok =
        pfnGetFrameTimestamps(display, surface, id, 1, &event_name, &t);
    if (!ok) return TimestampResult::FAIL;

    if (EGL_TIMESTAMP_INVALID_ANDROID == t) return TimestampResult::INVALID;
    if (EGL_TIMESTAMP_PENDING_ANDROID == t) return TimestampResult::PENDING;

    timestamp = static_cast<int64_t>(t);
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
