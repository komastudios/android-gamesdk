// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <gtest/gtest.h>

#include <xnnpack/common.h>
#include <xnnpack/isa-checks.h>

#include <xnnpack/pad.h>
#include "pad-microkernel-tester.h"


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(XX_PAD__NEON, fulltile_copy_channels_eq_16) {
    TEST_REQUIRES_ARM_NEON;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(16)
      .Test(xnn_xx_pad_ukernel__neon);
  }

  TEST(XX_PAD__NEON, fulltile_copy_channels_div_16) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t channels = 32; channels <= 48; channels += 16) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(channels)
        .Test(xnn_xx_pad_ukernel__neon);
    }
  }

  TEST(XX_PAD__NEON, fulltile_copy_channels_lt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t channels = 1; channels < 16; channels++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(channels)
        .Test(xnn_xx_pad_ukernel__neon);
    }
  }

  TEST(XX_PAD__NEON, fulltile_copy_channels_gt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t channels = 17; channels < 32; channels++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(channels)
        .Test(xnn_xx_pad_ukernel__neon);
    }
  }

  TEST(XX_PAD__NEON, fulltile_pre_padding_eq_1) {
    TEST_REQUIRES_ARM_NEON;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(1)
      .Test(xnn_xx_pad_ukernel__neon);
  }

  TEST(XX_PAD__NEON, fulltile_pre_padding_eq_2) {
    TEST_REQUIRES_ARM_NEON;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(2)
      .Test(xnn_xx_pad_ukernel__neon);
  }

  TEST(XX_PAD__NEON, fulltile_pre_padding_eq_4) {
    TEST_REQUIRES_ARM_NEON;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(4)
      .Test(xnn_xx_pad_ukernel__neon);
  }

  TEST(XX_PAD__NEON, fulltile_pre_padding_eq_16) {
    TEST_REQUIRES_ARM_NEON;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(16)
      .Test(xnn_xx_pad_ukernel__neon);
  }

  TEST(XX_PAD__NEON, fulltile_pre_padding_div_16) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t pre_padding = 32; pre_padding <= 48; pre_padding += 16) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .pre_padding(pre_padding)
        .Test(xnn_xx_pad_ukernel__neon);
    }
  }

  TEST(XX_PAD__NEON, fulltile_pre_padding_lt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t pre_padding = 1; pre_padding < 16; pre_padding++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .pre_padding(pre_padding)
        .Test(xnn_xx_pad_ukernel__neon);
    }
  }

  TEST(XX_PAD__NEON, fulltile_pre_padding_gt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t pre_padding = 17; pre_padding < 32; pre_padding++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .pre_padding(pre_padding)
        .Test(xnn_xx_pad_ukernel__neon);
    }
  }

  TEST(XX_PAD__NEON, fulltile_post_padding_eq_1) {
    TEST_REQUIRES_ARM_NEON;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(1)
      .Test(xnn_xx_pad_ukernel__neon);
  }

  TEST(XX_PAD__NEON, fulltile_post_padding_eq_2) {
    TEST_REQUIRES_ARM_NEON;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(2)
      .Test(xnn_xx_pad_ukernel__neon);
  }

  TEST(XX_PAD__NEON, fulltile_post_padding_eq_4) {
    TEST_REQUIRES_ARM_NEON;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(4)
      .Test(xnn_xx_pad_ukernel__neon);
  }

  TEST(XX_PAD__NEON, fulltile_post_padding_eq_16) {
    TEST_REQUIRES_ARM_NEON;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(16)
      .Test(xnn_xx_pad_ukernel__neon);
  }

  TEST(XX_PAD__NEON, fulltile_post_padding_div_16) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t post_padding = 32; post_padding <= 48; post_padding += 16) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .post_padding(post_padding)
        .Test(xnn_xx_pad_ukernel__neon);
    }
  }

  TEST(XX_PAD__NEON, fulltile_post_padding_lt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t post_padding = 1; post_padding < 16; post_padding++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .post_padding(post_padding)
        .Test(xnn_xx_pad_ukernel__neon);
    }
  }

  TEST(XX_PAD__NEON, fulltile_post_padding_gt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t post_padding = 17; post_padding < 32; post_padding++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .pre_padding(post_padding)
        .Test(xnn_xx_pad_ukernel__neon);
    }
  }

  TEST(XX_PAD__NEON, multitile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t rows = 2; rows <= 5; rows++) {
      for (size_t channels = 1; channels < 48; channels += 3) {
        PadMicrokernelTester()
          .rows(rows)
          .input_channels(channels)
          .pre_padding(channels)
          .post_padding(channels)
          .Test(xnn_xx_pad_ukernel__neon);
      }
    }
  }

  TEST(XX_PAD__NEON, multitile_with_input_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t rows = 2; rows <= 5; rows++) {
      for (size_t channels = 1; channels < 48; channels += 3) {
        PadMicrokernelTester()
          .rows(rows)
          .input_channels(channels)
          .pre_padding(channels)
          .post_padding(channels)
          .input_stride(51)
          .Test(xnn_xx_pad_ukernel__neon);
      }
    }
  }

  TEST(XX_PAD__NEON, multitile_with_output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t rows = 2; rows <= 5; rows++) {
      for (size_t channels = 1; channels < 48; channels += 3) {
        PadMicrokernelTester()
          .rows(rows)
          .input_channels(2 * channels)
          .pre_padding(channels)
          .post_padding(channels)
          .output_stride(193)
          .Test(xnn_xx_pad_ukernel__neon);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(XX_PAD__SSE2, fulltile_copy_channels_eq_16) {
    TEST_REQUIRES_X86_SSE2;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(16)
      .Test(xnn_xx_pad_ukernel__sse2);
  }

  TEST(XX_PAD__SSE2, fulltile_copy_channels_div_16) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t channels = 32; channels <= 48; channels += 16) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(channels)
        .Test(xnn_xx_pad_ukernel__sse2);
    }
  }

  TEST(XX_PAD__SSE2, fulltile_copy_channels_lt_16) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t channels = 1; channels < 16; channels++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(channels)
        .Test(xnn_xx_pad_ukernel__sse2);
    }
  }

  TEST(XX_PAD__SSE2, fulltile_copy_channels_gt_16) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t channels = 17; channels < 32; channels++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(channels)
        .Test(xnn_xx_pad_ukernel__sse2);
    }
  }

  TEST(XX_PAD__SSE2, fulltile_pre_padding_eq_1) {
    TEST_REQUIRES_X86_SSE2;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(1)
      .Test(xnn_xx_pad_ukernel__sse2);
  }

  TEST(XX_PAD__SSE2, fulltile_pre_padding_eq_2) {
    TEST_REQUIRES_X86_SSE2;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(2)
      .Test(xnn_xx_pad_ukernel__sse2);
  }

  TEST(XX_PAD__SSE2, fulltile_pre_padding_eq_4) {
    TEST_REQUIRES_X86_SSE2;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(4)
      .Test(xnn_xx_pad_ukernel__sse2);
  }

  TEST(XX_PAD__SSE2, fulltile_pre_padding_eq_16) {
    TEST_REQUIRES_X86_SSE2;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(16)
      .Test(xnn_xx_pad_ukernel__sse2);
  }

  TEST(XX_PAD__SSE2, fulltile_pre_padding_div_16) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t pre_padding = 32; pre_padding <= 48; pre_padding += 16) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .pre_padding(pre_padding)
        .Test(xnn_xx_pad_ukernel__sse2);
    }
  }

  TEST(XX_PAD__SSE2, fulltile_pre_padding_lt_16) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t pre_padding = 1; pre_padding < 16; pre_padding++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .pre_padding(pre_padding)
        .Test(xnn_xx_pad_ukernel__sse2);
    }
  }

  TEST(XX_PAD__SSE2, fulltile_pre_padding_gt_16) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t pre_padding = 17; pre_padding < 32; pre_padding++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .pre_padding(pre_padding)
        .Test(xnn_xx_pad_ukernel__sse2);
    }
  }

  TEST(XX_PAD__SSE2, fulltile_post_padding_eq_1) {
    TEST_REQUIRES_X86_SSE2;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(1)
      .Test(xnn_xx_pad_ukernel__sse2);
  }

  TEST(XX_PAD__SSE2, fulltile_post_padding_eq_2) {
    TEST_REQUIRES_X86_SSE2;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(2)
      .Test(xnn_xx_pad_ukernel__sse2);
  }

  TEST(XX_PAD__SSE2, fulltile_post_padding_eq_4) {
    TEST_REQUIRES_X86_SSE2;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(4)
      .Test(xnn_xx_pad_ukernel__sse2);
  }

  TEST(XX_PAD__SSE2, fulltile_post_padding_eq_16) {
    TEST_REQUIRES_X86_SSE2;
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(16)
      .Test(xnn_xx_pad_ukernel__sse2);
  }

  TEST(XX_PAD__SSE2, fulltile_post_padding_div_16) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t post_padding = 32; post_padding <= 48; post_padding += 16) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .post_padding(post_padding)
        .Test(xnn_xx_pad_ukernel__sse2);
    }
  }

  TEST(XX_PAD__SSE2, fulltile_post_padding_lt_16) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t post_padding = 1; post_padding < 16; post_padding++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .post_padding(post_padding)
        .Test(xnn_xx_pad_ukernel__sse2);
    }
  }

  TEST(XX_PAD__SSE2, fulltile_post_padding_gt_16) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t post_padding = 17; post_padding < 32; post_padding++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .pre_padding(post_padding)
        .Test(xnn_xx_pad_ukernel__sse2);
    }
  }

  TEST(XX_PAD__SSE2, multitile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t rows = 2; rows <= 5; rows++) {
      for (size_t channels = 1; channels < 48; channels += 3) {
        PadMicrokernelTester()
          .rows(rows)
          .input_channels(channels)
          .pre_padding(channels)
          .post_padding(channels)
          .Test(xnn_xx_pad_ukernel__sse2);
      }
    }
  }

  TEST(XX_PAD__SSE2, multitile_with_input_stride) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t rows = 2; rows <= 5; rows++) {
      for (size_t channels = 1; channels < 48; channels += 3) {
        PadMicrokernelTester()
          .rows(rows)
          .input_channels(channels)
          .pre_padding(channels)
          .post_padding(channels)
          .input_stride(51)
          .Test(xnn_xx_pad_ukernel__sse2);
      }
    }
  }

  TEST(XX_PAD__SSE2, multitile_with_output_stride) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t rows = 2; rows <= 5; rows++) {
      for (size_t channels = 1; channels < 48; channels += 3) {
        PadMicrokernelTester()
          .rows(rows)
          .input_channels(2 * channels)
          .pre_padding(channels)
          .post_padding(channels)
          .output_stride(193)
          .Test(xnn_xx_pad_ukernel__sse2);
      }
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_WASMSIMD
  TEST(XX_PAD__WASMSIMD, fulltile_copy_channels_eq_16) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(16)
      .Test(xnn_xx_pad_ukernel__wasmsimd);
  }

  TEST(XX_PAD__WASMSIMD, fulltile_copy_channels_div_16) {
    for (size_t channels = 32; channels <= 48; channels += 16) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(channels)
        .Test(xnn_xx_pad_ukernel__wasmsimd);
    }
  }

  TEST(XX_PAD__WASMSIMD, fulltile_copy_channels_lt_16) {
    for (size_t channels = 1; channels < 16; channels++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(channels)
        .Test(xnn_xx_pad_ukernel__wasmsimd);
    }
  }

  TEST(XX_PAD__WASMSIMD, fulltile_copy_channels_gt_16) {
    for (size_t channels = 17; channels < 32; channels++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(channels)
        .Test(xnn_xx_pad_ukernel__wasmsimd);
    }
  }

  TEST(XX_PAD__WASMSIMD, fulltile_pre_padding_eq_1) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(1)
      .Test(xnn_xx_pad_ukernel__wasmsimd);
  }

  TEST(XX_PAD__WASMSIMD, fulltile_pre_padding_eq_2) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(2)
      .Test(xnn_xx_pad_ukernel__wasmsimd);
  }

  TEST(XX_PAD__WASMSIMD, fulltile_pre_padding_eq_4) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(4)
      .Test(xnn_xx_pad_ukernel__wasmsimd);
  }

  TEST(XX_PAD__WASMSIMD, fulltile_pre_padding_eq_16) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(16)
      .Test(xnn_xx_pad_ukernel__wasmsimd);
  }

  TEST(XX_PAD__WASMSIMD, fulltile_pre_padding_div_16) {
    for (size_t pre_padding = 32; pre_padding <= 48; pre_padding += 16) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .pre_padding(pre_padding)
        .Test(xnn_xx_pad_ukernel__wasmsimd);
    }
  }

  TEST(XX_PAD__WASMSIMD, fulltile_pre_padding_lt_16) {
    for (size_t pre_padding = 1; pre_padding < 16; pre_padding++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .pre_padding(pre_padding)
        .Test(xnn_xx_pad_ukernel__wasmsimd);
    }
  }

  TEST(XX_PAD__WASMSIMD, fulltile_pre_padding_gt_16) {
    for (size_t pre_padding = 17; pre_padding < 32; pre_padding++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .pre_padding(pre_padding)
        .Test(xnn_xx_pad_ukernel__wasmsimd);
    }
  }

  TEST(XX_PAD__WASMSIMD, fulltile_post_padding_eq_1) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(1)
      .Test(xnn_xx_pad_ukernel__wasmsimd);
  }

  TEST(XX_PAD__WASMSIMD, fulltile_post_padding_eq_2) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(2)
      .Test(xnn_xx_pad_ukernel__wasmsimd);
  }

  TEST(XX_PAD__WASMSIMD, fulltile_post_padding_eq_4) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(4)
      .Test(xnn_xx_pad_ukernel__wasmsimd);
  }

  TEST(XX_PAD__WASMSIMD, fulltile_post_padding_eq_16) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(16)
      .Test(xnn_xx_pad_ukernel__wasmsimd);
  }

  TEST(XX_PAD__WASMSIMD, fulltile_post_padding_div_16) {
    for (size_t post_padding = 32; post_padding <= 48; post_padding += 16) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .post_padding(post_padding)
        .Test(xnn_xx_pad_ukernel__wasmsimd);
    }
  }

  TEST(XX_PAD__WASMSIMD, fulltile_post_padding_lt_16) {
    for (size_t post_padding = 1; post_padding < 16; post_padding++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .post_padding(post_padding)
        .Test(xnn_xx_pad_ukernel__wasmsimd);
    }
  }

  TEST(XX_PAD__WASMSIMD, fulltile_post_padding_gt_16) {
    for (size_t post_padding = 17; post_padding < 32; post_padding++) {
      PadMicrokernelTester()
        .rows(1)
        .input_channels(1)
        .pre_padding(post_padding)
        .Test(xnn_xx_pad_ukernel__wasmsimd);
    }
  }

  TEST(XX_PAD__WASMSIMD, multitile) {
    for (size_t rows = 2; rows <= 5; rows++) {
      for (size_t channels = 1; channels < 48; channels += 3) {
        PadMicrokernelTester()
          .rows(rows)
          .input_channels(channels)
          .pre_padding(channels)
          .post_padding(channels)
          .Test(xnn_xx_pad_ukernel__wasmsimd);
      }
    }
  }

  TEST(XX_PAD__WASMSIMD, multitile_with_input_stride) {
    for (size_t rows = 2; rows <= 5; rows++) {
      for (size_t channels = 1; channels < 48; channels += 3) {
        PadMicrokernelTester()
          .rows(rows)
          .input_channels(channels)
          .pre_padding(channels)
          .post_padding(channels)
          .input_stride(51)
          .Test(xnn_xx_pad_ukernel__wasmsimd);
      }
    }
  }

  TEST(XX_PAD__WASMSIMD, multitile_with_output_stride) {
    for (size_t rows = 2; rows <= 5; rows++) {
      for (size_t channels = 1; channels < 48; channels += 3) {
        PadMicrokernelTester()
          .rows(rows)
          .input_channels(2 * channels)
          .pre_padding(channels)
          .post_padding(channels)
          .output_stride(193)
          .Test(xnn_xx_pad_ukernel__wasmsimd);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD


TEST(XX_PAD__SCALAR, fulltile_copy_channels_eq_16) {
  PadMicrokernelTester()
    .rows(1)
    .input_channels(16)
    .Test(xnn_xx_pad_ukernel__scalar);
}

TEST(XX_PAD__SCALAR, fulltile_copy_channels_div_16) {
  for (size_t channels = 32; channels <= 48; channels += 16) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(channels)
      .Test(xnn_xx_pad_ukernel__scalar);
  }
}

TEST(XX_PAD__SCALAR, fulltile_copy_channels_lt_16) {
  for (size_t channels = 1; channels < 16; channels++) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(channels)
      .Test(xnn_xx_pad_ukernel__scalar);
  }
}

TEST(XX_PAD__SCALAR, fulltile_copy_channels_gt_16) {
  for (size_t channels = 17; channels < 32; channels++) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(channels)
      .Test(xnn_xx_pad_ukernel__scalar);
  }
}

