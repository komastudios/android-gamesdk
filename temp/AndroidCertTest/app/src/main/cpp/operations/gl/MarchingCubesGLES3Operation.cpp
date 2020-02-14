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

#include <cmath>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/util/GLHelpers.hpp>
#include <ancer/util/Json.hpp>
#include <ancer/util/UnownedPtr.hpp>

#include "operations/gl/marching_cubes/MarchingCubes.hpp"
#include "operations/gl/marching_cubes/Volume.hpp"

using namespace ancer;
using namespace marching_cubes;


//==============================================================================

namespace {
constexpr Log::Tag TAG{"MarchingCubesGLES3Operation"};
constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 1000.0f;
constexpr float FOV_DEGREES = 50.0F;

struct ProgramState {
  GLuint program = 0;
  GLint uniform_loc_MVP = -1;
  GLint uniform_loc_Model = -1;

  ~ProgramState() {
    if (program > 0) {
      glDeleteProgram(program);
    }
  }

  bool Build(const std::string& vertFile, const std::string& fragFile) {
    auto vertSrc = LoadText(vertFile.c_str());
    auto fragSrc = LoadText(fragFile.c_str());
    program = glh::CreateProgramSrc(vertSrc.c_str(), fragSrc.c_str());
    if (program == 0) {
      return false;
    }
    uniform_loc_MVP = glGetUniformLocation(program, "uMVP");
    uniform_loc_Model = glGetUniformLocation(program, "uModel");
    return true;
  }
};

/*
 * Volume sampler for sphere
 */
class SphereVolumeSampler : public IVolumeSampler {
 public:
  SphereVolumeSampler(vec3 position, float radius, Mode mode)
      : IVolumeSampler(mode), _position(position), _radius(radius) {
  }

  ~SphereVolumeSampler() override = default;

  float ValueAt(const vec3& p, float falloff_threshold) const override {
    float d2 = distance2(p, _position);
    float min2 = _radius * _radius;
    if (d2 < min2) {
      return 1;
    }

    float max2 = (_radius + falloff_threshold) * (_radius + falloff_threshold);
    if (d2 > max2) {
      return 0;
    }

    float d = sqrt(d2) - _radius;
    return 1 - (d / falloff_threshold);
  }

  void SetPosition(const vec3& center) { _position = center; }
  void SetRadius(float radius) { _radius = max<float>(radius, 0); }
  vec3 GetPosition() const { return _position; }
  float GetRadius() const { return _radius; }

 private:
  vec3 _position;
  float _radius;
};

/*
 * Volume sampler for plane
 */
class PlaneVolumeSampler : public IVolumeSampler {
 public:
  PlaneVolumeSampler(vec3 plane_origin,
                     vec3 plane_normal,
                     float plane_thickness,
                     Mode mode)
      : IVolumeSampler(mode),
        _origin(plane_origin),
        _normal(::normalize(plane_normal)),
        _thickness(std::max<float>(plane_thickness, 0)) {
  }

  float ValueAt(const vec3& p, float falloff_threshold) const override {
    // distance of p from plane
    float dist = abs(dot(_normal, p - _origin));

    if (dist <= _thickness) {
      return 1;
    } else if (dist > _thickness + falloff_threshold) {
      return 0;
    }
    dist -= _thickness;
    return 1 - (dist / falloff_threshold);
  }

  void SetOrigin(const vec3 plane_origin) { _origin = plane_origin; }
  vec3 GetOrigin() const { return _origin; }

  void SetNormal(const vec3 plane_normal) {
    _normal = normalize(plane_normal);
  }
  vec3 GetNormal() const { return _normal; }

  void SetThickness(float thickness) { _thickness = max<float>(thickness, 0); }
  float GetThickness() const { return _thickness; }

