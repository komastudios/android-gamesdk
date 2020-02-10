#include "sampler.h"

namespace ancer {
namespace vulkan {

Sampler::Sampler() {
  memset(&_create_info, 0, sizeof(_create_info));
  _create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  _create_info.pNext = nullptr;
}

Sampler &Sampler::MagFilter(VkFilter filter) {
  _create_info.magFilter = filter;
  return *this;
}

Sampler &Sampler::MinFilter(VkFilter filter) {
  _create_info.minFilter = filter;
  return *this;
}

Sampler &Sampler::MipmapMode(VkSamplerMipmapMode mode) {
  _create_info.mipmapMode = mode;
  return *this;
}

Sampler &Sampler::AddressModeU(VkSamplerAddressMode u) {
  _create_info.addressModeU = u;
  return *this;
}

Sampler &Sampler::AddressModeV(VkSamplerAddressMode v) {
  _create_info.addressModeV = v;
  return *this;
}

Sampler &Sampler::AddressModeW(VkSamplerAddressMode w) {
  _create_info.addressModeW = w;
  return *this;
}

Sampler &Sampler::MipLodBias(float bias) {
  _create_info.mipLodBias = bias;
  return *this;
}

Sampler &Sampler::DisableAnisotropy() {
  _create_info.anisotropyEnable = VK_FALSE;
  return *this;
}

Sampler &Sampler::Anisotropy(float max_anisotropy) {
  _create_info.anisotropyEnable = VK_TRUE;
  _create_info.maxAnisotropy = max_anisotropy;
  return *this;
}

Sampler &Sampler::DisableCompare() {
  _create_info.compareEnable = VK_FALSE;
  return *this;
}

Sampler &Sampler::Compare(VkCompareOp op) {
  _create_info.compareEnable = VK_TRUE;
  _create_info.compareOp = op;
  return *this;
}

Sampler &Sampler::MinLod(float lod) {
  _create_info.minLod = lod;
  return *this;
}

Sampler &Sampler::MaxLod(float lod) {
  _create_info.maxLod = lod;
  return *this;
}

Sampler &Sampler::BorderColor(VkBorderColor color) {
  _create_info.borderColor = color;
  return *this;
}

Sampler &Sampler::UnnormalizedCoordinates(bool value) {
  _create_info.unnormalizedCoordinates = value ? VK_TRUE : VK_FALSE;
  return *this;
}

}
}
