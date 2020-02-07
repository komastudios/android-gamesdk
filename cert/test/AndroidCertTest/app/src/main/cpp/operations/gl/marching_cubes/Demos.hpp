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

#include <random>

#include "VolumeSamplers.hpp"

namespace marching_cubes::demos {

//
//  Geometry Demos
//

/*
 * Base class for our demos
 */
class Demo {
 public:
  Demo() = default;
  virtual ~Demo() = default;

  virtual void Build(ancer::unowned_ptr<BaseCompositeVolume> volume) = 0;
  virtual void Step(float time) = 0;
};

/*
 * CubeDemo simply renders a spinning cubes at center of volume
 */
class CubeDemo : public Demo {
 public:
  CubeDemo() = default;

  void Build(ancer::unowned_ptr<BaseCompositeVolume> volume) override {
    auto size = vec3(volume->Size());
    auto center = size / 2.0F;

    _rect =
        volume->Add(std::make_unique<samplers::RectangularPrismVolumeSampler>(
            center,
            vec3(10, 10, 10),
            mat3{1},
            IVolumeSampler::Mode::Additive));
  }

  void Step(float time) override {
    float angle = static_cast<float>(M_PI * time * 0.1);
    _rect->SetRotation(mat3{glm::rotate(mat4{1}, angle, vec3(0, 0, 1))});
  }

 private:
  ancer::unowned_ptr<samplers::RectangularPrismVolumeSampler> _rect;
};

/*
 * SphereDemo renders a bobbing/resizing sphere about center of volume
 */
class SphereDemo : public Demo {
 public:
  SphereDemo() = default;

  void Build(ancer::unowned_ptr<BaseCompositeVolume> volume) override {
    auto size = vec3(volume->Size());
    _pos = size / 2.0F;
    _radius = 10;

    _sphere = volume->Add(std::make_unique<samplers::SphereVolumeSampler>(
        _pos, _radius, IVolumeSampler::Mode::Additive));
  }

  void Step(float time) override {
    auto cycle = static_cast<float>(M_PI * time * 0.1);
    auto yOffset = sin(cycle) * 5;
    auto radiusOffset = cos(cycle) * _radius * 0.25F;

    _sphere->SetPosition(_pos + vec3(0, yOffset, 0));
    _sphere->SetRadius(_radius + radiusOffset);
  }

 private:
  vec3 _pos;
  float _radius;
  ancer::unowned_ptr<samplers::SphereVolumeSampler> _sphere;
};

/*
 * BoundedPlaneDemo renders a bounded plane about the center of the volume
 */
class BoundedPlaneDemo : public Demo {
 public:
  BoundedPlaneDemo() = default;

  void Build(ancer::unowned_ptr<BaseCompositeVolume> volume) override {
    auto size = vec3(volume->Size());
    auto center = size / 2.0F;
    _plane = volume->Add(std::make_unique<samplers::BoundedPlaneVolumeSampler>(
        center, planeNormal(mat4{1}), 10, IVolumeSampler::Mode::Additive));
  }

  void Step(float time) override {
    auto angle = static_cast<float>(pi<float>() * -time * 0.2F);
    auto rot = rotate(mat4{1}, angle, normalize(vec3(1, 1, 0)));
    _plane->SetPlaneNormal(planeNormal(rot));
  }

 private:
  static vec3 planeNormal(const glm::mat4& rot) {
    return vec3(rot[0][1], rot[1][1], rot[2][1]);
  }

  ancer::unowned_ptr<samplers::BoundedPlaneVolumeSampler> _plane;
  mat3 _rotation{1};
};

/*
 * HalfspaceDemo renders a halfspace about the center of the volume
 */
class HalfspaceDemo : public Demo {
 public:
  HalfspaceDemo() = default;

  void Build(ancer::unowned_ptr<BaseCompositeVolume> volume) override {
    auto size = vec3(volume->Size());
    auto center = size / 2.0F;
    _plane = volume->Add(std::make_unique<samplers::HalfspaceVolumeSampler>(
        center, _PlaneNormal(mat4{1}), IVolumeSampler::Mode::Additive));
  }

  void Step(float time) override {
    auto angle = static_cast<float>(pi<float>() * -time * 0.2F);
    auto rot = rotate(mat4{1}, angle, normalize(vec3(1, 1, 0)));
    _plane->SetPlaneNormal(_PlaneNormal(rot));
  }

 private:
  static vec3 _PlaneNormal(const glm::mat4& rot) {
    return vec3(rot[0][1], rot[1][1], rot[2][1]);
  }

  ancer::unowned_ptr<samplers::HalfspaceVolumeSampler> _plane;
  mat3 _rotation{1};
};

/*
 * CompoundShapeDemo shows a bunch of bobbing cubes and spheres,
 * with a halfspace cut out from the bottom
 */
class CompoundShapesDemo : public Demo {
 public:
  CompoundShapesDemo() = default;
  CompoundShapesDemo(int num_spheres, int num_cubes)
      : _num_spheres(num_spheres), _num_cubes(num_cubes) {}

