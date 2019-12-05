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

#ifndef tAABBh
#define tAABBh

#include <GLES3/gl32.h>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <algorithm>
#include <limits>

using namespace glm;

template<typename T, precision P>
class tAABB {
 public:
  enum class Intersection {
    Inside, // The object being tested is entirely inside the testing object
    Intersects, // The object being tested intersects the bounds of the testing object
    Outside // The object being tested is entirely outside the testing object
  };

  typedef T value_type;

  /*
      Create a default tAABB. Will return true for invalid() because
      it hasn't been assigned any value yet.
  */
  tAABB(void)
      : min(std::numeric_limits<T>::max()),
        max(-std::numeric_limits<T>::max()) {
  }

  tAABB(const tAABB<T, P>& other)
      : min(other.min), max(other.max) {
  }

  tAABB(const tvec3<T, P>& min, const tvec3<T, P>& max)
      : min(min), max(max) {
  }

  tAABB(const tvec3<T, P>& c, const T radius)
      : min(c.x - radius, c.y - radius, c.z - radius),
        max(c.x + radius, c.y + radius, c.z + radius) {
  }

  bool operator==(const tAABB<T, P>& other) const {
    return min == other.min && max == other.max;
  }

  bool operator!=(const tAABB<T, P>& other) const {
    return min != other.min || max != other.max;
  }

  /*
      Two circumstances for "invalid" tAABB. It could be collapsed, where width,
      breadth or height are less than or equal to zero, or it is default constructed
      and not yet assigned values.
  */

  bool Valid(void) const {
    return (min.x < max.x) && (min.y < max.y) && (min.z < max.z);
  }

  /*
      return true if this AABB is valid, e.g, not collapsed and has been
      expanded to fit something.
  */
  operator bool() const {
    return Valid();
  }

  /*
      Make this tAABB invalid e.g., just like a default-constructed tAABB.
      Will fail the valid test.
  */
  void Invalidate(void) {
    min.x = std::numeric_limits<T>::max();
    max.x = -std::numeric_limits<T>::max();
    min.y = std::numeric_limits<T>::max();
    max.y = -std::numeric_limits<T>::max();
    min.z = std::numeric_limits<T>::max();
    max.z = -std::numeric_limits<T>::max();
  }

  /*
      return geometric center of tAABB
  */
  tvec3<T, P> Center(void) const {
    return tvec3<T, P>(
        (min.x + max.x) / 2,
        (min.y + max.y) / 2,
        (min.z + max.z) / 2);
  }

  /*
      return the radius of the sphere that would exactly enclose this tAABB
  */
  T Radius(void) const {
    T xSize(max.x - min.x), ySize(max.y - min.y), zSize(max.z - min.z);

    xSize /= 2;
    ySize /= 2;
    zSize /= 2;

    return sqrt(xSize * xSize + ySize * ySize + zSize * zSize);
  }

  /*
      return the magnitude along the 3 axes.
  */
  tvec3<T, P> Size(void) const {
    return tvec3<T, P>(max.x - min.x,
                       max.y - min.y,
                       max.z - min.z);
  }

  /*
      return tAABB containing both
  */
  const tAABB<T, P> operator+(const tAABB<T, P>& a) const {
    return tAABB<T, P>(
        std::min<T>(min.x, a.min.x),
        std::max<T>(max.x, a.max.x),
        std::min<T>(min.y, a.min.y),
        std::max<T>(max.y, a.max.y),
        std::min<T>(min.z, a.min.z),
        std::max<T>(max.z, a.max.z));
  }

  /*
      return tAABB containing both this tAABB and the point
  */
  const tAABB<T, P> operator+(const tvec3<T, P>& p) const {
    return tAABB<T, P>(
        std::min<T>(min.x, p.x),
        std::max<T>(max.x, p.x),
        std::min<T>(min.y, p.y),
        std::max<T>(max.y, p.y),
        std::min<T>(min.z, p.z),
        std::max<T>(max.z, p.z));
  }

  /*
      Expand tAABB by scalar value
  */
  const tAABB<T, P> operator+(const T d) const {
    return tAABB<T, P>(
        min.x - d, max.x + d,
        min.y - d, max.y + d,
        min.z - d, max.z + d);
  }

  /*
      Inset tAABB by scalar value
  */
  const tAABB<T, P> operator-(const T d) const {
    return tAABB<T, P>(
        min.x + d, max.x - d,
        min.y + d, max.y - d,
        min.z + d, max.z - d);
  }

