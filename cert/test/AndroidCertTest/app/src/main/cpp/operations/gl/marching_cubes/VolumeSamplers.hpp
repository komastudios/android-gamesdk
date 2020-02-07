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

#ifndef volume_samplers_hpp
#define volume_samplers_hpp

#include <ancer/util/GLHelpers.hpp>
#include "Volume.hpp"

namespace marching_cubes::samplers {

namespace detail {

/*
Test for intersection type of the volume defined by vertices and the planar volume defined
by the plane (origin,normal) with thickness 2*halfExtent. Note, it's assumed
that the volume defined by vertices is a rectangular prism. It may not be
strictly necessary, but it's only been tested as such.
*/
IVolumeSampler::AABBIntersection
BoundedPlaneIntersection(
    vec3 origin,
    vec3 normal,
    float half_extent,
    const std::array<vec3, 8>& vertices)
{
  int on_positive_side = 0;
  int on_negative_side = 0;
  int inside = 0;
  for (const auto& v : vertices) {
    float signed_distance = glm::dot(normal, v - origin);

    if (signed_distance > half_extent) {
      on_positive_side++;
    } else if (signed_distance < -half_extent) {
      on_negative_side++;
    } else {
      inside++;
    }

    // if we have vertices on both sides of the plane
    // we have an intersection (but not containment)
    if (on_positive_side && on_negative_side) {
      return IVolumeSampler::AABBIntersection::IntersectsAABB;
    }
  }

  switch(inside) {
    case 0: return IVolumeSampler::AABBIntersection::None;
    case 8: return IVolumeSampler::AABBIntersection::ContainsAABB;
    default: return IVolumeSampler::AABBIntersection::IntersectsAABB;
  }
}

IVolumeSampler::AABBIntersection
BoundedPlaneIntersection(
    vec3 origin,
    vec3 normal,
    float half_extent,
    ancer::glh::AABB bounds)
{
  return BoundedPlaneIntersection(origin, normal, half_extent, bounds.Corners());
}

} // namespace detail

/*
 Represents a simple sphere
 */
class SphereVolumeSampler : public IVolumeSampler {
 public:
  SphereVolumeSampler(vec3 position, float radius, Mode mode)
      : IVolumeSampler(mode)
      , _position(position)
      , _radius(radius)
      , _radius2(radius * radius)
  {
  }

  ~SphereVolumeSampler() override = default;

  bool Intersects(ancer::glh::AABB bounds) const override
  {
    using glm::max;
    using glm::min;

    // early exit if this sphere is contained by bounds
    if (bounds.Contains(_position))
      return true;

    // find the point on surface of bounds closest to _position
    const auto closest_point = vec3 {
        max(bounds.min.x, min(_position.x, bounds.max.x)),
        max(bounds.min.y, min(_position.y, bounds.max.y)),
        max(bounds.min.z, min(_position.z, bounds.max.z))
    };

    // confirm the closest point on the aabb is inside the sphere
    return glm::distance2(_position, closest_point) <= _radius2;
  }

  AABBIntersection Intersection(ancer::glh::AABB bounds) const override
  {
    int inside = 0;
    for (auto& v : bounds.Corners()) {
      if (glm::distance2(v, _position) < _radius2) {
        inside++;
      }
    }

    switch(inside) {
      case 0: return IVolumeSampler::AABBIntersection::None;
      case 8: return IVolumeSampler::AABBIntersection::ContainsAABB;
      default: return IVolumeSampler::AABBIntersection::IntersectsAABB;
    }
  }

  float ValueAt(const vec3& p, float fuzziness) const override
  {
    float d2 = glm::distance2(p, _position);
    float inner_radius = _radius - fuzziness;
    float min2 = inner_radius * inner_radius;
    if (d2 <= min2) {
      return 1;
    }

    float max2 = _radius * _radius;
    if (d2 >= max2) {
      return 0;
    }

    float d = std::sqrt(d2) - inner_radius;
    return 1 - (d / fuzziness);
  }

  void SetPosition(const vec3& center)
  {
    _position = center;
  }

