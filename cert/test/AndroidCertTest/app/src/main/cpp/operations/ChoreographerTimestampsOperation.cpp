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
 * This test gathers timestamps from Choreographer callbacks, at the same time
 * sampling frame presentation timestamps with the
 * EGL_ANDROID_get_frame_timestamps extensions, which can later be used to
 * determine how close the Choreographer timestamps were to hardware VSYNC.
 *
 * Choreographer can be used by applications to determine deltas between frames
 * timestamps and aid in updating animations and similar state.
 *
 * Input configuration:
 * - first_frame_id: The first frame for which to capture timestamps.
 * - last_frame_id:  The last frame for which to capture timestamps.
 *
 * Output report: Expect one "availability_datum" and zero or more of
 *                "choreographer_timestamp_datum" and
 *                "egl_frame_timestamp_datum" (though not necessarily the same
 *                number of them).
 * 
 * - availability_datum:
 *   - has_egl_extension:                     True if EGL_ANDROID_get_frame_
 *                                            timestamps extension is available.
 *   - enable_surface_timestamps_success:     True if succeeded in enable
 *                                            timestamps for EGL surface.
 *   - has_fp_get_instance:                   True if function pointer for
 *                                            Choreographer getInstance
 *                                            is available.
 *   - has_fp_post_frame_callback:            True if function pointer for
 *                                            Choreographer postFrameCallback
 *                                            is available.
 *   - has_fp_get_next_frame_id:              True if function pointer for
 *                                            EGL extension getNextFrameId
 *                                            is available.
 *   - has_fp_get_frame_timestamp_supported:  True if function pointer for
 *                                            EGL ext getFrameTimestampSupported
 *                                            is available.
 * 
 * - choreographer_timestamp_datum:
 *   - choreographer_timestamp_ns: timestamp in nanoseconds corresponding with
 *                                 a callback from Choreographer.
 *   - vsync_offset_ns:            VSYNC offset from GetAppVsyncOffsetNanos.
 *
 * - egl_frame_timestamp_datum:
 *   - frame_id:               frame ID assigned by EGL.
 *   - egl_frame_timestamp_ns: timestamp in nanoseconds corresponding a frame
 *                             being presented to the screen.
 *   - success:                true if a timestamp was collected for this frame.
 */


#include <mutex>
#include <string>
#include <vector>

#include <android/looper.h>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/util/LibAndroid.hpp>
#include <ancer/util/LibEGL.hpp>
#include <ancer/Reporter.hpp>

using namespace ancer;
using std::string;
using std::vector;

// =============================================================================

namespace {
constexpr Log::Tag TAG{"ChoreographerTimestampsOperation"};

// -----------------------------------------------------------------------------

struct configuration {
  int first_frame_id;
  int last_frame_id;
};

JSON_READER(configuration) {
  JSON_REQVAR(first_frame_id);
  JSON_REQVAR(last_frame_id);
}

// -----------------------------------------------------------------------------

struct availability_datum {
  bool has_egl_extension;
  bool enable_surface_timestamps_success;
  bool has_fp_get_instance;
  bool has_fp_post_frame_callback;
  bool has_fp_get_next_frame_id;
  bool has_fp_get_frame_timestamp_supported;
};

void WriteDatum(report_writers::Struct w, const availability_datum& d) {
  ADD_DATUM_MEMBER(w, d, has_egl_extension);
  ADD_DATUM_MEMBER(w, d, enable_surface_timestamps_success);
  ADD_DATUM_MEMBER(w, d, has_fp_get_instance);
  ADD_DATUM_MEMBER(w, d, has_fp_post_frame_callback);
  ADD_DATUM_MEMBER(w, d, has_fp_get_next_frame_id);
  ADD_DATUM_MEMBER(w, d, has_fp_get_frame_timestamp_supported);
}

struct choreographer_timestamp_datum {
  long choreographer_timestamp_ns;
  int32_t vsync_offset_ns;
};

void WriteDatum(report_writers::Struct w,
                const choreographer_timestamp_datum& d) {
  ADD_DATUM_MEMBER(w, d, choreographer_timestamp_ns);
  ADD_DATUM_MEMBER(w, d, vsync_offset_ns);
}

struct egl_frame_timestamp_datum {
  uint64_t frame_id;
  int64_t egl_frame_timestamp_ns;
  bool success;
};

void WriteDatum(report_writers::Struct w, const egl_frame_timestamp_datum& d) {
  ADD_DATUM_MEMBER(w, d, frame_id);
  ADD_DATUM_MEMBER(w, d, egl_frame_timestamp_ns);
  ADD_DATUM_MEMBER(w, d, success);
}
}  // namespace

