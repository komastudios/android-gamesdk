#include "resources.h"

#include <algorithm>

// ============================================================================
// usefull hashes .... STL makes hashes difficult
template<class T>
static void hash_combine(std::size_t &x, const T &y) {
  x ^= std::hash<T>{}(y) + 0x9e3779b9 + (x << 6) + (x >> 2);
}
static uint64_t _fnv_1a(const uint8_t *in, size_t length) {
  uint64_t hash = 0xcbf29ce484222325;
  for(size_t i = 0; i < length; ++i) {
    hash *= 0x00000100000001B3;
    hash ^= in[i];
  }
  return hash;
}
template<class T>
uint64_t fnv_1a(const T *in, size_t count = 1, size_t length = 0) {
  if(length == 0) length = sizeof(T);
  return _fnv_1a(reinterpret_cast<const uint8_t *>(in), length * count);
}
std::size_t std::hash<VkSamplerCreateInfo>::operator ()
    (const VkSamplerCreateInfo &s) const noexcept {
  return fnv_1a(&s);
}
std::size_t std::hash<VkDescriptorSetLayoutBinding>::operator ()
    (const VkDescriptorSetLayoutBinding &b) const noexcept {
  // everything but the samplers pointer
  uint64_t h = fnv_1a(&b, 1, sizeof(b) - sizeof(b.pImmutableSamplers));
  // the samplers
  if(b.pImmutableSamplers != nullptr) {
    h ^= fnv_1a(b.pImmutableSamplers, b.descriptorCount);
  }
  return h;
}
std::size_t std::hash<ancer::vulkan::ResourcesLayout>::operator ()
    (const ancer::vulkan::ResourcesLayout & rl) const noexcept {
  std::size_t h = 0;
  uint32_t e = rl.BindingCount();
  for(uint32_t i = 0; i < e; ++i) {
    hash_combine(h, i);
    hash_combine(h, rl.Bindings()[i]);
  }
  return h;
}
std::size_t std::hash<ancer::vulkan::Resources>::operator ()
    (const ancer::vulkan::Resources & r) const noexcept {
  return fnv_1a(r.Slots(), r.SlotCount());
}

// ============================================================================
// more operators needed for unordered_maps
static bool operator == (const VkSamplerCreateInfo &a,
                         const VkSamplerCreateInfo &b) {
  return ::memcmp(&a, &b, sizeof(a)) == 0;
}
namespace ancer {
namespace vulkan {
static bool operator == (const ResourcesLayout &a, const ResourcesLayout &b) {
  uint32_t count = a.BindingCount();
  if(count != b.BindingCount())
    return false;
  for(uint32_t i = 0; i < count; ++i) {
    auto a_binding = a.Bindings()[i];
    auto b_binding = b.Bindings()[i];
    // TODO(sarahburns@google.com) test samplers
    if(::memcmp(&a_binding, &b_binding, sizeof(a_binding)) != 0) {
      return false;
    }
  }
  return true;
}
static bool operator == (const Resources &a, const Resources &b) {
  uint32_t count = a.SlotCount();
  if(count != b.SlotCount())
    return false;
  for(uint32_t i = 0; i < count; ++i) {
    auto a_slot = a.Slots()[i];
    auto b_slot = b.Slots()[i];
    if(::memcmp(&a_slot, &b_slot, sizeof(a_slot)) != 0) {
      return false;
    }
  }
  return true;
}
}
}

