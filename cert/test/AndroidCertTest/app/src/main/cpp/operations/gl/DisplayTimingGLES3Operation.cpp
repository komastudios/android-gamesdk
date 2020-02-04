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

#include <utility>

#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/System.hpp>
#include <ancer/util/Json.hpp>

#include <dlfcn.h>  // TODO (baxtermichael@google.com): Remove

using namespace ancer;

// PURPOSE:

namespace {
constexpr Log::Tag TAG{"DisplayTimingGLES3Operation"};

struct configuration {
  int value;
};

JSON_CONVERTER(configuration) { JSON_OPTVAR(value); }

struct datum {
  uint64_t frame_id;
  int64_t timestamp;
  std::string timestamp_mode;
};

JSON_WRITER(datum) {
  JSON_REQVAR(frame_id);
  JSON_REQVAR(timestamp);
  JSON_REQVAR(timestamp_mode);
}
}  // namespace

// =============================================================================

namespace {  // TODO (baxtermichael@google.com): Remove
void *LoadLibrary(const char *lib_name) {
  return dlopen(lib_name, RTLD_NOW | RTLD_LOCAL);
}

void *LoadSymbol(void *lib, const char *function_name) {
  if (lib == nullptr) return nullptr;
  return dlsym(lib, function_name);
}

void *GetEGLLibrary() { return LoadLibrary("libEGL.so"); }

// typedef long names for readability
typedef PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC
    PFN_GET_COMPOSITOR_SUPPORTED;
typedef PFNEGLGETCOMPOSITORTIMINGANDROIDPROC PFN_GET_COMPOSITOR_TIMING;
typedef PFNEGLGETFRAMETIMESTAMPSUPPORTEDANDROIDPROC PFN_GET_TIMESTAMP_SUPPORTED;
typedef PFNEGLGETFRAMETIMESTAMPSANDROIDPROC PFN_GET_FRAME_TIMESTAMP;
typedef PFNEGLGETNEXTFRAMEIDANDROIDPROC PFN_GET_NEXT_FRAME_ID;

PFN_GET_COMPOSITOR_SUPPORTED GetPFN_CompositorSupported() {
  return reinterpret_cast<PFN_GET_COMPOSITOR_SUPPORTED>(
      LoadSymbol(GetEGLLibrary(), "eglGetCompositorTimingSupportedANDROID"));
}

PFN_GET_COMPOSITOR_TIMING GetPFN_GetCompositorTiming() {
  return reinterpret_cast<PFN_GET_COMPOSITOR_TIMING>(
      LoadSymbol(GetEGLLibrary(), "eglGetCompositorTimingANDROID"));
}

PFN_GET_TIMESTAMP_SUPPORTED GetPFN_TimestampSupported() {
  return reinterpret_cast<PFN_GET_TIMESTAMP_SUPPORTED>(
      LoadSymbol(GetEGLLibrary(), "eglGetFrameTimestampSupportedANDROID"));
}

PFN_GET_FRAME_TIMESTAMP GetPFN_GetFrameTimestamp() {
  return reinterpret_cast<PFN_GET_FRAME_TIMESTAMP>(
      LoadSymbol(GetEGLLibrary(), "eglGetFrameTimestampsANDROID"));
}

PFN_GET_NEXT_FRAME_ID GetPFN_GetNextFrameId() {
  return reinterpret_cast<PFN_GET_NEXT_FRAME_ID>(
      LoadSymbol(GetEGLLibrary(), "eglGetNextFrameIdANDROID"));
}

// TODO(baxtermichael@google.com): Consider renaming for clarity, or make hash
// map
#define ENUM_NAME_PAIR(name) std::make_pair(name, #name)

const std::vector<std::pair<EGLint, std::string>> egl_timestamp_names = {
    ENUM_NAME_PAIR(EGL_COMPOSITE_DEADLINE_ANDROID),
    ENUM_NAME_PAIR(EGL_COMPOSITE_INTERVAL_ANDROID),
    ENUM_NAME_PAIR(EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID),
    ENUM_NAME_PAIR(EGL_REQUESTED_PRESENT_TIME_ANDROID),
    ENUM_NAME_PAIR(EGL_RENDERING_COMPLETE_TIME_ANDROID),
    ENUM_NAME_PAIR(EGL_COMPOSITION_LATCH_TIME_ANDROID),
    ENUM_NAME_PAIR(EGL_FIRST_COMPOSITION_START_TIME_ANDROID),
    ENUM_NAME_PAIR(EGL_LAST_COMPOSITION_START_TIME_ANDROID),
    ENUM_NAME_PAIR(EGL_FIRST_COMPOSITION_GPU_FINISHED_TIME_ANDROID),
    ENUM_NAME_PAIR(EGL_DISPLAY_PRESENT_TIME_ANDROID),
    ENUM_NAME_PAIR(EGL_DEQUEUE_READY_TIME_ANDROID),
    ENUM_NAME_PAIR(EGL_READS_DONE_TIME_ANDROID),
};

#undef ENUM_NAME_PAIR

std::string GetEGLTimestampModeName(EGLint timestamp) {
  for (const auto &name : egl_timestamp_names) {
    if (name.first == timestamp) {
      return name.second;
    }
  }

  return "";
}

}  // namespace

