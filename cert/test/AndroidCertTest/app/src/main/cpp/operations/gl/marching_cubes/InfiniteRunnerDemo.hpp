/*
 * Copyright 2020 The Android Open Source Project
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

#ifndef infinite_runner_demo_hpp
#define infinite_runner_demo_hpp

#include <ancer/util/Log.hpp>

#include "OpQueue.hpp"
#include "TerrainSamplers.hpp"
#include "Materials.hpp"
#include "Camera.hpp"
#include "TerrainSegment.hpp"

namespace marching_cubes {

class InfiniteRunnerDemo {
 private:
  static constexpr ancer::Log::Tag TAG{"InfiniteRunnerDemo"};

 public:

  explicit InfiniteRunnerDemo(std::shared_ptr<ThreadPool> threadPool)
      : _threadPool(threadPool) {
  }

  void OnGlContextReady() {
    using namespace ancer::glh;
    CheckGlError("OnGlContextReady()");

    //
    // load materials
    //

    const auto skyboxTexture =
        ancer::glh::LoadTextureCube("Textures/MarchingCubesGLES3Operation/sky", ".jpg");

    const auto ambientLight = vec3(0.1, 0.15, 0.0);
    const auto renderDistance = kSegmentSize * 2;
    const auto terrainTexture0 =
        ancer::glh::LoadTexture2D("Textures/MarchingCubesGLES3Operation/granite.jpg");
    const auto terrainTexture0Scale = 30;
    const auto terrainTexture1 =
        ancer::glh::LoadTexture2D("Textures/MarchingCubesGLES3Operation/cracked-asphalt.jpg");
    const auto terrainTexture1Scale = 30;

    _terrainMaterial = std::make_unique<TerrainMaterial>(
        ambientLight,
        skyboxTexture,
        skyboxTexture,
        terrainTexture0, terrainTexture0Scale,
        terrainTexture1, terrainTexture1Scale,
        renderDistance);

    _lineMaterial = std::make_unique<LineMaterial>();

    _skydomeMaterial = std::make_unique<SkydomeMaterial>(skyboxTexture);

    //
    // some constant GL state
    //

    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //
    // Build static geometry
    //

    {
      CheckGlError("OnGlContextReady() - about to create sky quad");

      using V = decltype(_sky)::vertex_type;
      _sky.start();
      _sky.addTriangle(Triangle{
          V{vec3(-1, -1, 1), vec4(1, 0, 0, 1)},
          V{vec3(+1, -1, 1), vec4(0, 1, 0, 1)},
          V{vec3(+1, +1, 1), vec4(0, 0, 1, 1)}});
      _sky.addTriangle(Triangle{
          V{vec3(-1, -1, 1), vec4(0, 1, 1, 1)},
          V{vec3(+1, +1, 1), vec4(1, 0, 1, 1)},
          V{vec3(-1, +1, 1), vec4(1, 1, 0, 1)}});
      _sky.finish();

      _axisMarker.addAxis(mat4{1}, 64);
    }

    //
    //  Build our terrain segments
    //

    constexpr auto COUNT = 3;
    for (int i = 0; i < COUNT; i++) {
      _segments.emplace_back(std::make_unique<TerrainSegment>(kSegmentSize, _threadPool));
      _segments.back()->build(i);
      _segments.back()->march();
    }

    const auto size = vec3(kSegmentSize);
    const auto center = size / 2.0F;

    _lastWaypoint = vec3{center.x, center.y, 0};
    _nextWaypoint = _segments.front()->waypoints.front();

    if (kAutopilot) {
      _updateCamera(0);
    } else {
      _camera.lookAt(vec3(center.x, center.y, 0), center);
    }
  }

  void OnGlContextResized(int width, int height) {
    _aspect = static_cast<float>(width) / static_cast<float>(height);
    _projection = glm::perspective(radians(kFovDegrees), _aspect, kNearPlane, kFarPlane);
  }

  void Draw(double delta_seconds) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const mat4 view = _camera.view();

    // draw skydome
    glDepthMask(GL_TRUE);
    _skydomeMaterial->bind(_projection, view);
    _sky.draw();

    // draw the terrain segment volumes
    for (size_t i = 0, N = _segments.size(); i < N; i++) {
      const auto &segment = _segments[i];
      const auto model = translate(mat4{1}, vec3(0, 0, (i * kSegmentSize) - _distanceAlongZ));
      _terrainMaterial->bind(model, view, _projection, _camera.position);
      for (auto &tc : segment->triangles) {
        tc->draw();
      }
    }

    glDepthMask(GL_FALSE);

    // draw the origin of our scene
    _lineMaterial->bind(_projection * view * mat4{1});
    _axisMarker.draw();

    // draw optional markers, aabbs, etc
    if (kDrawOctreeAABBs || kDrawWaypoints || kDrawSegmentBounds) {
      for (size_t i = 0, N = _segments.size(); i < N; i++) {
        const auto &segment = _segments[i];
        const auto model = translate(mat4{1}, vec3(0, 0, (i * kSegmentSize) - _distanceAlongZ));
        _lineMaterial->bind(_projection * view * model);
        if (kDrawSegmentBounds) {
          segment->boundingLineBuffer.draw();
        }
        if (kDrawOctreeAABBs) {
          segment->aabbLineBuffer.draw();
        }
        if (kDrawWaypoints) {
          segment->waypointLineBuffer.draw();
        }
      }
    }

    glDepthMask(GL_TRUE);
  }

  void Step(double delta_seconds) {
    MainThreadQueue()->drain();
    _updateScroll(delta_seconds);
    _updateCamera(delta_seconds);
  }

 private:

  void _updateScroll(float deltaT) {
    const float scrollDelta = kScrolling ? 100 * deltaT : 0;
    _distanceAlongZ += scrollDelta;

    if (_distanceAlongZ > kSegmentSize) {
      _distanceAlongZ -= kSegmentSize;

      // we moved forward enough to hide first segment,
      // it can be repurposed. pop front segment, generate
      // a new end segment, and push it back

      auto seg = std::move(_segments.front());
      _segments.pop_front();

      seg->build(_segments.back()->idx + 1);
      seg->march();

      _segments.push_back(std::move(seg));
    }

    _lastWaypoint.z -= scrollDelta;
    _nextWaypoint.z -= scrollDelta;

    if (_nextWaypoint.z <= 0) {
      _lastWaypoint = _nextWaypoint;

      // find the next waypoint
      for (size_t i = 0, N = _segments.size(); i < N; i++) {
        bool found = false;
        float dz = (i * kSegmentSize) - _distanceAlongZ;
        const auto &segment = _segments[i];
        for (const auto &waypoint : segment->waypoints) {
          auto waypointWorld = vec3(waypoint.x, waypoint.y, waypoint.z + dz);
          if (waypointWorld.z > 0) {
            _nextWaypoint = waypointWorld;
            found = true;
            break;
          }
        }
        if (found) {
          break;
        }
      }
    }
  }

  void _updateCamera(float deltaT) {
    if (kAutopilot) {
      float t = (0 - _lastWaypoint.z) / (_nextWaypoint.z - _lastWaypoint.z);
      vec3 autoPilotPosition = mix(_lastWaypoint, _nextWaypoint, t);

      _autopilotPositionSpring.setTarget(autoPilotPosition);
      _autopilotTargetSpring.setTarget(_nextWaypoint);

      autoPilotPosition = _autopilotPositionSpring.step(deltaT);
      vec3 autoPilotTargetPosition = _autopilotTargetSpring.step(deltaT);

      _camera.lookAt(autoPilotPosition, autoPilotTargetPosition);
    }
  }

 private:

  static constexpr float kNearPlane = 0.1f;
  static constexpr float kFarPlane = 1000.0f;
  static constexpr float kFovDegrees = 50.0F;
  static constexpr int kSegmentSize = 256;
  static constexpr bool kAutopilot = true;

  // drawing flags
  static constexpr bool kScrolling = true;
  static constexpr bool kDrawOctreeAABBs = false;
  static constexpr bool kDrawWaypoints = true;
  static constexpr bool kDrawSegmentBounds = true;

 private:

  // render state
  Camera _camera;
  float _aspect = 1;
  mat4 _projection;
  std::unique_ptr<TerrainMaterial> _terrainMaterial;
  std::unique_ptr<LineMaterial> _lineMaterial;
  std::unique_ptr<SkydomeMaterial> _skydomeMaterial;
  TriangleConsumer<VertexP3C4> _sky;
  LineSegmentBuffer _axisMarker;

  // demo state
  std::shared_ptr<ThreadPool> _threadPool;
  float _distanceAlongZ = 0;
  std::deque<std::unique_ptr<TerrainSegment>> _segments;
  vec3 _lastWaypoint, _nextWaypoint;
  spring3 _autopilotPositionSpring{5, 40, 20};
  spring3 _autopilotTargetSpring{5, 20, 10};
};

} // namespace marching_cubes

#endif