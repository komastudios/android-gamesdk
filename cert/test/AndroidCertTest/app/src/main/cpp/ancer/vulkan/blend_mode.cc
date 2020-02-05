#include "blend_mode.h"

#include <cstring>
#include <algorithm>

namespace ancer {
namespace vulkan {

BlendMode::BlendMode() {
  std::memset(&_attachments, 0, sizeof(_attachments));
  std::memset(&_create_info, 0, sizeof(_create_info));
  _create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  _create_info.pAttachments = _attachments;

  for(uint32_t i = 0; i < MAX_ATTACHMENTS; ++i) {
    _attachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                     VK_COLOR_COMPONENT_G_BIT |
                                     VK_COLOR_COMPONENT_B_BIT |
                                     VK_COLOR_COMPONENT_A_BIT ;
  }
}

BlendMode &BlendMode::LogicOp(nullptr_t) {
  _create_info.logicOpEnable = VK_FALSE;
  return *this;
}

BlendMode &BlendMode::LogicOp(VkLogicOp op) {
  _create_info.logicOpEnable = VK_TRUE;
  _create_info.logicOp = op;
  return *this;
}

BlendMode &BlendMode::Blend(uint32_t index, nullptr_t) {
  _create_info.attachmentCount = std::max(_create_info.attachmentCount,
                                          index + 1);
  return *this;
}

BlendMode &BlendMode::BlendColor(uint32_t index, VkBlendFactor src,
                                 VkBlendFactor dst, VkBlendOp op) {
  _create_info.attachmentCount = std::max(_create_info.attachmentCount,
                                          index + 1);
  _attachments[index].blendEnable = VK_TRUE;
  _attachments[index].srcColorBlendFactor = src;
  _attachments[index].dstColorBlendFactor = dst;
  _attachments[index].colorBlendOp = op;
  return *this;
}

BlendMode &BlendMode::BlendAlpha(uint32_t index, VkBlendFactor src,
                                 VkBlendFactor dst, VkBlendOp op) {
  _create_info.attachmentCount = std::max(_create_info.attachmentCount,
                                          index + 1);
  _attachments[index].blendEnable = VK_TRUE;
  _attachments[index].srcAlphaBlendFactor = src;
  _attachments[index].dstAlphaBlendFactor = dst;
  _attachments[index].alphaBlendOp = op;
  return *this;
}

BlendMode &BlendMode::BlendWriteMask(uint32_t index, bool r, bool g, bool b,
                                    bool a) {
  _create_info.attachmentCount = std::max(_create_info.attachmentCount,
                                          index + 1);
  VkColorComponentFlags &flags = _attachments[index].colorWriteMask;
  flags = 0;
  if(r) flags |= VK_COLOR_COMPONENT_R_BIT;
  if(g) flags |= VK_COLOR_COMPONENT_G_BIT;
  if(b) flags |= VK_COLOR_COMPONENT_B_BIT;
  if(a) flags |= VK_COLOR_COMPONENT_A_BIT;
  return *this;
}

BlendMode BlendMode::kOpaque = BlendMode().Blend(0, nullptr);

}
}
