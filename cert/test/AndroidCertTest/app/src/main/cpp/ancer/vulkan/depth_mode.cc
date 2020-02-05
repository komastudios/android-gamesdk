#include "depth_mode.h"

#include <cstring>

namespace ancer {
namespace vulkan {

DepthMode::DepthMode() {
  std::memset(&_create_info, 0, sizeof(_create_info));
  _create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
}

DepthMode &DepthMode::DepthTest(bool value) {
  _create_info.depthTestEnable = value ? VK_TRUE : VK_FALSE;
  return *this;
}

DepthMode &DepthMode::DepthWrite(bool value) {
  _create_info.depthWriteEnable = value ? VK_TRUE : VK_FALSE;
  return *this;
}

DepthMode &DepthMode::DepthCompare(VkCompareOp op) {
  _create_info.depthCompareOp = op;
  return *this;
}

DepthMode &DepthMode::DepthBounds(nullptr_t) {
  _create_info.depthBoundsTestEnable = VK_FALSE;
  _create_info.minDepthBounds = 0;
  _create_info.maxDepthBounds = 1;
  return *this;
}

DepthMode &DepthMode::DepthBounds(float min, float max) {
  _create_info.depthBoundsTestEnable = VK_TRUE;
  _create_info.minDepthBounds = min;
  _create_info.maxDepthBounds = max;
  return *this;
}

DepthMode DepthMode::kLessThan = DepthMode()
  .DepthTest()
  .DepthWrite()
  .DepthCompare(VK_COMPARE_OP_LESS);

}
}