  void SetRadius(float radius)
  {
    _radius = glm::max<float>(radius, 0);
    _radius2 = _radius * _radius;
  }

  vec3 GetPosition() const { return _position; }

  float GetRadius() const { return _radius; }

 private:
  vec3 _position;
  float _radius;
  float _radius2;
};

/*
HalfspaceVolumeSampler represents a plane, where values on the positive side
(e.g., in the direction of the normal) are NOT in the plane's volume, and values
on the negative side are inside.
*/
class HalfspaceVolumeSampler : public IVolumeSampler {
 public:
  HalfspaceVolumeSampler(vec3 planeOrigin, vec3 planeNormal, Mode mode)
      : IVolumeSampler(mode)
      , _origin(planeOrigin)
      , _normal(planeNormal)
  {
  }

  bool Intersects(ancer::glh::AABB bounds) const override
  {
    for (const auto& v : bounds.Corners()) {
      auto signed_distance = glm::dot(_normal, v - _origin);
      if (signed_distance < 0) {
        return true;
      }
    }
    return false;
  }

  AABBIntersection Intersection(ancer::glh::AABB bounds) const override
  {
    int inside = 0;
    for (const auto& v : bounds.Corners()) {
      auto signed_distance = glm::dot(_normal, v - _origin);
      if (signed_distance < 0) {
        inside++;
      }
    }

    switch(inside) {
      case 0: return IVolumeSampler::AABBIntersection::None;
      case 8: return IVolumeSampler::AABBIntersection::ContainsAABB;
      default: return IVolumeSampler::AABBIntersection::IntersectsAABB;
    }
  }

  float ValueAt(const vec3& p, float fuzziness) const override
  {
    float signed_dist = glm::dot(_normal, p - _origin);
    if (signed_dist < -fuzziness) {
      return 1;
    } else if (signed_dist > 0) {
      return 0;
    }
    return -signed_dist / fuzziness;
  }

  void SetPlaneOrigin(const vec3 plane_origin) { _origin = plane_origin; }
  vec3 GetPlaneOrigin() const { return _origin; }

  void SetPlaneNormal(const vec3 plane_normal) { _normal = glm::normalize(plane_normal); }
  vec3 GetPlaneNormal() const { return _normal; }

 private:
  vec3 _origin;
  vec3 _normal;
};

/*
 BoundedPlaneVolumeSampler represents a plane with a thickness; points
 in space less than that thickness from the plane are considered "inside"
 the plane.
 */
class BoundedPlaneVolumeSampler : public IVolumeSampler {
 public:
  BoundedPlaneVolumeSampler(vec3 plane_origin, vec3 plane_normal, float plane_thickness, Mode mode)
      : IVolumeSampler(mode)
      , _origin(plane_origin)
      , _normal(glm::normalize(plane_normal))
      , _thickness(std::max<float>(plane_thickness, 0))
  {
  }

  bool Intersects(ancer::glh::AABB bounds) const override
  {
    return detail::BoundedPlaneIntersection(_origin, _normal, _thickness / 2, bounds) != AABBIntersection::None;
  }

  AABBIntersection Intersection(ancer::glh::AABB bounds) const override
  {
    return detail::BoundedPlaneIntersection(_origin, _normal, _thickness / 2, bounds);
  }

  float ValueAt(const vec3& p, float fuzziness) const override
  {
    // distance of p from plane
    float dist = abs(glm::dot(_normal, p - _origin));
    float outer_dist = _thickness * 0.5F;
    float inner_dist = outer_dist - fuzziness;

    if (dist <= inner_dist) {
      return 1;
    } else if (dist >= outer_dist) {
      return 0;
    }

    return 1 - ((dist - inner_dist) / fuzziness);
  }

  void SetPlaneOrigin(const vec3 plane_origin) { _origin = plane_origin; }
  vec3 GetPlaneOrigin() const { return _origin; }

  void SetPlaneNormal(const vec3 plane_normal) { _normal = normalize(plane_normal); }
  vec3 GetPlaneNormal() const { return _normal; }

