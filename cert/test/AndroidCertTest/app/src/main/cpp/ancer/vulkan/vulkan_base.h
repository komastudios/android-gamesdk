#ifndef AGDC_ANCER_VULKANBASE_H_
#define AGDC_ANCER_VULKANBASE_H_

#include <cstdarg>
#include <cstdint>
#include <memory>
#include <cassert>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <mutex>

#define VK_USE_PLATFORM_ANDROID_KHR 1

#if defined(__LP64__) || defined(_WIN64) || \
    (defined(__x86_64__) && !defined(__ILP32__) ) || defined(_M_X64) || \
    defined(__ia64) || defined (_M_IA64) || defined(__aarch64__) || \
    defined(__powerpc64__)
  #define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) \
      typedef struct object##_T *object;
#else
  // define a fake typedef to uint64_t for function overloading on non 32-bit
  // target platforms
  #define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) \
      typedef struct object##_T { \
        inline object##_T() : _(VK_NULL_HANDLE) { } \
        inline object##_T(uint64_t x) : _(x) { } \
        inline operator auto () const { return _; } \
       private: \
        uint64_t _; \
      } object;
#endif

#include <vulkan/vulkan.h>

namespace ancer {
namespace vulkan {

#define VK_RETURN_FAIL(x) do { \
  Result _err{x}; \
  if(!_err.is_success()) return _err; \
} while(0)

#define VK_GOTO_FAIL(x) do { \
  Result _err{x}; \
  if(!_err.is_success()) goto fail; \
} while(0)

class Context;
class GraphicsContext;
class Vulkan;

class Uploader;
class ResourcesStore;
class MemoryAllocator;

/**
 * A custom VkResult that ensures that every error is bubbled up to the a way
 * that can be handled correctly for the task at hand.
 */
struct [[nodiscard]] Result {
  Result() : vk_result(VK_SUCCESS) { }

  explicit Result(VkResult vk) : vk_result(vk) { }

  Result & operator = (VkResult vk) {
    vk_result = vk;
    return *this;
  }

  VkResult vk_result;

  inline bool is_success() { return vk_result == VK_SUCCESS; }
  inline void ok() { assert(is_success()); }

  static Result kSuccess;
};

/**
 * Requirements for initializing Vulkan. This influences picking the physical
 * device, loading layers and extensions.
 */
class VulkanRequirements {
 public:
  // 32MB
  const VkDeviceSize DEFAULT_UPLOAD_BUFFER_SIZE = 32 * 1024 * 1024;

  const uint32_t DEFAULT_CONCRRENT_UPLOADS = 32;

  VulkanRequirements();

  inline void Debug(bool value = true) {
    _debug = value;
  }

  inline void InstanceLayer(std::string layer) {
    _instance_layers.insert(std::move(layer));
  }

  inline void InstanceExtension(std::string extension) {
    _instance_extensions.insert(std::move(extension));
  }

#define FEATURE(X, Y) \
  inline void Y(bool value = true) { \
    _features.X = value ? VK_TRUE : VK_FALSE; \
  }