// =============================================================================

namespace {
struct FrameTimingCapture {
  EGLuint64KHR id = UINT64_MAX;
  EGLint timestamp_mode = 0;
  bool captured = false;
  EGLnsecsANDROID timestamp = 0;

  explicit FrameTimingCapture(EGLuint64KHR id, EGLint timestamp_mode)
      : id(id), timestamp_mode(timestamp_mode) {}

  datum GetDatum() const {
    datum d{};
    d.frame_id = static_cast<uint64_t>(id);
    d.timestamp = static_cast<int64_t>(timestamp);

    d.timestamp_mode = GetEGLTimestampModeName(timestamp_mode);
    // switch (timestamp_mode) {
    //   case EGL_REQUESTED_PRESENT_TIME_ANDROID:
    //     d.timestamp_mode = "EGL_REQUESTED_PRESENT_TIME_ANDROID";
    //     break;
    //   case EGL_RENDERING_COMPLETE_TIME_ANDROID:
    //     d.timestamp_mode = "EGL_RENDERING_COMPLETE_TIME_ANDROID";
    //     break;
    //   case EGL_COMPOSITION_LATCH_TIME_ANDROID:
    //     d.timestamp_mode = "EGL_COMPOSITION_LATCH_TIME_ANDROID";
    //     break;
    //   case EGL_FIRST_COMPOSITION_START_TIME_ANDROID:
    //     d.timestamp_mode = "EGL_FIRST_COMPOSITION_START_TIME_ANDROID";
    //     break;
    //   case EGL_LAST_COMPOSITION_START_TIME_ANDROID:
    //     d.timestamp_mode = "EGL_LAST_COMPOSITION_START_TIME_ANDROID";
    //     break;
    //   case EGL_FIRST_COMPOSITION_GPU_FINISHED_TIME_ANDROID:
    //     d.timestamp_mode = "EGL_FIRST_COMPOSITION_GPU_FINISHED_TIME_ANDROID";
    //     break;
    //   case EGL_DISPLAY_PRESENT_TIME_ANDROID:
    //     d.timestamp_mode = "EGL_DISPLAY_PRESENT_TIME_ANDROID";
    //     break;
    //   case EGL_DEQUEUE_READY_TIME_ANDROID:
    //     d.timestamp_mode = "EGL_DEQUEUE_READY_TIME_ANDROID";
    //     break;
    //   case EGL_READS_DONE_TIME_ANDROID:
    //     d.timestamp_mode = "EGL_READS_DONE_TIME_ANDROID";
    //     break;
    //   default:
    //     FatalError(TAG,
    //                "Unsupported timestamp mode (the resulting data will be "
    //                "meaningless)");
    // }

    return d;
  }
};
}  // namespace

// =============================================================================

class DisplayTimingGLES3Operation : public BaseGLES3Operation {
 public:
  DisplayTimingGLES3Operation() = default;