TEST(XX_PAD__SCALAR, fulltile_pre_padding_eq_1) {
  PadMicrokernelTester()
    .rows(1)
    .input_channels(1)
    .pre_padding(1)
    .Test(xnn_xx_pad_ukernel__scalar);
}

TEST(XX_PAD__SCALAR, fulltile_pre_padding_eq_2) {
  PadMicrokernelTester()
    .rows(1)
    .input_channels(1)
    .pre_padding(2)
    .Test(xnn_xx_pad_ukernel__scalar);
}

TEST(XX_PAD__SCALAR, fulltile_pre_padding_eq_4) {
  PadMicrokernelTester()
    .rows(1)
    .input_channels(1)
    .pre_padding(4)
    .Test(xnn_xx_pad_ukernel__scalar);
}

TEST(XX_PAD__SCALAR, fulltile_pre_padding_div_4) {
  for (size_t pre_padding = 8; pre_padding <= 12; pre_padding += 4) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(pre_padding)
      .Test(xnn_xx_pad_ukernel__scalar);
  }
}

TEST(XX_PAD__SCALAR, fulltile_pre_padding_lt_4) {
  for (size_t pre_padding = 1; pre_padding < 4; pre_padding++) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(pre_padding)
      .Test(xnn_xx_pad_ukernel__scalar);
  }
}

