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

#include "Volume.hpp"

namespace marching_cubes {

void OctreeVolume::DispatchMarch(
    ancer::unowned_ptr<glm::mat4> transform,
    bool compute_normals,
    std::function<void(OctreeVolume::Node*)> marched_node_observer) {
  _nodes_to_march.clear();
  Collect(_nodes_to_march);

  // if we hav an observer, pass collected march nodes to it
  if (marched_node_observer) {
    for (const auto& node : _nodes_to_march) {
      marched_node_observer(node);
    }
  }

  // collapse each node's set of samplers to a vector for faster iteration when marching
  for (const auto& node : _nodes_to_march) {
    node->_additive_samplers_vec.clear();
    node->_subtractive_samplers_vec.clear();

    std::copy(std::begin(node->additive_samplers),
              std::end(node->additive_samplers),
              std::back_inserter(node->_additive_samplers_vec));

    std::copy(std::begin(node->subtractive_samplers),
              std::end(node->subtractive_samplers),
              std::back_inserter(node->_subtractive_samplers_vec));
  }

  for (auto& tc : _triangle_consumers) {
    tc->Start();
  }

  std::mutex pop_mutex;
  for (std::size_t i = 0, N = _thread_pool->NumThreads(); i < N; i++) {
    _thread_pool->Enqueue([&pop_mutex, this, i, N, &transform, compute_normals]() {
      while (true) {
        Node* node = nullptr;
        {
          std::lock_guard<std::mutex> pop_lock(pop_mutex);
          if (_nodes_to_march.empty()) {
            // we're done, exit the loop
            return;
          }
          node = _nodes_to_march.back();
          _nodes_to_march.pop_back();
        }

        MarchNode(node,
                  *_triangle_consumers[i % N],
                  transform,
                  compute_normals);
      }
    });
  }

  _thread_pool->Wait();

  for (auto& tc : _triangle_consumers) {
    tc->Finish();
  }
}

void OctreeVolume::MarchNode(OctreeVolume::Node* node,
                             ITriangleConsumer& tc,
                             ancer::unowned_ptr<glm::mat4> transform,
                             bool compute_normals) {
  auto fuzziness = this->_fuzziness;
  auto value_sampler = [fuzziness, node](const vec3& p) {
    float value = 0;
    for (auto additive_sampler : node->_additive_samplers_vec) {
      value += additive_sampler->ValueAt(p, fuzziness);
    }
    value = min<float>(value, 1.0F);
    for (auto subtractive_sampler : node->_subtractive_samplers_vec) {
      value -= subtractive_sampler->ValueAt(p, fuzziness);
    }
    value = max<float>(value, 0.0F);
    return value;
  };

  if (compute_normals) {
    auto normal_sampler = [&value_sampler](const vec3& p) {
      // from GPUGems 3 Chap 1 -- compute normal of voxel space
      const float d = 0.1f;
      vec3 grad(
          value_sampler(p + vec3(d, 0, 0)) - value_sampler(p + vec3(-d, 0, 0)),
          value_sampler(p + vec3(0, d, 0)) - value_sampler(p + vec3(0, -d, 0)),
          value_sampler(p + vec3(0, 0, d)) - value_sampler(p + vec3(0, 0, -d)));

      return -normalize(grad);
    };

    march(ancer::glh::AABBi(node->bounds),
          value_sampler,
          normal_sampler,
          tc,
          transform);
  } else {
    march(ancer::glh::AABBi(node->bounds),
          value_sampler,
          nullptr,
          tc,
          transform);
  }
}

}