#ifndef AGDC_ANCER_DEPTHMODE_H_
#define AGDC_ANCER_DEPTHMODE_H_

#include <cstdio>

#include <vulkan/vulkan.h>

namespace ancer {
namespace vulkan {

/**
 * A class used to create depth configuration for pipelines. Being a class like
 * this means common depth configurations can be reused.
 */
class DepthMode {
 public:
  DepthMode();

  DepthMode &DepthTest(bool value = true);
  DepthMode &DepthWrite(bool value = true);
  DepthMode &DepthCompare(VkCompareOp op);
  DepthMode &DepthBounds(nullptr_t);
  DepthMode &DepthBounds(float min, float max);

  // TODO(sarahburns@google.com): stencil parameters

  inline const VkPipelineDepthStencilStateCreateInfo &CreateInfo() const {
    return _create_info;
  }

  static DepthMode kLessThan;

 private:
  VkPipelineDepthStencilStateCreateInfo _create_info;
};

}
}

#endif

