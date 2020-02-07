#ifndef AGDC_ANCER_SHADERMODULE_H_
#define AGDC_ANCER_SHADERMODULE_H_

#include <cstdio>

#include "vulkan_base.h"

namespace ancer {
namespace vulkan {

/**
 * A ShaderModule can either be in CPU mode or not. CPU mode gives the
 * application access to information about the shader in question.
 *
 * TODO(sarahburns@google.com): bytecode analysis functions
 */
class ShaderModule {
 public:
  /**
   * Initialize the ShaderModule with this bytecode
   */
  Result Initialize(Vulkan &vk, std::vector<uint32_t> bytecode,
                            VkShaderStageFlagBits stage);

  /**
   * Compile the GLSL to SPIR-V and initialize the ShaderModule with the
   * bytecode
   */
  Result InitializeFromGLSL(Vulkan &vk, const char * glsl,
                            VkShaderStageFlagBits stage);

  /**
   * Reset this ShaderModule, destroying the vulkan shader module
   */
  void Shutdown();

  /**
   * The bytecode is used for SPIR-V analysis, if thats not needed we can forget
   * it to not use the RAM
   */
  inline void ForgetBytecode() {
    _bytecode.clear();
  }

  inline VkShaderStageFlagBits Stage() const {
    return _stage;
  }

  inline VkShaderModule Handle() const {
    return _shader_module;
  }

  inline const char * Name() const {
    return "main";
  }

  inline VkSpecializationInfo * SpecializationInfo() const {
    return nullptr;
  }

 private:
  Vulkan _vk;

  Result Initialize(Vulkan &vk);

  VkShaderStageFlagBits _stage;
  std::vector<uint32_t> _bytecode;
  VkShaderModule _shader_module;
};

class VertexShaderModule : public ShaderModule {
 public:
  inline Result Initialize(Vulkan &vk, std::vector<uint32_t> bytecode) {
    return ShaderModule::Initialize(vk, bytecode,
                                    VK_SHADER_STAGE_VERTEX_BIT);
  }

  inline Result InitializeFromGLSL(Vulkan &vk, const char * glsl) {
    return ShaderModule::InitializeFromGLSL(vk, glsl,
                                          VK_SHADER_STAGE_VERTEX_BIT);
  }
};

class FragmentShaderModule : public ShaderModule {
 public:
  inline Result Initialize(Vulkan &vk, std::vector<uint32_t> bytecode) {
    return ShaderModule::Initialize(vk, bytecode,
                                    VK_SHADER_STAGE_FRAGMENT_BIT);
  }

  inline Result InitializeFromGLSL(Vulkan &vk, const char * glsl) {
    return ShaderModule::InitializeFromGLSL(vk, glsl,
                                        VK_SHADER_STAGE_FRAGMENT_BIT);
  }
};

}
}

#endif