// =============================================================================

namespace {
// Choreographer functions
const auto getInstance = libandroid::GetFP_AChoreographer_getInstance();
const auto postFrameCallback =
    libandroid::GetFP_AChoreographer_postFrameCallback();

// EGL_ANDROID_get_frame_timestamps functions
const string get_frame_timestamps_ext = "EGL_ANDROID_get_frame_timestamps";
const auto getNextFrameId = libegl::PfnGetNextFrameId();
const auto getFrameTimestampSupported = libegl::PfnGetFrameTimestampSupported();
const auto getFrameTimestamps = libegl::PfnGetFrameTimestamps();
}  // namespace

// =============================================================================

namespace {
struct FrameEvent {
  EGLuint64KHR frame_id;
  EGLint timing_event;
  bool pending = true;

  FrameEvent(EGLuint64KHR frame_id, EGLint timing_event)
      : frame_id(frame_id), timing_event(timing_event) {}
};
}  // namespace

// =============================================================================

class ChoreographerTimestampsOperation : public BaseGLES3Operation {
 public:
  ChoreographerTimestampsOperation() = default;

  void Start() override {
    BaseGLES3Operation::Start();

    _configuration = GetConfiguration<configuration>();

    // A valid Looper context is required for any thread calling Choreographer's
    // postFrameCallback function, as we do when we call SetupFrameCallback().
    ALooper* looper = ALooper_forThread();
    if (looper == nullptr) {
      FatalError(TAG, "Unable to get an instance of Looper");
    }

    _choreographer_instance = getInstance();
    if (_choreographer_instance == nullptr) {
      FatalError(TAG, "Unable to get an instance of Choreographer");
    }

    SetupFrameCallback();
  }

  void OnGlContextReady(const GLContextConfig& ctx_config) override {
    Log::D(TAG, "GlContextReady");

    SetHeartbeatPeriod(0ms);

    _egl_context = eglGetCurrentContext();
    if (_egl_context == EGL_NO_CONTEXT) {
      FatalError(TAG, "No EGL context available");
    }

    // Verify availability
    bool available = ReportAvailability();
    if (!available) {
      Stop();
    }
  }

  void Wait() override {
    BaseGLES3Operation::Wait();
    // Wait until we've rendered enough frames and have received all the
    // choreographer callbacks.
    bool wait = true;
    while (wait) {
      std::lock_guard guard{_frame_counter_mutex};
      wait = _frame_counter < _configuration.last_frame_id || !IsStopped();
    }
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);

    std::lock_guard guard{_frame_counter_mutex};
    if (_frame_counter < _configuration.last_frame_id) {
      // Enqueue `FrameEvent`s (to get timestamps) for yet-to-be-rendered frames
      CreatePendingFrameEvent();
    }

    // Continue to poll pending frames, as it will take several frames for each
    // frame to make it all the way through rendering/composition/presentation.
    ReportPendingFrameTimingEvents();