  FEATURE(robustBufferAccess, RobustBufferAccess)
  FEATURE(fullDrawIndexUint32, FullDrawIndexUint32)
  FEATURE(imageCubeArray, ImageCubeArray)
  FEATURE(independentBlend, IndependentBlend)
  FEATURE(geometryShader, GeometryShader)
  FEATURE(tessellationShader, TessellationShader)
  FEATURE(sampleRateShading, SampleRateShading)
  FEATURE(dualSrcBlend, DualSrcBlend)
  FEATURE(logicOp, LogicOp)
  FEATURE(multiDrawIndirect, MultiDrawIndirect)
  FEATURE(drawIndirectFirstInstance, DrawIndirectFirstInstance)
  FEATURE(depthClamp, DepthClamp)
  FEATURE(depthBiasClamp, DepthBiasClamp)
  FEATURE(fillModeNonSolid, FillModeNonSolid)
  FEATURE(depthBounds, DepthBounds)
  FEATURE(wideLines, WideLines)
  FEATURE(largePoints, LargePoints)
  FEATURE(alphaToOne, AlphaToOne)
  FEATURE(multiViewport, MultiViewport)
  FEATURE(samplerAnisotropy, SamplerAnisotropy)
  FEATURE(textureCompressionETC2, TextureCompressionETC2)
  FEATURE(textureCompressionASTC_LDR, TextureCompressionASTC_LDR)
  FEATURE(textureCompressionBC, TextureCompressionBC)
  FEATURE(occlusionQueryPrecise, OcclusionQueryPrecise)
  FEATURE(pipelineStatisticsQuery, PipelineStatisticsQuery)
  FEATURE(vertexPipelineStoresAndAtomics, VertexPipelineStoresAndAtomics)
  FEATURE(fragmentStoresAndAtomics, FragmentStoresAndAtomics)
  FEATURE(shaderTessellationAndGeometryPointSize, ShaderTessellationAndGeometryPointSize)
  FEATURE(shaderImageGatherExtended, ShaderImageGatherExtended)
  FEATURE(shaderStorageImageExtendedFormats, ShaderStorageImageExtendedFormats)
  FEATURE(shaderStorageImageMultisample, ShaderStorageImageMultisample)
  FEATURE(shaderStorageImageReadWithoutFormat, ShaderStorageImageReadWithoutFormat)
  FEATURE(shaderStorageImageWriteWithoutFormat, ShaderStorageImageWriteWithoutFormat)
  FEATURE(shaderUniformBufferArrayDynamicIndexing, ShaderUniformBufferArrayDynamicIndexing)
  FEATURE(shaderSampledImageArrayDynamicIndexing, ShaderSampledImageArrayDynamicIndexing)
  FEATURE(shaderStorageBufferArrayDynamicIndexing, ShaderStorageBufferArrayDynamicIndexing)
  FEATURE(shaderStorageImageArrayDynamicIndexing, ShaderStorageImageArrayDynamicIndexing)
  FEATURE(shaderClipDistance, ShaderClipDistance)
  FEATURE(shaderCullDistance, ShaderCullDistance)
  FEATURE(shaderFloat64, ShaderFloat64)
  FEATURE(shaderInt64, ShaderInt64)
  FEATURE(shaderInt16, ShaderInt16)
  FEATURE(shaderResourceResidency, ShaderResourceResidency)
  FEATURE(shaderResourceMinLod, ShaderResourceMinLod)
  FEATURE(sparseBinding, SparseBinding)
  FEATURE(sparseResidencyBuffer, SparseResidencyBuffer)
  FEATURE(sparseResidencyImage2D, SparseResidencyImage2D)
  FEATURE(sparseResidencyImage3D, SparseResidencyImage3D)
  FEATURE(sparseResidency2Samples, SparseResidency2Samples)
  FEATURE(sparseResidency4Samples, SparseResidency4Samples)
  FEATURE(sparseResidency8Samples, SparseResidency8Samples)
  FEATURE(sparseResidency16Samples, SparseResidency16Samples)
  FEATURE(sparseResidencyAliased, SparseResidencyAliased)
  FEATURE(variableMultisampleRate, VariableMultisampleRate)
  FEATURE(inheritedQueries, InheritedQueries)

#undef FEATURE

  inline void DeviceLayer(std::string layer) {
    _device_layers.insert(std::move(layer));
  }

  inline void DeviceExtension(std::string extension) {
    _device_extensions.insert(std::move(extension));
  }

  /**
   * Sets common layers and extensions used to create a Swapchain
   */
  void Swapchain();

  inline void UploadBufferSize(VkDeviceSize size) {
    _upload_buffer_size = size;
  }

  inline void ConcurrentUploads(uint32_t num) {
    _concurrent_uploads = num;
  }

 private:
  friend class Vulkan;

  bool _debug;

  std::unordered_set<std::string> _instance_layers;
  std::unordered_set<std::string> _instance_extensions;

  VkPhysicalDeviceFeatures _features;

  std::unordered_set<std::string> _device_layers;
  std::unordered_set<std::string> _device_extensions;

  VkDeviceSize _upload_buffer_size;
  uint32_t _concurrent_uploads;
};

/**
 * A wrapper over the VkFence object. It is reference counted where a command
 * buffer is the holder of these references. When a command buffer is submitted,
 * it is done with a Fence. Several command buffers can use the same fence. When
 * a command buffer would be reused, first it waits for it's associated fence.
 * thus letting all command buffers using the same fence that it has already
 * been waited for on the CPU side.
 *
 * Fences also control lazy resource deallocation/destruction. The root Vulkan
 * object knows what 'generation' a resource should be destroyed on and when a
 * Fence is successfully waited on it notifies the Vulkan object to advance the
 * 'generation'.
 */
struct Fence {
 public:
  void AdvanceFrame(bool value = true) {
    data->advance_frame = value;
  }

 private:
  friend class Vulkan;

  struct Data {
    bool waited;
    bool advance_frame;
    uint32_t frame;
    uint32_t references;
    VkFence fence;
  };

  std::shared_ptr<Data> data;
};

/**
 * a simple enum for Queues we create
 */
enum Queue {
  Q_GRAPHICS,
  Q_COMPUTE,
  Q_TRANSFER,
  Q_PRESENT,
  Q_COUNT
};