  /*
      Make this tAABB become the add of it and the other tAABB
  */
  tAABB<T, P>& operator+=(const tAABB<T, P>& a) {
    min.x = std::min(min.x, a.min.x),
    max.x = std::max(max.x, a.max.x),
    min.y = std::min(min.y, a.min.y),
    max.y = std::max(max.y, a.max.y),
    min.z = std::min(min.z, a.min.z),
    max.z = std::max(max.z, a.max.z);

    return *this;
  }

  /*
      Make this tAABB become the union of it and the other tAABB
  */
  void add(const tAABB<T, P>& a) {
    min.x = std::min(min.x, a.min.x),
    max.x = std::max(max.x, a.max.x),
    min.y = std::min(min.y, a.min.y),
    max.y = std::max(max.y, a.max.y),
    min.z = std::min(min.z, a.min.z),
    max.z = std::max(max.z, a.max.z);
  }

  /*
      Expand this tAABB ( if necessary ) to contain the given point
  */
  tAABB<T, P>& operator+=(const tvec3<T, P>& p) {
    min.x = std::min(min.x, p.x);
    max.x = std::max(max.x, p.x);
    min.y = std::min(min.y, p.y);
    max.y = std::max(max.y, p.y);
    min.z = std::min(min.z, p.z);
    max.z = std::max(max.z, p.z);

    return *this;
  }

  /*
      Expand this tAABB ( if necessary ) to contain the given point
  */
  void add(const tvec3<T, P>& p) {
    min.x = std::min(min.x, p.x);
    max.x = std::max(max.x, p.x);
    min.y = std::min(min.y, p.y);
    max.y = std::max(max.y, p.y);
    min.z = std::min(min.z, p.z);
    max.z = std::max(max.z, p.z);
  }

  /*
      Expand this tAABB ( if necessary ) ton contain the given sphere
  */
  void add(const tvec3<T, P>& p, T radius) {
    min.x = std::min(min.x, p.x - radius);
    max.x = std::max(max.x, p.x + radius);
    min.y = std::min(min.y, p.y - radius);
    max.y = std::max(max.y, p.y + radius);
    min.z = std::min(min.z, p.z - radius);
    max.z = std::max(max.z, p.z + radius);
  }

  /*
      Outset this tAABB by scalar factor
  */
  tAABB<T, P>& operator+=(const T d) {
    min.x -= d;
    max.x += d;
    min.y -= d;
    max.y += d;
    min.z -= d;
    max.z += d;

    return *this;
  }

  /*
      Outset this tAABB by scalar factor
  */
  void Outset(T d) {
    min.x -= d;
    max.x += d;
    min.y -= d;
    max.y += d;
    min.z -= d;
    max.z += d;
  }

  /*
      Inset this tAABB by scalar factor
  */
  tAABB<T, P>& operator-=(const T d) {
    min.x += d;
    max.x -= d;
    min.y += d;
    max.y -= d;
    min.z += d;
    max.z -= d;

    return *this;
  }

  /*
      Inset this tAABB by scalar factor
  */
  void Inset(T d) {
    min += vec3(d);
    max -= vec3(d);
  }

  /*
      Transform this tAABB by p
  */
  void Translate(const tvec3<T, P>& p) {
    min += p;
    max += p;
  }

  /*
      return true if point is in this tAABB
  */
  bool Contains(const tvec3<T, P>& point) const {
    return (point.x >= min.x && point.x <= max.x && point.y >= min.y
        && point.y <= max.y && point.z >= min.z && point.z <= max.z);
  }

  /*
      return the intersection type of this tAABB with other
  */
  Intersection Intersect(const tAABB<T, P>& other) const {
    // Check first to see if 'other' is completely inside this tAABB.
    // If it is, return Inside.

    if (other.min.x >= min.x && other.max.x <= max.x && other.min.y >= min.y
        && other.max.y <= max.y && other.min.z >= min.z
        && other.max.z <= max.z) {
      return Intersection::Inside;
    }

    // If we're here, determine if there's any overlap

    bool xOverlap = (min.x < other.max.x && max.x > other.min.x);
    bool yOverlap = (min.y < other.max.y && max.y > other.min.y);
    bool zOverlap = (min.z < other.max.z && max.z > other.min.z);

    return (xOverlap && yOverlap && zOverlap) ? Intersection::Intersects
                                              : Intersection::Outside;
  }

  /*
      return the intersection type of this tAABB with the sphere at
      center with given radius
  */
  Intersection Intersect(const tvec3<T, P>& center, T radius) const {
    return Intersect(tAABB<T, P>(center, radius));
  }

 public:
  tvec3<T, P> min, max;
};

typedef tAABB<float, defaultp> AABB;
typedef tAABB<int32, defaultp> AABBi;

#endif /* tAABBh */