 private:
  vec3 _origin;
  vec3 _normal;
  float _thickness;
};

//==============================================================================

enum class ThreadAffinitySetup {
  OneBigCore, OneLittleCore, AllBigCores, AllLittleCores, AllCores
};

constexpr const char* ThreadAffinitySetupNames[] = {
    "OneBigCore", "OneLittleCore", "AllBigCores", "AllLittleCores", "AllCores"
};

enum class ThreadPinningSetup {
  Unspecified, Pinned, Floating
};

constexpr const char* ThreadPinningSetupNames[] = {
    "Unspecified", "Pinned", "Floating"
};

constexpr auto ToAffinity(ThreadAffinitySetup setup) {
  switch (setup) {
    case ThreadAffinitySetup::OneBigCore:
    case ThreadAffinitySetup::AllBigCores:return ThreadAffinity::kBigCore;
    case ThreadAffinitySetup::OneLittleCore:
    case ThreadAffinitySetup::AllLittleCores:return ThreadAffinity::kLittleCore;
    case ThreadAffinitySetup::AllCores: return ThreadAffinity::kAnyCore;
    default:FatalError(TAG, "Unknown thread setup %d", (int) setup);
  }
}

struct configuration_thread_setup {
  ThreadAffinitySetup thread_setup;
  ThreadPinningSetup pinning;
};

JSON_CONVERTER(configuration_thread_setup) {
  JSON_REQENUM(thread_setup, ThreadAffinitySetupNames);
  JSON_OPTENUM(pinning, ThreadPinningSetupNames);
}

struct configuration {
  std::vector<configuration_thread_setup> thread_setups = {
      {ThreadAffinitySetup::OneBigCore},
      {ThreadAffinitySetup::AllBigCores},
      {ThreadAffinitySetup::OneLittleCore},
      {ThreadAffinitySetup::AllLittleCores},
      {ThreadAffinitySetup::AllCores}
  };
  Duration warm_up_time = 5s;
  Duration sleep_per_iteration_min = 0ms;
  Duration sleep_per_iteration_max = 0ms;
  Duration sleep_per_iteration_increment = 500000ns; // 0.5ms
  Duration permutation_execution_duration = 10s;
};

JSON_CONVERTER(configuration) {
  JSON_OPTVAR(warm_up_time);
  JSON_REQVAR(thread_setups);
  JSON_REQVAR(sleep_per_iteration_min);
  JSON_REQVAR(sleep_per_iteration_max);
  JSON_REQVAR(sleep_per_iteration_increment);
  JSON_REQVAR(permutation_execution_duration);
}

struct execution_configuration {
  ThreadAffinitySetup thread_setup;
  bool pinned;
  Duration sleep_per_iteration;
};

JSON_CONVERTER(execution_configuration) {
  JSON_REQENUM(thread_setup, ThreadAffinitySetupNames);
  JSON_REQVAR(pinned);
  JSON_REQVAR(sleep_per_iteration);
}

/*
 * Generate all permutations of a provided configuration
 */
std::vector<execution_configuration> Permute(const configuration config) {
  std::vector<execution_configuration> permutations;
  for (auto thread_setup : config.thread_setups) {
    if (thread_setup.pinning == ThreadPinningSetup::Unspecified) {
      for (auto i = 0; i < 2; i++) { // (0: floating, 1: pinned)
        bool pinned = i == 1;
        for (Duration dur = config.sleep_per_iteration_min;
             dur <= config.sleep_per_iteration_max;
             dur += config.sleep_per_iteration_increment) {
          permutations.push_back({thread_setup.thread_setup, pinned, dur});
        }
      }
    } else {
      bool pinned = thread_setup.pinning == ThreadPinningSetup::Pinned;
      for (Duration dur = config.sleep_per_iteration_min;
           dur <= config.sleep_per_iteration_max;
           dur += config.sleep_per_iteration_increment) {
        permutations.push_back({thread_setup.thread_setup, pinned, dur});
      }
    }

  }
  return permutations;
}


//------------------------------------------------------------------------------

struct result {
  execution_configuration configuration;
  size_t num_threads_used = 0;
  size_t num_voxels_marched_per_iteration = 0;
  size_t num_iterations = 0;
  Duration min_calc_duration{0};
  Duration max_calc_duration{0};
  Duration average_calc_duration{0};
  Duration median_calc_duration{0};
  Duration fifth_percentile_duration{0};
  Duration twentyfifth_percentile_duration{0};
  Duration seventyfifth_percentile_duration{0};
  Duration ninetyfifth_percentile_duration{0};
};

JSON_CONVERTER(result) {
  JSON_REQVAR(configuration);
  JSON_REQVAR(num_threads_used);
  JSON_REQVAR(num_voxels_marched_per_iteration);
  JSON_REQVAR(num_iterations);
  JSON_REQVAR(min_calc_duration);
  JSON_REQVAR(max_calc_duration);
  JSON_REQVAR(average_calc_duration);
  JSON_REQVAR(median_calc_duration);
  JSON_REQVAR(fifth_percentile_duration);
  JSON_REQVAR(twentyfifth_percentile_duration);
  JSON_REQVAR(seventyfifth_percentile_duration);
  JSON_REQVAR(ninetyfifth_percentile_duration);
}

struct datum {
  result marching_cubes_permutation_results;
};

JSON_WRITER(datum) {
  JSON_REQVAR(marching_cubes_permutation_results);
}
} // anonymous namespace