/**
 * Simplify required memory properties to these concepts
 */
enum class ResourceUse {
  GPU,
  TransientGPU,
  CPUToGPU,
  GPUToCPU
};

/**
 * A memory allocation from our allocator
 *
 * each allocation is a block of memory from a larger vulkan memory object. Due
 * to alignments, we store the start of a block and the offset in that block to
 * the usable memory.
 */
class MemoryAllocation {
 public:
  inline const VkDeviceMemory &Memory() const {
    return _memory;
  }

  inline VkDeviceSize Offset() const {
    return _offset;
  }

  inline VkDeviceSize TotalSize() const {
    return _end - _start;
  }

  inline VkDeviceSize Size() const {
    return _end - _offset;
  }

  inline void *Map() const {
    return _map;
  }

 private:
  friend class Vulkan;

  VkDeviceMemory _memory;
  void * _map;
  VkDeviceSize _start;
  VkDeviceSize _offset;
  VkDeviceSize _end;
};

/**
 * The Vulkan object is primarily just a shared_ptr to Vulkan::Data. The Data
 * is all the data that is shared. Functions in Vulkan are for managing the
 * Data. The Vulkan::Data also keeps the function pointers into Vulkan and it's
 * extensions.
 *
 * Every object is tracked for destruction, Fences control the timing of
 * deallocating/destroying objects.
 *
 * Below are special considerations currently in place or planned for in the
 * future:
 *
 * Fence. These are allocated and a free list is used to minimize allocations.
 *
 * Queue. Created and managed by the Vulkan::Data. Nothing should touch a Queue
 * besides the logic in Vulkan.
 *
 * RenderPass. Some information about a VkRenderPass and it's sub passes are
 * stored here for the creation of `GraphicsPipeline`s
 *
 * PipelineCache. A cache for pipeline creation.
 */
class Vulkan {
 public:
  struct Data;

  Result Initialize(const VulkanRequirements & requirements);
  void Shutdown();

  inline VkInstance Intance() const {
    return vk->instance;
  }

  inline VkPhysicalDevice PhysicalDevice() const {
    return vk->physical_device;
  }

  /**
   * Return the Data, prefer using `operator ->`
   */
  inline Data & get() {
    return *vk.get();
  }

  /**
   * Since Vulkan is mostly a shared_ptr<Data>, act like one a little bit.
   */
  inline Data * operator -> () {
    return vk.get();
  }

  inline Uploader &GetUploader() {
    return *vk->uploader;
  }

  inline ResourcesStore &GetResourcesStore() {
    return *vk->resources_store;
  }

#define DEBUG_NAME(ENUM, TYPE) \
  /** \
   * Add a name to this object for debugging \
   */ \
  inline Result DebugName(TYPE object, const char * format, ...) { \
    va_list ap; \
    va_start(ap, format); \
    Result res; \
    res = DebugName(VK_DEBUG_REPORT_OBJECT_TYPE_ ## ENUM ## _EXT, \
                    uint64_t(object), format, ap); \
    va_end(ap); \
    return res; \
  }

