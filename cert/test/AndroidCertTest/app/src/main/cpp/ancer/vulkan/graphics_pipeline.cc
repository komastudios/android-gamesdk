#include "graphics_pipeline.h"

#include "render_pass.h"

namespace ancer {
namespace vulkan {

namespace graphics_pipeline {

Builder::Builder(GraphicsPipeline &graphics_pipeline) :
    _graphics_pipeline(graphics_pipeline) {

#define CLEAR(S, T) do { \
  memset(&S, 0, sizeof(S)); \
  S.sType = VK_STRUCTURE_TYPE_ ## T; \
} while(false)

  CLEAR(_vertex_input_state, PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO);
  CLEAR(_input_assembly_state, PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO);
  CLEAR(_tessellation_state, PIPELINE_TESSELLATION_STATE_CREATE_INFO);
  CLEAR(_viewport_state, PIPELINE_VIEWPORT_STATE_CREATE_INFO);
  CLEAR(_rasterization_state, PIPELINE_RASTERIZATION_STATE_CREATE_INFO);
  CLEAR(_multisample_state, PIPELINE_MULTISAMPLE_STATE_CREATE_INFO);
  CLEAR(_depth_stencil_state, PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO);
  CLEAR(_color_blend_state, PIPELINE_COLOR_BLEND_STATE_CREATE_INFO);
  CLEAR(_dynamic_state, PIPELINE_DYNAMIC_STATE_CREATE_INFO);
  CLEAR(_create_info, GRAPHICS_PIPELINE_CREATE_INFO);

#undef CLEAR

  RasterizationState().lineWidth = 1.0f;
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
  VkShaderStageFlagBits stage = shader_module.Stage();

  // TODO(sarahburns@google.com) only allow some states to be created from
  // here?

  // touch states required
  switch(stage) {
  case VK_SHADER_STAGE_VERTEX_BIT:
    VertexInputState();
    InputAssemblyState();
    break;
  case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
    /* fallthrough */
  case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
    TessellationState();
    break;
  case VK_SHADER_STAGE_FRAGMENT_BIT:
    /* nothing */
    break;
  case VK_SHADER_STAGE_COMPUTE_BIT:
    // cannot build a compute shader into a graphics pipeline
    assert(false);
  default:
    // unknown shader stage being built into a graphics pipeline
    assert(false);
  }

  _shader_stages.push_back(VkPipelineShaderStageCreateInfo {
    /* sType               */
                           VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    /* pNext               */ nullptr,
    /* flags               */ 0,
    /* stage               */ stage,
    /* module              */ shader_module.Handle(),
    /* pName               */ shader_module.Name(),
    /* pSpecializationInfo */ shader_module.SpecializationInfo()
  });
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
  _create_info.renderPass = render_pass.Handle();
  _create_info.subpass = subpass;
  return *this;
}

Builder &Builder::DynamicViewport(bool value) {
  SetDynamicState(VK_DYNAMIC_STATE_VIEWPORT, value);
  return *this;
}

Builder &Builder::DynamicScissor(bool value) {
  SetDynamicState(VK_DYNAMIC_STATE_SCISSOR, value);
  return *this;
}

Builder &Builder::DynamicLineWidth(bool value) {
  SetDynamicState(VK_DYNAMIC_STATE_LINE_WIDTH, value);
  return *this;
}

Builder &Builder::DynamicDepthBias(bool value) {
  SetDynamicState(VK_DYNAMIC_STATE_DEPTH_BIAS, value);
  return *this;
}

Builder &Builder::DynamicBlendConstants(bool value) {
  SetDynamicState(VK_DYNAMIC_STATE_BLEND_CONSTANTS, value);
  return *this;
}

Builder &Builder::DynamicDepthBounds(bool value) {
  SetDynamicState(VK_DYNAMIC_STATE_DEPTH_BOUNDS, value);
  return *this;
}

Builder &Builder::DynamicStencilCompareMask(bool value) {
  SetDynamicState(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK, value);
  return *this;
}

Builder &Builder::DynamicStencilWriteMask(bool value) {
  SetDynamicState(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK, value);
  return *this;
}

Builder &Builder::DynamicStencilReference(bool value) {
  SetDynamicState(VK_DYNAMIC_STATE_STENCIL_REFERENCE, value);
  return *this;
}

void Builder::SetDynamicState(VkDynamicState dstate, bool add) {
  std::size_t i = 0;
  std::size_t e = _dynamic_states.size();
  while(i < e) {
    if(_dynamic_states[i] == dstate)
      break;
    ++i;
  }
  if(i >= e && add)
    _dynamic_states.push_back(dstate);
  else if(i != e && !add) {
    std::iter_swap(_dynamic_states.begin() + i, _dynamic_states.end() - 1);
    _dynamic_states.pop_back();
  }
}

Result Builder::Build(Vulkan &vk) {
  _create_info.stageCount = static_cast<uint32_t>(_shader_stages.size());
  _create_info.pStages = _shader_stages.data();

  if(_vertex_bindings.size() > 0) {
    auto & vistate = VertexInputState();
    vistate.vertexBindingDescriptionCount =
                                static_cast<uint32_t>(_vertex_bindings.size());
    vistate.pVertexBindingDescriptions = _vertex_bindings.data();
  }

  if(_vertex_attributes.size() > 0) {
    auto & vistate = VertexInputState();
    vistate.vertexAttributeDescriptionCount =
                              static_cast<uint32_t>(_vertex_attributes.size());
    vistate.pVertexAttributeDescriptions = _vertex_attributes.data();
  }

  auto & vstate = ViewportState();
  if(_viewports.size() > 0) {
    SetDynamicState(VK_DYNAMIC_STATE_VIEWPORT, false);
    SetDynamicState(VK_DYNAMIC_STATE_SCISSOR, false);
    vstate.viewportCount = static_cast<uint32_t>(_viewports.size());
    vstate.pViewports = _viewports.data();
    vstate.scissorCount = static_cast<uint32_t>(_scissors.size());
    vstate.pScissors = _scissors.data();
  } else {
    SetDynamicState(VK_DYNAMIC_STATE_VIEWPORT, true);
    SetDynamicState(VK_DYNAMIC_STATE_SCISSOR, true);
    vstate.viewportCount = 1;
    vstate.pViewports = nullptr;
    vstate.scissorCount = 1;
    vstate.pScissors = nullptr;
  }

  auto & rstate = RasterizationState();
  if(rstate.rasterizerDiscardEnable == VK_FALSE) {
    // touch states required for rasterization
    MultisampleState();
  }

  if(_dynamic_states.size() > 0) {
    auto & dstate = DynamicState();
    dstate.dynamicStateCount = static_cast<uint32_t>(_dynamic_states.size());
    dstate.pDynamicStates = _dynamic_states.data();
  }

  if(_create_info.layout == VK_NULL_HANDLE)
    _create_info.layout = vk->empty_pipeline_layout;

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