  void SetThickness(float thickness) { _thickness = std::max<float>(thickness, 0); }
  float GetThickness() const { return _thickness; }

 private:
  vec3 _origin;
  vec3 _normal;
  float _thickness;
};

class RectangularPrismVolumeSampler : public IVolumeSampler {
 public:
  RectangularPrismVolumeSampler(vec3 origin, vec3 half_extents, mat3 rotation, Mode mode)
      : IVolumeSampler(mode)
      , _origin(origin)
      , _half_extents(max(half_extents, vec3(0)))
      , _rotation(rotation)
  {
    _Update();
  }

  bool Intersects(ancer::glh::AABB bounds) const override
  {
    using detail::BoundedPlaneIntersection;

    // coarse AABB check
    if (bounds.Intersect(_bounds) == ancer::glh::AABB::Intersection::Outside) {
      return false;
    }

    // now check if the aabb intersects the bounded planar volumes
    // represented along x, y and z. This is like a simplification
    // of separating axes theorem when one prism is an AABB.
    // if all three volumes intersect bounds, we know bounds intersects
    // our prism
    std::array<vec3, 8> corners;
    bounds.Corners(corners);
    if (BoundedPlaneIntersection(_origin, _pos_x, _half_extents.x, corners) == AABBIntersection::None) {
      return false;
    }
    if (BoundedPlaneIntersection(_origin, _pos_y, _half_extents.y, corners) == AABBIntersection::None) {
      return false;
    }
    if (BoundedPlaneIntersection(_origin, _pos_z, _half_extents.z, corners) == AABBIntersection::None) {
      return false;
    }

    return true;
  }

  AABBIntersection Intersection(ancer::glh::AABB bounds) const override
  {
    using detail::BoundedPlaneIntersection;

    // coarse AABB check
    if (bounds.Intersect(_bounds) == ancer::glh::AABB::Intersection::Outside) {
      return AABBIntersection::None;
    }

    std::array<vec3, 8> corners;
    bounds.Corners(corners);

    // check for intersection type; if it's not ContainsAABB, early exit
    // if all 3 are ContainsAABB, we can return ContainsAABB
    auto intersection = BoundedPlaneIntersection(_origin, _pos_x, _half_extents.x, corners);
    if (intersection != AABBIntersection::ContainsAABB) {
      return intersection;
    }

    intersection = BoundedPlaneIntersection(_origin, _pos_y, _half_extents.y, corners);
    if (intersection != AABBIntersection::ContainsAABB) {
      return intersection;
    }

    intersection = BoundedPlaneIntersection(_origin, _pos_z, _half_extents.z, corners);
    if (intersection != AABBIntersection::ContainsAABB) {
      return intersection;
    }

    return AABBIntersection::ContainsAABB;
  }

  float ValueAt(const vec3& p, float fuzziness) const override
  {
    using glm::min;

    fuzziness += 1e-5F;

    // postivie values denote that the point is on the positive
    // side of the plane, or, outside the cube. if all values
    // are negative, the point is *inside* the cube.

    const auto dir = p - _origin;
    const auto pos_x_distance = glm::dot(_pos_x, dir);
    const auto pos_y_distance = glm::dot(_pos_y, dir);
    const auto pos_z_distance = glm::dot(_pos_z, dir);

    const auto pos_x = pos_x_distance - _half_extents.x;
    const auto neg_x = -pos_x_distance - _half_extents.x;
    const auto pos_y = pos_y_distance - _half_extents.y;
    const auto neg_y = -pos_y_distance - _half_extents.y;
    const auto pos_z = pos_z_distance - _half_extents.z;
    const auto neg_z = -pos_z_distance - _half_extents.z;

    if (pos_x <= 0 && neg_x <= 0 && pos_y <= 0 && neg_y <= 0 && pos_z <= 0 && neg_z <= 0) {
      auto pos_x_amt = -pos_x / fuzziness;
      auto neg_x_amt = -neg_x / fuzziness;
      auto pos_y_amt = -pos_y / fuzziness;
      auto neg_y_amt = -neg_y / fuzziness;
      auto pos_z_amt = -pos_z / fuzziness;
      auto neg_z_amt = -neg_z / fuzziness;
      return min(min(pos_x_amt, min(neg_x_amt, min(pos_y_amt, min(neg_y_amt, min(pos_z_amt, neg_z_amt))))), 1.0F);
    }
    return 0;
  }