  //DEBUG_NAME(INSTANCE, VkInstance)
  //DEBUG_NAME(PHYSICAL_DEVICE, VkPhysicalDevice)
  //DEBUG_NAME(DEVICE, VkDevice)
  //DEBUG_NAME(QUEUE, VkQueue)
  //DEBUG_NAME(SEMAPHORE, VkSemaphore)
  DEBUG_NAME(COMMAND_BUFFER, VkCommandBuffer)
  //DEBUG_NAME(FENCE, VkFence)
  //DEBUG_NAME(DEVICE_MEMORY, VkDeviceMemory)
  //DEBUG_NAME(BUFFER, VkBuffer)
  //DEBUG_NAME(IMAGE, VkImage)
  //DEBUG_NAME(EVENT, VkEvent)
  //DEBUG_NAME(QUERY_POOL, VkQueryPool)
  //DEBUG_NAME(BUFFER_VIEW, VkBufferView)
  //DEBUG_NAME(IMAGE_VIEW, VkImageView)
  //DEBUG_NAME(SHADER_MODULE, VkShaderModule)
  //DEBUG_NAME(PIPELINE_CACHE, VkPipelineCache)
  //DEBUG_NAME(PIPELINE_LAYOUT, VkPipelineLayout)
  //DEBUG_NAME(RENDER_PASS, VkRenderPass)
  //DEBUG_NAME(PIPELINE, VkPipeline)
  //DEBUG_NAME(DESCRIPTOR_SET_LAYOUT, VkDescriptorSetLayout)
  //DEBUG_NAME(SAMPLER, VkSampler)
  //DEBUG_NAME(DESCRIPTOR_POOL, VkDescriptorPool)
  //DEBUG_NAME(DESCRIPTOR_SET, VkDescriptorSet)
  //DEBUG_NAME(FRAMEBUFFER, VkFramebuffer)
  DEBUG_NAME(COMMAND_POOL, VkCommandPool)
  //DEBUG_NAME(SURFACE_KHR, VkSurfaceKHR)
  //DEBUG_NAME(SWAPCHAIN_KHR, VkSwapchainKHR)
  //DEBUG_NAME(DEBUG_REPORT_CALLBACK_EXT, VkDebugReportCallbackEXT)
  //DEBUG_NAME(DISPLAY_KHR, VkDisplayKHR)
  //DEBUG_NAME(DISPLAY_MODE_KHR, VkDisplayModeKHR)
  //DEBUG_NAME(OBJECT_TABLE_NVX, VkObjectTableNVX)
  //DEBUG_NAME(INDIRECT_COMMANDS_LAYOUT_NVX, VkIndirectCommandsLayoutNVX)
  //DEBUG_NAME(VALIDATION_CACHE_EXT, VkValidationCacheEXT)
  //DEBUG_NAME(SAMPLER_YCBCR_CONVERSION, VkSamplerYcbcrConversionKHR)
  //DEBUG_NAME(DESCRIPTOR_UPDATE_TEMPLATE, VkDescriptorUpdateTemplate)
  //DEBUG_NAME(ACCELERATION_STRUCTURE_NV, VkAccelerationStructureNV)
  //DEBUG_NAME(DEBUG_REPORT, VkDebugReportEXT)
  //DEBUG_NAME(VALIDATION_CACHE, VkValidationCache)
  //DEBUG_NAME(DESCRIPTOR_UPDATE_TEMPLATE_KHR, VkDescriptorUpdateTemplateKHR)
  //DEBUG_NAME(SAMPLER_YCBCR_CONVERSION_KHR, VkSamplerYcbcrConversionKHR)

#undef DEBUG_NAME

#define DESTROY(ENUM, TYPE) \
  /** \
   * Add this object to the to-be-destroy list \
   */ \
  inline void Destroy(TYPE object, bool now = false) { \
    DestroyEntry destroy = { \
      /* object_type */ VK_DEBUG_REPORT_OBJECT_TYPE_ ## ENUM ## _EXT, \
      /* object      */ uint64_t(object), \
      /* frame       */ vk->frame \
    }; \
    if(now) DoDestroy(destroy); \
    else AddDestroy(destroy); \
  }

  //DESTROY(INSTANCE, VkInstance)
  //DESTROY(DEVICE, VkDevice)
  //DESTROY(QUEUE, VkQueue)
  //DESTROY(SEMAPHORE, VkSemaphore)
  //DESTROY(COMMAND_BUFFER, VkCommandBuffer)
  //DESTROY(FENCE, VkFence)
  //DESTROY(DEVICE_MEMORY, VkDeviceMemory)
  DESTROY(BUFFER, VkBuffer)
  DESTROY(IMAGE, VkImage)
  //DESTROY(EVENT, VkEvent)
  //DESTROY(QUERY_POOL, VkQueryPool)
  DESTROY(BUFFER_VIEW, VkBufferView)
  DESTROY(IMAGE_VIEW, VkImageView)
  DESTROY(SHADER_MODULE, VkShaderModule)
  //DESTROY(PIPELINE_CACHE, VkPipelineCache)
  //DESTROY(PIPELINE_LAYOUT, VkPipelineLayout)
  //DESTROY(RENDER_PASS, VkRenderPass)
  //DESTROY(PIPELINE, VkPipeline)
  //DESTROY(DESCRIPTOR_SET_LAYOUT, VkDescriptorSetLayout)
  //DESTROY(SAMPLER, VkSampler)
  //DESTROY(DESCRIPTOR_POOL, VkDescriptorPool)
  //DESTROY(DESCRIPTOR_SET, VkDescriptorSet)
  //DESTROY(FRAMEBUFFER, VkFramebuffer)
  //DESTROY(COMMAND_POOL, VkCommandPool)
  //DESTROY(SURFACE_KHR, VkSurfaceKHR)
  //DESTROY(SWAPCHAIN_KHR, VkSwapchainKHR)
  //DESTROY(DEBUG_REPORT_CALLBACK_EXT, VkDebugReportCallbackEXT)
  //DESTROY(DISPLAY_KHR, VkDisplayKHR)
  //DESTROY(DISPLAY_MODE_KHR, VkDisplayModeKHR)
  //DESTROY(OBJECT_TABLE_NVX, VkObjectTableNVX)
  //DESTROY(INDIRECT_COMMANDS_LAYOUT_NVX, VkIndirectCommandsLayoutNVX)
  //DESTROY(VALIDATION_CACHE_EXT, VkValidationCacheEXT)
  //DESTROY(SAMPLER_YCBCR_CONVERSION, VkSamplerYcbcrConversionKHR)
  //DESTROY(DESCRIPTOR_UPDATE_TEMPLATE, VkDescriptorUpdateTemplate)
  //DESTROY(ACCELERATION_STRUCTURE_NV, VkAccelerationStructureNV)
  //DESTROY(DEBUG_REPORT, VkDebugReportEXT)
  //DESTROY(VALIDATION_CACHE, VkValidationCache)
  //DESTROY(DESCRIPTOR_UPDATE_TEMPLATE_KHR, VkDescriptorUpdateTemplateKHR)
  //DESTROY(SAMPLER_YCBCR_CONVERSION_KHR, VkSamplerYcbcrConversionKHR)

#undef DESTROY

