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

/*
 * IsoSurfaceValueFunction is the sampler function for the isosurface,
 * such that sampling a point(x,y,z) returns a value from 0 (empty) to 1 (full)
 */
typedef std::function<float(const vec3& p)> IsoSurfaceValueFunction;

/*
 * IsoSurfaceNormalFunction is the normal sampler, returning the normal of the
 * local gradient of the IsoSurface
 */
typedef std::function<vec3(const vec3& p)> IsoSurfaceNormalFunction;

//
// Marching
//

/*
  March region of a volume passing generated triangles into triangleConsumer
  region: The subregion to march
  value_sampler: source of isosurface values
  normal_sampler: if provided, will be used to compute per-vertex surface normals. if null,
    each vertex will receive the normal of the triangle it is a part of
  triangle_consumer: Receives each generated triangle
  transform: If not null, a transform to apply to each generated vertex
*/
void march(ancer::glh::AABBi region,
           IsoSurfaceValueFunction value_sampler,
           IsoSurfaceNormalFunction normal_sampler,
           ITriangleConsumer& triangle_consumer,
           ancer::unowned_ptr<glm::mat4> transform);

} // namespace marching_cubes

#endif /* marching_cubes_hpp */