namespace ancer {
namespace vulkan {

// ============================================================================
// ResourcesLayout
VkSampler ResourcesLayout::_fake_sampler;

ResourcesLayout::ResourcesLayout() {
}

ResourcesLayout &ResourcesLayout::Sampler(uint32_t binding,
                                          VkShaderStageFlags stages,
                                          uint32_t count,
                                          const VkSampler *i_samplers) {
  // invalid memory address, only used to store offset of _samplers
  const VkSampler *fake = nullptr;
  if(i_samplers != nullptr) {
    fake = Fakify();
    for(uint32_t i = 0; i < count; ++i)
      _samplers.push_back(i_samplers[i]);
  }
  Insert({
    /* binding            */ binding,
    /* descriptorType     */ VK_DESCRIPTOR_TYPE_SAMPLER,
    /* descriptorCount    */ count,
    /* stageFlags         */ stages,
    /* pImmutableSamplers */ fake
  });
  return *this;
}

ResourcesLayout &ResourcesLayout::ImageSampler(uint32_t binding,
                                               VkShaderStageFlags stages,
                                               uint32_t count,
                                               const VkSampler *i_samplers) {
  // invalid memory address, only used to store offset of _samplers
  const VkSampler *fake = nullptr;
  if(i_samplers != nullptr) {
    fake = Fakify();
    for(uint32_t i = 0; i < count; ++i)
      _samplers.push_back(i_samplers[i]);
  }
  Insert({
    /* binding            */ binding,
    /* descriptorType     */ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    /* descriptorCount    */ count,
    /* stageFlags         */ stages,
    /* pImmutableSamplers */ fake
  });
  return *this;
}

ResourcesLayout &ResourcesLayout::SampledImage(uint32_t binding,
                                               VkShaderStageFlags stages,
                                               uint32_t count) {
  Insert({
    /* binding            */ binding,
    /* descriptorType     */ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    /* descriptorCount    */ count,
    /* stageFlags         */ stages,
    /* pImmutableSamplers */ nullptr
  });
  return *this;
}

ResourcesLayout &ResourcesLayout::StorageImage(uint32_t binding,
                                               VkShaderStageFlags stages,
                                               uint32_t count) {
  Insert({
    /* binding            */ binding,
    /* descriptorType     */ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    /* descriptorCount    */ count,
    /* stageFlags         */ stages,
    /* pImmutableSamplers */ nullptr
  });
  return *this;
}

ResourcesLayout &ResourcesLayout::UniformTexelBuffer(uint32_t binding,
                                                     VkShaderStageFlags stages,
                                                     uint32_t count) {
  Insert({
    /* binding            */ binding,
    /* descriptorType     */ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
    /* descriptorCount    */ count,
    /* stageFlags         */ stages,
    /* pImmutableSamplers */ nullptr
  });
  return *this;
}

ResourcesLayout &ResourcesLayout::StorageTexelBuffer(uint32_t binding,
                                                     VkShaderStageFlags stages,
                                                     uint32_t count) {
  Insert({
    /* binding            */ binding,
    /* descriptorType     */ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
    /* descriptorCount    */ count,
    /* stageFlags         */ stages,
    /* pImmutableSamplers */ nullptr
  });
  return *this;
}

ResourcesLayout &ResourcesLayout::UniformBuffer(uint32_t binding,
                                                VkShaderStageFlags stages,
                                                bool dynamic, uint32_t count) {
  Insert({
    /* binding            */ binding,
    /* descriptorType     */ dynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
                                     VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
    /* descriptorCount    */ count,
    /* stageFlags         */ stages,
    /* pImmutableSamplers */ nullptr
  });
  return *this;
}

ResourcesLayout &ResourcesLayout::StorageBuffer(uint32_t binding,
                                                VkShaderStageFlags stages,
                                                bool dynamic, uint32_t count) {
  Insert({
    /* binding            */ binding,
    /* descriptorType     */ dynamic ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER :
                                     VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
    /* descriptorCount    */ count,
    /* stageFlags         */ stages,
    /* pImmutableSamplers */ nullptr
  });
  return *this;
}

ResourcesLayout &ResourcesLayout::InputAttachment(uint32_t binding,
                                                  VkShaderStageFlags stages,
                                                  uint32_t count) {
  Insert({
    /* binding            */ binding,
    /* descriptorType     */ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
    /* descriptorCount    */ count,
    /* stageFlags         */ stages,
    /* pImmutableSamplers */ nullptr
  });
  return *this;
}

ResourcesLayout &ResourcesLayout::Finish() {
  if(!_is_fake_offsets)
    return *this;
  _is_fake_offsets = false;

  for(auto && binding : _bindings) {
    if(binding.pImmutableSamplers != nullptr) {
      auto offset = binding.pImmutableSamplers - &_fake_sampler;
      binding.pImmutableSamplers = _samplers.data() + offset;
    }
  }

  return *this;
}

void ResourcesLayout::Insert(VkDescriptorSetLayoutBinding binding) {
  auto it = std::upper_bound(_bindings.begin(), _bindings.end(), binding,
                             [](const auto &a, const auto &b) -> bool {
                               return a.binding < b.binding;
                             });
  if(it != _bindings.end())
    assert(it->binding != binding.binding);
  _bindings.insert(it, binding);
}

uint32_t ResourcesLayout::BindingCount() const {
  return static_cast<uint32_t>(_bindings.size());
}

const VkDescriptorSetLayoutBinding *ResourcesLayout::Bindings() const {
  return _bindings.data();
}

VkSampler *ResourcesLayout::Fakify() {
  if(!_is_fake_offsets) {
    _is_fake_offsets = true;

    for(auto && binding : _bindings) {
      if(binding.pImmutableSamplers != nullptr) {
        auto offset = binding.pImmutableSamplers - _samplers.data();
        binding.pImmutableSamplers = &_fake_sampler + offset;
      }
    }
  }

  return &_fake_sampler + _samplers.size();
}

// ============================================================================
// Resources
Resources &Resources::Layout(const ResourcesLayout *layout) {
  _layout = layout;
  return *this;
}

Resources &Resources::Bind(uint32_t binding, uint32_t element,
                           VkSampler sampler) {
  Insert({
    /* binding       */ binding,
    /* array_element */ element,
    /* buffer        */ VK_NULL_HANDLE,
    /* offset        */ 0,
    /* range         */ 0,
    /* sampler       */ sampler,
    /* image_view    */ VK_NULL_HANDLE,
    /* image_layout  */ VK_IMAGE_LAYOUT_UNDEFINED,
    /* buffer_view   */ VK_NULL_HANDLE
  });
  return *this;
}

Resources &Resources::Bind(uint32_t binding, uint32_t element,
                           const Image &image) {
  Insert({
    /* binding       */ binding,
    /* array_element */ element,
    /* buffer        */ VK_NULL_HANDLE,
    /* offset        */ 0,
    /* range         */ 0,
    /* sampler       */ image.Sampler(),
    /* image_view    */ image.View(),
    /* image_layout  */ image.Layout(),
    /* buffer_view   */ VK_NULL_HANDLE
  });
  return *this;
}

Resources &Resources::Bind(uint32_t binding, uint32_t element,
                           const Buffer &buffer) {
  Insert({
    /* binding       */ binding,
    /* array_element */ element,
    /* buffer        */ buffer.Handle(),
    /* offset        */ buffer.Offset(),
    /* range         */ buffer.Range(),
    /* sampler       */ VK_NULL_HANDLE,
    /* image_view    */ VK_NULL_HANDLE,
    /* image_layout  */ VK_IMAGE_LAYOUT_UNDEFINED,
    /* buffer_view   */ buffer.View()
  });
  return *this;
}

void Resources::Insert(Slot slot) {
  auto it = std::upper_bound(_slots.begin(), _slots.end(), slot,
                             [](const auto &a, const auto &b) -> bool {
                               if(a.binding == b.binding)
                                 return a.array_element < b.array_element;
                               return a.binding < b.binding;
                             });
  if(it != _slots.end())
    assert(it->binding != slot.binding ||
           it->array_element != slot.array_element);
  _slots.insert(it, slot);
}

// ============================================================================
// ResourcesStore
Result ResourcesStore::Initialize(Vulkan &vk, uint32_t pool_size,
                                 uint32_t min_descriptor_count) {
  _vk = vk;
  _pool_size = pool_size;
  _min_descriptor_count = min_descriptor_count;
  return Result::kSuccess;
}

void ResourcesStore::Shutdown() {
  for(auto && sampler : _samplers)
    _vk->destroySampler(_vk->device, sampler.second, nullptr);
  for(auto && dsl : _layouts)
    _vk->destroyDescriptorSetLayout(_vk->device, dsl.second, nullptr);
}

Result ResourcesStore::Resolve(const Sampler &sampler, VkSampler &out) {
  VkSamplerCreateInfo create_info = sampler.CreateInfo();
  auto i = _samplers.find(create_info);
  if(i != _samplers.end()) {
    out = i->second;
    return Result::kSuccess;
  }
  VK_RETURN_FAIL(_vk->createSampler(_vk->device, &create_info, nullptr, &out));
  _samplers[create_info] = out;
  return Result::kSuccess;
}

Result ResourcesStore::Resolve(const ResourcesLayout &layout,
                              VkDescriptorSetLayout &out) {
  auto i = _layouts.find(layout);
  if(i != _layouts.end()) {
    out = i->second;
    return Result::kSuccess;
  }
  VkDescriptorSetLayoutCreateInfo create_info = {
    /* sType        */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    /* pNext        */ nullptr,
    /* flags        */ 0,
    /* bindingCount */ layout.BindingCount(),
    /* pBindings    */ layout.Bindings()
  };
  VK_RETURN_FAIL(_vk->createDescriptorSetLayout(_vk->device, &create_info,
                                                nullptr, &out));
  _layouts[layout] = out;
  return Result::kSuccess;
}

Result ResourcesStore::Resolve(const Resources &resources,
                               VkDescriptorSet &out) {
  std::lock_guard<std::mutex> guard(_sets_mutex);

  // retrieve memoized set if any
  auto set_find = _sets.find(resources);
  if(set_find != _sets.end()) {
    auto &set = set_find->second;
    set.list.InsertBack(_active_sets); // keep this set alive
    out = set.descriptor_set;
    return Result::kSuccess;
  }

  VkDescriptorSetLayout layout;
  VK_RETURN_FAIL(Resolve(resources.Layout(), layout));

#define ATTEMPT() do { \
    VK_RETURN_FAIL(AllocateSet(layout, out)); \
    if(out != VK_NULL_HANDLE) \
      return Result::kSuccess; \
  } while(false)

