#ifndef AGDC_ANCER_RENDERPASS_H_
#define AGDC_ANCER_RENDERPASS_H_

#include <cstdio>

#include "vulkan_base.h"

namespace ancer {
namespace vulkan {

/*
 * Initialize a VkClearValue for a image with Depth and Stencil
 */
inline VkClearValue ClearDepth(float depth = 0.f, uint32_t stencil = 0) {
  VkClearValue cv;
  cv.depthStencil.depth = depth;
  cv.depthStencil.stencil = stencil;
  return cv;
}

/*
 * Initialize a VkClearValue for a image with Color
 */
inline VkClearValue ClearColor(float r = 0.f, float g = 0.f, float b = 0.f,
                               float a = 0.f) {
  VkClearValue cv;
  cv.color.float32[0] = r;
  cv.color.float32[1] = g;
  cv.color.float32[2] = b;
  cv.color.float32[3] = a;
  return cv;
}

// forward declaration for render_pass::Builder
class RenderPass;

namespace render_pass {

// forward delaration for sub builders
struct Builder;

// TODO(sarahburns@google.com): resolve
// TODO(sarahburns@google.com): preserve
// TODO(sarahburns@google.com): subpass dependencies

struct [[nodiscard]] AttachmentBuilder {
  AttachmentBuilder(Builder &builder, uint32_t index);

  AttachmentBuilder &MayAlias(bool value = true);
  AttachmentBuilder &Samples(VkSampleCountFlagBits samples);
  AttachmentBuilder &Format(VkFormat format);
  AttachmentBuilder &Load();
  AttachmentBuilder &Clear();
  AttachmentBuilder &Store();
  AttachmentBuilder &StencilLoad();
  AttachmentBuilder &StencilClear();
  AttachmentBuilder &StencilStore();
  AttachmentBuilder &InitialLayout(VkImageLayout layout);
  AttachmentBuilder &FinalLayout(VkImageLayout layout);

  Builder &End();

 private:
  Builder &_builder;
  uint32_t _index;
  VkAttachmentDescription _attachment;
};

struct [[nodiscard]] SubpassBuilder {
 public:
  SubpassBuilder(Builder &builder, uint32_t index);

  SubpassBuilder &Input(uint32_t index, VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED);
  SubpassBuilder &Color(uint32_t index, VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED);
  SubpassBuilder &DepthStencil(uint32_t index, VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED);

  Builder &End();

 private:
  Builder &_builder;
  uint32_t _index;
  std::vector<VkAttachmentReference> _input;
  std::vector<VkAttachmentReference> _color;
  VkAttachmentReference _depth_stencil;
  VkSubpassDescription _description;
};

struct [[nodiscard]] Builder {
 public:
  Builder(RenderPass &render_pass);

  AttachmentBuilder DepthStencil(uint32_t index, VkFormat format);
  AttachmentBuilder Color(uint32_t index, VkFormat format);
  SubpassBuilder Subpass(uint32_t index);

  Result Build(Vulkan &vk);

 private:
  friend struct AttachmentBuilder;
  friend struct SubpassBuilder;

  RenderPass &_render_pass;

  std::vector<VkAttachmentDescription> _attachments;
  std::vector<VkAttachmentReference> _references;
  std::vector<VkSubpassDescription> _subpasses;
};

} // namespace render_pass

/*
 * A RenderPass represents a process that stages multiple subpasses together
 */
class RenderPass {
 public:
  void Shutdown();

  render_pass::Builder Builder();

  inline VkRenderPass Handle() const {
    return _render_pass;
  }

 private:
  friend struct render_pass::Builder;

  Vulkan _vk;
  VkRenderPass _render_pass;
};

}
}

#endif
