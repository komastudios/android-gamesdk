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

#ifndef terrain_samplers_hpp
#define terrain_samplers_hpp

#include <stdexcept>

#include <ancer/util/UnownedPtr.hpp>
#include <ancer/util/GLHelpers.hpp>

#include "Volume.hpp"

namespace marching_cubes {

class HeightmapSampler : public IVolumeSampler {
 public:
  using AABB = ancer::glh::AABB;

  typedef std::function<float(vec3)> NoiseSampler3D;

 public:
  HeightmapSampler() = delete;
  HeightmapSampler(const HeightmapSampler &) = delete;
  HeightmapSampler(HeightmapSampler &&) = delete;

  HeightmapSampler(float *heightmap, int size, float maxHeight, float floorThreshold,
                   const MaterialState &floorMaterial,
                   const MaterialState &lowMaterial,
                   const MaterialState &highMaterial)
      : IVolumeSampler(IVolumeSampler::Mode::Additive),
        _heightmap(heightmap),
        _size(size),
        _maxHeight(maxHeight),
        _floorThreshold(floorThreshold),
        _floorMaterial(floorMaterial),
        _lowMaterial(lowMaterial),
        _highMaterial(highMaterial) {
  }

  ~HeightmapSampler() = default;

  bool intersects(AABB bounds) const override {
    // We know that the geometry will span [x,y] and be no taller than
    // the _height, so we can do an easy test
    return bounds.min.y <= _maxHeight;
  }

  AABBIntersection intersection(AABB bounds) const override {
    // This is only meaningful for subtractive samplers
    throw std::runtime_error("intersection only meanignful for subtractive volumes");
  }

  float valueAt(const vec3 &p, float fuzziness, MaterialState &material) const override {
    const float height = this->height(p.x, p.z);
    const float innerHeight = max(height - fuzziness, 1.0F);

    if (height < _floorThreshold) {
      float t = height / _floorThreshold;
      material = mix(_floorMaterial, _lowMaterial, t);
    } else {
      float t = (height - _floorThreshold) / (_maxHeight - _floorThreshold);
      material = mix(_lowMaterial, _highMaterial, t);
    }

    if (p.y > height) {
      return 0;
    } else if (p.y < innerHeight || p.y < 1e-3) {
      return 1;
    }
    return 1 - ((p.y - innerHeight) / fuzziness);
  }

 private:
  inline float height(int x, int y) const {
    const int offset = y * _size + x;
    return _heightmap[offset];
  }

  float *_heightmap = nullptr;
  int _size = 0;
  float _maxHeight = 0;
  float _floorThreshold{0};
  MaterialState _floorMaterial, _lowMaterial, _highMaterial;
};

class Tube : public IVolumeSampler {
 private:
  vec3 _tubeAxisOrigin{0};
  vec3 _tubeAxisDir{0, 0, 1};
  vec3 _tubeAxisPerp{0, 1, 0};
  vec3 _innerRadiusOffset{0};
  float _innerRadius{0};
  float _outerRadius{0};
  float _innerRadius2{0};
  float _outerRadius2{0};

  vec3 _frontFaceNormal{0, 0, 1};
  vec3 _frontFaceOrigin{0};
  vec3 _backFaceNormal{0, 0, -1};
  vec3 _backFaceOrigin{0, 0, 0};

  float _cutAngleRadians{0};
  float _cosCutAngle{0};
  bool _hasInnerCylinderOffset = false;
  MaterialState _material;

 public:
  using AABB = ancer::glh::AABB;

  struct Config {
    // origin of the cylinder representing the outer radius of the tube
    vec3 axisOrigin{0};
    // major axis of the cylinder making the outer radius of the tube
    vec3 axisDir{0, 0, 1};

    // perpendicular to the axisDir, used for computing the cut angle
    vec3 axisPerp{0, 1, 0};

    // offset of the cylinder making up the inner radius from axisOrigin
    // if 0, both cylinders are coaxial, but an offset can produce interesting
    // non-symmetries
    vec3 innerRadiusAxisOffset{0};

    // inner radius of tube
    float innerRadius = 0;

    // outer radius of tube
    float outerRadius = 0;

    // length of tube (end to end)
    float length = 1;