  /**
   * Did we initialize with this instance extension, extension names can be
   * found in vulkan.h
   */
  inline bool HaveInstanceExtension(const char * name) {
    return vk->enabled_instance_extensions.count(name) > 0;
  }

  /**
   * Did we initialize with this device extension, extension names can be found
   * in vulkan.h
   */
  inline bool HaveDeviceExtension(const char * name) {
    return vk->enabled_instance_extensions.count(name) > 0;
  }

  /**
   * Allocate a Fence to be used with SubmitToQueue. The same fence can be used
   * multiple times in a frame.
   */
  Result AllocateFence(Fence &fence);

  /**
   * Wait for a Fence to complete. if force is false this becomes a query for
   * the Fence state and does cleanup after if it has been set.
   */
  Result WaitForFence(Fence &fence, bool &completed, bool force = true);

  /**
   * Submit a GraphicsContext to the graphics VkQueue. This is the best way to
   * interact with the graphics VkQueue.
   */
  inline Result SubmitToQueue(GraphicsContext &context, Fence &fence) {
    return SubmitToQueue(Q_GRAPHICS, reinterpret_cast<Context&>(context),
                         fence);
  }

  /**
   * Submit a Context to a VkQueue. Generic form of SubmitToQueue
   */
  Result SubmitToQueue(Queue queue, Context &context, Fence &fence);

  // ==========================================================================
  Result AllocateMemory(VkMemoryRequirements &requirements,
                        VkMemoryPropertyFlags flags,
                        MemoryAllocation &allocation);

  void Free(const MemoryAllocation &allocation);

  // ==========================================================================
  Result GetFramebuffer(VkRenderPass render_pass,
                        uint32_t width, uint32_t height, uint32_t layers,
                        std::initializer_list<VkImageView> image_views,
                        VkFramebuffer &framebuffer);

  // ==========================================================================
  Result AcquireTemporaryCommandBuffer(Queue queue,
                                       VkCommandBuffer &cmd_buffer);

  Result ReleaseTemporaryCommandBuffer(VkCommandBuffer cmd_buffer);

  Result QueueTemporaryCommandBuffer(VkCommandBuffer cmd_buffer, Fence &fence);

  Result SubmitTemporaryCommandBuffers(Queue queue, VkSemaphore &semaphore);

  // ==========================================================================
  Result CreateDescriptorSetLayout(
                  std::initializer_list<VkDescriptorSetLayoutBinding> bindings,
                  VkDescriptorSetLayout &layout);

  Result GetDescriptorSet(VkDescriptorSetLayout layout,
                          VkDescriptorSet &descriptor_set);

  inline Result GetDescriptorSet(
                  std::initializer_list<VkDescriptorSetLayoutBinding> bindings,
                  VkDescriptorSet &descriptor_set) {
    VkDescriptorSetLayout layout;
    VK_RETURN_FAIL(CreateDescriptorSetLayout(bindings, layout));
    return GetDescriptorSet(layout, descriptor_set);
  }

  // ==========================================================================

 public:
  struct DestroyEntry {
    VkDebugReportObjectTypeEXT object_type;
    uint64_t object;
    uint32_t frame;
  };

  struct Framebuffer {
    uint64_t h1, h2;
    uint32_t frame;
    VkFramebuffer framebuffer;
  };

  struct TemporaryCommandBuffer {
    enum State {
      Initial,
      Recording,
      Acquired,
      InFlight
    };

    State state;
    VkCommandPool pool;
    VkCommandBuffer buffer;
    Fence fence;
  };

