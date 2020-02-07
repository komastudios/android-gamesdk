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

#include <array>
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <random>
#include <thread>

#include <ancer/BaseGLES3Operation.hpp>
#include <ancer/util/GLHelpers.hpp>
#include <ancer/util/Json.hpp>
#include <ancer/util/UnownedPtr.hpp>

#include "operations/gl/marching_cubes/MarchingCubes.hpp"
#include "operations/gl/marching_cubes/Volume.hpp"
#include "operations/gl/marching_cubes/VolumeSamplers.hpp"
#include "operations/gl/marching_cubes/Demos.hpp"

using std::make_unique;
using std::unique_ptr;
using ancer::unowned_ptr;

using namespace ancer;
namespace mc = marching_cubes;

namespace {

//
//  Constants
//

constexpr Log::Tag TAG{"MarchingCubesGLES3Operation"};
constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 1000.0f;
constexpr float FOV_DEGREES = 50.0F;
constexpr float OCTREE_NODE_VISUAL_INSET_FACTOR = 0.0025F;

//
//  Configuration
//

enum class ThreadAffinitySetup {
  OneBigCore, OneLittleCore, AllBigCores, AllLittleCores, AllCores
};

constexpr const char* ThreadAffinitySetupNames[] = {
    "OneBigCore", "OneLittleCore", "AllBigCores", "AllLittleCores", "AllCores"
};

enum class JobBatchingSetup {
  OneNodePerJob, ManyNodesPerJob, AutoBalancedNodesPerJob, AutoQueuedNodesPerJob
};

constexpr const char* JobBatchingSetupNames[] = {
    "OneNodePerJob", "ManyNodesPerJob", "AutoBalancedNodesPerJob",
    "AutoQueuedNodesPerJob"
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

struct configuration {
  Duration warm_up_time = 5s;
  ThreadAffinitySetup thread_affinity = ThreadAffinitySetup::AllBigCores;
  bool thread_pinned = true;
  Duration sleep_duration = 0ns;
  JobBatchingSetup
      job_batching_setup = JobBatchingSetup::AutoBalancedNodesPerJob;
};

JSON_CONVERTER(configuration) {
  JSON_OPTVAR(warm_up_time);
  JSON_REQENUM(thread_affinity, ThreadAffinitySetupNames);
  JSON_REQVAR(thread_pinned);
  JSON_REQVAR(sleep_duration);
  JSON_REQENUM(job_batching_setup, JobBatchingSetupNames);
}

//
//  Reporting Data
//

struct result {
  configuration exec_configuration;
  size_t num_threads_used = 0;
  size_t num_iterations = 0;
  double min_vps = 0;
  double max_vps = 0;
  double average_vps = 0;
  double median_vps = 0;
  double fifth_percentile_vps = 0;
  double twentyfifth_percentile_vps = 0;
  double seventyfifth_percentile_vps = 0;
  double ninetyfifth_percentile_vps = 0;
};

JSON_CONVERTER(result) {
  JSON_REQVAR(exec_configuration);
  JSON_REQVAR(num_threads_used);
  JSON_REQVAR(num_iterations);
  JSON_REQVAR(min_vps);
  JSON_REQVAR(max_vps);
  JSON_REQVAR(average_vps);
  JSON_REQVAR(median_vps);
  JSON_REQVAR(fifth_percentile_vps);
  JSON_REQVAR(twentyfifth_percentile_vps);
  JSON_REQVAR(seventyfifth_percentile_vps);
  JSON_REQVAR(ninetyfifth_percentile_vps);
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

  ~MarchingCubesGLES3Operation() override {
    if (!_march_performance_data.empty()) {
      ReportPerformanceData();
    }
  }

  void OnGlContextReady(const GLContextConfig& ctx_config) override {
    _configuration = GetConfiguration<configuration>();

    {
      auto vert_file = "Shaders/MarchingCubesGLES3Operation/volume.vsh";
      auto frag_file = "Shaders/MarchingCubesGLES3Operation/volume.fsh";
      if (!_volume_program.Build(vert_file, frag_file)) {
        Stop();
        return;
      }
    }

    {
      auto vert_file = "Shaders/MarchingCubesGLES3Operation/line.vsh";
      auto frag_file = "Shaders/MarchingCubesGLES3Operation/line.fsh";
      if (!_line_program.Build(vert_file, frag_file)) {
        Stop();
        return;
      }
    }

    glClearColor(0.2F, 0.2F, 0.22F, 0.0F);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    BuildExecConfiguration();
  }

  void OnGlContextResized(int width, int height) override {
    BaseOperation::OnGlContextResized(width, height);
    _aspect = static_cast<float>(width) / static_cast<float>(height);
  }

  void Draw(double delta_seconds) override {
    BaseGLES3Operation::Draw(delta_seconds);

    auto sleep_dur = _configuration.sleep_duration;
    if (sleep_dur > 0ms) {
      std::this_thread::sleep_for(sleep_dur);
    }

    Step(delta_seconds);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto mvp = MVP();

    _volume_program.Bind(mvp, _model);

    glDepthMask(GL_TRUE);
    for (auto& tc : _triangle_consumers) {
      tc->Draw();
    }

    glDepthMask(GL_FALSE);
    _line_program.Bind(mvp, _model);
    _node_aabb_line_buffer.Draw();

    glDepthMask(GL_TRUE);
  }

  void Step(double delta_seconds) {
    _animation_time += static_cast<float>(delta_seconds);
    if (_volume && _current_demo) {

      // make camera orbit about y, with a gently bobbing up and down
      float orbit_y = _animation_time * glm::pi<float>() * 0.125F;
      float orbit_tilt_phase = _animation_time * glm::pi<float>() * 0.0625F;
      float orbit_tilt = sin(orbit_tilt_phase) * glm::pi<float>() * 0.125F;
      _trackball_rotation =
          glm::rotate(glm::rotate(mat4{1}, orbit_tilt, vec3(1, 0, 0)),
                      orbit_y,
                      vec3(0, 1, 0));

      // update demo animation cycle
      _current_demo->Step(_animation_time);

      // update geometry for rendering
      MarchVolume();
    }
  }

  void Stop() override {
    BaseGLES3Operation::Stop();
    ReportPerformanceData();
  }

 private:

  /*
   * Create a model-view-projection matrix for rendering the current frame
   */
  mat4 MVP() const {
    // extract trackball Z and Y for building view matrix via lookAt
    auto trackball_y =
        glm::vec3{_trackball_rotation[0][1], _trackball_rotation[1][1],
                  _trackball_rotation[2][1]};
    auto trackball_z =
        glm::vec3{_trackball_rotation[0][2], _trackball_rotation[1][2],
                  _trackball_rotation[2][2]};
    mat4 view, proj;

    if (_use_ortho_projection) {
      auto bounds = _volume->GetBounds();
      auto size = length(bounds.Size());

      auto scaleMin = 0.1F;
      auto scaleMax = 5.0F;
      auto scale = mix(scaleMin, scaleMax, pow(_dolly, 2.5F));

      auto width = scale * _aspect * size;
      auto height = scale * size;

      auto distance = FAR_PLANE / 2;
      view = lookAt(-distance * trackball_z, vec3(0), trackball_y);

      proj = glm::ortho(-width / 2,
                        width / 2,
                        -height / 2,
                        height / 2,
                        NEAR_PLANE,
                        FAR_PLANE);
    } else {
      auto bounds = _volume->GetBounds();
      auto minDistance = 0.1F;
      auto maxDistance = length(bounds.Size()) * 2;

      auto distance = mix(minDistance, maxDistance, pow<float>(_dolly, 2));
      view = lookAt(-distance * trackball_z, vec3(0), trackball_y);

      proj = glm::perspective(radians(FOV_DEGREES),
                              _aspect,
                              NEAR_PLANE,
                              FAR_PLANE);
    }
    return proj * view * _model;
  }

  void ReportPerformanceData() {
    if (_march_performance_data.empty()) {
      Log::E(TAG, "ReportPerformanceData - _march_durations is empty");
      return;
    }

    // transform our perf datums into an array of voxels per second
    std::vector<double> voxels_per_second;
    voxels_per_second.resize(_march_performance_data.size());
    std::transform(_march_performance_data.begin(),
                   _march_performance_data.end(), voxels_per_second.begin(),
                   [](const PerfDatum& d) {
                     return d.VoxelsPerSecond();
                   });

    // compute the average
    const auto avg_vps = std::accumulate(
        voxels_per_second.begin(), voxels_per_second.end(), 0.0
    ) / voxels_per_second.size();

    // compute min/max/median
    std::sort(voxels_per_second.begin(), voxels_per_second.end());
    const auto min_vps = voxels_per_second.front();
    const auto max_vps = voxels_per_second.back();

    const auto median_vps = [&voxels_per_second]() {
      if (voxels_per_second.size() % 2 == 1) {
        return voxels_per_second[voxels_per_second.size() / 2];
      } else {
        size_t center = voxels_per_second.size() / 2;
        return (voxels_per_second[std::max<size_t>(center - 1, 0)]
            + voxels_per_second[center]) / 2;
      }
    }();

    // compute percentiles
    auto find_percentile = [&voxels_per_second](int percentile) -> double {
      percentile = min(max(percentile, 0), 100);
      auto fractional_percentile = static_cast<double>(percentile) / 100.0;
      auto idx = fractional_percentile * (voxels_per_second.size() - 1);
      auto fractional = fract(idx);
      if (fractional > 0.0) {
        // if we landed between two indices, use the latter
        return voxels_per_second[static_cast<size_t>(ceil(idx))];
      } else {
        // if we landed on an index, use the average of it and the one following
        auto i_idx = static_cast<size_t>(floor(idx));
        auto j_idx = min(i_idx + 1, voxels_per_second.size() - 1);
        return (voxels_per_second[i_idx] + voxels_per_second[j_idx]) / 2;
      }
    };

    const auto fifth_percentile_vps = find_percentile(5);
    const auto twentyfifth_percentile_vps = find_percentile(25);
    const auto seventyfifth_percentile_vps = find_percentile(75);
    const auto ninetyfifth_percentile_vps = find_percentile(95);

    Report(datum{result{
        _configuration,
        _num_threads_used,
        _march_performance_data.size(),
        min_vps,
        max_vps,
        avg_vps,
        median_vps,
        fifth_percentile_vps,
        twentyfifth_percentile_vps,
        seventyfifth_percentile_vps,
        ninetyfifth_percentile_vps
    }});

    _march_performance_data.clear();
  }

  void BuildExecConfiguration() {

    //
    //  Create the job queue with our configuration's
    //  affinity, pinning, count
    //
    auto affinity = ToAffinity(_configuration.thread_affinity);
    auto pinned = _configuration.thread_pinned;
    auto max_thread_count = NumCores(affinity);

    if (_configuration.thread_affinity == ThreadAffinitySetup::OneBigCore ||
        _configuration.thread_affinity == ThreadAffinitySetup::OneLittleCore) {
      max_thread_count = 1;
    }

    _job_queue = std::make_unique<mc::job::JobQueue>(
        affinity, pinned, max_thread_count);

    _num_threads_used = _job_queue->NumThreads();

    //
    //  We need ONE ITriangleConsumer per thread, so that each thread
    //  processing the volume can safely write to its dedicated store.
    //  We own the triangle consumers, so we make unowned_ptrs to hand
    //  to the _volume
    //

    std::vector<unowned_ptr<mc::ITriangleConsumer>> tcs;
    _triangle_consumers.clear();
    for (auto i = 0; i < _num_threads_used; i++) {
      auto tc = make_unique<mc::TriangleConsumer>();
      tcs.emplace_back(tc.get());
      _triangle_consumers.push_back(std::move(tc));
    }

    Log::I(TAG,
           "Using %d %s threads (%s) to march volume; sleep_dur: %d ns; batching: %s",
           _num_threads_used,
           pinned ? "pinned" : "floating",
           ThreadAffinitySetupNames[static_cast<int>(_configuration.thread_affinity)],
           _configuration.sleep_duration,
           JobBatchingSetupNames[static_cast<int>(_configuration.job_batching_setup)]
    );

    //
    //  Build the volume and demo to render into it
    //

    _volume = make_unique<mc::OctreeVolume>(64, 1.0F, 4, _job_queue.get(), tcs);
    _model = glm::translate(mat4{1}, -vec3(_volume->GetBounds().Center()));

    _current_demo = make_unique<mc::demos::CompoundShapesDemo>(10, 10);
    _current_demo->Build(_volume.get());

    //
    //  Trigger a warmup phase for the next test pass
    //

    _warming_up = true;
    _recorded_first_step_timestamp = false;
  }

  /*
   * Helper for visualation of the octree - generates a unique
   * color for an octree node based on its depth
   */
  vec4 GetNodeColor(int atDepth) const {
    using namespace ancer::glh::color;
    static std::vector<vec4> nodeColors;

    auto depth = _volume->GetDepth();
    if (nodeColors.size() != depth) {
      nodeColors.clear();
      const float hueStep = 360.0F / depth;
      for (auto i = 0U; i <= depth; i++) {
        const hsv hC{i * hueStep, 0.6F, 1.0F};
        const auto rgbC = hsv2rgb(hC);
        nodeColors.emplace_back(rgbC.r,
                                rgbC.g,
                                rgbC.b,
                                mix<float>(0.6F,
                                           0.25F,
                                           static_cast<float>(i) / depth));
      }
    }

    return nodeColors[atDepth];
  }

  /*
   * March the volume, recording a PerfDatum
   */
  void MarchVolume() {

    _node_aabb_line_buffer.Clear();
    const auto start_time = SteadyClock::now();
    size_t num_voxels = 0;
    auto batch_size = 0;
    switch (_configuration.job_batching_setup) {
      case JobBatchingSetup::AutoBalancedNodesPerJob:
        batch_size = mc::OctreeVolume::BATCH_USING_BALANCED_LOAD;
        break;
      case JobBatchingSetup::AutoQueuedNodesPerJob:
        batch_size = mc::OctreeVolume::BATCH_USING_QUEUE;
        break;
      case JobBatchingSetup::OneNodePerJob:
        batch_size = 1;
        break;
      case JobBatchingSetup::ManyNodesPerJob:
        batch_size = 32;
        break;
    }

    _volume->March(false, batch_size,
                   [this, &num_voxels](mc::OctreeVolume::Node* node) {
                     {
                       // we add each node to our line buffer so we can visualize marched nodes
                       auto bounds = node->bounds;
                       bounds.Inset(
                           node->depth * OCTREE_NODE_VISUAL_INSET_FACTOR);
                       _node_aabb_line_buffer.Add(bounds,
                                                  GetNodeColor(node->depth));
                     }

                     // record # of voxels marched
                     num_voxels += static_cast<size_t>(node->bounds.Volume());
                   });

    const auto end_time = SteadyClock::now();

    // we don't record timestamps until we've been running for a while
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
      _march_performance_data.emplace_back(end_time - start_time, num_voxels);
    }

  }

 private:

  struct ProgramState {
   private:
    GLuint _program = 0;
    GLint _uniform_loc_MVP = -1;
    GLint _uniform_loc_Model = -1;

   public:
    ProgramState() = default;
    ProgramState(const ProgramState& other) = delete;
    ProgramState(const ProgramState&& other) = delete;
    ~ProgramState() {
      if (_program > 0) {
        glDeleteProgram(_program);
      }
    }

    bool Build(const std::string& vert_file, const std::string& frag_file) {
      auto vert_src = LoadText(vert_file.c_str());
      auto frag_src = LoadText(frag_file.c_str());
      _program = glh::CreateProgramSrc(vert_src.c_str(), frag_src.c_str());
      if (_program == 0) {
        return false;
      }
      _uniform_loc_MVP = glGetUniformLocation(_program, "uMVP");
      _uniform_loc_Model = glGetUniformLocation(_program, "uModel");
      return true;
    }

    void Bind(const mat4& mvp, const mat4& model) {
      glUseProgram(_program);
      glUniformMatrix4fv(_uniform_loc_MVP, 1, GL_FALSE, value_ptr(mvp));
      glUniformMatrix4fv(_uniform_loc_Model, 1, GL_FALSE, value_ptr(model));
    }

  };

  struct PerfDatum {
   private:
    Duration _duration = Duration{0};
    size_t _num_voxels = 0;

   public:
    PerfDatum() = default;

    PerfDatum(Duration duration, size_t num_voxels)
        : _duration(duration), _num_voxels(num_voxels) {}

    PerfDatum(const PerfDatum&) = default;
    PerfDatum(PerfDatum&&) = default;

    double VoxelsPerSecond() const {
      auto elapsed_seconds = duration_cast<SecondsAs<double>>(_duration);
      return static_cast<double>(_num_voxels) / elapsed_seconds.count();
    }
  };

  // configuration state
  configuration _configuration;

  // render state
  ProgramState _volume_program, _line_program;
  bool _use_ortho_projection = false;
  mat4 _model{1};
  mat4 _trackball_rotation{1};
  float _dolly = 0.9F; // camera distance, ranges [0,1], remapped in MVP()
  float _aspect = 1; // apsect ratio of the framebuffer
  float _animation_time = 0;
  mc::LineSegmentBuffer _node_aabb_line_buffer;

  // marching cubes state
  unique_ptr<mc::job::JobQueue> _job_queue;
  unique_ptr<mc::OctreeVolume> _volume;
  std::vector<unique_ptr<mc::ITriangleConsumer>> _triangle_consumers;
  unique_ptr<mc::demos::Demo> _current_demo;

  // perf recording state
  size_t _num_threads_used = 0;
  std::vector<PerfDatum> _march_performance_data;
  bool _warming_up = true;
  bool _recorded_first_step_timestamp = false;
  Timestamp _first_step_timestamp{};
};

EXPORT_ANCER_OPERATION(MarchingCubesGLES3Operation)