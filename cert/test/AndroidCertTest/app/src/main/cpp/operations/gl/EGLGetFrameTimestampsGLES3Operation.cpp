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

#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/DatumReporting.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>
#include <ancer/util/LibEGL.hpp>

using namespace ancer;
using std::string;
using std::vector;

// PURPOSE: Determine if EGL frame timestamps extension is present and works.

// The `EGL_ANDROID_get_frame_timestamps` extension allows the application to
// fetch timestamps related to frame compositing and frame processing. This can
// be used to determine latency and detect if frames are being displayed late.
//
// The extension provides for fetching three types of values related to surface
// composition, as well as nine types of events related to the rendering and
// processing of a frame. Additionally, it allows the application to query for
// the integer id associated with the next frame available for rendering.
//
// To test the availability of the extension, we will check if the extension
// is reported among the list of EGL extensions, as well as if function pointers
// for the extension's methods can be loaded from the EGL library on device.
// Additionally, we need to check that the rendering surface can be enabled to
// produce timestamp values.
//
// The remainder of the test consists of two independent parts: one that tests
// surface composite queries and one that tests frame timing queries. They are
// essentially parallel in that we must first check which queries are supported
// and then attempt to get values for each supported query. Since frame ids are
// not needed for surface composite queries, the "next frame id" query can be
// tested alongside the test for frame timing queries (where ids are required.)

// =============================================================================

namespace {
constexpr Log::Tag TAG{"EGLGetFrameTimestampsGLES3Operation"};

const string extension_name = "EGL_ANDROID_get_frame_timestamps";

// -----------------------------------------------------------------------------

struct configuration {
  int first_frame_id;  // id of first frame we consider
  int last_frame_id;   // id of last frame we consider
};

JSON_CONVERTER(configuration) {
  JSON_OPTVAR(first_frame_id);
  JSON_OPTVAR(last_frame_id);
}

// -----------------------------------------------------------------------------

// Data logged

struct extension_availability_datum {
  bool has_ext_egl_android_get_frame_timestamps;
  bool did_enable_surface_egl_timestamps;
  bool has_fp_get_compositor_timing_supported;
  bool has_fp_get_compositor_timing;
  bool has_fp_get_next_frame_id;
  bool has_fp_get_frame_timestamp_supported;
  bool has_fp_get_frame_timestamps;
};

struct supported_composite_values_datum {
  bool supports_composite_deadline;
  bool supports_composite_interval;
  bool supports_composite_to_present_latency;

  void set(EGLint timing_event, bool supported);
};

struct composite_value_datum {
  string measurement_name;  // EGL documentation's "timestamp" or "name"
  int64_t value_ns;         // EGL documentation's "value", nanoseconds
  // `value_ns` could be either a timestamp or a duration
};

struct supported_frame_timing_events_datum {
  bool supports_requested_present_time;
  bool supports_rendering_complete_time;
  bool supports_composition_latch_time;
  bool supports_first_composition_start_time;
  bool supports_last_composition_start_time;
  bool supports_first_composition_gpu_finished_time;
  bool supports_display_present_time;
  bool supports_dequeue_ready_time;
  bool supports_reads_done_time;