  void SetPosition(const vec3& position)
  {
    _origin = position;
    _Update();
  }

  vec3 GetPosition() const { return _origin; }

  void SetHalfExtents(const vec3& half_extents)
  {
    _half_extents = max(half_extents, vec3(0));
    _Update();
  }

  vec3 GetHalfExtents() const { return _half_extents; }

  void SetRotation(const mat3& rotation)
  {
    _rotation = rotation;
    _Update();
  }

  mat3 GetRotation() const { return _rotation; }

  void Set(const vec3& position, const vec3& half_extents, const mat3& rotation)
  {
    _origin = position;
    _half_extents = max(half_extents, vec3(0));
    _rotation = rotation;
    _Update();
  }

  ancer::glh::AABB GetBounds() const { return _bounds; }

  std::array<vec3, 8> GetCorners() const
  {
    return _corners;
  }

  void AddTo(LineSegmentBuffer& line_buffer, const vec4& color) const
  {
    auto corners = this->GetCorners();

    // trace bottom
    line_buffer.Add(Vertex { corners[0], color }, Vertex { corners[1], color });
    line_buffer.Add(Vertex { corners[1], color }, Vertex { corners[2], color });
    line_buffer.Add(Vertex { corners[2], color }, Vertex { corners[3], color });
    line_buffer.Add(Vertex { corners[3], color }, Vertex { corners[0], color });

    // trace top
    line_buffer.Add(Vertex { corners[4], color }, Vertex { corners[5], color });
    line_buffer.Add(Vertex { corners[5], color }, Vertex { corners[6], color });
    line_buffer.Add(Vertex { corners[6], color }, Vertex { corners[7], color });
    line_buffer.Add(Vertex { corners[7], color }, Vertex { corners[4], color });

    // add bars connecting bottom to top
    line_buffer.Add(Vertex { corners[0], color }, Vertex { corners[4], color });
    line_buffer.Add(Vertex { corners[1], color }, Vertex { corners[5], color });
    line_buffer.Add(Vertex { corners[2], color }, Vertex { corners[6], color });
    line_buffer.Add(Vertex { corners[3], color }, Vertex { corners[7], color });
  }

 private:
  void _Update()
  {
    // extract plane normals from rotation
    _pos_x = vec3 { _rotation[0][0], _rotation[1][0], _rotation[2][0] };
    _pos_y = vec3 { _rotation[0][1], _rotation[1][1], _rotation[2][1] };
    _pos_z = vec3 { _rotation[0][2], _rotation[1][2], _rotation[2][2] };

    // generate _corners
    auto e = _half_extents;
    _corners[0] = _origin + (vec3(+e.x, -e.y, -e.z) * _rotation);
    _corners[1] = _origin + (vec3(+e.x, -e.y, +e.z) * _rotation);
    _corners[2] = _origin + (vec3(-e.x, -e.y, +e.z) * _rotation);
    _corners[3] = _origin + (vec3(-e.x, -e.y, -e.z) * _rotation);
    _corners[4] = _origin + (vec3(+e.x, +e.y, -e.z) * _rotation);
    _corners[5] = _origin + (vec3(+e.x, +e.y, +e.z) * _rotation);
    _corners[6] = _origin + (vec3(-e.x, +e.y, +e.z) * _rotation);
    _corners[7] = _origin + (vec3(-e.x, +e.y, -e.z) * _rotation);

    _bounds.Invalidate();
    for (auto& c : _corners) {
      _bounds.add(c);
    }
  }

 private:
  vec3 _origin;
  vec3 _half_extents;
  mat3 _rotation;
  vec3 _pos_x, _pos_y, _pos_z;

  std::array<vec3, 8> _corners;
  ancer::glh::AABB _bounds;
};


} // end namespace marching_cubes::samplers

#endif