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

#ifndef terrain_segment_hpp
#define terrain_segment_hpp

#include <vector>
#include <glm/gtc/noise.hpp>

#include <ancer/util/Log.hpp>
#include <ancer/util/GLHelpers.hpp>
#include <ancer/util/UnownedPtr.hpp>

#include "Camera.hpp"
#include "MarchingCubes.hpp"
#include "Materials.hpp"
#include "Spring.hpp"
#include "TerrainSamplers.hpp"
#include "ThreadPool.hpp"
#include "Volume.hpp"
#include "XORShiftRand.hpp"

namespace marching_cubes {

struct TerrainSegment {
 private:

  static constexpr ancer::Log::Tag TAG{"TerrainSegment"};

  const MaterialState kFloorTerrainMaterial{
      vec4(0, 0, 0, 1),
      1,
      0,
      0
  };

  const MaterialState kLowTerrainMaterial{
      vec4(1, 1, 1, 1),
      0,
      1,
      0
  };

  const MaterialState kHighTerrainMaterial{
      vec4(0.3, 0.3, 0.3, 1),
      0,
      1,
      1
  };

  const MaterialState kArchMaterial{
      vec4(0.1, 0.2, 0.1, 1),
      0.125,
      0,
      1
  };

  const float kFloorThreshold = 1.0F;

 public:
  TerrainSegment(int size, std::shared_ptr<ThreadPool> threadPool)
      : idx(-1),
        size(size),
        threadPool(threadPool) {
    std::vector<ancer::unowned_ptr<TriangleConsumer<Vertex >>> unownedTriangleConsumers;
    for (size_t i = 0, N = threadPool->size(); i < N; i++) {
      triangles.push_back(std::make_unique<TriangleConsumer<Vertex >>());
      unownedTriangleConsumers.push_back(triangles.back().get());
    }

    const int minNodeSize = 4;
    const float fuzziness = 2.0F;
    volume = std::make_unique<OctreeVolume>(size,
                                            fuzziness,
                                            minNodeSize,
                                            threadPool,
                                            unownedTriangleConsumers);

    //  +1 gives us the edge case for marching
    heightmap.resize((size + 1) * (size + 1));
  }

  ~TerrainSegment() = default;
  TerrainSegment(const TerrainSegment &) = delete;
  TerrainSegment(TerrainSegment &&) = delete;
  TerrainSegment &operator==(const TerrainSegment &) = delete;

  void build(int idx) {
    using namespace glm;

    this->idx = idx;
    volume->clear();
    triangleCount = 0;
    for (auto &tc : triangles) {
      tc->clear();
    }

    waypoints.clear();
    waypointLineBuffer.clear();

    const auto segmentColor = rainbow(static_cast<float>(idx % 5) / 5.0F);

    //
    // build terrain sampler
    //

    volume->add(std::make_unique<HeightmapSampler>(heightmap.data(),
                                                   size,
                                                   maxTerrainHeight,
                                                   kFloorThreshold,
                                                   kFloorTerrainMaterial,
                                                   kLowTerrainMaterial,
                                                   kHighTerrainMaterial));

    //
    //  Build an RNG seeded for this segment
    //

    auto rng = rng_xorshift64{static_cast<uint64_t>(12345 * (idx + 1))};

    //
    // build arches
    //

    const float zOffset = idx * size;
    const auto size = vec3(volume->size());
    const auto center = size / 2.0F;
    const auto maxArches = 7;
    for (int i = 0; i < maxArches; i++) {

      // roll the dice to see if we get an arch here
      if (rng.nextInt(10) < 5) {
        continue;
      }

      // we have an arch, get its z position, use that to feed simplex
      // noise to perturb x position smoothly. Note - we inset a bit to
      // reduce likelyhood of clipping
      // TODO: Use simplex to seed our arches too... this would prevent clipping
      // because arches would clone across segment boundaries
      float archZ = 30 + (this->size - 60) * static_cast<float>(i) / maxArches;
      float archX = center.x + glm::simplex(vec2((archZ + zOffset) / size.z, 0)) * size.x * 0.125F;

      Tube::Config arch;
      arch.axisOrigin = vec3{archX, 0, archZ};

      arch.innerRadiusAxisOffset = vec3(0, rng.nextFloat(4, 10), 0);

      arch.axisDir = normalize(vec3(rng.nextFloat(-0.6, 0.6), rng.nextFloat(-0.2, 0.2), 1));

      arch.axisPerp = normalize(vec3(rng.nextFloat(-0.2, 0.2), 1, 0));
      arch.length = rng.nextFloat(7, 11);
      arch.innerRadius = rng.nextFloat(35, 43);
      arch.outerRadius = rng.nextFloat(48, 55);
      arch.frontFaceNormal = arch.axisDir;
      arch.backFaceNormal = -arch.axisDir;
      arch.cutAngleRadians = radians(rng.nextFloat(16, 32));
      arch.material = kArchMaterial;

      volume->add(std::make_unique<Tube>(arch));

      vec3 waypoint = arch.axisOrigin +
          (arch.axisPerp * arch.innerRadius * rng.nextFloat(0.2F, 0.8F));
      waypoint.y = std::max(waypoint.y, maxTerrainHeight + 2);
      waypoints.push_back(waypoint);
      waypointLineBuffer.addMarker(waypoint, 4, segmentColor);
    }

    // all segments have to have at least 1 waypoint; if the dice roll
    // didn't give us any arches, make a default waypoint
    if (waypoints.empty()) {
      vec3 waypoint = vec3(center.x, maxTerrainHeight + rng.nextFloat(10), center.z);
      waypoints.push_back(waypoint);
      waypointLineBuffer.addMarker(waypoint, 4, vec4(1, 1, 0, 1));
    }

    //
    //  Build a debug frame to show our volume
    //

    boundingLineBuffer.clear();
    boundingLineBuffer.add(ancer::glh::AABB(vec3{0.0F}, size).Inset(1), segmentColor);
  }

