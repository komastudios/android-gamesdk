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

#ifndef volume_hpp
#define volume_hpp

#include <memory>
#include <unordered_set>
#include <vector>

#include <ancer/util/ThreadPool.hpp>
#include <ancer/util/UnownedPtr.hpp>

#include "MarchingCubes.hpp"

namespace marching_cubes {

//
// VolumeSampler
//

/*
 * Base class for a thing which occupies the space in a volume; e.g.,
 * a SphereVolumeSampler would represent a sphere in a volume, and would
 * implement methods for testing AABB intersection and isosurface value taps
 */
class IVolumeSampler {
 public:

  enum class AABBIntersection {
    None,
    IntersectsAABB,
    ContainsAABB
  };

  enum class Mode {
    Additive,
    Subtractive
  };

 public:
  IVolumeSampler(Mode mode)
      : _mode(mode) {
  }

  virtual ~IVolumeSampler() = default;

  Mode GetMode() const { return _mode; }

  /*
   Return true iff bounds intersects the region affected by this sampler
  */
  virtual bool Intersects(ancer::glh::AABB bounds) const = 0;

  /*
  Subtractive samplers can offer an optimization by overriding this method
  to return one of AABBIntersection. In the case that an AABB is completely
  contained by the volume, this method should return AABBIntersection::ContainsAABB
  which allows the OctreeVolume to optimize the set of nodes to march.
  */
  virtual AABBIntersection Intersection(ancer::glh::AABB bounds) const {
    return Intersects(bounds) ? AABBIntersection::IntersectsAABB
                              : AABBIntersection::None;
  }

  /*
   Return the amount a given point in space is "inside" the volume. The
   fuzziness is a gradient to allow for smoothing. For
   example, in the case of a circle or radius r, a point distance <= r - fuziness
   returns a value of 1, and a distance >= r returns 0,
   and values in between return a linear transition.
   */
  virtual float ValueAt(const vec3& p, float fuzziness) const = 0;

 private:
  Mode _mode;
};

//
// Volume
//

/*
 * Base class for a volume made up of a collection of IVolumeSampler instances
 */
class BaseCompositeVolume {
 public:
  BaseCompositeVolume(ivec3 size, float fuzziness)
      : _size(size), _fuzziness(fuzziness) {
  }

  virtual ~BaseCompositeVolume() = default;

  template<typename T>
  ancer::unowned_ptr<T> Add(std::unique_ptr<T>&& sampler) {
    auto sPtr = sampler.get();
    _samplers.push_back(std::move(sampler));

    switch (sPtr->GetMode()) {
      case IVolumeSampler::Mode::Additive:_additive_samplers.push_back(sPtr);
        break;

      case IVolumeSampler::Mode::Subtractive:_subtractive_samplers.push_back(sPtr);
        break;
    }

    return sPtr;
  }

  void Clear() {
    _samplers.clear();
    _additive_samplers.clear();
    _subtractive_samplers.clear();
  }

  ivec3 Size() const {
    return _size;
  }

  void SetFuzziness(float ft) { _fuzziness = std::max<float>(ft, 0); }
  float GetFuzziness() const { return _fuzziness; }

 protected:
  ivec3 _size;
  float _fuzziness;
  std::vector<IVolumeSampler*> _additive_samplers, _subtractive_samplers;
  std::vector<std::unique_ptr<IVolumeSampler>> _samplers;
};

/*
 * Implements BaseCompositeVolume using octree subdivision to reduce the
 * number of voxels marched to properly represent the volume's contents
 */
class OctreeVolume : public BaseCompositeVolume {
 public:
  struct Node {
    Node() = delete;
    Node(const Node&) = delete;
    Node(const Node&&) = delete;
    Node(const ancer::glh::AABB bounds, int depth, int childIdx)
        : bounds(bounds), depth(depth), child_idx(childIdx) {
    }
    ~Node() = default;

    ancer::glh::AABB bounds;
    int depth = 0;
    int child_idx = 0;
    bool is_leaf = false;
    bool march = false;
    bool empty = false;
    std::array<std::unique_ptr<Node>, 8> children;
    std::unordered_set<IVolumeSampler*> additive_samplers;
    std::unordered_set<IVolumeSampler*> subtractive_samplers;

   private:
    friend class OctreeVolume;

    std::vector<IVolumeSampler*> _additive_samplers_vec;
    std::vector<IVolumeSampler*> _subtractive_samplers_vec;
  };

