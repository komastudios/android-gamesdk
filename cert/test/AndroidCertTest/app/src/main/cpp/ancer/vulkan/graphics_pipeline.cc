#include "graphics_pipeline.h"

namespace ancer {
namespace vulkan {

namespace graphics_pipeline {

Builder::Builder(GraphicsPipeline &graphics_pipeline) :
  _graphics_pipeline(graphics_pipeline) {
}

Builder &Builder::DisableOptimization(bool value) {
  if(value)
    _create_info.flags |= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
  else
    _create_info.flags &= ~VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
  return *this;
}

Builder &Builder::AllowDerivatives(bool value) {
  if(value)
    _create_info.flags |= VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
  else
    _create_info.flags &= ~VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
  return *this;
}

Builder &Builder::Derivative(bool value) {
  if(value)
    _create_info.flags |= VK_PIPELINE_CREATE_DERIVATIVE_BIT;
  else
    _create_info.flags &= ~VK_PIPELINE_CREATE_DERIVATIVE_BIT;
  return *this;
}

Builder &Builder::Shader(ShaderModule &shader_module) {
  // TODO
  return *this;
}

Builder &Builder::VertexBinding(uint32_t binding, uint32_t stride,
                                VkVertexInputRate input_rate) {
  _vertex_bindings.push_back(VkVertexInputBindingDescription {
    /* binding   */ binding,
    /* stride    */ stride,
    /* inputRate */ input_rate
  });
  return *this;
}

Builder &Builder::VertexAttribute(uint32_t binding, uint32_t location,
                                  VkFormat format, uint32_t offset) {
  _vertex_attributes.push_back(VkVertexInputAttributeDescription {
    /* location */ location,
    /* binding  */ binding,
    /* format   */ format,
    /* offset   */ offset
  });
  return *this;
}

Builder &Builder::PrimitiveTopology(VkPrimitiveTopology topology) {
  InputAssemblyState().topology = topology;
  return *this;
}

Builder &Builder::PrimitiveRestart(bool value) {
  InputAssemblyState().primitiveRestartEnable = value ? VK_TRUE : VK_FALSE;
  return *this;
}

Builder &Builder::PatchControlPoints(uint32_t patch_control_points) {
  TessellationState().patchControlPoints = patch_control_points;
  return *this;
}

Builder &Builder::StaticViewport(float x, float y, float w, float h,
                                 float min_depth, float max_depth,
                                 int32_t sx, int32_t sy, uint32_t sw,
                                 uint32_t sh) {
  _viewports.push_back(VkViewport {
    /* x        */ x,
    /* y        */ y,
    /* width    */ w,
    /* height   */ h,
    /* minDepth */ min_depth,
    /* maxDepth */ max_depth
  });
  _scissors.push_back(VkRect2D {
    /* offset */ {
      /* x */ sx,
      /* y */ sy
    },
    /* extent */ {
      /* width  */ sw,
      /* height */ sh
    }
  });
  return *this;
}

Builder &Builder::DepthClamp(bool value) {
  RasterizationState().depthClampEnable = value ? VK_TRUE : VK_FALSE;
  return *this;
}

Builder &Builder::RasterizerDiscard(bool value) {
  RasterizationState().rasterizerDiscardEnable = value ? VK_TRUE : VK_FALSE;
  return *this;
}

Builder &Builder::PolygonMode(VkPolygonMode mode) {
  RasterizationState().polygonMode = mode;
  return *this;
}

Builder &Builder::CullMode(VkCullModeFlags mode) {
  RasterizationState().cullMode = mode;
  return *this;
}

Builder &Builder::FrontFace(VkFrontFace front_face) {
  RasterizationState().frontFace = front_face;
  return *this;
}

Builder &Builder::DisableDepthBias() {
  RasterizationState().depthBiasEnable = VK_FALSE;
  return *this;
}

Builder &Builder::DepthBias(float constant_factor, float clamp,
                            float slope_factor) {
  auto state = RasterizationState();
  state.depthBiasEnable = VK_TRUE;
  state.depthBiasConstantFactor = constant_factor;
  state.depthBiasClamp = clamp;
  state.depthBiasSlopeFactor = slope_factor;
  return *this;
}

Builder &Builder::StaticLineWidth(float line_width) {
  RasterizationState().lineWidth = line_width;
  return *this;
}

Builder &Builder::RasterizationSamples(VkSampleCountFlagBits samples) {
  MultisampleState().rasterizationSamples = samples;
  return *this;
}

Builder &Builder::SampleShading(bool value) {
  MultisampleState().sampleShadingEnable = value ? VK_TRUE : VK_FALSE;
  return *this;
}

Builder &Builder::MinSampleShading(float min_sample_shading) {
  MultisampleState().minSampleShading = min_sample_shading;
  return *this;
}

Builder &Builder::SampleMask(uint32_t mask) {
  _sample_mask = mask;
  MultisampleState().pSampleMask = &_sample_mask;
  return *this;
}

Builder &Builder::AlphaToCoverage(bool value) {
  MultisampleState().alphaToCoverageEnable = value ? VK_TRUE : VK_FALSE;
  return *this;
}

Builder &Builder::AlphaToOne(bool value) {
  MultisampleState().alphaToOneEnable = value ? VK_TRUE : VK_FALSE;
  return *this;
}

Builder &Builder::DepthMode(const class DepthMode & depth_mode) {
  DepthStencilState() = depth_mode.CreateInfo();
  return *this;
}

Builder &Builder::BlendMode(const class BlendMode & blend_mode) {
  ColorBlendState() = blend_mode.CreateInfo();
  return *this;
}

Builder &Builder::Layout(VkPipelineLayout pipeline_layout) {
  _create_info.layout = pipeline_layout;
  return *this;
}

Builder &Builder::RenderPass(const class RenderPass &render_pass,
                             uint32_t subpass) {
  return *this;
}

Result Builder::Build(Vulkan &vk) {
  // finalize create info?

  _graphics_pipeline._vk = vk;

  VkPipeline &pipeline = _graphics_pipeline._pipeline;

  VK_RETURN_FAIL(vk->createGraphicsPipelines(vk->device, vk->pipeline_cache,
                                             1, &_create_info, nullptr,
                                             &pipeline));

  return Result::kSuccess;
}

VkPipelineVertexInputStateCreateInfo &Builder::VertexInputState() {
  if(_create_info.pVertexInputState == nullptr)
    _create_info.pVertexInputState = &_vertex_input_state;
  return _vertex_input_state;
}

VkPipelineInputAssemblyStateCreateInfo &Builder::InputAssemblyState() {
  if(_create_info.pInputAssemblyState == nullptr)
    _create_info.pInputAssemblyState = &_input_assembly_state;
  return _input_assembly_state;
}

VkPipelineTessellationStateCreateInfo &Builder::TessellationState() {
  if(_create_info.pTessellationState == nullptr)
    _create_info.pTessellationState = &_tessellation_state;
  return _tessellation_state;
}

VkPipelineViewportStateCreateInfo &Builder::ViewportState() {
  if(_create_info.pViewportState == nullptr)
    _create_info.pViewportState = &_viewport_state;
  return _viewport_state;
}

VkPipelineRasterizationStateCreateInfo &Builder::RasterizationState() {
  if(_create_info.pRasterizationState == nullptr)
    _create_info.pRasterizationState = &_rasterization_state;
  return _rasterization_state;
}

VkPipelineMultisampleStateCreateInfo &Builder::MultisampleState() {
  if(_create_info.pMultisampleState == nullptr)
    _create_info.pMultisampleState = &_multisample_state;
  return _multisample_state;
}

VkPipelineDepthStencilStateCreateInfo &Builder::DepthStencilState() {
  if(_create_info.pDepthStencilState == nullptr)
    _create_info.pDepthStencilState = &_depth_stencil_state;
  return _depth_stencil_state;
}

VkPipelineColorBlendStateCreateInfo &Builder::ColorBlendState() {
  if(_create_info.pColorBlendState == nullptr)
    _create_info.pColorBlendState = &_color_blend_state;
  return _color_blend_state;
}

VkPipelineDynamicStateCreateInfo &Builder::DynamicState() {
  if(_create_info.pDynamicState == nullptr)
    _create_info.pDynamicState = &_dynamic_state;
  return _dynamic_state;
}

} // namespace graphics_pipeline

void GraphicsPipeline::Shutdown() {
}

graphics_pipeline::Builder GraphicsPipeline::Builder() {
  return graphics_pipeline::Builder(*this);
}

}
}