    // [0,2*pi] cuts a notch our of the tube lengthwise with center of
    // notch aligned via axisPerp
    float cutAngleRadians = 0;

    // normal of the front capping plane of the tube
    vec3 frontFaceNormal{0, 0, 1};

    // normal of the back capping plane of the tube
    vec3 backFaceNormal{0, 0, -1};

    // material tubes will emit
    MaterialState material;
  };

  Tube() = delete;

  Tube(Config c)
      : IVolumeSampler(Mode::Additive),
        _tubeAxisOrigin(c.axisOrigin),
        _tubeAxisDir(normalize(c.axisDir)),
        _tubeAxisPerp(normalize(c.axisPerp)),
        _innerRadiusOffset(c.innerRadiusAxisOffset),
        _innerRadius(c.innerRadius),
        _outerRadius(c.outerRadius),
        _innerRadius2(c.innerRadius * c.innerRadius),
        _outerRadius2(c.outerRadius * c.outerRadius),
        _frontFaceNormal(normalize(c.frontFaceNormal)),
        _frontFaceOrigin(_tubeAxisOrigin + _tubeAxisDir * (c.length / 2)),
        _backFaceNormal(normalize(c.backFaceNormal)),
        _backFaceOrigin(_tubeAxisOrigin - _tubeAxisDir * (c.length / 2)),
        _cutAngleRadians(clamp<float>(c.cutAngleRadians, 0, 2 * pi<float>())),
        _material(c.material) {
    _cosCutAngle = cos(_cutAngleRadians);
    _hasInnerCylinderOffset = length2(_innerRadiusOffset) > 0;
  }

  Tube(const Tube &) = delete;
  Tube(Tube &&) = delete;
  ~Tube() = default;

  bool intersects(AABB bounds) const override {
    const auto corners = bounds.Corners();

    // quick check on our bounding planes
    if (_boundedSpaceIntersection(
        _frontFaceOrigin, _frontFaceNormal, _backFaceOrigin, _backFaceNormal,
        corners)
        == AABBIntersection::None) {
      return false;
    }

    // find the closest and farthest vertices on the bounds to our
    // axis and discard if the closest is beyond the cylinder outer
    // radius, or if the farthest is inside the inner radius
    float closestDist2 = std::numeric_limits<float>::max();
    float farthestDist2 = 0;

    for (const auto &c : corners) {
      float d2 = _distanceToOuterCylinderAxis2(c);
      closestDist2 = min(d2, closestDist2);

      if (_hasInnerCylinderOffset) {
        d2 = _distanceToInnerCylinderAxis2(c);
      }
      farthestDist2 = max(d2, farthestDist2);
    }

    if (closestDist2 > _outerRadius2) {
      return false;
    }

    if (farthestDist2 < _innerRadius2) {
      return false;
    }

    return true;
  }

  AABBIntersection intersection(AABB bounds) const override {
    // This is only meaningful for subtractive samplers
    throw std::runtime_error("intersection only meanignful for subtractive volumes");
  }

