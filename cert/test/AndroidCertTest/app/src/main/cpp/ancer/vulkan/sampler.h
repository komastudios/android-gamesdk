#ifndef AGDC_ANCER_SAMPLER_H_
#define AGDC_ANCER_SAMPLER_H_

#include "vulkan_base.h"

namespace ancer {
namespace vulkan {

/**
 * A Sampler resource, these sampler configurations can be saved for later use
 * and resolved with the ResourcesStore into Vulkan objects
 */
class Sampler {
 public:
  Sampler();

  Sampler &MagFilter(VkFilter filter);
  Sampler &MinFilter(VkFilter filter);
  Sampler &MipmapMode(VkSamplerMipmapMode mode);
  Sampler &AddressModeU(VkSamplerAddressMode u);
  Sampler &AddressModeV(VkSamplerAddressMode v);
  Sampler &AddressModeW(VkSamplerAddressMode w);
  Sampler &MipLodBias(float bias);
  Sampler &DisableAnisotropy();
  Sampler &Anisotropy(float max_anisotropy);
  Sampler &DisableCompare();
  Sampler &Compare(VkCompareOp op);
  Sampler &MinLod(float lod);
  Sampler &MaxLod(float lod);
  Sampler &BorderColor(VkBorderColor color);
  Sampler &UnnormalizedCoordinates(bool value = true);

  inline const VkSamplerCreateInfo &CreateInfo() const {
    return _create_info;
  }

 private:
  VkSamplerCreateInfo _create_info;
};

}
}

#endif
