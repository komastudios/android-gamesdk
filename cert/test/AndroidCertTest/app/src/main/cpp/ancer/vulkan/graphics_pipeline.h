#ifndef AGDC_ANCER_GRAPHICSPIPELINE_H_
#define AGDC_ANCER_GRAPHICSPIPELINE_H_

#include <cstdio>

#include "vulkan_base.h"
#include "blend_mode.h"
#include "depth_mode.h"
#include "shader_module.h"

namespace ancer {
namespace vulkan {

class GraphicsPipeline;
class RenderPass;

namespace graphics_pipeline {

// TODO(sarahburns@google): shader specialization

struct [[nodiscard]] Builder {
 public:
  Builder(GraphicsPipeline &graphics_pipeline);

  Builder &DisableOptimization(bool value = true);
  Builder &AllowDerivatives(bool value = true);
  Builder &Derivative(bool value = true);
  Builder &Shader(ShaderModule &shader_module);
  Builder &VertexBinding(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate = VK_VERTEX_INPUT_RATE_VERTEX);
  Builder &VertexAttribute(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);
  Builder &PrimitiveTopology(VkPrimitiveTopology topology);
  Builder &PrimitiveRestart(bool value = true);
  Builder &PatchControlPoints(uint32_t patch_control_points);
  Builder &StaticViewport(float x, float y, float w, float h, float min_depth,
                          float max_depth, int32_t sx, int32_t sy,
                          uint32_t sw, uint32_t sh);
  Builder &DepthClamp(bool value = true);
  Builder &RasterizerDiscard(bool value = true);
  Builder &PolygonMode(VkPolygonMode mode);
  Builder &CullMode(VkCullModeFlags mode);
  Builder &FrontFace(VkFrontFace front_face);
  Builder &DisableDepthBias();
  Builder &DepthBias(float constant_factor, float clamp, float slope_factor);
  Builder &StaticLineWidth(float line_width);
  Builder &RasterizationSamples(VkSampleCountFlagBits samples);
  Builder &SampleShading(bool value = true);
  Builder &MinSampleShading(float min_sample_shading);
  Builder &SampleMask(uint32_t mask);
  Builder &AlphaToCoverage(bool value = true);
  Builder &AlphaToOne(bool value = true);
  Builder &DepthMode(const DepthMode & depth_mode);
  Builder &BlendMode(const BlendMode & blend_mode);
  Builder &Layout(VkPipelineLayout pipeline_layout);
  Builder &RenderPass(const RenderPass &render_pass, uint32_t subpass);
  Builder &DynamicViewport(bool value = true);
  Builder &DynamicScissor(bool value = true);
  Builder &DynamicLineWidth(bool value = true);
  Builder &DynamicDepthBias(bool value = true);
  Builder &DynamicBlendConstants(bool value = true);
  Builder &DynamicDepthBounds(bool value = true);
  Builder &DynamicStencilCompareMask(bool value = true);
  Builder &DynamicStencilWriteMask(bool value = true);
  Builder &DynamicStencilReference(bool value = true);

  Result Build(Vulkan &vk);

 private:
  GraphicsPipeline &_graphics_pipeline;

  void SetDynamicState(VkDynamicState dstate, bool add);

  std::vector<VkPipelineShaderStageCreateInfo> _shader_stages;
  std::vector<VkVertexInputBindingDescription> _vertex_bindings;
  std::vector<VkVertexInputAttributeDescription> _vertex_attributes;
  VkPipelineVertexInputStateCreateInfo _vertex_input_state;
  VkPipelineInputAssemblyStateCreateInfo _input_assembly_state;
  VkPipelineTessellationStateCreateInfo _tessellation_state;
  std::vector<VkViewport> _viewports;
  std::vector<VkRect2D> _scissors;
  VkPipelineViewportStateCreateInfo _viewport_state;
  VkPipelineRasterizationStateCreateInfo _rasterization_state;
  VkSampleMask _sample_mask;
  VkPipelineMultisampleStateCreateInfo _multisample_state;
  VkPipelineDepthStencilStateCreateInfo _depth_stencil_state;
  VkPipelineColorBlendAttachmentState _blend_attachments[BlendMode::MAX_ATTACHMENTS];
  VkPipelineColorBlendStateCreateInfo _color_blend_state;
  std::vector<VkDynamicState> _dynamic_states;
  VkPipelineDynamicStateCreateInfo _dynamic_state;
  VkGraphicsPipelineCreateInfo _create_info;

  VkPipelineVertexInputStateCreateInfo &VertexInputState();
  VkPipelineInputAssemblyStateCreateInfo &InputAssemblyState();
  VkPipelineTessellationStateCreateInfo &TessellationState();
  VkPipelineViewportStateCreateInfo &ViewportState();
  VkPipelineRasterizationStateCreateInfo &RasterizationState();
  VkPipelineMultisampleStateCreateInfo &MultisampleState();
  VkPipelineDepthStencilStateCreateInfo &DepthStencilState();
  VkPipelineColorBlendStateCreateInfo &ColorBlendState();
  VkPipelineDynamicStateCreateInfo &DynamicState();
};

} // namespace graphics_pipeline

/**
 * A Vulkan Graphics Pipeline with a builder interface
 */
class GraphicsPipeline {
 public:
  void Shutdown();

  graphics_pipeline::Builder Builder();

  inline VkPipeline Handle() const {
    return _pipeline;
  }

 private:
  friend graphics_pipeline::Builder;

  Vulkan _vk;

  VkPipeline _pipeline;
};

}
}

#endif
