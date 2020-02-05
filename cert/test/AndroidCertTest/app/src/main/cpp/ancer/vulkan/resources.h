#ifndef AGDC_ANCER_RESOURCES_H_
#define AGDC_ANCER_RESOURCES_H_

#include <cstdio>

#include "vulkan_base.h"
#include "buffer.h"
#include "image.h"
#include "sampler.h"
#include "ancer/util/InternalList.h"

namespace ancer {
namespace vulkan {
class ResourcesLayout;
class Resources;
}
};

// ============================================================================
// usefull hashes
namespace std {
template<> struct hash<VkSamplerCreateInfo> {
  std::size_t operator()(const VkSamplerCreateInfo &s) const noexcept;
};
template<> struct hash<VkDescriptorSetLayoutBinding> {
  std::size_t operator()(const VkDescriptorSetLayoutBinding &b) const noexcept;
};
template<> struct hash<ancer::vulkan::ResourcesLayout> {
  std::size_t operator()(const ancer::vulkan::ResourcesLayout & rl) const noexcept;
};
template<> struct hash<ancer::vulkan::Resources> {
  std::size_t operator()(const ancer::vulkan::Resources & rl) const noexcept;
};
} // namespace std

namespace ancer {
namespace vulkan {

/**
 * This class functionally describes a structure for use in shaders with data
 * provided by the application
 */
class ResourcesLayout {
 public:
  ResourcesLayout();

  ResourcesLayout &Sampler(uint32_t binding, VkShaderStageFlags stages,
                           uint32_t count = 1,
                           const VkSampler *immutable_samplers = nullptr);

  ResourcesLayout &ImageSampler(uint32_t binding, VkShaderStageFlags stages,
                                uint32_t count = 1,
                                const VkSampler *immutable_samplers = nullptr);

  ResourcesLayout &SampledImage(uint32_t binding, VkShaderStageFlags stages,
                                uint32_t count = 1);

  ResourcesLayout &StorageImage(uint32_t binding, VkShaderStageFlags stages,
                                uint32_t count = 1);

  ResourcesLayout &UniformTexelBuffer(uint32_t binding,
                                      VkShaderStageFlags stages,
                                      uint32_t count = 1);

  ResourcesLayout &StorageTexelBuffer(uint32_t binding,
                                      VkShaderStageFlags stages,
                                      uint32_t count = 1);

  ResourcesLayout &UniformBuffer(uint32_t binding,
                                 VkShaderStageFlags stages,
                                 bool dynamic = false, uint32_t count = 1);

  ResourcesLayout &StorageBuffer(uint32_t binding,
                                 VkShaderStageFlags stages,
                                 bool dynamic = false, uint32_t count = 1);

  ResourcesLayout &InputAttachment(uint32_t binding, VkShaderStageFlags stages,
                                   uint32_t count = 1);

  ResourcesLayout &Finish();

  uint32_t BindingCount() const;

  const VkDescriptorSetLayoutBinding *Bindings() const;

 private:
  void Insert(VkDescriptorSetLayoutBinding binding);
  VkSampler *Fakify();

  std::vector<VkSampler> _samplers;
  static VkSampler _fake_sampler;
  bool _is_fake_offsets;
  std::vector<VkDescriptorSetLayoutBinding> _bindings;
  VkDescriptorSetLayoutCreateInfo _create_info;
};

/**
 * This contains a set a reosurces that is aligned with a particular layout.
 */
class Resources {
 public:
  Resources &Layout(const ResourcesLayout *rl);

  Resources &Bind(uint32_t binding, uint32_t element, VkSampler sampler);
  inline Resources &Bind(uint32_t binding, VkSampler sampler) {
    return Bind(binding, 0, sampler);
  }

  Resources &Bind(uint32_t binding, uint32_t element, const Image &image);
  inline Resources &Bind(uint32_t binding, const Image &image) {
    return Bind(binding, 0, image);
  }

  Resources &Bind(uint32_t binding, uint32_t element, const Buffer &buffer);
  inline Resources &Bind(uint32_t binding, const Buffer &buffer) {
    return Bind(binding, 0, buffer);
  }

  struct Slot {
    uint32_t binding;
    uint32_t array_element;
    VkBuffer buffer;
    VkDeviceSize offset;
    VkDeviceSize range;
    VkSampler sampler;
    VkImageView image_view;
    VkImageLayout image_layout;
    VkBufferView buffer_view;
  };

  inline const ResourcesLayout &Layout() const {
    assert(_layout != nullptr);
    return *_layout;
  }

  inline uint32_t SlotCount() const {
    return static_cast<uint32_t>(_slots.size());
  }

  inline const Slot *Slots() const {
    return _slots.data();
  }

 private:
  void Insert(Slot slot);

  const ResourcesLayout *_layout;
  std::vector<Slot> _slots;
};

/**
 * This class is a store of resources. Resources are either eternally created or
 * cleaned up over time.
 *
 * Resources are resolved from our class substitutes to Vulkan objects.
 */
class ResourcesStore {
 public:
  Result Initialize(Vulkan &vk, uint32_t pool_size,
                    uint32_t min_descriptor_count);

  void Shutdown();

  Result Resolve(const Sampler &sampler, VkSampler &out);
  inline Result Resolve(const VkSampler &sampler, VkSampler &out) {
    out = sampler;
    return Result::kSuccess;
  }

  Result Resolve(const ResourcesLayout &layout, VkDescriptorSetLayout &out);
  inline Result Resolve(const VkDescriptorSetLayout &layout,
                        VkDescriptorSetLayout &out) {
    out = layout;
    return Result::kSuccess;
  }

  Result Resolve(const Resources &resources, VkDescriptorSet &out);
  inline Result Resolve(const VkDescriptorSet &resources,
                        VkDescriptorSet &out) {
    out = resources;
    return Result::kSuccess;
  }

  Result Cleanup(bool advance = false);

 private:
  Result CreatePool();
  Result AllocateSet(VkDescriptorSetLayout layout, VkDescriptorSet &out);

  struct Pool {
    VkDescriptorPool descriptor_pool;
    uint32_t allocated;
    uint32_t freed;
    std::vector<VkDescriptorSet> cleanup;
  };

  struct DescriptorSet {
    InternalList list;
    uint32_t pool_index;
    VkDescriptorSet descriptor_set;
  };

  static const std::size_t kDescriptorSetLifetime = 3;

  Vulkan _vk;

  std::unordered_map<VkSamplerCreateInfo, VkSampler> _samplers;
  std::unordered_map<ResourcesLayout, VkDescriptorSetLayout> _layouts;

  uint32_t _pool_size;
  uint32_t _min_descriptor_count;
  std::vector<Pool> _pools;

  std::mutex _sets_mutex;
  InternalList _inactive_sets[kDescriptorSetLifetime];
  InternalList _active_sets;
  std::unordered_map<Resources, DescriptorSet> _sets;
};

}
}

#endif