  void OnGlContextReady(const GLContextConfig &ctx_config) override {
    Log::D(TAG, "GlContextReady");
    _configuration = GetConfiguration<configuration>();

    SetHeartbeatPeriod(0ms);

    _egl_context = eglGetCurrentContext();
    if (_egl_context == EGL_NO_CONTEXT) {
      FatalError(TAG, "No EGL context available");
    }

    GetFunctionPointers();
    CheckSupport();
    EnableSurfaceTimestampCollection();
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);

    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    // Capture values from any pending frames
    for (FrameTimingCapture &frame : _captured_frames) {
      if (!frame.captured) CaptureFrameTiming(display, surface, frame);
    }

    // Queue next frame for capture
    EGLuint64KHR frame_id = 0;
    if (!_pfn_get_next_frame_id(display, surface, &frame_id)) {
      FatalError(TAG, "Call to GetNextFrameId failed");
    }

    const std::vector<EGLint> timing_modes = {
        EGL_RENDERING_COMPLETE_TIME_ANDROID, EGL_COMPOSITION_LATCH_TIME_ANDROID,
        EGL_DISPLAY_PRESENT_TIME_ANDROID,    EGL_DEQUEUE_READY_TIME_ANDROID,
        EGL_READS_DONE_TIME_ANDROID,
    };

    for (const auto &timing_mode : timing_modes) {
      FrameTimingCapture new_frame{frame_id, timing_mode};
      _captured_frames.push_back(new_frame);
    }

    const std::vector<EGLint> compositor_modes = {
        EGL_COMPOSITE_DEADLINE_ANDROID,
        // EGL_COMPOSITE_INTERVAL_ANDROID,
        // EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID,
    };

