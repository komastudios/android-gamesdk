#include "device_info/texture_test_cases.h"
#include "device_info/device_info.h"

// clang-format off
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>
// clang-format on

namespace {
template <typename T>
constexpr auto sizeof_array(const T& iarray) {
  return (sizeof(iarray) / sizeof(iarray[0]));
}
}  // namespace

namespace androidgamesdk_deviceinfo {

// Dummy textures used for offscreen rendering tests.
// Include files were generated with xxd.
#include "device_info/textures/green32-astc-4x4.astc.h"
#include "device_info/textures/green32-astc-6x6.astc.h"
#include "device_info/textures/green32-astc-8x8.astc.h"
#include "device_info/textures/green32-dxt1-bc1.pvr.h"
#include "device_info/textures/green32-dxt5-bc3.expected_rgba.h"
#include "device_info/textures/green32-dxt5-bc3.pvr.h"
#include "device_info/textures/green32-etc1-rgb.pvr.h"
#include "device_info/textures/green32-etc2.pvr.h"
#include "device_info/textures/green32-pvrtci-2bpp-RGB.expected_rgba.h"
#include "device_info/textures/green32-pvrtci-2bpp-RGB.pvr.h"
#include "device_info/textures/green32-pvrtci-2bpp-RGBA.pvr.h"
#include "device_info/textures/green32-pvrtci-4bpp-RGB.expected_rgba.h"
#include "device_info/textures/green32-pvrtci-4bpp-RGB.pvr.h"
#include "device_info/textures/green32-pvrtci-4bpp-RGBA.pvr.h"
constexpr GLsizei greenTextureWidth = 32;
constexpr GLsizei greenTextureHeight = 32;
constexpr int greenTextureRed = 34;
constexpr int greenTextureGreen = 177;
constexpr int greenTextureBlue = 76;

// Sanity check for the *_expected_rgba. If this is erroring, you either
// modified VIEW_WIDTH or VIEW_HEIGHT without updating *_expected_rgba, or
// wrongly updated *_expected_rgba (check the size of the array).
constexpr size_t expected_rgba_len = VIEW_WIDTH * VIEW_HEIGHT * sizeof(RGBA);
static_assert(
    sizeof_array(green32_dxt5_bc3_expected_rgba) == expected_rgba_len,
    "green32_dxt5_bc3_expected_rgba size is inconsistent with test viewport");
static_assert(sizeof_array(green32_pvrtci_2bpp_RGB_expected_rgba) ==
                  expected_rgba_len,
              "green32_pvrtci_2bpp_RGB_expected_rgba size is inconsistent with "
              "test viewport");
static_assert(sizeof_array(green32_pvrtci_4bpp_RGB_expected_rgba) ==
                  expected_rgba_len,
              "green32_pvrtci_4bpp_RGB_expected_rgba size is inconsistent with "
              "test viewport");

// An ideal rendering would be the exact color of the texture without deltas
RGB expectedPixelsColor{greenTextureRed, greenTextureGreen, greenTextureBlue};
RGB noDeltaAllowed{0, 0, 0};

constexpr size_t astcHeaderSize = 16;
constexpr size_t pvrHeaderSize = 52;  // No metadata in our PVR files
RGB pvrtcAllowedDelta{2, 1, 1};
constexpr size_t pvrDxtHeaderSize = 52;  // No metadata in our PVR files
RGB dxtAllowedDelta{1, 1, 2};
constexpr size_t pvrEtc2HeaderSize = 52;  // No metadata in our PVR files
constexpr size_t pvrEtc1HeaderSize = 52;  // No metadata in our PVR files

constexpr size_t allCompressedTextureTestCasesCount = 11;
TextureTestCase
    allCompressedTextureTestCases[allCompressedTextureTestCasesCount] = {
        {GL_COMPRESSED_RGBA_ASTC_4x4_KHR, greenTextureWidth, greenTextureHeight,
         green32_astc_4x4_astc_len - astcHeaderSize,
         &green32_astc_4x4_astc[astcHeaderSize], expectedPixelsColor,
         noDeltaAllowed},
        {GL_COMPRESSED_RGBA_ASTC_6x6_KHR, greenTextureWidth, greenTextureHeight,
         green32_astc_6x6_astc_len - astcHeaderSize,
         &green32_astc_6x6_astc[astcHeaderSize], expectedPixelsColor,
         noDeltaAllowed},
        {GL_COMPRESSED_RGBA_ASTC_8x8_KHR, greenTextureWidth, greenTextureHeight,
         green32_astc_8x8_astc_len - astcHeaderSize,
         &green32_astc_8x8_astc[astcHeaderSize], expectedPixelsColor,
         noDeltaAllowed},

        // PVRTC tests
        {GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, greenTextureWidth,
         greenTextureHeight, green32_pvrtci_2bpp_RGB_pvr_len - pvrHeaderSize,
         &green32_pvrtci_2bpp_RGB_pvr[pvrHeaderSize], expectedPixelsColor,
         pvrtcAllowedDelta, (RGBA*)green32_pvrtci_2bpp_RGB_expected_rgba},
        {GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, greenTextureWidth,
         greenTextureHeight, green32_pvrtci_2bpp_RGBA_pvr_len - pvrHeaderSize,
         &green32_pvrtci_2bpp_RGBA_pvr[pvrHeaderSize], expectedPixelsColor,
         pvrtcAllowedDelta, (RGBA*)green32_pvrtci_2bpp_RGB_expected_rgba},
        {GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, greenTextureWidth,
         greenTextureHeight, green32_pvrtci_4bpp_RGB_pvr_len - pvrHeaderSize,
         &green32_pvrtci_4bpp_RGB_pvr[pvrHeaderSize], expectedPixelsColor,
         pvrtcAllowedDelta, (RGBA*)green32_pvrtci_4bpp_RGB_expected_rgba},
        {GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, greenTextureWidth,
         greenTextureHeight, green32_pvrtci_4bpp_RGBA_pvr_len - pvrHeaderSize,
         &green32_pvrtci_4bpp_RGBA_pvr[pvrHeaderSize], expectedPixelsColor,
         pvrtcAllowedDelta, (RGBA*)green32_pvrtci_4bpp_RGB_expected_rgba},

        // DXT tests
        {GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, greenTextureWidth,
         greenTextureHeight, green32_dxt1_bc1_pvr_len - pvrDxtHeaderSize,
         &green32_dxt1_bc1_pvr[pvrDxtHeaderSize], expectedPixelsColor,
         dxtAllowedDelta},
        {GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, greenTextureWidth,
         greenTextureHeight, green32_dxt5_bc3_pvr_len - pvrDxtHeaderSize,
         &green32_dxt5_bc3_pvr[pvrDxtHeaderSize], expectedPixelsColor,
         dxtAllowedDelta, (RGBA*)green32_dxt5_bc3_expected_rgba},

        // ETC2 tests
        {GL_COMPRESSED_RGB8_ETC2,
         greenTextureWidth,
         greenTextureHeight,
         green32_etc2_pvr_len - pvrEtc2HeaderSize,
         &green32_etc2_pvr[pvrEtc2HeaderSize],
         {greenTextureRed - 2, greenTextureGreen, greenTextureBlue + 1},
         noDeltaAllowed},

        // ETC1 tests
        {GL_ETC1_RGB8_OES,
         greenTextureWidth,
         greenTextureHeight,
         green32_etc1_rgb_pvr_len - pvrEtc1HeaderSize,
         &green32_etc1_rgb_pvr[pvrEtc1HeaderSize],
         {greenTextureRed + 1, greenTextureGreen - 2, greenTextureBlue},
         noDeltaAllowed}};

TextureTestCase* getCompressedTextureTestCase(GLenum internalformat) {
  for (size_t i = 0; i < allCompressedTextureTestCasesCount; ++i) {
    if (allCompressedTextureTestCases[i].internalformat == internalformat) {
      return &allCompressedTextureTestCases[i];
    }
  }

  return nullptr;
}

}  // namespace androidgamesdk_deviceinfo
