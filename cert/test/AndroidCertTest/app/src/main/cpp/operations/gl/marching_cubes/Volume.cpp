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

#include "Volume.hpp"

#include <ancer/util/Error.hpp>
#include <ancer/util/Log.hpp>

namespace marching_cubes {

namespace {

constexpr ancer::Log::Tag TAG{"OctreeVolume"};

}

void OctreeVolume::DispatchMarch(
    ancer::unowned_ptr<glm::mat4> transform,
    bool compute_normals,
    int batch_size,
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

  switch (batch_size) {

    case 1: {
      // issue one node per job
      for (const auto node : _nodes_to_march) {
        _job_queue->AddJob([=](int thread_idx) {
          auto tc_idx = thread_idx % _triangle_consumers.size();
          auto tc = _triangle_consumers[tc_idx];
          MarchNode(node, *tc, transform, compute_normals);
        });
      }
      _job_queue->RunAllReadiedJobs();
      break;
    }

    case BATCH_USING_QUEUE: {
      // we'll create NumThreads jobs, and each treats _nodes_to_march
      // as a queue to pop a node to process off of until empty. Note: this
      // works well because _nodes_to_march has a bias towards large low-depth
      // nodes near the head, meaning all cores get to chew on big nodes,
      // whereas if we just set batch_size = _nodes_to_march.size() / NumThreads
      // we'd end up stiffing a few cores with all the big nodes
      const auto num_jobs = _job_queue->NumThreads();
      std::mutex pop_mutex;
      for (std::size_t i = 0; i < num_jobs; i++) {
        _job_queue->AddJob([=, &pop_mutex](int thread_idx) {
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

            auto tc_idx = thread_idx % _triangle_consumers.size();
            auto tc = _triangle_consumers[tc_idx];
            MarchNode(node, *tc, transform, compute_normals);
          }
        });
      }
      _job_queue->RunAllReadiedJobs();
      break;
    }

    default: {
      // distribute nodes across threads such that each thread is equally
      // likely to get nodes near start, middle and end of the list
      // this is to mitigate situation where one thread gets all the low-depth
      // big volume nodes and the others go idle

      size_t num_jobs = 0;
      const auto count = _nodes_to_march.size();

      if (batch_size == BATCH_USING_BALANCED_LOAD) {
        num_jobs = _job_queue->NumThreads();
        batch_size = static_cast<int>(ceil(
            static_cast<double>(count) / num_jobs));
      } else if (batch_size > 1) {
        num_jobs =
            static_cast<size_t>(ceil(static_cast<double>(count) / batch_size));
      } else {
        ancer::FatalError(TAG, "Unrecognized batch size %d", batch_size);
      }

      for (size_t i = 0; i < num_jobs; i++) {
        _job_queue->AddJob([=](int thread_idx) {
          auto tc_idx = thread_idx % _triangle_consumers.size();
          auto tc = _triangle_consumers[tc_idx];
          for (size_t j = 0; j < batch_size; j++) {
            auto idx = i + j * num_jobs;
            if (idx < count) {
              MarchNode(_nodes_to_march[idx], *tc, transform, compute_normals);
            }
          }
        });
      }
      _job_queue->RunAllReadiedJobs();
      break;
    }
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