//==============================================================================

class MarchingCubesGLES3Operation : public BaseGLES3Operation {
 public:

  MarchingCubesGLES3Operation() = default;

  ~MarchingCubesGLES3Operation() {
    if (!_march_durations.empty()) {
      ReportPerformanceData();
    }
  }

  void OnGlContextReady(const GLContextConfig& ctx_config) override {
    _configuration = GetConfiguration<configuration>();
    _configuration_permutations = Permute(_configuration);
    Log::I(TAG,
           "Generated %d execution configurations, expect run time "
           "duration of %d seconds",
           _configuration_permutations.size(),
           _configuration_permutations.size()
               * duration_cast<SecondsAs<int>>(
                   _configuration.permutation_execution_duration)
    );

    auto vertFile = "Shaders/MarchingCubesGLES3Operation/vert.glsl";
    auto fragFile = "Shaders/MarchingCubesGLES3Operation/frag.glsl";
    if (!_program.Build(vertFile, fragFile)) {
      Stop();
      return;
    }

    BuildVolume();

    glClearColor(0.2F, 0.2F, 0.22F, 0.0F);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SetCurrentConfigurationPermutation(_current_configuration_permutation);
    SetHeartbeatPeriod(_configuration.permutation_execution_duration);
  }

  void OnGlContextResized(int width, int height) override {
    BaseOperation::OnGlContextResized(width, height);
    auto aspect = static_cast<float>(width) / static_cast<float>(height);
    _proj = perspective(radians(FOV_DEGREES), aspect, NEAR_PLANE, FAR_PLANE);
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);

    auto sleep_dur =
        _configuration_permutations[_current_configuration_permutation].sleep_per_iteration;
    if (sleep_dur > 0ms) {
      std::this_thread::sleep_for(sleep_dur);
    }

    Step(delta_seconds);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(_program.program);

    auto model = rotate(mat4(1), radians(45.0F), vec3(0, 1, 0));
    auto view = lookAt(vec3(0, 0, _camera_z_position),
                       vec3(0, 0, 0),
                       vec3(0, 1, 0));

    auto mvp = _proj * view * model;

    glUniformMatrix4fv(_program.uniform_loc_MVP,
                       1,
                       GL_FALSE,
                       value_ptr(mvp));

    glUniformMatrix4fv(_program.uniform_loc_Model,
                       1,
                       GL_FALSE,
                       value_ptr(model));