TEST(XX_PAD__SCALAR, fulltile_pre_padding_gt_4) {
  for (size_t pre_padding = 5; pre_padding < 8; pre_padding++) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(pre_padding)
      .Test(xnn_xx_pad_ukernel__scalar);
  }
}

TEST(XX_PAD__SCALAR, fulltile_post_padding_eq_1) {
  PadMicrokernelTester()
    .rows(1)
    .input_channels(1)
    .post_padding(1)
    .Test(xnn_xx_pad_ukernel__scalar);
}

TEST(XX_PAD__SCALAR, fulltile_post_padding_eq_2) {
  PadMicrokernelTester()
    .rows(1)
    .input_channels(1)
    .post_padding(2)
    .Test(xnn_xx_pad_ukernel__scalar);
}

TEST(XX_PAD__SCALAR, fulltile_post_padding_eq_4) {
  PadMicrokernelTester()
    .rows(1)
    .input_channels(1)
    .post_padding(4)
    .Test(xnn_xx_pad_ukernel__scalar);
}

TEST(XX_PAD__SCALAR, fulltile_post_padding_div_4) {
  for (size_t post_padding = 8; post_padding <= 12; post_padding += 4) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(post_padding)
      .Test(xnn_xx_pad_ukernel__scalar);
  }
}