  void set(EGLint timing_event, bool supported);
};

struct next_frame_id_datum {
  uint64_t next_frame_id;
};

struct frame_timing_datum {
  uint64_t frame_id;
  string event_name;  // EGL documentation's "timestamp"
  int64_t value_ns;   // EGL documentation's "value", nanoseconds
};

struct frame_timing_error_datum {
  uint64_t frame_id;
  string event_name;
  string error_message;
};

// -----------------------------------------------------------------------------

// Test-specific utility methods

void supported_composite_values_datum::set(EGLint timing_event,
                                           bool supported) {
  switch (timing_event) {
    case EGL_COMPOSITE_DEADLINE_ANDROID:
      supports_composite_deadline = supported;
      break;
    case EGL_COMPOSITE_INTERVAL_ANDROID:
      supports_composite_interval = supported;
      break;
    case EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID:
      supports_composite_to_present_latency = supported;
      break;
  }
}

void supported_frame_timing_events_datum::set(EGLint timing_event,
                                              bool supported) {
  switch (timing_event) {
    case EGL_REQUESTED_PRESENT_TIME_ANDROID:
      supports_requested_present_time = supported;
      break;
    case EGL_RENDERING_COMPLETE_TIME_ANDROID:
      supports_rendering_complete_time = supported;
      break;
    case EGL_COMPOSITION_LATCH_TIME_ANDROID:
      supports_composition_latch_time = supported;
      break;
    case EGL_FIRST_COMPOSITION_START_TIME_ANDROID:
      supports_first_composition_start_time = supported;
      break;
    case EGL_LAST_COMPOSITION_START_TIME_ANDROID:
      supports_last_composition_start_time = supported;
      break;
    case EGL_FIRST_COMPOSITION_GPU_FINISHED_TIME_ANDROID:
      supports_first_composition_gpu_finished_time = supported;
      break;
    case EGL_DISPLAY_PRESENT_TIME_ANDROID:
      supports_display_present_time = supported;
      break;
    case EGL_DEQUEUE_READY_TIME_ANDROID:
      supports_dequeue_ready_time = supported;
      break;
    case EGL_READS_DONE_TIME_ANDROID:
      supports_reads_done_time = supported;
      break;
  }
}

// -----------------------------------------------------------------------------

void WriteDatum(report_writers::Struct w,
                const extension_availability_datum& d) {
  ADD_DATUM_MEMBER(w, d, has_ext_egl_android_get_frame_timestamps);
  ADD_DATUM_MEMBER(w, d, did_enable_surface_egl_timestamps);
  ADD_DATUM_MEMBER(w, d, has_fp_get_compositor_timing_supported);
  ADD_DATUM_MEMBER(w, d, has_fp_get_compositor_timing);
  ADD_DATUM_MEMBER(w, d, has_fp_get_next_frame_id);
  ADD_DATUM_MEMBER(w, d, has_fp_get_frame_timestamp_supported);
  ADD_DATUM_MEMBER(w, d, has_fp_get_frame_timestamps);
}

void WriteDatum(report_writers::Struct w,
                const supported_composite_values_datum& d) {
  ADD_DATUM_MEMBER(w, d, supports_composite_deadline);
  ADD_DATUM_MEMBER(w, d, supports_composite_interval);
  ADD_DATUM_MEMBER(w, d, supports_composite_to_present_latency);
}

void WriteDatum(report_writers::Struct w, const composite_value_datum& d) {
  ADD_DATUM_MEMBER(w, d, measurement_name);
  ADD_DATUM_MEMBER(w, d, value_ns);
}

void WriteDatum(report_writers::Struct w,
                const supported_frame_timing_events_datum& d) {
  ADD_DATUM_MEMBER(w, d, supports_requested_present_time);
  ADD_DATUM_MEMBER(w, d, supports_rendering_complete_time);
  ADD_DATUM_MEMBER(w, d, supports_composition_latch_time);
  ADD_DATUM_MEMBER(w, d, supports_first_composition_start_time);
  ADD_DATUM_MEMBER(w, d, supports_last_composition_start_time);
  ADD_DATUM_MEMBER(w, d, supports_first_composition_gpu_finished_time);
  ADD_DATUM_MEMBER(w, d, supports_display_present_time);
  ADD_DATUM_MEMBER(w, d, supports_dequeue_ready_time);
  ADD_DATUM_MEMBER(w, d, supports_reads_done_time);
}

void WriteDatum(report_writers::Struct w, const next_frame_id_datum& d) {
  ADD_DATUM_MEMBER(w, d, next_frame_id);
}

void WriteDatum(report_writers::Struct w, const frame_timing_datum& d) {
  ADD_DATUM_MEMBER(w, d, frame_id);
  ADD_DATUM_MEMBER(w, d, event_name);
  ADD_DATUM_MEMBER(w, d, value_ns);
}

void WriteDatum(report_writers::Struct w, const frame_timing_error_datum& d) {
  ADD_DATUM_MEMBER(w, d, frame_id);
  ADD_DATUM_MEMBER(w, d, event_name);
  ADD_DATUM_MEMBER(w, d, error_message);
}
}  // namespace

