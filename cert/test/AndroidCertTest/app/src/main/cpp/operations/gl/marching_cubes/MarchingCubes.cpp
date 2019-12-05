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

#include <utility>

using ancer::unowned_ptr;

namespace mc {

namespace {
constexpr float IsoLevel = 0.5F;
}

//------------------------------------------------------------------------------

vec3 IIsoSurface::NormalAt(const vec3& p) const {
  // from GPUGems 3 Chap 1 -- compute normal of voxel space
  const float d = 0.1F;
  vec3 grad(
      ValueAt(p + vec3(d, 0, 0)) - ValueAt(p + vec3(-d, 0, 0)),
      ValueAt(p + vec3(0, d, 0)) - ValueAt(p + vec3(0, -d, 0)),
      ValueAt(p + vec3(0, 0, d)) - ValueAt(p + vec3(0, 0, -d)));

  return -normalize(grad);
}

//------------------------------------------------------------------------------

void march(const IIsoSurface& volume,
           ITriangleConsumer& tc,
           const mat4& transform,
           bool computeNormals) {
  tc.Start();
  march(volume, AABBi(ivec3(0), volume.Size()), tc, transform, computeNormals);
  tc.Finish();
}

void march(const IIsoSurface& volume,
           AABBi region,
           ITriangleConsumer& tc,
           const mat4& transform,
           bool computeNormals) {
  using detail::GetGridCell;
  using detail::GridCell;
  using detail::Polygonise;

  Triangle triangles[5];
  GridCell cell;

  region.min = max(region.min, ivec3(0));
  region.max = min(region.max, volume.Size());

  for (int z = region.min.z; z < region.max.z; z++) {
    for (int y = region.min.y; y < region.max.y; y++) {
      for (int x = region.min.x; x < region.max.x; x++) {
        if (GetGridCell(x, y, z, volume, cell, transform)) {
          for (int t = 0, nTriangles =
              Polygonise(cell, IsoLevel, volume, triangles, computeNormals);
               t < nTriangles; t++) {
            tc.AddTriangle(triangles[t]);
          }
        }
      }
    }
  }
}

//------------------------------------------------------------------------------

ThreadedMarcher::ThreadedMarcher(const IIsoSurface& volume,
                                 std::vector<unowned_ptr<ITriangleConsumer>> triangle_consumers,
                                 std::unique_ptr<ThreadPool> && thread_pool,
                                 const mat4& transform,
                                 bool computeNormals)
    : _volume(volume),
      _consumers(std::move(triangle_consumers)),
      _thread_pool(std::move(thread_pool)),
      _transform(transform),
      _compute_normals(computeNormals),
      _num_threads(_consumers.size()),
      _slices(_num_threads) {

  // cut _volume into _slices
  auto region = AABBi(ivec3(0), _volume.Size());
  auto slice_size = static_cast<int>(ceil(
      static_cast<float>(_volume.Size().y) / static_cast<float>(_num_threads)));

  for (auto i = 0; i < _num_threads; i++) {
    AABBi slice = region;
    slice.min.y = i * slice_size;
    slice.max.y = slice.min.y + slice_size;
    _slices[i] = slice;
  }
}

void ThreadedMarcher::March() {
  for (auto i = 0; i < _num_threads; i++) {
    _consumers[i]->Start();
    _thread_pool->Enqueue([this, i]() {
      mc::march(_volume,
                _slices[i],
                *_consumers[i],
                _transform,
                _compute_normals);
    });
  }

  _thread_pool->Wait();

  for (auto i = 0; i < _num_threads; i++) {
    _consumers[i]->Finish();
  }
}

}
