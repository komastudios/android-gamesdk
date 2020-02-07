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

namespace {
/*
 * Number of nodes to collect into a single batch to process on the job queue.
 * If set to 1, each node will be marched by a single independent job.
 * If set to 0, number of jobs is set to # threads, which consume nodes
 *  until the nodes to march are exhausted
 */
constexpr int BatchSize = 0;
}

void OctreeVolume::DispatchMarch(
    ancer::unowned_ptr<glm::mat4> transform,
    bool compute_normals,
    std::function<void(OctreeVolume::Node*)> marched_node_observer) {

  // gather the nodes to march
  _nodes_to_march.clear();
  CollectNodesToMarch(_nodes_to_march);

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

  if (BatchSize > 1) {
    for (int i = 0, N = static_cast<int>(_nodes_to_march.size());
         i < N;
         i += BatchSize) {
      _job_queue->AddJob([=](int thread_idx) {
        auto tc_idx = thread_idx % _triangle_consumers.size();
        auto tc = _triangle_consumers[tc_idx];

        for (int j = 0; j < BatchSize; j++) {
          int k = i + j;
          if (k < N) {
            MarchNode(_nodes_to_march[k], *tc, transform, compute_normals);
          }
        }
      });
    }
    _job_queue->RunAllReadiedJobs();
  } else if (BatchSize == 1) {
    for (const auto node : _nodes_to_march) {
      _job_queue->AddJob([=](int thread_idx) {
        auto tc_idx = thread_idx % _triangle_consumers.size();
        auto tc = _triangle_consumers[tc_idx];
        MarchNode(node, *tc, transform, compute_normals);
      });
    }
    _job_queue->RunAllReadiedJobs();
  } else {
    std::mutex pop_mutex;
    for (std::size_t i = 0, N = _job_queue->NumThreads(); i < N; i++) {
      _job_queue->AddJob([=,&pop_mutex](int thread_idx) {
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
                    *_triangle_consumers[thread_idx % N],
                    transform,
                    compute_normals);
        }
      });
    }
    _job_queue->RunAllReadiedJobs();
  }

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

bool OctreeVolume::MarkNodesToMarch(Node* current_node) const {
  current_node->empty = true;
  current_node->march = false;

  current_node->additive_samplers.clear();
  current_node->subtractive_samplers.clear();

  for (const auto sampler : _additive_samplers) {
    if (sampler->Intersects(current_node->bounds)) {
      current_node->additive_samplers.insert(sampler);
      current_node->empty = false;
    }
  }

  // we don't care about subtractiveSamplers UNLESS the
  // current node has additive ones; because without any
  // additive samplers, there is no volume to subtract from
  if (!current_node->empty) {
    for (const auto sampler : _subtractive_samplers) {
      auto intersection = sampler->Intersection(current_node->bounds);
      switch (intersection) {
        case IVolumeSampler::AABBIntersection::IntersectsAABB:
          current_node->subtractive_samplers.insert(sampler);
          break;
        case IVolumeSampler::AABBIntersection::ContainsAABB:
          // special case - this node is completely contained
          // by the volume, which means it is EMPTY.
          current_node->additive_samplers.clear();
          current_node->subtractive_samplers.clear();
          current_node->empty = true;
          break;
        case IVolumeSampler::AABBIntersection::None:break;
      }

      // if a subtractive sampler cleared this node,
      // break the loop, we're done
      if (current_node->empty) {
        break;
      }
    }
  }

  if (!current_node->empty) {

    if (current_node->is_leaf) {
      current_node->march = true;
      return true;
    }

    // some samplers intersect this node; traverse down
    int occupied = 0;
    for (auto& child : current_node->children) {
      if (MarkNodesToMarch(child.get())) {
        occupied++;
      }
    }

    if (occupied == 8) {

      // all 8 children intersect samplers; mark self to march, and
      // coalesce their samplers into currentNode
      current_node->march = true;

      // copy up their samplers
      for (auto& child : current_node->children) {
        child->march = false;
        current_node->additive_samplers.insert(std::begin(child->additive_samplers),
                                               std::end(child->additive_samplers));
        current_node->subtractive_samplers.insert(std::begin(child->subtractive_samplers),
                                                  std::end(child->subtractive_samplers));
      }

      return true;
    }
  }

  return false;
}

}