    ++_frame_counter;
  }

 private:
  bool ReportAvailability() {
    availability_datum datum = {};
    datum.has_fp_get_instance = getInstance != nullptr;
    datum.has_fp_post_frame_callback = postFrameCallback != nullptr;
    datum.has_fp_get_next_frame_id = getNextFrameId != nullptr;
    datum.has_fp_get_frame_timestamp_supported =
        getFrameTimestampSupported != nullptr;
    datum.has_egl_extension = glh::CheckEglExtension(get_frame_timestamps_ext);
    datum.enable_surface_timestamps_success = EnableSurfaceTimestamps();

    Report(datum);

    return datum.has_fp_get_instance && datum.has_fp_post_frame_callback &&
           datum.has_fp_get_next_frame_id &&
           datum.has_fp_get_frame_timestamp_supported &&
           datum.has_egl_extension && datum.enable_surface_timestamps_success;
  }

  // EGL_ANDROID_get_frame_timestamps
  bool EnableSurfaceTimestamps() {
    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    const EGLBoolean did_set_surface =
        eglSurfaceAttrib(display, surface, EGL_TIMESTAMPS_ANDROID, EGL_TRUE);

    return static_cast<bool>(did_set_surface);
  }

  bool GetNextFrameId(EGLuint64KHR* next_frame_id) const {
    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    const EGLBoolean ok = getNextFrameId(display, surface, next_frame_id);
    return static_cast<bool>(ok);
  }

  void CreatePendingFrameEvent() {
    EGLuint64KHR frame_id = 0;
    if (!GetNextFrameId(&frame_id)) {
      Log::D(TAG, "CreatePendingFrameEvents : Failed to get next frame id");
      return;
    }

    _frame_events.push_back(
        FrameEvent{frame_id, EGL_DISPLAY_PRESENT_TIME_ANDROID});
  }

  void ReportPendingFrameTimingEvents() {
    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    for (auto& event : _frame_events) {
      if (!event.pending) continue;

      EGLnsecsANDROID value = 0;
      if (!getFrameTimestamps(display, surface, event.frame_id, 1,
                              &event.timing_event, &value))
        continue;  // the timestamp may be pending

      if (value == EGL_TIMESTAMP_INVALID_ANDROID) {
        event.pending = false;

        egl_frame_timestamp_datum datum = {};
        datum.frame_id = static_cast<uint64_t>(event.frame_id);
        datum.success = false;
        Report(datum);
        continue;
      }

      if (value != EGL_TIMESTAMP_PENDING_ANDROID) {
        event.pending = false;

        egl_frame_timestamp_datum datum = {};
        datum.frame_id = static_cast<uint64_t>(event.frame_id);
        datum.egl_frame_timestamp_ns = static_cast<int64_t>(value);
        datum.success = true;
        Report(datum);
        Log::D(TAG, "EGL_TIMESTAMP_PENDING_ANDROID");
        continue;
      }
    }
  }

  // Choreographer
  void SetupFrameCallback() {
    postFrameCallback(_choreographer_instance, FrameCallback, this);
  }

  static void FrameCallback(long frame_time_nanos, void* data) {
    if (data == nullptr) return;
    auto self = reinterpret_cast<ChoreographerTimestampsOperation*>(data);

    std::lock_guard guard{self->_frame_counter_mutex};
    if (self->_frame_counter > self->_configuration.first_frame_id) {
      choreographer_timestamp_datum datum = {};
      datum.choreographer_timestamp_ns = frame_time_nanos;
      datum.vsync_offset_ns = GetAppVsyncOffsetNanos();
      self->Report(datum);
    }

    // The counter must be incremented after a timestamp has been logged, but
    // neither the counter nor its mutex can be accessed after the counter limit
    // is reached, since that condition is the only thing keeping the operation
    // from being destroyed in the Wait() method.

    if (self->_frame_counter < self->_configuration.last_frame_id) {
      self->SetupFrameCallback();
    }
  }

 private:
  configuration _configuration;
  EGLContext _egl_context = EGL_NO_CONTEXT;

  AChoreographer* _choreographer_instance = nullptr;

  int _frame_counter = 0;
  std::mutex _frame_counter_mutex;

  // `_current_frame_id` is relative to when we start drawing, NOT
  // eglGetNextFrameIdANDROID. (We have to allow for arbitrary values from EGL,
  // but we still want to be able to think about the Nth frame.)
  vector<FrameEvent> _frame_events;
};

EXPORT_ANCER_OPERATION(ChoreographerTimestampsOperation)
