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

#ifndef volume_hpp
#define volume_hpp

#include <memory>
#include <vector>

#include <ancer/util/UnownedPtr.hpp>

#include "MarchingCubes.hpp"

namespace marching_cubes {

class IVolumeSampler {
 public:
  enum class Mode {
    Additive,
    Subtractive
  };

 public:
  IVolumeSampler(Mode mode)
      : _mode(mode) {
  }

  virtual ~IVolumeSampler() = default;

  Mode GetMode() const { return _mode; }

  virtual float ValueAt(const vec3& p, float falloffThreshold) const = 0;

 private:
  Mode _mode;
};

//------------------------------------------------------------------------------

/*
 * Volume is a simple IIsoSurface imlpementation which allows the composition
 * of IVolumeSamplers (which are additive or subtractive). This is like a crude
 * SDF approach.
 */
class Volume : public marching_cubes::IIsoSurface {
 public:
  /*
   * Creates a volume of given size, with a "fuzziness" factor
   * of falloff_threshold; the fuzziness results in smooth blending
   * between IVolumeSamplers.
   */
  Volume(ivec3 size, float falloff_threshold)
      : _size(size), _falloff_threshold(falloff_threshold) {
  }

  template<typename T>
  ancer::unowned_ptr<T> Add(std::unique_ptr<T>&& sampler) {
    auto ret = sampler.get();
    switch (sampler->GetMode()) {
      case IVolumeSampler::Mode::Additive:
        _additive_samplers.push_back(std::move(sampler));
        break;

      case IVolumeSampler::Mode::Subtractive:
        _subtractive_samplers.push_back(std::move(sampler));
        break;
    }
    return ret;
  }

  ivec3 Size() const override {
    return _size;
  }

  float ValueAt(const vec3& p) const override {
    float v = 0;
    for (auto& s : _additive_samplers) {
      v += s->ValueAt(p, _falloff_threshold);
    }

    for (auto& s : _subtractive_samplers) {
      v -= s->ValueAt(p, _falloff_threshold);
    }

    return min(max(v, 0.0F), 1.0F);
  }

  void SetFalloffThreshold(float ft) { _falloff_threshold = max<float>(ft, 0); }
  float GetFalloffThreshold() const { return _falloff_threshold; }

 private:
  ivec3 _size;
  float _falloff_threshold;
  std::vector<std::unique_ptr<IVolumeSampler>> _additive_samplers;
  std::vector<std::unique_ptr<IVolumeSampler>> _subtractive_samplers;
};

} // namespace marching_cubes

#endif /* volume_hpp */