// =============================================================================

namespace {
string TimestampTypeName(EGLint timestamp_type) {
#define CASE_RETURN_STRING(val) \
  case val:                     \
    return #val;

  switch (timestamp_type) {
    CASE_RETURN_STRING(EGL_COMPOSITE_DEADLINE_ANDROID)
    CASE_RETURN_STRING(EGL_COMPOSITE_INTERVAL_ANDROID)
    CASE_RETURN_STRING(EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID)
    CASE_RETURN_STRING(EGL_REQUESTED_PRESENT_TIME_ANDROID)
    CASE_RETURN_STRING(EGL_RENDERING_COMPLETE_TIME_ANDROID)
    CASE_RETURN_STRING(EGL_COMPOSITION_LATCH_TIME_ANDROID)
    CASE_RETURN_STRING(EGL_FIRST_COMPOSITION_START_TIME_ANDROID)
    CASE_RETURN_STRING(EGL_LAST_COMPOSITION_START_TIME_ANDROID)
    CASE_RETURN_STRING(EGL_FIRST_COMPOSITION_GPU_FINISHED_TIME_ANDROID)
    CASE_RETURN_STRING(EGL_DISPLAY_PRESENT_TIME_ANDROID)
    CASE_RETURN_STRING(EGL_DEQUEUE_READY_TIME_ANDROID)
    CASE_RETURN_STRING(EGL_READS_DONE_TIME_ANDROID)
  }

#undef CASE_RETURN_STRING

  return "";
};

const vector<EGLint> composite_value_names = {
    EGL_COMPOSITE_DEADLINE_ANDROID,
    EGL_COMPOSITE_INTERVAL_ANDROID,
    EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID,
};

const vector<EGLint> frame_timing_events = {
    EGL_REQUESTED_PRESENT_TIME_ANDROID,
    EGL_RENDERING_COMPLETE_TIME_ANDROID,
    EGL_COMPOSITION_LATCH_TIME_ANDROID,
    EGL_FIRST_COMPOSITION_START_TIME_ANDROID,
    EGL_LAST_COMPOSITION_START_TIME_ANDROID,
    EGL_FIRST_COMPOSITION_GPU_FINISHED_TIME_ANDROID,
    EGL_DISPLAY_PRESENT_TIME_ANDROID,
    EGL_DEQUEUE_READY_TIME_ANDROID,
    EGL_READS_DONE_TIME_ANDROID,
};

struct FrameEvent {
  EGLuint64KHR frame_id;
  EGLint timing_event;
  bool pending = true;

  FrameEvent(EGLuint64KHR frame_id, EGLint timing_event)
      : frame_id(frame_id), timing_event(timing_event) {}
};
}  // namespace

// =============================================================================

class EGLGetFrameTimestampsGLES3Operation : public BaseGLES3Operation {
 public:
  EGLGetFrameTimestampsGLES3Operation() = default;

  void OnGlContextReady(const GLContextConfig& ctx_config) override {
    Log::D(TAG, "GlContextReady");
    _configuration = GetConfiguration<configuration>();

    SetHeartbeatPeriod(0ms);

    _egl_context = eglGetCurrentContext();
    if (_egl_context == EGL_NO_CONTEXT) {
      FatalError(TAG, "No EGL context available");
    }

    // First phase of reporting
    ReportExtensionAvailability();
    StoreSupportedCompositeNames();
    StoreSupportedFrameTimingEvents();
    ReportSupportedCompositeValues();
    ReportSupportedFrameTimingEvents();
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);

