#ifndef AGDC_ANCER_BLENDMODE_H_
#define AGDC_ANCER_BLENDMODE_H_

#include <cstdio>

#include <vulkan/vulkan.h>

namespace ancer {
namespace vulkan {

/**
 * A class used to create blending configuration for pipelines. Being a class
 * like this means common blending configurations can be reused.
 */
class BlendMode {
 public:
  enum {
    MAX_ATTACHMENTS = 4
  };

  BlendMode();

  BlendMode &LogicOp(nullptr_t);
  BlendMode &LogicOp(VkLogicOp op);
  BlendMode &Blend(uint32_t index, nullptr_t);
  BlendMode &BlendColor(uint32_t index, VkBlendFactor src, VkBlendFactor dst,
                        VkBlendOp op);
  BlendMode &BlendAlpha(uint32_t index, VkBlendFactor src, VkBlendFactor dst,
                        VkBlendOp op);
  BlendMode &BlendWriteMask(uint32_t index, bool r, bool g, bool b, bool a);

  inline const VkPipelineColorBlendStateCreateInfo &CreateInfo() const {
    return _create_info;
  }

  static BlendMode kOpaque;

 private:
  VkPipelineColorBlendAttachmentState _attachments[MAX_ATTACHMENTS];
  VkPipelineColorBlendStateCreateInfo _create_info;
};

}
}

#endif