    for (const auto &mode : compositor_modes) {
      CaptureCompositorTiming(display, surface, mode);
    }
  }

 private:
  void CaptureCompositorTiming(EGLDisplay display, EGLSurface surface,
                               EGLint mode) {
    EGLnsecsANDROID timestamp;
    if (!_pfn_get_compositor_timing(display, surface, 1, &mode, &timestamp)) {
      FatalError(TAG, "Unable to get compositor timing");
      return;
    }

    FrameTimingCapture frame(0, mode);
    frame.timestamp = timestamp;
    Report(frame.GetDatum());

    // Log::D(TAG, GetEGLTimestampModeName(mode) + " : %lld", timestamp);
  }

  void CaptureFrameTiming(EGLDisplay display, EGLSurface surface,
                          FrameTimingCapture &frame) {
    if (!_pfn_get_frame_timestamp(display, surface, frame.id, 1,
                                  &frame.timestamp_mode, &frame.timestamp)) {
      frame.captured = true;
      FatalError(TAG, InterpretEGLError(eglGetError()));
      return;
    }

    if (EGL_TIMESTAMP_INVALID_ANDROID == frame.timestamp) {
      frame.captured = true;
      return;
    }

    if (EGL_TIMESTAMP_PENDING_ANDROID != frame.timestamp) {
      frame.captured = true;
      Report(frame.GetDatum());
    }
  }

  std::string InterpretEGLError(const EGLint &err) const {
    switch (err) {
      case EGL_BAD_ACCESS:
        return "EGL_BAD_ACCESS (Frame probably no longer exists)";
      case EGL_BAD_SURFACE:
        return "EGL_BAD_SURFACE";
      case EGL_TIMESTAMP_PENDING_ANDROID:
        return "EGL_TIMESTAMP_PENDING_ANDROID";
      case EGL_TIMESTAMP_INVALID_ANDROID:
        return "EGL_TIMESTAMP_INVALID_ANDROID";
      case EGL_BAD_PARAMETER:
        return "EGL_BAD_PARAMETER";
      default:
        return "Call to PFN_GET_FRAME_TIMESTAMP failed";
    }
  }

  void GetFunctionPointers() {
    // TODO(baxtermichael@google.com): Handle exceptions gracefully

    _pfn_get_compositor_supported = GetPFN_CompositorSupported();
    if (nullptr == _pfn_get_compositor_supported) {
      FatalError(TAG, "Failed to load symbol for GetPFN_CompositorSupported");
    }

    _pfn_get_compositor_timing = GetPFN_GetCompositorTiming();
    if (nullptr == _pfn_get_compositor_timing) {
      FatalError(TAG, "Failed to load symbol for GetPFN_GetCompositorTiming");
    }

    _pfn_get_next_frame_id = GetPFN_GetNextFrameId();
    if (nullptr == _pfn_get_next_frame_id) {
      FatalError(TAG, "Failed to load symbol for GetPFN_GetNextFrameId");
    }

    _pfn_get_timestamp_supported = GetPFN_TimestampSupported();
    if (nullptr == _pfn_get_timestamp_supported) {
      FatalError(TAG, "Failed to load symbol for GetPFN_TimestampSupported");
    }

    _pfn_get_frame_timestamp = GetPFN_GetFrameTimestamp();
    if (nullptr == _pfn_get_frame_timestamp) {
      FatalError(TAG, "Failed to load symbol for GetPFN_GetFrameTimestamp");
    }
  }

  void CheckSupport() const {
    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);

    const std::vector<std::pair<std::string, EGLint>> names = {
        std::make_pair("EGL_REQUESTED_PRESENT_TIME_ANDROID",
                       EGL_REQUESTED_PRESENT_TIME_ANDROID),
        std::make_pair("EGL_RENDERING_COMPLETE_TIME_ANDROID",
                       EGL_RENDERING_COMPLETE_TIME_ANDROID),
        std::make_pair("EGL_COMPOSITION_LATCH_TIME_ANDROID",
                       EGL_COMPOSITION_LATCH_TIME_ANDROID),
        std::make_pair("EGL_FIRST_COMPOSITION_START_TIME_ANDROID",
                       EGL_FIRST_COMPOSITION_START_TIME_ANDROID),
        std::make_pair("EGL_LAST_COMPOSITION_START_TIME_ANDROID",
                       EGL_LAST_COMPOSITION_START_TIME_ANDROID),
        std::make_pair("EGL_FIRST_COMPOSITION_START_TIME_ANDROID",
                       EGL_FIRST_COMPOSITION_START_TIME_ANDROID),
        std::make_pair("EGL_FIRST_COMPOSITION_GPU_FINISHED_TIME_ANDROID",
                       EGL_FIRST_COMPOSITION_GPU_FINISHED_TIME_ANDROID),
        std::make_pair("EGL_DISPLAY_PRESENT_TIME_ANDROID",
                       EGL_DISPLAY_PRESENT_TIME_ANDROID),
        std::make_pair("EGL_DEQUEUE_READY_TIME_ANDROID",
                       EGL_DEQUEUE_READY_TIME_ANDROID),
        std::make_pair("EGL_READS_DONE_TIME_ANDROID",
                       EGL_READS_DONE_TIME_ANDROID),
    };

    for (const auto &name : names) {
      const EGLBoolean supported =
          _pfn_get_timestamp_supported(display, surface, name.second);

      Log::D(TAG, name.first + " %s", supported ? "true" : "false");
    }
  }

  void EnableSurfaceTimestampCollection() {
    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
    if (!eglSurfaceAttrib(display, surface, EGL_TIMESTAMPS_ANDROID, EGL_TRUE)) {
      FatalError(TAG, "Failed to set timestamp collection");
    }
  }

 private:
  configuration _configuration;
  EGLContext _egl_context = EGL_NO_CONTEXT;

  PFN_GET_COMPOSITOR_SUPPORTED _pfn_get_compositor_supported = nullptr;
  PFN_GET_COMPOSITOR_TIMING _pfn_get_compositor_timing = nullptr;
  PFN_GET_NEXT_FRAME_ID _pfn_get_next_frame_id = nullptr;
  PFN_GET_TIMESTAMP_SUPPORTED _pfn_get_timestamp_supported = nullptr;
  PFN_GET_FRAME_TIMESTAMP _pfn_get_frame_timestamp = nullptr;

  std::vector<FrameTimingCapture> _captured_frames;
};

EXPORT_ANCER_OPERATION(DisplayTimingGLES3Operation)