  void Build(ancer::unowned_ptr<BaseCompositeVolume> volume) override {
    auto size = volume->Size();
    std::random_device rng;
    std::default_random_engine gen{static_cast<long unsigned int>(12345)};

    _bottom_plane =
        volume->Add(std::make_unique<samplers::HalfspaceVolumeSampler>(
            vec3(0, size.y * 0.45, 0),
            vec3(0, 1, 0),
            IVolumeSampler::Mode::Subtractive));

    if (_num_spheres > 0) {
      // create spheres
      auto max_radius = size.x / 6;
      std::uniform_real_distribution<float>
          x_dist(max_radius, size.x - max_radius);
      std::uniform_real_distribution<float>
          z_dist(max_radius, size.z - max_radius);
      std::uniform_real_distribution<float> y_dist(size.y * 0.4F, size.y * 0.6F);
      std::uniform_real_distribution<float>
          radius_dist(size.x / 20, max_radius);
      std::uniform_real_distribution<float> bob_rate_dist(0.4F, 2);
      std::uniform_real_distribution<float> bob_phase_dist(0, pi<float>());
      std::uniform_real_distribution<float>
          bobExtentDist(size.y * 0.0625F, size.y * 0.125F);

      for (int i = 0; i < _num_spheres; i++) {
        auto pos = vec3{x_dist(gen), y_dist(gen), z_dist(gen)};
        auto radius = radius_dist(gen);
        auto rate = bob_rate_dist(gen);
        auto phase = bob_phase_dist(gen);
        auto bobExtent = bobExtentDist(gen);

        auto
            sphere =
            volume->Add(std::make_unique<samplers::SphereVolumeSampler>(
                pos, radius, IVolumeSampler::Mode::Additive));

        _spheres.push_back(SphereState{
            sphere, pos, rate, phase, bobExtent});
      }
    }

    if (_num_cubes > 0) {
      auto max_size = size.x / 5;
      std::uniform_real_distribution<float> x_dist(max_size, size.x - max_size);
      std::uniform_real_distribution<float> z_dist(max_size, size.z - max_size);
      std::uniform_real_distribution<float> y_dist(size.y * 0.4F, size.y * 0.6F);
      std::uniform_real_distribution<float> size_dist(size.x / 10, max_size);
      std::uniform_real_distribution<float> bob_rate_dist(0.4, 2);
      std::uniform_real_distribution<float> bob_phase_dist(0, pi<float>());
      std::uniform_real_distribution<float>
          bob_extent_dist(size.y * 0.0625F, size.y * 0.125F);
      std::uniform_real_distribution<float> spin_rate_dist(0.2F, 0.6F);
      std::uniform_real_distribution<float> spin_phase_dist(0, pi<float>());
      std::uniform_real_distribution<float> axis_component_dist(-1, 1);

      for (int i = 0; i < _num_cubes; i++) {
        auto pos = vec3{x_dist(gen), y_dist(gen), z_dist(gen)};
        auto size = size_dist(gen);
        auto bob_rate = bob_rate_dist(gen);
        auto bob_phase = bob_phase_dist(gen);
        auto bob_extent = bob_extent_dist(gen);
        auto spin_rate = spin_rate_dist(gen);
        auto spin_phase = spin_phase_dist(gen);
        auto spin_axis = normalize(vec3{
            axis_component_dist(gen),
            axis_component_dist(gen),
            axis_component_dist(gen)});

        auto cube =
            volume->Add(std::make_unique<samplers::RectangularPrismVolumeSampler>(
                pos,
                vec3(size) / 2.0F,
                _Rotation(0, spin_phase, spin_rate, spin_axis),
                IVolumeSampler::Mode::Additive));

        _cubes.push_back(CubeState{
            cube,
            pos,
            bob_rate,
            bob_phase,
            bob_extent,
            spin_rate,
            spin_phase,
            spin_axis});
      }
    }
  }

  void Step(float time) override {
    for (auto& sphere_state : _spheres) {
      auto cycle = time * sphere_state.rate + sphere_state.phase;
      vec3 pos = sphere_state.position;
      pos.y += sphere_state.bob_extent * sin(cycle);
      sphere_state.shape->SetPosition(pos);
    }

    for (auto& cube_state : _cubes) {
      auto bob_extent = cube_state.bob_extent
          * sin(time * cube_state.bob_rate + cube_state.bob_phase);
      auto rot = _Rotation(time,
                           cube_state.spin_phase,
                           cube_state.spin_rate,
                           cube_state.spin_axis);
      auto pos = cube_state.position + vec3(0, bob_extent, 0);
      cube_state.shape->Set(pos, cube_state.shape->GetHalfExtents(), rot);
    }
  }

 private:
  mat3 _Rotation(float time, float phase, float rate, vec3 axis) {
    auto angle = sin(time * rate + phase);
    return mat3{glm::rotate(mat4{1}, angle, axis)};
  }

  struct SphereState {
    ancer::unowned_ptr<samplers::SphereVolumeSampler> shape;
    vec3 position;
    float rate;
    float phase;
    float bob_extent;
  };

  struct CubeState {
    ancer::unowned_ptr<samplers::RectangularPrismVolumeSampler> shape;
    vec3 position;
    float bob_rate;
    float bob_phase;
    float bob_extent;
    float spin_rate;
    float spin_phase;
    vec3 spin_axis;
  };

  std::vector<SphereState> _spheres;
  std::vector<CubeState> _cubes;
  ancer::unowned_ptr<samplers::HalfspaceVolumeSampler> _bottom_plane;
  int _num_spheres = 10;
  int _num_cubes = 10;
};
}

