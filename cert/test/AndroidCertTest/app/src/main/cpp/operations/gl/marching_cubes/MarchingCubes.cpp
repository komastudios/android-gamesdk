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

#include "MarchingCubes.hpp"
#include "MarchingCubes_Detail.hpp"

using ancer::unowned_ptr;
using ancer::glh::AABBi;

namespace marching_cubes {

namespace {
constexpr float IsoLevel = 0.5F;
}

void march(AABBi region,
           IsoSurfaceValueFunction value_sampler,
           IsoSurfaceNormalFunction normal_sampler,
           ITriangleConsumer& tc,
           const unowned_ptr<glm::mat4> transform) {
  Triangle triangles[5];
  detail::GridCell cell;

  for (int z = region.min.z; z < region.max.z; z++) {
    for (int y = region.min.y; y < region.max.y; y++) {
      for (int x = region.min.x; x < region.max.x; x++) {
        if (detail::GetGridCell(x, y, z, value_sampler, cell, transform)) {
          for (int t = 0, nTriangles =
              detail::Polygonise(cell, IsoLevel, normal_sampler, triangles);
               t < nTriangles; t++) {
            tc.AddTriangle(triangles[t]);
          }
        }
      }
    }
  }
}

} // namespace marching_cubes
