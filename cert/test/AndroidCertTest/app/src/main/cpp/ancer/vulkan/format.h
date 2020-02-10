#ifndef AGDC_ANCER_FORMAT_H_
#define AGDC_ANCER_FORMAT_H_

#include "vulkan_base.h"

namespace ancer {
namespace vulkan {

/**
 * given a format of 8,16,24,32 and other widths, this describes how those bits
 * are decoded
 */
enum class NumericFormat {
  UNorm,
  SNorm,
  UScaled,
  SScaled,
  UInt,
  SInt,
  UFloat,
  SFloat,
  SRGB
};

/**
 * the output of sampling over a format, directly related to NumericFormat.
 */
enum class SampleType {
  Int,
  Float,
  DepthStencil
};

/**
 * The style of compression, if any, of this format
 */
enum class CompressionScheme {
  None,
  BC,
  ETC2,
  EAC,
  ASTC
};

/**
 * Properties of a format, features is read from the Vulkan API the rest are
 * static from the format itself.
 */
struct FormatProperties {
  uint32_t size_in_bytes;
  uint32_t block_width;
  uint32_t block_height;
  NumericFormat numeric_format;
  SampleType sample_type;
  CompressionScheme compression_scheme;
  VkFormatFeatureFlags linear_tiling_features;
  VkFormatFeatureFlags optimal_tiling_featrures;
  VkFormatFeatureFlags buffer_features;
};

/**
 * Generate a FormatProperties from Vulkan API and feature.
 */
FormatProperties GetFormatProperties(Vulkan &vk, VkFormat format);

}
}

#endif