  struct ThreadData {
    VkCommandPool temporary_command_pool[Q_COUNT];
    std::vector<TemporaryCommandBuffer> temporary_command_buffers;
  };

  struct QueueData {
    QueueData() : mutex(), family_index(0), queue(VK_NULL_HANDLE) { }
    ~QueueData() { }

    std::mutex mutex;
    uint32_t family_index;
    VkQueue queue;
  };

  struct Queues {
    Queues() : graphics(), compute(), transfer(), present() { }
    ~Queues() { }

    union {
      struct {
        QueueData graphics;
        QueueData compute;
        QueueData transfer;
        QueueData present;
      };
      QueueData queue[Q_COUNT];
    };
  };

  struct Data : public Queues {
    std::unordered_set<std::string> available_instance_layers;
    std::unordered_set<std::string> available_instance_extensions;
    std::unordered_set<std::string> enabled_instance_layers;
    std::unordered_set<std::string> enabled_instance_extensions;

    VkInstance instance;

    bool debug_enabled;

    VkPhysicalDevice physical_device;
    VkPhysicalDeviceProperties physical_device_properties;
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    VkPhysicalDeviceFeatures physical_device_features;

    uint32_t window_width;
    uint32_t window_height;

    VkSurfaceKHR surface;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    std::vector<VkPresentModeKHR> surface_present_modes;
    std::vector<VkSurfaceFormatKHR> surface_formats;

    std::unordered_set<std::string> available_device_layers;
    std::unordered_set<std::string> available_device_extensions;
    std::unordered_set<std::string> enabled_device_layers;
    std::unordered_set<std::string> enabled_device_extensions;

    VkFormat depth_format;
    VkFormat depth_stencil_format;

    VkDevice device;

    uint32_t frame;

    std::mutex free_fences_mutex;
    std::vector<Fence> free_fences;

    std::mutex destroy_mutex;
    std::vector<DestroyEntry> destroy;

    VkPipelineCache pipeline_cache;

    // framebuffers
    std::mutex framebuffer_mutex;
    std::unordered_map<uint64_t, Framebuffer> framebuffers;

    // temporary command buffers
    std::mutex thread_data_mutex;
    std::unordered_map<std::thread::id, std::unique_ptr<ThreadData>> thread_data;

    // upload system
    Uploader *uploader;

    // resources
    ResourcesStore *resources_store;

