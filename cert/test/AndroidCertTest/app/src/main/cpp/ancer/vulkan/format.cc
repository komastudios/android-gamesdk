#include "format.h"

#include <unordered_map>
#include <atomic>

namespace ancer {
namespace vulkan {

static const std::unordered_map<VkFormat, FormatProperties> &Builtin() {
  static std::atomic_flag generated = ATOMIC_FLAG_INIT;
  static std::unordered_map<VkFormat, FormatProperties> map;

  auto unorm = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 1,
      /* block_height       */ 1,
      /* numeric_format     */ NumericFormat::UNorm,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::None
    };
  };

  auto snorm = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 1,
      /* block_height       */ 1,
      /* numeric_format     */ NumericFormat::SNorm,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::None
    };
  };

  auto uscaled = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 1,
      /* block_height       */ 1,
      /* numeric_format     */ NumericFormat::UScaled,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::None
    };
  };

  auto sscaled = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 1,
      /* block_height       */ 1,
      /* numeric_format     */ NumericFormat::SScaled,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::None
    };
  };

  auto uint = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 1,
      /* block_height       */ 1,
      /* numeric_format     */ NumericFormat::UInt,
      /* sample_type        */ SampleType::Int,
      /* compression_scheme */ CompressionScheme::None
    };
  };

  auto sint = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 1,
      /* block_height       */ 1,
      /* numeric_format     */ NumericFormat::SInt,
      /* sample_type        */ SampleType::Int,
      /* compression_scheme */ CompressionScheme::None
    };
  };

  auto ufloat = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 1,
      /* block_height       */ 1,
      /* numeric_format     */ NumericFormat::UFloat,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::None
    };
  };

  auto sfloat = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 1,
      /* block_height       */ 1,
      /* numeric_format     */ NumericFormat::SFloat,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::None
    };
  };

  auto srgb = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 1,
      /* block_height       */ 1,
      /* numeric_format     */ NumericFormat::SRGB,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::None
    };
  };

  auto bc_unorm = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 4,
      /* block_height       */ 4,
      /* numeric_format     */ NumericFormat::UNorm,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::BC
    };
  };

  auto bc_srgb = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 4,
      /* block_height       */ 4,
      /* numeric_format     */ NumericFormat::SRGB,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::BC
    };
  };

  auto bc_ufloat = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 4,
      /* block_height       */ 4,
      /* numeric_format     */ NumericFormat::UFloat,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::BC
    };
  };

  auto bc_sfloat = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 4,
      /* block_height       */ 4,
      /* numeric_format     */ NumericFormat::SFloat,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::BC
    };
  };

  auto etc2_unorm = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 4,
      /* block_height       */ 4,
      /* numeric_format     */ NumericFormat::UNorm,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::ETC2
    };
  };

  auto etc2_srgb = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 4,
      /* block_height       */ 4,
      /* numeric_format     */ NumericFormat::SRGB,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::ETC2
    };
  };

  auto eac_unorm = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 4,
      /* block_height       */ 4,
      /* numeric_format     */ NumericFormat::UNorm,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::EAC
    };
  };

  auto eac_snorm = [](uint32_t size) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ size,
      /* block_width        */ 4,
      /* block_height       */ 4,
      /* numeric_format     */ NumericFormat::SNorm,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::EAC
    };
  };

  auto astc_unorm = [](uint32_t width, uint32_t height) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ 16,
      /* block_width        */ width,
      /* block_height       */ height,
      /* numeric_format     */ NumericFormat::UNorm,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::ASTC
    };
  };

  auto astc_srgb = [](uint32_t width, uint32_t height) -> FormatProperties {
    return FormatProperties {
      /* size_in_bytes      */ 16,
      /* block_width        */ width,
      /* block_height       */ height,
      /* numeric_format     */ NumericFormat::SRGB,
      /* sample_type        */ SampleType::Float,
      /* compression_scheme */ CompressionScheme::ASTC
    };
  };

  if(!generated.test_and_set()) {
    map[VK_FORMAT_R4G4_UNORM_PACK8] = unorm(1);
    map[VK_FORMAT_R4G4B4A4_UNORM_PACK16] = unorm(2);
    map[VK_FORMAT_B4G4R4A4_UNORM_PACK16] = unorm(2);
    map[VK_FORMAT_R5G6B5_UNORM_PACK16] = unorm(2);
    map[VK_FORMAT_B5G6R5_UNORM_PACK16] = unorm(2);
    map[VK_FORMAT_R5G5B5A1_UNORM_PACK16] = unorm(2);
    map[VK_FORMAT_B5G5R5A1_UNORM_PACK16] = unorm(2);
    map[VK_FORMAT_A1R5G5B5_UNORM_PACK16] = unorm(2);
    map[VK_FORMAT_R8_UNORM] = unorm(1);
    map[VK_FORMAT_R8_SNORM] = snorm(1);
    map[VK_FORMAT_R8_USCALED] = uscaled(1);
    map[VK_FORMAT_R8_SSCALED] = sscaled(1);
    map[VK_FORMAT_R8_UINT] = uint(1);
    map[VK_FORMAT_R8_SINT] = sint(1);
    map[VK_FORMAT_R8_SRGB] = srgb(1);
    map[VK_FORMAT_R8G8_UNORM] = unorm(2);
    map[VK_FORMAT_R8G8_SNORM] = snorm(2);
    map[VK_FORMAT_R8G8_USCALED] = uscaled(2);
    map[VK_FORMAT_R8G8_SSCALED] = sscaled(2);
    map[VK_FORMAT_R8G8_UINT] = uint(2);
    map[VK_FORMAT_R8G8_SINT] = sint(2);
    map[VK_FORMAT_R8G8_SRGB] = srgb(2);
    map[VK_FORMAT_R8G8B8_UNORM] = unorm(3);
    map[VK_FORMAT_R8G8B8_SNORM] = snorm(3);
    map[VK_FORMAT_R8G8B8_USCALED] = uscaled(3);
    map[VK_FORMAT_R8G8B8_SSCALED] = sscaled(3);
    map[VK_FORMAT_R8G8B8_UINT] = uint(3);
    map[VK_FORMAT_R8G8B8_SINT] = sint(3);
    map[VK_FORMAT_R8G8B8_SRGB] = srgb(3);
    map[VK_FORMAT_B8G8R8_UNORM] = unorm(3);
    map[VK_FORMAT_B8G8R8_SNORM] = snorm(3);
    map[VK_FORMAT_B8G8R8_USCALED] = uscaled(3);
    map[VK_FORMAT_B8G8R8_SSCALED] = sscaled(3);
    map[VK_FORMAT_B8G8R8_UINT] = uint(3);
    map[VK_FORMAT_B8G8R8_SINT] = sint(3);
    map[VK_FORMAT_B8G8R8_SRGB] = srgb(3);
    map[VK_FORMAT_R8G8B8A8_UNORM] = unorm(4);
    map[VK_FORMAT_R8G8B8A8_SNORM] = snorm(4);
    map[VK_FORMAT_R8G8B8A8_USCALED] = uscaled(4);
    map[VK_FORMAT_R8G8B8A8_SSCALED] = sscaled(4);
    map[VK_FORMAT_R8G8B8A8_UINT] = uint(4);
    map[VK_FORMAT_R8G8B8A8_SINT] = sint(4);
    map[VK_FORMAT_R8G8B8A8_SRGB] = srgb(4);
    map[VK_FORMAT_B8G8R8A8_UNORM] = unorm(4);
    map[VK_FORMAT_B8G8R8A8_SNORM] = snorm(4);
    map[VK_FORMAT_B8G8R8A8_USCALED] = uscaled(4);
    map[VK_FORMAT_B8G8R8A8_SSCALED] = sscaled(4);
    map[VK_FORMAT_B8G8R8A8_UINT] = uint(4);
    map[VK_FORMAT_B8G8R8A8_SINT] = sint(4);
    map[VK_FORMAT_B8G8R8A8_SRGB] = srgb(4);
    map[VK_FORMAT_A8B8G8R8_UNORM_PACK32] = unorm(4);
    map[VK_FORMAT_A8B8G8R8_SNORM_PACK32] = snorm(4);
    map[VK_FORMAT_A8B8G8R8_USCALED_PACK32] = uscaled(4);
    map[VK_FORMAT_A8B8G8R8_SSCALED_PACK32] = sscaled(4);
    map[VK_FORMAT_A8B8G8R8_UINT_PACK32] = uint(4);
    map[VK_FORMAT_A8B8G8R8_SINT_PACK32] = sint(4);
    map[VK_FORMAT_A8B8G8R8_SRGB_PACK32] = srgb(4);
    map[VK_FORMAT_A2R10G10B10_UNORM_PACK32] = unorm(4);
    map[VK_FORMAT_A2R10G10B10_SNORM_PACK32] = snorm(4);
    map[VK_FORMAT_A2R10G10B10_USCALED_PACK32] = uscaled(4);
    map[VK_FORMAT_A2R10G10B10_SSCALED_PACK32] = sscaled(4);
    map[VK_FORMAT_A2R10G10B10_UINT_PACK32] = uint(4);
    map[VK_FORMAT_A2R10G10B10_SINT_PACK32] = sint(4);
    map[VK_FORMAT_A2B10G10R10_UNORM_PACK32] = unorm(4);
    map[VK_FORMAT_A2B10G10R10_SNORM_PACK32] = snorm(4);
    map[VK_FORMAT_A2B10G10R10_USCALED_PACK32] = uscaled(4);
    map[VK_FORMAT_A2B10G10R10_SSCALED_PACK32] = sscaled(4);
    map[VK_FORMAT_A2B10G10R10_UINT_PACK32] = uint(4);
    map[VK_FORMAT_A2B10G10R10_SINT_PACK32] = sint(4);
    map[VK_FORMAT_R16_UNORM] = unorm(2);
    map[VK_FORMAT_R16_SNORM] = snorm(2);
    map[VK_FORMAT_R16_USCALED] = uscaled(2);
    map[VK_FORMAT_R16_SSCALED] = sscaled(2);
    map[VK_FORMAT_R16_UINT] = uint(2);
    map[VK_FORMAT_R16_SINT] = sint(2);
    map[VK_FORMAT_R16_SFLOAT] = sfloat(2);
    map[VK_FORMAT_R16G16_UNORM] = unorm(4);
    map[VK_FORMAT_R16G16_SNORM] = snorm(4);
    map[VK_FORMAT_R16G16_USCALED] = uscaled(4);
    map[VK_FORMAT_R16G16_SSCALED] = sscaled(4);
    map[VK_FORMAT_R16G16_UINT] = uint(4);
    map[VK_FORMAT_R16G16_SINT] = sint(4);
    map[VK_FORMAT_R16G16_SFLOAT] = sfloat(4);
    map[VK_FORMAT_R16G16B16_UNORM] = unorm(6);
    map[VK_FORMAT_R16G16B16_SNORM] = snorm(6);
    map[VK_FORMAT_R16G16B16_USCALED] = uscaled(6);
    map[VK_FORMAT_R16G16B16_SSCALED] = sscaled(6);
    map[VK_FORMAT_R16G16B16_UINT] = uint(6);
    map[VK_FORMAT_R16G16B16_SINT] = sint(6);
    map[VK_FORMAT_R16G16B16_SFLOAT] = sfloat(6);
    map[VK_FORMAT_R16G16B16A16_UNORM] = unorm(8);
    map[VK_FORMAT_R16G16B16A16_SNORM] = snorm(8);
    map[VK_FORMAT_R16G16B16A16_USCALED] = uscaled(8);
    map[VK_FORMAT_R16G16B16A16_SSCALED] = sscaled(8);
    map[VK_FORMAT_R16G16B16A16_UINT] = uint(8);
    map[VK_FORMAT_R16G16B16A16_SINT] = sint(8);
    map[VK_FORMAT_R16G16B16A16_SFLOAT] = sfloat(8);
    map[VK_FORMAT_R32_UINT] = uint(4);
    map[VK_FORMAT_R32_SINT] = sint(4);
    map[VK_FORMAT_R32_SFLOAT] = sfloat(4);
    map[VK_FORMAT_R32G32_UINT] = uint(8);
    map[VK_FORMAT_R32G32_SINT] = sint(8);
    map[VK_FORMAT_R32G32_SFLOAT] = sfloat(8);
    map[VK_FORMAT_R32G32B32_UINT] = uint(12);
    map[VK_FORMAT_R32G32B32_SINT] = sint(12);
    map[VK_FORMAT_R32G32B32_SFLOAT] = sfloat(12);
    map[VK_FORMAT_R32G32B32A32_UINT] = uint(16);
    map[VK_FORMAT_R32G32B32A32_SINT] = sint(16);
    map[VK_FORMAT_R32G32B32A32_SFLOAT] = sfloat(16);
    map[VK_FORMAT_R64_UINT] = uint(8);
    map[VK_FORMAT_R64_SINT] = sint(8);
    map[VK_FORMAT_R64_SFLOAT] = sfloat(8);
    map[VK_FORMAT_R64G64_UINT] = uint(16);
    map[VK_FORMAT_R64G64_SINT] = sint(16);
    map[VK_FORMAT_R64G64_SFLOAT] = sfloat(16);
    map[VK_FORMAT_R64G64B64_UINT] = uint(24);
    map[VK_FORMAT_R64G64B64_SINT] = sint(24);
    map[VK_FORMAT_R64G64B64_SFLOAT] = sfloat(24);
    map[VK_FORMAT_R64G64B64A64_UINT] = uint(32);
    map[VK_FORMAT_R64G64B64A64_SINT] = sint(32);
    map[VK_FORMAT_R64G64B64A64_SFLOAT] = sfloat(32);
    map[VK_FORMAT_B10G11R11_UFLOAT_PACK32] = ufloat(4);
    map[VK_FORMAT_E5B9G9R9_UFLOAT_PACK32] = ufloat(4);
    map[VK_FORMAT_D16_UNORM] = unorm(2);
    map[VK_FORMAT_X8_D24_UNORM_PACK32] = unorm(4);
    map[VK_FORMAT_D32_SFLOAT] = sfloat(4);
    map[VK_FORMAT_S8_UINT] = uint(1);
    map[VK_FORMAT_D16_UNORM_S8_UINT] = uint(3);
    map[VK_FORMAT_D24_UNORM_S8_UINT] = uint(4);
    map[VK_FORMAT_D32_SFLOAT_S8_UINT] = uint(5);
    map[VK_FORMAT_BC1_RGB_UNORM_BLOCK] = bc_unorm(8);
    map[VK_FORMAT_BC1_RGB_SRGB_BLOCK] = bc_srgb(8);
    map[VK_FORMAT_BC1_RGBA_UNORM_BLOCK] = bc_unorm(8);
    map[VK_FORMAT_BC1_RGBA_SRGB_BLOCK] = bc_srgb(8);
    map[VK_FORMAT_BC2_UNORM_BLOCK] = bc_unorm(16);
    map[VK_FORMAT_BC2_SRGB_BLOCK] = bc_srgb(16);
    map[VK_FORMAT_BC3_UNORM_BLOCK] = bc_unorm(16);
    map[VK_FORMAT_BC3_SRGB_BLOCK] = bc_srgb(16);
    map[VK_FORMAT_BC4_UNORM_BLOCK] = bc_unorm(8);
    map[VK_FORMAT_BC4_SNORM_BLOCK] = bc_srgb(8);
    map[VK_FORMAT_BC5_UNORM_BLOCK] = bc_unorm(16);
    map[VK_FORMAT_BC5_SNORM_BLOCK] = bc_srgb(16);
    map[VK_FORMAT_BC6H_UFLOAT_BLOCK] = bc_ufloat(16);
    map[VK_FORMAT_BC6H_SFLOAT_BLOCK] = bc_sfloat(16);
    map[VK_FORMAT_BC7_UNORM_BLOCK] = bc_unorm(16);
    map[VK_FORMAT_BC7_SRGB_BLOCK] = bc_srgb(16);
    map[VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK] = etc2_unorm(8);
    map[VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK] = etc2_srgb(8);
    map[VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK] = etc2_unorm(8);
    map[VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK] = etc2_srgb(8);
    map[VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK] = etc2_unorm(16);
    map[VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK] = etc2_srgb(16);
    map[VK_FORMAT_EAC_R11_UNORM_BLOCK] = eac_unorm(8);
    map[VK_FORMAT_EAC_R11_SNORM_BLOCK] = eac_snorm(8);
    map[VK_FORMAT_EAC_R11G11_UNORM_BLOCK] = eac_unorm(16);
    map[VK_FORMAT_EAC_R11G11_SNORM_BLOCK] = eac_snorm(16);
    map[VK_FORMAT_ASTC_4x4_UNORM_BLOCK] = astc_unorm(4, 4);
    map[VK_FORMAT_ASTC_4x4_SRGB_BLOCK] = astc_srgb(4, 4);
    map[VK_FORMAT_ASTC_5x4_UNORM_BLOCK] = astc_unorm(5, 4);
    map[VK_FORMAT_ASTC_5x4_SRGB_BLOCK] = astc_srgb(5, 4);
    map[VK_FORMAT_ASTC_5x5_UNORM_BLOCK] = astc_unorm(5, 5);
    map[VK_FORMAT_ASTC_5x5_SRGB_BLOCK] = astc_srgb(5, 5);
    map[VK_FORMAT_ASTC_6x5_UNORM_BLOCK] = astc_unorm(6, 5);
    map[VK_FORMAT_ASTC_6x5_SRGB_BLOCK] = astc_srgb(6, 5);
    map[VK_FORMAT_ASTC_6x6_UNORM_BLOCK] = astc_unorm(6, 6);
    map[VK_FORMAT_ASTC_6x6_SRGB_BLOCK] = astc_srgb(6, 6);
    map[VK_FORMAT_ASTC_8x5_UNORM_BLOCK] = astc_unorm(8, 5);
    map[VK_FORMAT_ASTC_8x5_SRGB_BLOCK] = astc_srgb(8, 5);
    map[VK_FORMAT_ASTC_8x6_UNORM_BLOCK] = astc_unorm(8, 6);
    map[VK_FORMAT_ASTC_8x6_SRGB_BLOCK] = astc_srgb(8, 6);
    map[VK_FORMAT_ASTC_8x8_UNORM_BLOCK] = astc_unorm(8, 8);
    map[VK_FORMAT_ASTC_8x8_SRGB_BLOCK] = astc_srgb(8, 8);
    map[VK_FORMAT_ASTC_10x5_UNORM_BLOCK] = astc_unorm(10, 5);
    map[VK_FORMAT_ASTC_10x5_SRGB_BLOCK] = astc_srgb(10, 5);
    map[VK_FORMAT_ASTC_10x6_UNORM_BLOCK] = astc_unorm(10, 6);
    map[VK_FORMAT_ASTC_10x6_SRGB_BLOCK] = astc_srgb(10, 6);
    map[VK_FORMAT_ASTC_10x8_UNORM_BLOCK] = astc_unorm(10, 8);
    map[VK_FORMAT_ASTC_10x8_SRGB_BLOCK] = astc_srgb(10, 8);
    map[VK_FORMAT_ASTC_10x10_UNORM_BLOCK] = astc_unorm(10, 10);
    map[VK_FORMAT_ASTC_10x10_SRGB_BLOCK] = astc_srgb(10, 10);
    map[VK_FORMAT_ASTC_12x10_UNORM_BLOCK] = astc_unorm(12, 10);
    map[VK_FORMAT_ASTC_12x10_SRGB_BLOCK] = astc_srgb(12, 10);
    map[VK_FORMAT_ASTC_12x12_UNORM_BLOCK] = astc_unorm(12, 12);
    map[VK_FORMAT_ASTC_12x12_SRGB_BLOCK] = astc_srgb(12, 12);
  }

  return map;
}

FormatProperties GetFormatProperties(Vulkan &vk, VkFormat format) {
  FormatProperties fp;

  memset(&fp, 0, sizeof(fp));

  auto &builtin = Builtin();
  auto builtin_find = builtin.find(format);
  if(builtin_find != builtin.end()) {
    fp = builtin_find->second;
  }

  VkFormatProperties vkfp;
  vk->getPhysicalDeviceFormatProperties(vk->physical_device, format, &vkfp);

  fp.linear_tiling_features = vkfp.linearTilingFeatures;
  fp.optimal_tiling_featrures = vkfp.optimalTilingFeatures;
  fp.buffer_features = vkfp.bufferFeatures;

  return fp;
}

}
}