  float valueAt(const vec3 &p, float fuzziness, MaterialState &material) const override {
    material = _material;

    // A point is inside the ring volume if:
    // - On negative side front and back planes
    // - Between inner and outer radius from axis

    float frontFaceDist = glm::dot(_frontFaceNormal, p - _frontFaceOrigin);
    float backFaceDist = glm::dot(_backFaceNormal, p - _backFaceOrigin);

    // early exit
    if (frontFaceDist > 0 || backFaceDist > 0) {
      return 0;
    }

    vec3 closestPointOnAxis;
    float distanceToOuterCylinderAxis2 = _distanceToOuterCylinderAxis2(p, closestPointOnAxis);
    float distanceToInnerCylinderAxis2 = _hasInnerCylinderOffset
                                         ? _distanceToInnerCylinderAxis2(p)
                                         : distanceToOuterCylinderAxis2;

    // early exit
    if (distanceToOuterCylinderAxis2 > _outerRadius2
        || distanceToInnerCylinderAxis2 < _innerRadius2) {
      return 0;
    }

    float frontFaceContribution = min<float>(-frontFaceDist / fuzziness, 1);
    float backFaceContribution = min<float>(-backFaceDist / fuzziness, 1);

    float tubeContribution = 1;
    float outerRadiusInner = _outerRadius - fuzziness;
    float innerRadiusInner = _innerRadius + fuzziness;
    float outerRadiusInner2 = outerRadiusInner * outerRadiusInner;
    float innerRadiusInner2 = innerRadiusInner * innerRadiusInner;

    if (distanceToInnerCylinderAxis2 < innerRadiusInner2) {
      // gradient on inner
      float radialDist = sqrt(distanceToInnerCylinderAxis2);
      tubeContribution = (radialDist - _innerRadius) / fuzziness;
    } else if (distanceToOuterCylinderAxis2 > outerRadiusInner2) {
      // gradient on outer
      float radialDist = sqrt(distanceToOuterCylinderAxis2);
      tubeContribution = 1 - ((radialDist - outerRadiusInner) / fuzziness);
    }

    float totalContribution = frontFaceContribution * backFaceContribution * tubeContribution;

    if (_cutAngleRadians > 0) {
      vec3 dir = normalize(p - closestPointOnAxis);
      auto d = dot(_tubeAxisPerp, dir);
      if (d > _cosCutAngle) {
        totalContribution = 0;
      }
    }

    return totalContribution;
  }

 private:
  //http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
  // vec3 a = _tubeAxisOrigin;
  // vec3 b = _tubeAxisOrigin + _tubeAxisDir;
  // float d = dot((a - p), (b - a));
  // float t = -d / length2(b - a);
  // vec3 tp = mix(a,b,t);
  // return distance(tp, p);

  inline float _distanceToOuterCylinderAxis2(vec3 p) const {
    float t = -glm::dot(_tubeAxisOrigin - p, _tubeAxisDir);
    auto tp = _tubeAxisOrigin + t * _tubeAxisDir;
    return distance2(tp, p);
  }

  inline float _distanceToOuterCylinderAxis2(vec3 p, vec3 &pointOnAxis) const {
    float t = -glm::dot(_tubeAxisOrigin - p, _tubeAxisDir);
    pointOnAxis = _tubeAxisOrigin + t * _tubeAxisDir;
    return distance2(pointOnAxis, p);
  }

  inline float _distanceToInnerCylinderAxis2(vec3 p) const {
    const vec3 origin = _tubeAxisOrigin + _innerRadiusOffset;
    float t = -glm::dot(origin - p, _tubeAxisDir);
    auto tp = origin + t * _tubeAxisDir;
    return distance2(tp, p);
  }

  inline float _distanceToInnerCylinderAxis2(vec3 p, vec3 &pointOnAxis) const {
    const vec3 origin = _tubeAxisOrigin + _innerRadiusOffset;
    float t = -glm::dot(origin - p, _tubeAxisDir);
    pointOnAxis = origin + t * _tubeAxisDir;
    return distance2(pointOnAxis, p);
  }

  IVolumeSampler::AABBIntersection
  _boundedSpaceIntersection(
      glm::vec3 frontFaceOrigin,
      glm::vec3 frontFaceNormal,
      glm::vec3 backFaceOrigin,
      glm::vec3 backFaceNormal,
      const std::array<glm::vec3, 8> &vertices) const {
    // we need to see if the bounds actually intersects
    int onPositiveSide = 0;
    int onNegativeSide = 0;
    int inside = 0;
    for (const auto &v : vertices) {
      float frontFaceDist = glm::dot(frontFaceNormal, v - frontFaceOrigin);
      float backFaceDist = glm::dot(backFaceNormal, v - backFaceOrigin);

      if (frontFaceDist > 0) {
        onPositiveSide++;
      } else if (backFaceDist > 0) {
        onNegativeSide++;
      } else {
        inside++;
      }

      // if we have vertices on both sides of the plane
      // we have an intersection (but not containment)
      if (onPositiveSide && onNegativeSide) {
        return IVolumeSampler::AABBIntersection::IntersectsAABB;
      }
    }

    switch (inside) {
      case 0:return IVolumeSampler::AABBIntersection::None;
      case 8:return IVolumeSampler::AABBIntersection::ContainsAABB;
      default:return IVolumeSampler::AABBIntersection::IntersectsAABB;
    }
  }
};

}

#endif