    // 1.0 core functions
    PFN_vkCreateInstance createInstance;
    PFN_vkDestroyInstance destroyInstance;
    PFN_vkEnumeratePhysicalDevices enumeratePhysicalDevices;
    PFN_vkGetPhysicalDeviceFeatures getPhysicalDeviceFeatures;
    PFN_vkGetPhysicalDeviceFormatProperties getPhysicalDeviceFormatProperties;
    PFN_vkGetPhysicalDeviceImageFormatProperties getPhysicalDeviceImageFormatProperties;
    PFN_vkGetPhysicalDeviceProperties getPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties getPhysicalDeviceQueueFamilyProperties;
    PFN_vkGetPhysicalDeviceMemoryProperties getPhysicalDeviceMemoryProperties;
    PFN_vkGetInstanceProcAddr getInstanceProcAddr;
    PFN_vkGetDeviceProcAddr getDeviceProcAddr;
    PFN_vkCreateDevice createDevice;
    PFN_vkDestroyDevice destroyDevice;
    PFN_vkEnumerateInstanceExtensionProperties enumerateInstanceExtensionProperties;
    PFN_vkEnumerateDeviceExtensionProperties enumerateDeviceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties enumerateInstanceLayerProperties;
    PFN_vkEnumerateDeviceLayerProperties enumerateDeviceLayerProperties;
    PFN_vkGetDeviceQueue getDeviceQueue;
    PFN_vkQueueSubmit queueSubmit;
    PFN_vkQueueWaitIdle queueWaitIdle;
    PFN_vkDeviceWaitIdle deviceWaitIdle;
    PFN_vkAllocateMemory allocateMemory;
    PFN_vkFreeMemory freeMemory;
    PFN_vkMapMemory mapMemory;
    PFN_vkUnmapMemory unmapMemory;
    PFN_vkFlushMappedMemoryRanges flushMappedMemoryRanges;
    PFN_vkInvalidateMappedMemoryRanges invalidateMappedMemoryRanges;
    PFN_vkGetDeviceMemoryCommitment getDeviceMemoryCommitment;
    PFN_vkBindBufferMemory bindBufferMemory;
    PFN_vkBindImageMemory bindImageMemory;
    PFN_vkGetBufferMemoryRequirements getBufferMemoryRequirements;
    PFN_vkGetImageMemoryRequirements getImageMemoryRequirements;
    PFN_vkGetImageSparseMemoryRequirements getImageSparseMemoryRequirements;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties getPhysicalDeviceSparseImageFormatProperties;
    PFN_vkQueueBindSparse queueBindSparse;
    PFN_vkCreateFence createFence;
    PFN_vkDestroyFence destroyFence;
    PFN_vkResetFences resetFences;
    PFN_vkGetFenceStatus getFenceStatus;
    PFN_vkWaitForFences waitForFences;
    PFN_vkCreateSemaphore createSemaphore;
    PFN_vkDestroySemaphore destroySemaphore;
    PFN_vkCreateEvent createEvent;
    PFN_vkDestroyEvent destroyEvent;
    PFN_vkGetEventStatus getEventStatus;
    PFN_vkSetEvent setEvent;
    PFN_vkResetEvent resetEvent;
    PFN_vkCreateQueryPool createQueryPool;
    PFN_vkDestroyQueryPool destroyQueryPool;
    PFN_vkGetQueryPoolResults getQueryPoolResults;
    PFN_vkCreateBuffer createBuffer;
    PFN_vkDestroyBuffer destroyBuffer;
    PFN_vkCreateBufferView createBufferView;
    PFN_vkDestroyBufferView destroyBufferView;
    PFN_vkCreateImage createImage;
    PFN_vkDestroyImage destroyImage;
    PFN_vkGetImageSubresourceLayout getImageSubresourceLayout;
    PFN_vkCreateImageView createImageView;
    PFN_vkDestroyImageView destroyImageView;
    PFN_vkCreateShaderModule createShaderModule;
    PFN_vkDestroyShaderModule destroyShaderModule;
    PFN_vkCreatePipelineCache createPipelineCache;
    PFN_vkDestroyPipelineCache destroyPipelineCache;
    PFN_vkGetPipelineCacheData getPipelineCacheData;
    PFN_vkMergePipelineCaches mergePipelineCaches;
    PFN_vkCreateGraphicsPipelines createGraphicsPipelines;
    PFN_vkCreateComputePipelines createComputePipelines;
    PFN_vkDestroyPipeline destroyPipeline;
    PFN_vkCreatePipelineLayout createPipelineLayout;
    PFN_vkDestroyPipelineLayout destroyPipelineLayout;
    PFN_vkCreateSampler createSampler;
    PFN_vkDestroySampler destroySampler;
    PFN_vkCreateDescriptorSetLayout createDescriptorSetLayout;
    PFN_vkDestroyDescriptorSetLayout destroyDescriptorSetLayout;
    PFN_vkCreateDescriptorPool createDescriptorPool;
    PFN_vkDestroyDescriptorPool destroyDescriptorPool;
    PFN_vkResetDescriptorPool resetDescriptorPool;
    PFN_vkAllocateDescriptorSets allocateDescriptorSets;
    PFN_vkFreeDescriptorSets freeDescriptorSets;
    PFN_vkUpdateDescriptorSets updateDescriptorSets;
    PFN_vkCreateFramebuffer createFramebuffer;
    PFN_vkDestroyFramebuffer destroyFramebuffer;
    PFN_vkCreateRenderPass createRenderPass;
    PFN_vkDestroyRenderPass destroyRenderPass;
    PFN_vkGetRenderAreaGranularity getRenderAreaGranularity;
    PFN_vkCreateCommandPool createCommandPool;
    PFN_vkDestroyCommandPool destroyCommandPool;
    PFN_vkResetCommandPool resetCommandPool;
    PFN_vkAllocateCommandBuffers allocateCommandBuffers;
    PFN_vkFreeCommandBuffers freeCommandBuffers;
    PFN_vkBeginCommandBuffer beginCommandBuffer;
    PFN_vkEndCommandBuffer endCommandBuffer;
    PFN_vkResetCommandBuffer resetCommandBuffer;
    PFN_vkCmdBindPipeline cmdBindPipeline;
    PFN_vkCmdSetViewport cmdSetViewport;
    PFN_vkCmdSetScissor cmdSetScissor;
    PFN_vkCmdSetLineWidth cmdSetLineWidth;
    PFN_vkCmdSetDepthBias cmdSetDepthBias;
    PFN_vkCmdSetBlendConstants cmdSetBlendConstants;
    PFN_vkCmdSetDepthBounds cmdSetDepthBounds;
    PFN_vkCmdSetStencilCompareMask cmdSetStencilCompareMask;
    PFN_vkCmdSetStencilWriteMask cmdSetStencilWriteMask;
    PFN_vkCmdSetStencilReference cmdSetStencilReference;
    PFN_vkCmdBindDescriptorSets cmdBindDescriptorSets;
    PFN_vkCmdBindIndexBuffer cmdBindIndexBuffer;
    PFN_vkCmdBindVertexBuffers cmdBindVertexBuffers;
    PFN_vkCmdDraw cmdDraw;
    PFN_vkCmdDrawIndexed cmdDrawIndexed;
    PFN_vkCmdDrawIndirect cmdDrawIndirect;
    PFN_vkCmdDrawIndexedIndirect cmdDrawIndexedIndirect;
    PFN_vkCmdDispatch cmdDispatch;
    PFN_vkCmdDispatchIndirect cmdDispatchIndirect;
    PFN_vkCmdCopyBuffer cmdCopyBuffer;
    PFN_vkCmdCopyImage cmdCopyImage;
    PFN_vkCmdBlitImage cmdBlitImage;
    PFN_vkCmdCopyBufferToImage cmdCopyBufferToImage;
    PFN_vkCmdCopyImageToBuffer cmdCopyImageToBuffer;
    PFN_vkCmdUpdateBuffer cmdUpdateBuffer;
    PFN_vkCmdFillBuffer cmdFillBuffer;
    PFN_vkCmdClearColorImage cmdClearColorImage;
    PFN_vkCmdClearDepthStencilImage cmdClearDepthStencilImage;
    PFN_vkCmdClearAttachments cmdClearAttachments;
    PFN_vkCmdResolveImage cmdResolveImage;
    PFN_vkCmdSetEvent cmdSetEvent;
    PFN_vkCmdResetEvent cmdResetEvent;
    PFN_vkCmdWaitEvents cmdWaitEvents;
    PFN_vkCmdPipelineBarrier cmdPipelineBarrier;
    PFN_vkCmdBeginQuery cmdBeginQuery;
    PFN_vkCmdEndQuery cmdEndQuery;
    PFN_vkCmdResetQueryPool cmdResetQueryPool;
    PFN_vkCmdWriteTimestamp cmdWriteTimestamp;
    PFN_vkCmdCopyQueryPoolResults cmdCopyQueryPoolResults;
    PFN_vkCmdPushConstants cmdPushConstants;
    PFN_vkCmdBeginRenderPass cmdBeginRenderPass;
    PFN_vkCmdNextSubpass cmdNextSubpass;
    PFN_vkCmdEndRenderPass cmdEndRenderPass;
    PFN_vkCmdExecuteCommands cmdExecuteCommands;

