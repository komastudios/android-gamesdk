#include "shader_module.h"

#include "shaderc/shaderc.hpp"

#include <ancer/util/Log.hpp>

namespace ancer {
namespace vulkan {

namespace {
  constexpr Log::Tag TAG{"ancer::vulkan::ShaderModule"};
}

Result ShaderModule::Initialize(Vulkan &vk, std::vector<uint32_t> bytecode,
                                VkShaderStageFlagBits stage) {
  _stage = stage;
  _bytecode = std::move(bytecode);
  return Initialize(vk);
}

Result ShaderModule::InitializeFromGLSL(Vulkan &vk, const char * glsl,
                                        VkShaderStageFlagBits stage) {
  using namespace shaderc;

  shaderc_shader_kind kind;
  switch(stage) {
  case VK_SHADER_STAGE_VERTEX_BIT:
    kind = shaderc_vertex_shader;
    break;
  case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
    kind = shaderc_tess_control_shader;
    break;
  case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
    kind = shaderc_tess_evaluation_shader;
    break;
  case VK_SHADER_STAGE_GEOMETRY_BIT:
    kind = shaderc_geometry_shader;
    break;
  case VK_SHADER_STAGE_FRAGMENT_BIT:
    kind = shaderc_fragment_shader;
    break;
  case VK_SHADER_STAGE_COMPUTE_BIT:
    kind = shaderc_compute_shader;
    break;
  default:
    assert(false);
    break;
  }

  Compiler compiler{};

  auto results = compiler.CompileGlslToSpv(glsl, ::strlen(glsl), kind, "-");
  if(results.GetNumErrors() > 0) {
    const char * error = results.GetErrorMessage().c_str();
    Log::E(TAG, "%s", error);
    return Result(VK_ERROR_INITIALIZATION_FAILED);
  }

  _stage = stage;
  _bytecode = std::move(std::vector<uint32_t>(results.cbegin(),
                                              results.cend()));

  return Initialize(vk);
}

void ShaderModule::Shutdown() {
  ForgetBytecode();
  _vk.Destroy(_shader_module);
  _shader_module = VK_NULL_HANDLE;
}

Result ShaderModule::Initialize(Vulkan &vk) {
  _vk = vk;

  VkShaderModuleCreateInfo create_info = {
    /* sType    */ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    /* pNext    */ nullptr,
    /* flags    */ 0,
    /* codeSize */ static_cast<uint32_t>(_bytecode.size()) * sizeof(uint32_t),
    /* pCode    */ _bytecode.data()
  };

  VK_RETURN_FAIL(vk->createShaderModule(vk->device, &create_info, nullptr,
                                        &_shader_module));

  return Result::kSuccess;
}

}
}