    for (auto& tc : _triangle_consumers) {
      tc->Draw();
    }
  }

  void Step(double delta_seconds) {
    if (_marcher) {

      const float plane_speed = 3;
      auto origin = _sampler_cutout_plane->GetOrigin();
      auto min_y = -4 * _sampler_cutout_plane->GetThickness();
      auto max_y = _volume.Size().y + 4 * _sampler_cutout_plane->GetThickness();
      if (origin.y < min_y) {
        origin.y = max_y;
      }

      _sampler_cutout_plane->SetOrigin(origin + vec3(0, -1, 0) * plane_speed
          * static_cast<float>(delta_seconds));

      float angle = static_cast<float>(M_PI * origin.y / _volume.Size().y);
      vec3 normal{rotate(mat4(1), angle, vec3(1, 0, 0)) * vec4(0, 1, 0, 1)};

      _sampler_cutout_plane->SetNormal(normal);

      auto start_time = SteadyClock::now();
      _marcher->March();
      auto end_time = SteadyClock::now();

      if (_warming_up) {
        if (!_recorded_first_step_timestamp) {
          Log::D(TAG, "Warm up starting");
          _first_step_timestamp = SteadyClock::now();
          _recorded_first_step_timestamp = true;
        } else {
          auto elapsed_since_first_step =
              SteadyClock::now() - _first_step_timestamp;
          if (elapsed_since_first_step >= _configuration.warm_up_time) {
            Log::D(TAG, "Warm up finished, will start recording perf timings");
            _warming_up = false;
          }
        }
      }

      if (!_warming_up) {
        _march_durations.push_back(end_time - start_time);
      }
    }
  }

  void OnHeartbeat(Duration elapsed) override {
    if (HasMoreConfigurationPermutations()) {
      ReportPerformanceData();
      ResetPerformanceData();
      NextConfigurationPermutation();
    } else {
      Log::I(TAG, "Have run all configuration permutations; Stopping.");
      Stop();
    }
  }

 private:

  void ReportPerformanceData() {
    if (_march_durations.empty()) {
      Log::E(TAG, "ReportPerformanceData - _march_durations is empty");
      return;
    }

    const auto avg_duration =
        std::accumulate(_march_durations.begin(),
                        _march_durations.end(),
                        Duration{0})
            / _march_durations.size();

    // compute min/max/median
    std::sort(_march_durations.begin(), _march_durations.end());

    const auto min_duration = _march_durations.front();
    const auto max_duration = _march_durations.back();

    const auto median_duration = [this]() {
      if (_march_durations.size() % 2 == 1) {
        return _march_durations[_march_durations.size() / 2];
      } else {
        size_t center = _march_durations.size() / 2;
        return (_march_durations[std::max<size_t>(center - 1, 0)]
            + _march_durations[center]) / 2;
      }
    }();

    // compute percentiles
    auto find_percentile = [this](int percentile) -> Duration {
      percentile = min(max(percentile, 0), 100);
      auto fractional_percentile = static_cast<double>(percentile) / 100.0;
      auto idx = fractional_percentile * (_march_durations.size() - 1);
      auto fractional = fract(idx);
      if (fractional > 0.0) {
        // if we landed between two indices, use the latter
        return _march_durations[static_cast<size_t>(ceil(idx))];
      } else {
        // if we landed on an index, use the average of it and the one following
        auto i_idx = static_cast<size_t>(floor(idx));
        auto j_idx = min(i_idx + 1, _march_durations.size() - 1);
        return (_march_durations[i_idx] + _march_durations[j_idx]) / 2;
      }
    };

    const auto fifth_percentile_duration = find_percentile(5);
    const auto twentyfifth_percentile_duration = find_percentile(25);
    const auto seventyfifth_percentile_duration = find_percentile(75);
    const auto ninetyfifth_percentile_duration = find_percentile(95);

    Report(datum{result{
        _configuration_permutations[_current_configuration_permutation],
        _num_threads_used,
        _num_voxels_marched,
        _march_durations.size(),
        min_duration,
        max_duration,
        avg_duration,
        median_duration,
        fifth_percentile_duration,
        twentyfifth_percentile_duration,
        seventyfifth_percentile_duration,
        ninetyfifth_percentile_duration
    }});
  }

  void ResetPerformanceData() {
    _march_durations.clear();
  }

  bool HasMoreConfigurationPermutations() const {
    return _current_configuration_permutation
        < _configuration_permutations.size() - 1;
  }

  void NextConfigurationPermutation() {
    SetCurrentConfigurationPermutation(
        _current_configuration_permutation + 1);
  }

  /*
   * Set the current execution_configuration and re-build the
   * marching stuff.
   */
  void SetCurrentConfigurationPermutation(size_t idx) {

    Log::I(TAG,
           "Switching to configuration %d of %d",
           idx,
           _configuration_permutations.size());

    _current_configuration_permutation = idx;

    BuildExecConfiguration(_configuration_permutations[idx]);
  }

  /*
   * Called each time SetCurrentConfigurationPermutation
   * is called; configures the ThreadedMarcher & ThreadPool
   * for the current execution_configuration
   */
  void BuildExecConfiguration(execution_configuration ex_config) {
    auto affinity = ToAffinity(ex_config.thread_setup);
    auto max_thread_count = NumCores(affinity);

    if (ex_config.thread_setup == ThreadAffinitySetup::OneBigCore ||
        ex_config.thread_setup == ThreadAffinitySetup::OneLittleCore
        ) {
      max_thread_count = 1;
    }

    auto pool = std::make_unique<ThreadPool>(
        affinity, ex_config.pinned, max_thread_count);

    _num_threads_used = pool->NumThreads();

    // create one triangle consumer per thread, and unowned_ptrs
    // for the ThreadedMarcher to access
    std::vector<unowned_ptr<ITriangleConsumer>> tcs;
    _triangle_consumers.clear();
    for (auto i = 0; i < _num_threads_used; i++) {
      auto tc = std::make_unique<TriangleConsumer>();
      tcs.emplace_back(tc.get());
      _triangle_consumers.push_back(std::move(tc));
    }

    Log::I(TAG, "Using %d %s threads (%s) to march volume; sleep_dur: %d ms",
           _num_threads_used, ex_config.pinned ? "pinned" : "floating",
           ThreadAffinitySetupNames[static_cast<int>(ex_config.thread_setup)],
           ex_config.sleep_per_iteration
    );

    // now create the marcher
    _marcher = std::make_unique<marching_cubes::ThreadedMarcher>(
        _volume, tcs, std::move(pool), _volume_transform, false);

    // trigger a new warmup for the next test pass
    _warming_up = true;
    _recorded_first_step_timestamp = false;
  }

  /*
   * Called once at setup time; builds the volume which will be marched
   */
  void BuildVolume() {
    auto size = vec3(_volume.Size());
    auto center = size / 2.0F;

    _volume.Add(std::make_unique<SphereVolumeSampler>(
        center,
        length(size) * 0.25F,
        IVolumeSampler::Mode::Additive));

    _sampler_cutout_plane =
        _volume.Add(std::make_unique<PlaneVolumeSampler>(
            center,
            vec3(0, 1, 0),
            1,
            IVolumeSampler::Mode::Subtractive));

    // create a transform to map our _volume to the origin,
    // and be of reasonable size
    auto volume_size = length(vec3(_volume.Size()));
    _volume_transform = glm::scale(mat4(1), vec3(2.5F / volume_size))
        * glm::translate(mat4(1), -vec3(_volume.Size()) / 2.0F);

    // record the number of voxels that will be marched
    _num_voxels_marched = static_cast<size_t>(
        _volume.Size().x * _volume.Size().y * _volume.Size().z);
  }

 private:

  // configuration state
  configuration _configuration;
  std::vector<execution_configuration> _configuration_permutations;
  size_t _current_configuration_permutation = 0;

  // render state
  ProgramState _program;
  mat4 _proj{1};
  float _camera_z_position = -4;

  // marching cubes state
  Volume _volume{vec3(25), 2};
  mat4 _volume_transform{1};
  std::vector<std::unique_ptr<ITriangleConsumer>> _triangle_consumers;
  unowned_ptr<PlaneVolumeSampler> _sampler_cutout_plane;
  std::unique_ptr<marching_cubes::ThreadedMarcher> _marcher;

  // perf recording state
  size_t _num_voxels_marched = 0;
  size_t _num_threads_used = 0;
  std::vector<Duration> _march_durations;

  bool _warming_up = true;
  bool _recorded_first_step_timestamp = false;
  Timestamp _first_step_timestamp{};
};

EXPORT_ANCER_OPERATION(MarchingCubesGLES3Operation)