    PFN_vkDestroySurfaceKHR destroySurfaceKHR;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR getPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR getPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR getPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR getPhysicalDeviceSurfacePresentModesKHR;

#ifdef VK_USE_PLATFORM_ANDROID_KHR
    PFN_vkCreateAndroidSurfaceKHR createAndroidSurfaceKHR;
#endif

    PFN_vkCreateSwapchainKHR createSwapchainKHR;
    PFN_vkDestroySwapchainKHR destroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR getSwapchainImagesKHR;
    PFN_vkAcquireNextImageKHR acquireNextImageKHR;
    PFN_vkQueuePresentKHR queuePresentKHR;
    PFN_vkGetDeviceGroupPresentCapabilitiesKHR getDeviceGroupPresentCapabilitiesKHR;
    PFN_vkGetDeviceGroupSurfacePresentModesKHR getDeviceGroupSurfacePresentModesKHR;
    PFN_vkGetPhysicalDevicePresentRectanglesKHR getPhysicalDevicePresentRectanglesKHR;
    PFN_vkAcquireNextImage2KHR acquireNextImage2KHR;
  };

  std::shared_ptr<Data> vk{nullptr};

 private:
  static const uint32_t FRAMEBUFFER_SAVED_FRAMES = 8;

  Result InitEntry(const VulkanRequirements & requirements);
  Result InitInstanceExtensions(const VulkanRequirements & requirements);
  Result InitInstance(const VulkanRequirements & requirements);
  Result InitDebugReporting(const VulkanRequirements & requirements);
  Result InitPhysicalDevice(const VulkanRequirements & requirements);
  Result InitSurface(const VulkanRequirements & requirements);
  Result InitDeviceExtensions(const VulkanRequirements & requirements);
  Result InitDevice(const VulkanRequirements & requirements);

  Result DebugName(VkDebugReportObjectTypeEXT object_type, uint64_t object,
                   const char * format, va_list args);

  ThreadData *GetThreadData();

  void AddDestroy(DestroyEntry & destroy);
  void DoDestroy(DestroyEntry & destroy);

  void AdvanceFrame(uint32_t frame);
};

};
};

#endif // AGDC_ANCER_VULKANBASE_H_