TEST(XX_PAD__SCALAR, fulltile_post_padding_lt_4) {
  for (size_t post_padding = 1; post_padding < 4; post_padding++) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .post_padding(post_padding)
      .Test(xnn_xx_pad_ukernel__scalar);
  }
}

TEST(XX_PAD__SCALAR, fulltile_post_padding_gt_4) {
  for (size_t post_padding = 5; post_padding < 8; post_padding++) {
    PadMicrokernelTester()
      .rows(1)
      .input_channels(1)
      .pre_padding(post_padding)
      .Test(xnn_xx_pad_ukernel__scalar);
  }
}

TEST(XX_PAD__SCALAR, multitile) {
  for (size_t rows = 2; rows <= 5; rows++) {
    for (size_t channels = 1; channels < 12; channels += 3) {
      PadMicrokernelTester()
        .rows(rows)
        .input_channels(channels)
        .pre_padding(channels)
        .post_padding(channels)
        .Test(xnn_xx_pad_ukernel__scalar);
    }
  }
}

TEST(XX_PAD__SCALAR, multitile_with_input_stride) {
  for (size_t rows = 2; rows <= 5; rows++) {
    for (size_t channels = 1; channels < 12; channels += 3) {
      PadMicrokernelTester()
        .rows(rows)
        .input_channels(channels)
        .pre_padding(channels)
        .post_padding(channels)
        .input_stride(51)
        .Test(xnn_xx_pad_ukernel__scalar);
    }
  }
}

TEST(XX_PAD__SCALAR, multitile_with_output_stride) {
  for (size_t rows = 2; rows <= 5; rows++) {
    for (size_t channels = 1; channels < 12; channels += 3) {
      PadMicrokernelTester()
        .rows(rows)
        .input_channels(2 * channels)
        .pre_padding(channels)
        .post_padding(channels)
        .output_stride(193)
        .Test(xnn_xx_pad_ukernel__scalar);
    }
  }
}
