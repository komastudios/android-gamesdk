#include "render_pass.h"

namespace ancer {
namespace vulkan {

class RenderPass;

namespace render_pass {

// ============================================================================
// AttachmentBuilder
AttachmentBuilder::AttachmentBuilder(Builder &builder, uint32_t index) :
    _builder(builder), _index(index) {
  _attachment = VkAttachmentDescription {
    /* flags          */ 0,
    /* format         */ VK_FORMAT_UNDEFINED,
    /* samples        */ VK_SAMPLE_COUNT_1_BIT,
    /* loadOp         */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    /* storeOp        */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
    /* stencilLoadOp  */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    /* stencilStoreOp */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
    /* initialLayout  */ VK_IMAGE_LAYOUT_UNDEFINED,
    /* finalLayout    */ VK_IMAGE_LAYOUT_UNDEFINED
  };
}

AttachmentBuilder &AttachmentBuilder::MayAlias(bool value) {
  if(value)
    _attachment.flags |= VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
  else
    _attachment.flags &= ~VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
  return *this;
}

AttachmentBuilder &AttachmentBuilder::Samples(VkSampleCountFlagBits samples) {
  _attachment.samples = samples;
  return *this;
}

AttachmentBuilder &AttachmentBuilder::Format(VkFormat format) {
  _attachment.format = format;
  return *this;
}

AttachmentBuilder &AttachmentBuilder::Load() {
  _attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  return *this;
}

AttachmentBuilder &AttachmentBuilder::Clear() {
  _attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  return *this;
}

AttachmentBuilder &AttachmentBuilder::Store() {
  _attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  return *this;
}

AttachmentBuilder &AttachmentBuilder::StencilLoad() {
  _attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  return *this;
}

AttachmentBuilder &AttachmentBuilder::StencilClear() {
  _attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  return *this;
}

AttachmentBuilder &AttachmentBuilder::StencilStore() {
  _attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
  return *this;
}

AttachmentBuilder &AttachmentBuilder::InitialLayout(VkImageLayout layout) {
  _attachment.initialLayout = layout;
  return *this;
}

AttachmentBuilder &AttachmentBuilder::FinalLayout(VkImageLayout layout) {
  _attachment.initialLayout = layout;
  return *this;
}

Builder &AttachmentBuilder::End() {
  if(_builder._attachments.size() <= _index)
    _builder._attachments.resize(_index + 1);
  _builder._attachments[_index] = _attachment;
  return _builder;
}

// ============================================================================
// SubpassBuilder
SubpassBuilder::SubpassBuilder(Builder &builder, uint32_t index) :
    _builder(builder), _index(index), _input(), _color(), _depth_stencil() {
  _description = {
    /* flags                   */ 0,
    /* pipelineBindPoint       */ VK_PIPELINE_BIND_POINT_GRAPHICS,
    /* inputAttachmentCount    */ 0,
    /* pInputAttachments       */ nullptr,
    /* colorAttachmentCount    */ 0,
    /* pColorAttachments       */ nullptr,
    /* pResolveAttachments     */ nullptr,
    /* pDepthStencilAttachment */ nullptr,
    /* preserveAttachmentCount */ 0,
    /* pPreserveAttachments    */ nullptr
  };
}

SubpassBuilder &SubpassBuilder::Input(uint32_t index, VkImageLayout layout) {
  if(layout == VK_IMAGE_LAYOUT_UNDEFINED)
    layout = _builder._attachments[index].initialLayout;
  _input.push_back(VkAttachmentReference {
    /* attachment */ index,
    /* layout     */ layout
  });
  return *this;
}

SubpassBuilder &SubpassBuilder::Color(uint32_t index, VkImageLayout layout) {
  if(layout == VK_IMAGE_LAYOUT_UNDEFINED)
    layout = _builder._attachments[index].initialLayout;
  _color.push_back(VkAttachmentReference {
    /* attachment */ index,
    /* layout     */ layout
  });
  return *this;
}

SubpassBuilder &SubpassBuilder::DepthStencil(uint32_t index, VkImageLayout layout) {
  if(layout == VK_IMAGE_LAYOUT_UNDEFINED)
    layout = _builder._attachments[index].initialLayout;
  _depth_stencil = VkAttachmentReference {
    /* attachment */ index,
    /* layout     */ layout
  };
  _description.pDepthStencilAttachment = &_depth_stencil;
  return *this;
}

Builder &SubpassBuilder::End() {
  _description.inputAttachmentCount = static_cast<uint32_t>(_input.size());
  _description.colorAttachmentCount = static_cast<uint32_t>(_color.size());

  for(auto ref : _input)
    _builder._references.push_back(ref);
  for(auto ref : _color)
    _builder._references.push_back(ref);
  if(_description.pDepthStencilAttachment != nullptr)
    _builder._references.push_back(_depth_stencil);

  if(_builder._subpasses.size() <= _index)
    _builder._subpasses.resize(_index + 1);
  _builder._subpasses[_index] = _description;

  return _builder;
}

// ============================================================================
// Builder
Builder::Builder(RenderPass &render_pass) : _render_pass(render_pass),
    _attachments(), _references(), _subpasses() {
}

AttachmentBuilder Builder::DepthStencil(uint32_t index, VkFormat format) {
  return AttachmentBuilder(*this, index)
      .Format(format)
      .InitialLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
      .FinalLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

AttachmentBuilder Builder::Color(uint32_t index, VkFormat format) {
  return AttachmentBuilder(*this, index)
      .Format(format)
      .InitialLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
      .FinalLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

SubpassBuilder Builder::Subpass(uint32_t index) {
  return SubpassBuilder(*this, index);
}

Result Builder::Build(Vulkan &vk) {
  // fix the attachments in the subpasses
  VkAttachmentReference *references = _references.data();
  for(auto && subpass : _subpasses) {
    if(subpass.inputAttachmentCount > 0) {
      subpass.pInputAttachments = references;
      references += subpass.inputAttachmentCount;
    }
    if(subpass.colorAttachmentCount > 0) {
      subpass.pColorAttachments = references;
      references += subpass.colorAttachmentCount;
    }
    if(subpass.pDepthStencilAttachment != nullptr) {
      subpass.pDepthStencilAttachment = references;
      references += 1;
    }
  }

  VkRenderPassCreateInfo create_info = {
    /* sType           */ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    /* pNext           */ nullptr,
    /* flags           */ 0,
    /* attachmentCount */ static_cast<uint32_t>(_attachments.size()),
    /* pAttachments    */ _attachments.empty() ? nullptr : _attachments.data(),
    /* subpassCount    */ static_cast<uint32_t>(_subpasses.size()),
    /* pSubpasses      */ _subpasses.empty() ? nullptr : _subpasses.data(),
    /* dependencyCount */ 0,
    /* pDependencies   */ nullptr
  };

  _render_pass._vk = vk;
  VK_RETURN_FAIL(vk->createRenderPass(vk->device, &create_info, nullptr,
                                      &_render_pass._render_pass));

  return Result::kSuccess;
}

} // namespace render_pass

void RenderPass::Shutdown() {
}

render_pass::Builder RenderPass::Builder() {
  return render_pass::Builder(*this);
}

}
}
