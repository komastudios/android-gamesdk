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

#ifndef marching_cubes_hpp
#define marching_cubes_hpp

#include <ancer/util/UnownedPtr.hpp>
#include <ancer/util/ThreadPool.hpp>
#include <ancer/util/GLHelpers.hpp>

#include "Storage.hpp"

namespace marching_cubes {

class IIsoSurface {
 public:
  IIsoSurface() = default;
  virtual ~IIsoSurface() = default;

  /*
   Return the bounded size of the isosurface volume
   */
  virtual ivec3 Size() const = 0;

  /*
   Return the value of the isosurface at a point in space, where 0 means
   nothing there" and 1 means fully occupied.
   */
  virtual float ValueAt(const vec3& p) const = 0;

  /*
   Return the normal of the gradient at a point in space.
   Default implementation takes 6 taps about the sample point to compute
   local gradient. Custom IsoSurface implementations may be able to
   implement a smarter domain-specific approach.
   */
  virtual vec3 NormalAt(const vec3& p) const;
};

//------------------------------------------------------------------------------

/*
 March entire volume passing generated triangles into triangleConsumer
 volume: The volume to march
 triangleConsumer: Receives each generated triangle
 transform: A transform to apply to each generated vertex
 computeNormals: If true, vertex normals will be computed via
  IIsoSurface::normalAt; else, generated triangle normals will be used
 */
void march(const IIsoSurface& volume,
           ITriangleConsumer& triangle_consumer,
           const mat4& transform = mat4(1),
           bool compute_normals = true);

/*
 March subregion of a volume passing generated triangles into triangleConsumer
 volume: The volume to march
 region: The subregion to march
 triangleConsumer: Receives each generated triangle
 transform: A transform to apply to each generated vertex
 computeNormals: If true, vertex normals will be computed via
  IIsoSurface::normalAt; else, generated triangle normals will be used
 */
void march(const marching_cubes::IIsoSurface& volume,
           ancer::glh::AABBi region,
           ITriangleConsumer& tc,
           const mat4& transform = mat4(1),
           bool compute_normals = true);

/*
 Marches a volume using a thread pool where each thread processes one
 "slice" of the volume.
 volume: The volume to march
 triangle_consumers: Each consumer receives a slice of the marched volume
 transform: A transform to apply to each generated vertex
 compute_normals: If true, vertex normals will be computed via
  IIsoSurface::normalAt; else, generated triangle normals will be used
 */
class ThreadedMarcher {
 public:
  ThreadedMarcher(const IIsoSurface& volume,
                  std::vector<ancer::unowned_ptr<ITriangleConsumer>> triangle_consumers,
                  std::unique_ptr<ancer::ThreadPool>&& _thread_pool,
                  const mat4& transform = mat4(1),
                  bool compute_normals = true);

  ~ThreadedMarcher() = default;

  /*
   March the current state of the volume; the triangle consumers
   which were passed during construction will be updated with new
   geometry.
   Note: ITriangleConsumer::start() and ITriangleConsumer::finish()
   will be called on the calling thread; ITriangleConsumer::addTriangle()
   will be called via a dedicated thread owned by the internal pool.
   */
  void March();

 private:
  const IIsoSurface& _volume;
  std::unique_ptr<ancer::ThreadPool> _thread_pool;
  std::vector<ancer::unowned_ptr<ITriangleConsumer>> _consumers;
  mat4 _transform;
  bool _compute_normals;
  std::size_t _num_threads;
  std::vector<ancer::glh::AABBi> _slices;
};

} // namespace marching_cubes

#endif /* marching_cubes_hpp */