  void march() {
    aabbLineBuffer.clear();

    const auto nodeObserver = [this](OctreeVolume::Node *node) {
      {
        // update the occupied aabb display
        auto bounds = node->bounds;
        bounds.Inset(node->depth * 0.005F);
        aabbLineBuffer.add(bounds, nodeColor(node->depth));
      }
    };

    const auto onMarchComplete = [this]() {
      triangleCount = 0;
      for (const auto &tc : triangles) {
        triangleCount += tc->numTriangles();
      }

      ancer::Log::I(TAG, "march of segment %d complete", this->idx);
    };

    updateHeightmap();
    volume->marchAsync(onMarchComplete, nodeObserver);
  }

  void updateHeightmap() {
    const auto zOffset = idx * size;
    const auto dim = size + 1;

    int slices = threadPool->size();
    int sliceHeight = dim / slices;
    float noiseFrequency = 1.0f / size;
    std::vector<std::future<void>> workers;
    for (int slice = 0; slice < slices; slice++) {
      workers.push_back(threadPool->enqueue([=](int tIdx) {
        int sliceStart = slice * sliceHeight;
        int sliceEnd = sliceStart + sliceHeight;
        if (slice == slices - 1) {
          sliceEnd = dim;
        }

        for (int y = sliceStart; y < sliceEnd; y++) {
          for (int x = 0; x < dim; x++) {
            float n = glm::simplex(vec2(x * noiseFrequency, (y + zOffset) * noiseFrequency)) * 0.5F
                + 0.5F;

            // sand-dune like structures
            float dune = n;
            dune = dune - floor(dune);
            dune = dune * dune * maxTerrainHeight;

            float dune2 = n * 2;
            dune2 = dune2 - floor(dune2);
            dune2 = smoothstep(dune2) * maxTerrainHeight;

            // floor
            float f = smoothstep(n);
            float floor = maxTerrainHeight * f;

            heightmap[y * size + x] = (floor + dune + dune2) / 3.0F;
          }
        }
      }));
    }

    for (auto &w : workers) {
      w.wait();
    }
  }

  static float smoothstep(float t) {
    return t * t * (3 - 2 * t);
  }

  static float smoothstep(float edge0, float edge1, float t) {
    t = (t - edge0) / (edge1 - edge0);
    return t * t * (3 - 2 * t);
  }

  const float maxTerrainHeight = 8.0F;

  int idx = 0;
  int size = 0;
  std::shared_ptr<ThreadPool> threadPool;
  int triangleCount;
  std::unique_ptr<OctreeVolume> volume;
  std::vector<std::unique_ptr<TriangleConsumer<Vertex>>>
      triangles;
  LineSegmentBuffer aabbLineBuffer;
  LineSegmentBuffer boundingLineBuffer;
  LineSegmentBuffer waypointLineBuffer;
  std::vector<vec3> waypoints;
  std::vector<float> heightmap;
  double lastMarchDurationSeconds = 0;

 private:
  std::vector<vec4> _nodeColors;

  vec4 rainbow(float dist) const {
    using namespace ancer::glh::color;
    const hsv hC{360 * dist, 0.6F, 1.0F};
    const auto rgbC = hsv2rgb(hC);
    return vec4(rgbC.r, rgbC.g, rgbC.b, 1);
  }

  vec4 nodeColor(int atDepth) {
    using namespace ancer::glh::color;

    auto depth = volume->depth();
    if (_nodeColors.size() < depth) {
      _nodeColors.clear();
      const float hueStep = 360.0F / depth;
      for (auto i = 0U; i <= depth; i++) {
        const hsv hC{i * hueStep, 0.6F, 1.0F};
        const auto rgbC = hsv2rgb(hC);
        _nodeColors.emplace_back(rgbC.r,
                                 rgbC.g,
                                 rgbC.b,
                                 glm::mix<float>(0.6, 0.25, static_cast<float>(i) / depth));
      }
    }

    return _nodeColors[atDepth];
  }
};

} // namespace marching_cubes

#endif