    // Second phase of reporting
    ReportNextFrameId();

    if (current_frame_id >= _configuration.first_frame_id &&
        current_frame_id <= _configuration.last_frame_id) {
      ReportCompositeValues();

      // Enqueue `FrameEvent`s (to get timestamps) for yet-to-be-rendered frames
      CreatePendingFrameEvents();
    }

    // Continue to poll pending frames, as it will take several frames for each
    // frame to make it all the way through rendering/composition/presentation.
    ReportPendingFrameTimingEvents();

    ++current_frame_id;
  }

 private:
  void ReportExtensionAvailability() {
    extension_availability_datum datum = {};

    datum.has_ext_egl_android_get_frame_timestamps =
        glh::CheckEglExtension(extension_name);
    datum.did_enable_surface_egl_timestamps = EnableSurfaceTimestamps();
    datum.has_fp_get_compositor_timing_supported =
        libegl::GetFP_GetCompositorTimingSupported() != nullptr;
    datum.has_fp_get_compositor_timing =
        libegl::GetFP_GetCompositorTiming() != nullptr;
    datum.has_fp_get_next_frame_id = libegl::GetFP_GetNextFrameId() != nullptr;
    datum.has_fp_get_frame_timestamp_supported =
        libegl::GetFP_GetFrameTimestampSupported() != nullptr;
    datum.has_fp_get_frame_timestamps =
        libegl::GetFP_GetFrameTimestamps() != nullptr;

    Report(datum);
  }

  void StoreSupportedCompositeNames() {
    const auto fp = libegl::GetFP_GetCompositorTimingSupported();
    if (fp == nullptr) return;

    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    for (const auto& name : composite_value_names) {
      if (fp(display, surface, name)) {
        _supported_composite_names.push_back(name);
      }
    }
  }

  void StoreSupportedFrameTimingEvents() {
    const auto fp = libegl::GetFP_GetFrameTimestampSupported();
    if (fp == nullptr) return;

    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    for (const auto& event : frame_timing_events) {
      if (fp(display, surface, event)) {
        _supported_frame_timing_events.push_back(event);
      }
    }
  }

  void ReportSupportedCompositeValues() const {
    supported_composite_values_datum datum = {};

    auto available = [this](EGLint name) {
      return std::find(_supported_composite_names.begin(),
                       _supported_composite_names.end(),
                       name) != _supported_composite_names.end();
    };

    datum.supports_composite_deadline =
        available(EGL_COMPOSITE_DEADLINE_ANDROID);
    datum.supports_composite_interval =
        available(EGL_COMPOSITE_INTERVAL_ANDROID);
    datum.supports_composite_to_present_latency =
        available(EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID);

    Report(datum);
  }

  void ReportSupportedFrameTimingEvents() const {
    supported_frame_timing_events_datum datum = {};

    auto available = [this](EGLint event) {
      return std::find(_supported_frame_timing_events.begin(),
                       _supported_frame_timing_events.end(),
                       event) != _supported_frame_timing_events.end();
    };

    datum.supports_requested_present_time =
        available(EGL_REQUESTED_PRESENT_TIME_ANDROID);
    datum.supports_rendering_complete_time =
        available(EGL_RENDERING_COMPLETE_TIME_ANDROID);
    datum.supports_composition_latch_time =
        available(EGL_COMPOSITION_LATCH_TIME_ANDROID);
    datum.supports_first_composition_start_time =
        available(EGL_FIRST_COMPOSITION_START_TIME_ANDROID);
    datum.supports_last_composition_start_time =
        available(EGL_LAST_COMPOSITION_START_TIME_ANDROID);
    datum.supports_first_composition_gpu_finished_time =
        available(EGL_FIRST_COMPOSITION_GPU_FINISHED_TIME_ANDROID);
    datum.supports_display_present_time =
        available(EGL_DISPLAY_PRESENT_TIME_ANDROID);
    datum.supports_dequeue_ready_time =
        available(EGL_DEQUEUE_READY_TIME_ANDROID);
    datum.supports_reads_done_time = available(EGL_READS_DONE_TIME_ANDROID);

    Report(datum);
  }

  void ReportCompositeValues() const {
    const auto fp = libegl::GetFP_GetCompositorTiming();
    if (fp == nullptr) return;

    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    for (const auto& name : _supported_composite_names) {
      EGLnsecsANDROID value;
      const EGLBoolean ok = fp(display, surface, 1, &name, &value);
      if (ok) {
        composite_value_datum datum = {};
        datum.measurement_name = TimestampTypeName(name);
        datum.value_ns = static_cast<int64_t>(value);
        Report(datum);
      }
    }
  }

  void CreatePendingFrameEvents() {
    EGLuint64KHR frame_id = 0;
    if (!GetNextFrameId(&frame_id)) {
      Log::D(TAG, "CreatePendingFrameEvents : Failed to get next frame id");
      return;
    }

    for (const auto& event : _supported_frame_timing_events) {
      _frame_events.push_back(FrameEvent{frame_id, event});
    }
  }

  void ReportPendingFrameTimingEvents() {
    const auto fp = libegl::GetFP_GetFrameTimestamps();
    if (fp == nullptr) return;

    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    for (auto& event : _frame_events) {
      if (!event.pending) continue;

      EGLnsecsANDROID value = 0;
      const EGLBoolean ok =
          fp(display, surface, event.frame_id, 1, &event.timing_event, &value);
      if (!ok) continue;  // the timestamp may be pending

      if (EGL_TIMESTAMP_INVALID_ANDROID == value) {
        event.pending = false;

        frame_timing_error_datum datum = {};
        datum.frame_id = static_cast<uint64_t>(event.frame_id);
        datum.event_name = TimestampTypeName(event.timing_event);
        datum.error_message = "Frame information became invalid";
        Report(datum);

        continue;
      }

      if (EGL_TIMESTAMP_PENDING_ANDROID != value) {
        event.pending = false;

        frame_timing_datum datum = {};
        datum.frame_id = static_cast<uint64_t>(event.frame_id);
        datum.event_name = TimestampTypeName(event.timing_event);
        datum.value_ns = static_cast<int64_t>(value);
        Report(datum);
      }
    }
  }

  void ReportFrameTimingEvent() const {
    composite_value_datum datum = {};
    Report(datum);
  }

  void ReportNextFrameId() const {
    EGLuint64KHR next_frame_id = 0;
    if (GetNextFrameId(&next_frame_id)) {
      Report(next_frame_id_datum{next_frame_id});
    }
  }

  bool GetNextFrameId(EGLuint64KHR* next_frame_id) const {
    const auto fp = libegl::GetFP_GetNextFrameId();
    if (fp == nullptr || next_frame_id == nullptr) return EGL_FALSE;

    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    return static_cast<bool>(fp(display, surface, next_frame_id));
  }

  bool EnableSurfaceTimestamps() {
    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    const EGLBoolean did_set_surface =
        eglSurfaceAttrib(display, surface, EGL_TIMESTAMPS_ANDROID, EGL_TRUE);

    return static_cast<bool>(did_set_surface);
  }

 private:
  configuration _configuration;
  EGLContext _egl_context = EGL_NO_CONTEXT;

  vector<EGLint> _supported_composite_names;
  vector<EGLint> _supported_frame_timing_events;

  // `current_frame_id` is relative to when we start drawing, NOT
  // eglGetNextFrameIdANDROID. (We have to allow for arbitrary values from EGL,
  // but we still want to be able to think about the Nth frame.)
  int current_frame_id = 0;

  vector<FrameEvent> _frame_events;
};

EXPORT_ANCER_OPERATION(EGLGetFrameTimestampsGLES3Operation)