 public:
  OctreeVolume(int size,
               float fuzziness,
               int min_node_size,
               ancer::unowned_ptr<ancer::ThreadPool> thread_pool,
               const std::vector<ancer::unowned_ptr<ITriangleConsumer>>
               & triangle_consumers)
      :
      BaseCompositeVolume(ivec3{size, size, size}, fuzziness),
      _bounds(ancer::glh::AABB(ivec3(0, 0, 0), ivec3(size, size, size))),
      _root(BuildOctreeNode(_bounds, min_node_size, 0, 0)),
      _thread_pool(thread_pool),
      _triangle_consumers(triangle_consumers) {
  }

  void WalkOctree(std::function<bool(Node*)> visitor) {
    std::function<void(Node*)> walker = [&visitor, &walker](Node* node) {
      if (visitor(node)) {
        if (!node->is_leaf) {
          for (auto& child : node->children) {
            walker(child.get());
          }
        }
      }
    };

    walker(_root.get());
  }

  /**
   * March the represented volume into the triangle consumers provided in the constructor
  */
  void March(mat4 transform,
             bool compute_normals,
             std::function<void(OctreeVolume::Node*)> marched_node_observer = nullptr) {
    DispatchMarch(ancer::unowned_ptr<mat4>(&transform),
                  compute_normals,
                  marched_node_observer);
  }

  void March(bool compute_normals,
             std::function<void(OctreeVolume::Node*)> marched_node_observer = nullptr) {
    DispatchMarch(nullptr, compute_normals, marched_node_observer);
  }

  /**
   * Get the bounds of this volume - no geometry will exceed this region
   */
  ancer::glh::AABB GetBounds() const {
    return _bounds;
  }

  /**
   * Get the max octree node depth
   */
  size_t GetDepth() const {
    return _tree_depth;
  }

 protected:

  void DispatchMarch(ancer::unowned_ptr<mat4> transform,
                     bool compute_normals,
                     std::function<void(OctreeVolume::Node*)> marched_node_observer);

  void MarchNode(OctreeVolume::Node* node,
                 ITriangleConsumer& tc,
                 ancer::unowned_ptr<mat4> transform,
                 bool compute_normals);

  /**
   * Mark the nodes which should be marched.
  */
  bool Mark(Node* current_node) const {
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
        if (Mark(child.get())) {
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

  void Collect(std::vector<Node*>& collector) {
    /*
        TODO: I believe I should probably skip the root node; it will always intersect
        something; it's not useful.
    */
    Mark(_root.get());
    Collect(_root.get(), collector);
  }

  /**
   * After calling mark(), this will collect all nodes which should be marched
  */
  void Collect(Node* current_node, std::vector<Node*>& nodes_to_march) const {
    if (current_node->empty)
      return;

    if (current_node->march) {
      // collect this node, don't recurse further
      nodes_to_march.push_back(current_node);
    } else if (!current_node->is_leaf) {
      // if this is not a leaf node and not marked
      for (const auto& child : current_node->children) {
        Collect(child.get(), nodes_to_march);
      }
    }
  }

  std::unique_ptr<Node>
  BuildOctreeNode(ancer::glh::AABB bounds,
                  size_t min_node_size,
                  size_t depth,
                  size_t child_idx) {
    _tree_depth = std::max(depth, _tree_depth);
    auto node = std::make_unique<Node>(bounds, depth, child_idx);

    // we're working on cubes, so only one bounds size is checked
    size_t size = static_cast<size_t>(bounds.Size().x);
    if (size / 2 >= min_node_size) {
      node->is_leaf = false;
      auto childBounds = bounds.OctreeSubdivide();
      for (size_t i = 0, N = childBounds.size(); i < N; i++) {
        node->children[i] =
            BuildOctreeNode(childBounds[i], min_node_size, depth + 1, i);
      }
    } else {
      node->is_leaf = true;
    }

    return node;
  }

 private:
  ancer::glh::AABB _bounds;
  size_t _tree_depth = 0;
  std::unique_ptr<Node> _root;
  std::vector<Node*> _nodes_to_march;
  ancer::unowned_ptr<ancer::ThreadPool> _thread_pool;
  std::vector<ancer::unowned_ptr<ITriangleConsumer>> _triangle_consumers;
};

} // namespace marching_cubes

#endif /* volume_hpp */