  out = VK_NULL_HANDLE;
  ATTEMPT();

  VK_RETURN_FAIL(Cleanup());
  ATTEMPT();

  VK_RETURN_FAIL(CreatePool());
  ATTEMPT();

#undef ATTEMPT

  return Result(VK_ERROR_FRAGMENTED_POOL);
}

Result ResourcesStore::Cleanup(bool advance) {
  std::lock_guard<std::mutex> guard(_sets_mutex);

  InternalList *item;
  while(_inactive_sets[0].ListPopFront(item)) {
    auto &ds = item->Get(&DescriptorSet::list);
    auto &pool = _pools[ds.pool_index];
    pool.cleanup.push_back(ds.descriptor_set);
    ds.pool_index = 0;
    ds.descriptor_set = VK_NULL_HANDLE;
  }
  for(auto && pool : _pools) {
    uint32_t cleanup_count = static_cast<uint32_t>(pool.cleanup.size());
    if(cleanup_count == 0)
      continue;
    VK_RETURN_FAIL(_vk->freeDescriptorSets(_vk->device, pool.descriptor_pool,
                                           cleanup_count,
                                           pool.cleanup.data()));
    pool.freed += cleanup_count;
    pool.cleanup.clear();
  }
  if(advance) {
    for(uint32_t i = 0; i < kDescriptorSetLifetime - 1; ++i) {
      _inactive_sets[i] = std::move(_inactive_sets[i + 1]);
    }
    _inactive_sets[kDescriptorSetLifetime - 1] = std::move(_active_sets);
  }
  return Result::kSuccess;
}

Result ResourcesStore::CreatePool() {
  VkDescriptorPoolSize pool_sizes[] = {
    { VK_DESCRIPTOR_TYPE_SAMPLER, _min_descriptor_count },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _min_descriptor_count },
    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, _min_descriptor_count },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, _min_descriptor_count },
    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, _min_descriptor_count },
    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, _min_descriptor_count },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _min_descriptor_count },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, _min_descriptor_count },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, _min_descriptor_count },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, _min_descriptor_count },
    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, _min_descriptor_count },
  };
  VkDescriptorPoolCreateInfo create_info = {
    /* sType         */ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    /* pNext         */ nullptr,
    /* flags         */ VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
    /* maxSets       */ _pool_size,
    /* poolSizeCount */ sizeof(pool_sizes) / sizeof(pool_sizes[0]),
    /* pPoolSizes    */ pool_sizes
  };
  VkDescriptorPool pool;
  VK_RETURN_FAIL(_vk->createDescriptorPool(_vk->device, &create_info, nullptr,
                                           &pool));
  _pools.push_back({
    /* descriptor_pool */ pool,
    /* allocated       */ 0,
    /* freed           */ 0,
    /* cleanup         */ std::vector<VkDescriptorSet>()
  });

  return Result::kSuccess;
}

Result ResourcesStore::AllocateSet(VkDescriptorSetLayout layout,
                                   VkDescriptorSet &out) {
  for(auto && pool : _pools) {
    uint32_t available = _pool_size - (pool.allocated - pool.freed);
    if(available == 0)
      continue;
    VkDescriptorSetAllocateInfo allocate_info = {
      /* sType              */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      /* pNext              */ nullptr,
      /* descriptorPool     */ pool.descriptor_pool,
      /* descriptorSetCount */ 1,
      /* pSetLayouts        */ &layout
    };
    VkResult r = _vk->allocateDescriptorSets(_vk->device, &allocate_info,
                                             &out);
    if(r == VK_SUCCESS) {
      pool.allocated += 1;
      return Result::kSuccess;
    }

    // most non success should be treated as if it is a fragmented pool
  }

  return Result::kSuccess;
}

}
}
