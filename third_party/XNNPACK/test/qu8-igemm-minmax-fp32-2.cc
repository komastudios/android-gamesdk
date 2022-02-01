// Copyright (c) Facebook, Inc. and its affiliates.
// All rights reserved.
//
// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.
//
// Auto-generated file. Do not edit!
//   Specification: test/qu8-igemm-minmax-fp32.yaml
//   Generator: tools/generate-gemm-test.py


#include <gtest/gtest.h>

#include <xnnpack/allocator.h>
#include <xnnpack/common.h>
#include <xnnpack/isa-checks.h>

#include <xnnpack/gemm.h>
#include <xnnpack/igemm.h>
#include <xnnpack/ppmm.h>
#include "gemm-microkernel-tester.h"


#if XNN_ARCH_ARM64 && XNN_ENABLE_ASSEMBLY
  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, k_eq_16) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(4)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(4)
      .n(16)
      .k(16)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, strided_cn) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(4)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(4)
      .n(16)
      .k(16)
      .cn_stride(19)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, k_eq_16_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 1; n <= 16; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(m)
          .n(n)
          .k(16)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, k_eq_16_subtile_m) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t m = 1; m <= 4; m++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(m)
        .n(16)
        .k(16)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, k_eq_16_subtile_n) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 1; n <= 16; n++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(n)
        .k(16)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, k_lt_16) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, k_lt_16_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k < 16; k++) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, k_gt_16) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 17; k < 32; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, k_gt_16_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 17; k < 32; k++) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, k_div_16) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 32; k <= 160; k += 16) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, k_div_16_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 32; k <= 160; k += 16) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, n_gt_16) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 80; k += 17) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, n_gt_16_strided_cn) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 80; k += 17) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(19)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, n_gt_16_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 80; k += 17) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, n_div_16) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 80; k += 17) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, n_div_16_strided_cn) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 80; k += 17) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(19)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, n_div_16_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 80; k += 17) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, small_kernel) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 80; k += 17) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, small_kernel_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 80; k += 17) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, n_gt_16_small_kernel) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 80; k += 17) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, n_div_16_small_kernel) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 80; k += 17) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, strided_cm_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 80; k += 17) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(19)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, a_offset) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 80; k += 17) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .ks(3)
        .a_offset(331)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, zero) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 80; k += 17) {
      for (uint32_t mz = 0; mz < 4; mz++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(16)
          .k(k)
          .ks(3)
          .a_offset(331)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, qmin) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(4)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(4)
      .n(16)
      .k(16)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, qmax) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(4)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(4)
      .n(16)
      .k(16)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, strided_cm) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(4)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(4)
      .n(16)
      .k(16)
      .cm_stride(19)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, no_a_zero_point) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 80; k += 17) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, no_b_zero_point) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 80; k += 17) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__AARCH64_NEONDOT_CORTEX_A55, no_zero_point) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 80; k += 17) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__aarch64_neondot_cortex_a55, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_ARM64 && XNN_ENABLE_ASSEMBLY


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, k_eq_8) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(1)
      .nr(8)
      .kr(1)
      .sr(1)
      .m(1)
      .n(8)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, strided_cn) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(1)
      .nr(8)
      .kr(1)
      .sr(1)
      .m(1)
      .n(8)
      .k(8)
      .cn_stride(11)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, k_eq_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n <= 8; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, k_eq_8_subtile_m) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(m)
        .n(8)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, k_eq_8_subtile_n) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n <= 8; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, k_lt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(1)
        .n(8)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, k_lt_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, k_gt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(1)
        .n(8)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, k_gt_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, k_div_8) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(1)
        .n(8)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, k_div_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, n_gt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, n_gt_8_strided_cn) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(11)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, n_gt_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, n_div_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, n_div_8_strided_cn) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(11)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, n_div_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, small_kernel) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(1)
        .n(8)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, small_kernel_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, n_gt_8_small_kernel) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, n_div_8_small_kernel) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, strided_cm_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(11)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, a_offset) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(1)
        .n(8)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, zero) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(1)
          .n(8)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, qmin) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(1)
      .nr(8)
      .kr(1)
      .sr(1)
      .m(1)
      .n(8)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, qmax) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(1)
      .nr(8)
      .kr(1)
      .sr(1)
      .m(1)
      .n(8)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, strided_cm) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(1)
      .nr(8)
      .kr(1)
      .sr(1)
      .m(1)
      .n(8)
      .k(8)
      .cm_stride(11)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, no_a_zero_point) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(1)
        .n(8)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, no_b_zero_point) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(1)
        .n(8)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X8__NEON_MLAL_LANE, no_zero_point) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(1)
        .n(8)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, k_eq_8) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(4)
      .nr(8)
      .kr(1)
      .sr(1)
      .m(4)
      .n(8)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, strided_cn) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(4)
      .nr(8)
      .kr(1)
      .sr(1)
      .m(4)
      .n(8)
      .k(8)
      .cn_stride(11)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, k_eq_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n <= 8; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, k_eq_8_subtile_m) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m <= 4; m++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(m)
        .n(8)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, k_eq_8_subtile_n) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n <= 8; n++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, k_lt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(4)
        .n(8)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, k_lt_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, k_gt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(4)
        .n(8)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, k_gt_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, k_div_8) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(4)
        .n(8)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, k_div_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, n_gt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, n_gt_8_strided_cn) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(11)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, n_gt_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, n_div_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, n_div_8_strided_cn) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(11)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, n_div_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, small_kernel) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(4)
        .n(8)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, small_kernel_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, n_gt_8_small_kernel) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, n_div_8_small_kernel) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, strided_cm_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(8)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(11)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, a_offset) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(4)
        .n(8)
        .k(k)
        .ks(3)
        .a_offset(163)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, zero) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 4; mz++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(8)
          .kr(1)
          .sr(1)
          .m(4)
          .n(8)
          .k(k)
          .ks(3)
          .a_offset(163)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, qmin) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(4)
      .nr(8)
      .kr(1)
      .sr(1)
      .m(4)
      .n(8)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, qmax) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(4)
      .nr(8)
      .kr(1)
      .sr(1)
      .m(4)
      .n(8)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, strided_cm) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(4)
      .nr(8)
      .kr(1)
      .sr(1)
      .m(4)
      .n(8)
      .k(8)
      .cm_stride(11)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, no_a_zero_point) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(4)
        .n(8)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, no_b_zero_point) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(4)
        .n(8)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X8__NEON_MLAL_LANE, no_zero_point) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(8)
        .kr(1)
        .sr(1)
        .m(4)
        .n(8)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x8__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, k_eq_8) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(1)
      .nr(16)
      .kr(1)
      .sr(1)
      .m(1)
      .n(16)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, strided_cn) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(1)
      .nr(16)
      .kr(1)
      .sr(1)
      .m(1)
      .n(16)
      .k(8)
      .cn_stride(19)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, k_eq_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n <= 16; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(16)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, k_eq_8_subtile_m) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(16)
        .kr(1)
        .sr(1)
        .m(m)
        .n(16)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, k_eq_8_subtile_n) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n <= 16; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(16)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, k_lt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(16)
        .kr(1)
        .sr(1)
        .m(1)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, k_lt_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(16)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, k_gt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(16)
        .kr(1)
        .sr(1)
        .m(1)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, k_gt_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(16)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, k_div_8) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(16)
        .kr(1)
        .sr(1)
        .m(1)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, k_div_8_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(16)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, n_gt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(16)
          .kr(1)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, n_gt_16_strided_cn) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(16)
          .kr(1)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(19)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, n_gt_16_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(16)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, n_div_16) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(16)
          .kr(1)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, n_div_16_strided_cn) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(16)
          .kr(1)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(19)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, n_div_16_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(16)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, small_kernel) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(16)
        .kr(1)
        .sr(1)
        .m(1)
        .n(16)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, small_kernel_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(16)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, n_gt_16_small_kernel) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(16)
          .kr(1)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, n_div_16_small_kernel) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(16)
          .kr(1)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, strided_cm_subtile) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(16)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(19)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, a_offset) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(16)
        .kr(1)
        .sr(1)
        .m(1)
        .n(16)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, zero) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(16)
          .kr(1)
          .sr(1)
          .m(1)
          .n(16)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, qmin) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(1)
      .nr(16)
      .kr(1)
      .sr(1)
      .m(1)
      .n(16)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, qmax) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(1)
      .nr(16)
      .kr(1)
      .sr(1)
      .m(1)
      .n(16)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, strided_cm) {
    TEST_REQUIRES_ARM_NEON;
    GemmMicrokernelTester()
      .mr(1)
      .nr(16)
      .kr(1)
      .sr(1)
      .m(1)
      .n(16)
      .k(8)
      .cm_stride(19)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, no_a_zero_point) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(16)
        .kr(1)
        .sr(1)
        .m(1)
        .n(16)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, no_b_zero_point) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(16)
        .kr(1)
        .sr(1)
        .m(1)
        .n(16)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X16__NEON_MLAL_LANE, no_zero_point) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(16)
        .kr(1)
        .sr(1)
        .m(1)
        .n(16)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x16__neon_mlal_lane, xnn_init_qu8_conv_minmax_fp32_neon_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM && !XNN_PLATFORM_IOS || XNN_ARCH_ARM64
  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, k_eq_8) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(2)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(2)
      .n(16)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, strided_cn) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(2)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(2)
      .n(16)
      .k(8)
      .cn_stride(19)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, k_eq_8_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 1; n <= 16; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, k_eq_8_subtile_m) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(m)
        .n(16)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, k_eq_8_subtile_n) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 1; n <= 16; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, k_lt_8) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(2)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, k_lt_8_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, k_gt_8) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(2)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, k_gt_8_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, k_div_8) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(2)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, k_div_8_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, n_gt_16) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, n_gt_16_strided_cn) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(19)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, n_gt_16_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, n_div_16) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, n_div_16_strided_cn) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(19)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, n_div_16_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, small_kernel) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(2)
        .n(16)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, small_kernel_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, n_gt_16_small_kernel) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, n_div_16_small_kernel) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, strided_cm_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(19)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, a_offset) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(2)
        .n(16)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, zero) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(2)
          .n(16)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, qmin) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(2)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(2)
      .n(16)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, qmax) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(2)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(2)
      .n(16)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, strided_cm) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(2)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(2)
      .n(16)
      .k(8)
      .cm_stride(19)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, no_a_zero_point) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(2)
        .n(16)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, no_b_zero_point) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(2)
        .n(16)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X16C4__NEONDOT, no_zero_point) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(2)
        .n(16)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_ARM && !XNN_PLATFORM_IOS || XNN_ARCH_ARM64


#if XNN_ARCH_ARM && !XNN_PLATFORM_IOS || XNN_ARCH_ARM64
  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, k_eq_8) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(4)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(4)
      .n(16)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, strided_cn) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(4)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(4)
      .n(16)
      .k(8)
      .cn_stride(19)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, k_eq_8_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 1; n <= 16; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, k_eq_8_subtile_m) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t m = 1; m <= 4; m++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(m)
        .n(16)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, k_eq_8_subtile_n) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 1; n <= 16; n++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, k_lt_8) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, k_lt_8_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, k_gt_8) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, k_gt_8_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, k_div_8) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, k_div_8_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, n_gt_16) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, n_gt_16_strided_cn) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(19)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, n_gt_16_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, n_div_16) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, n_div_16_strided_cn) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(19)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, n_div_16_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, small_kernel) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, small_kernel_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, n_gt_16_small_kernel) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, n_div_16_small_kernel) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, strided_cm_subtile) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(16)
            .kr(4)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(19)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, a_offset) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .ks(3)
        .a_offset(163)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, zero) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 4; mz++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(16)
          .kr(4)
          .sr(1)
          .m(4)
          .n(16)
          .k(k)
          .ks(3)
          .a_offset(163)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, qmin) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(4)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(4)
      .n(16)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, qmax) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(4)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(4)
      .n(16)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, strided_cm) {
    TEST_REQUIRES_ARM_NEON_DOT;
    GemmMicrokernelTester()
      .mr(4)
      .nr(16)
      .kr(4)
      .sr(1)
      .m(4)
      .n(16)
      .k(8)
      .cm_stride(19)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, no_a_zero_point) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, no_b_zero_point) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X16C4__NEONDOT, no_zero_point) {
    TEST_REQUIRES_ARM_NEON_DOT;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(16)
        .kr(4)
        .sr(1)
        .m(4)
        .n(16)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x16c4__neondot, xnn_init_qu8_conv_minmax_fp32_neonv8_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_ARM && !XNN_PLATFORM_IOS || XNN_ARCH_ARM64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, k_eq_8) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, k_lt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, k_gt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, k_div_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, n_gt_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, n_div_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, a_offset) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, zero) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, qmin) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, qmax) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, strided_cm) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD64, no_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, k_eq_8) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, k_lt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, k_gt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, k_div_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, n_gt_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, n_div_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, a_offset) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, zero) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, qmin) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, qmax) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, strided_cm) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD64, no_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, k_eq_8) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t m = 1; m <= 4; m++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, k_lt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, k_gt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, k_div_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, n_gt_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, n_div_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, a_offset) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(163)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, zero) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 4; mz++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(163)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, qmin) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, qmax) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, strided_cm) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD64, no_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, k_eq_8) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, k_lt_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, k_gt_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, k_div_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, n_gt_4) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, n_div_4) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, a_offset) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(127)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, zero) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(127)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, qmin) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, qmax) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, strided_cm) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__SSE41_LD64, no_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, k_eq_8) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, strided_cn) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, k_eq_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, k_lt_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, k_lt_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, k_gt_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, k_gt_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, k_div_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, k_div_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, n_gt_4) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, n_gt_4_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, n_div_4) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, n_div_4_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, small_kernel_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, strided_cm_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, a_offset) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, zero) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, qmin) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, qmax) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, strided_cm) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, no_a_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, no_b_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD64, no_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, k_eq_8) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, strided_cn) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, k_eq_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, k_lt_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, k_lt_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, k_gt_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, k_gt_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, k_div_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, k_div_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, n_gt_4) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, n_gt_4_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, n_div_4) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, n_div_4_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, small_kernel_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, strided_cm_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, a_offset) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, zero) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, qmin) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, qmax) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, strided_cm) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, no_a_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, no_b_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__XOP_LD64, no_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, k_eq_8) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, k_lt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, k_gt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, k_div_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, n_gt_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, n_div_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, a_offset) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, zero) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, qmin) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, qmax) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, strided_cm) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE2_LD128, no_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, k_eq_8) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, k_lt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, k_gt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, k_div_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, n_gt_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, n_div_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, a_offset) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, zero) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, qmin) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, qmax) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, strided_cm) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE2_LD128, no_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, k_eq_8) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t m = 1; m <= 4; m++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, k_lt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, k_gt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, k_div_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, n_gt_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, n_div_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, a_offset) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(163)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, zero) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 4; mz++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(163)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, qmin) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, qmax) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, strided_cm) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__SSE2_LD128, no_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, k_eq_8) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, k_lt_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, k_gt_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, k_div_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, n_gt_4) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, n_div_4) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, a_offset) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, zero) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, qmin) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, qmax) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, strided_cm) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__SSE41_LD128, no_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, k_eq_8) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, k_lt_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, k_gt_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, k_div_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, n_gt_4) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, n_div_4) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, a_offset) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, zero) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, qmin) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, qmax) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, strided_cm) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__SSE41_LD128, no_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, k_eq_8) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, strided_cn) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, k_lt_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, k_gt_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, k_div_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, n_gt_4) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, n_div_4) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, a_offset) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, zero) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, qmin) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, qmax) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, strided_cm) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__AVX_LD128, no_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, k_eq_8) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, strided_cn) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, k_lt_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, k_gt_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, k_div_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, n_gt_4) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, n_div_4) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, a_offset) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, zero) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, qmin) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, qmax) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, strided_cm) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__AVX_LD128, no_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, k_eq_8) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, strided_cn) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t m = 1; m <= 4; m++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, k_lt_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, k_gt_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, k_div_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, n_gt_4) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, n_div_4) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, a_offset) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(163)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, zero) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 4; mz++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(163)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, qmin) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, qmax) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, strided_cm) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__AVX_LD128, no_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, k_eq_8) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, strided_cn) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, k_lt_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, k_gt_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, k_div_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, n_gt_4) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, n_div_4) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, a_offset) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(127)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, zero) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(127)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, qmin) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, qmax) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, strided_cm) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__XOP_LD128, no_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, k_eq_8) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, k_lt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, k_gt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, k_div_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, n_gt_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, n_div_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, a_offset) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(127)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, zero) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(127)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, qmin) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, qmax) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, strided_cm) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE2_LD64, no_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse2_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, k_eq_8) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, k_lt_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, k_gt_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, k_div_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, n_gt_4) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, n_div_4) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, a_offset) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(127)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, zero) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(127)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, qmin) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, qmax) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, strided_cm) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD64, no_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, k_eq_8) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, strided_cn) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, k_eq_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, k_lt_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, k_lt_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, k_gt_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, k_gt_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, k_div_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, k_div_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, n_gt_4) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, n_gt_4_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, n_div_4) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, n_div_4_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, small_kernel_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, strided_cm_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, a_offset) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, zero) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, qmin) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, qmax) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, strided_cm) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, no_a_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, no_b_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__AVX_LD64, no_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__avx_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, k_eq_8) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, strided_cn) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, k_eq_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, k_lt_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, k_lt_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, k_gt_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, k_gt_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, k_div_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, k_div_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, n_gt_4) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, n_gt_4_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, n_div_4) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, n_div_4_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, small_kernel_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, strided_cm_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, a_offset) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, zero) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, qmin) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, qmax) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, strided_cm) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, no_a_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, no_b_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD64, no_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld64, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, k_eq_8) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, k_lt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, k_gt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, k_div_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, n_gt_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, n_div_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, a_offset) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, zero) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, qmin) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, qmax) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, strided_cm) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__SSE2_LD128, no_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, k_eq_8) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, k_lt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, k_gt_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, k_div_8) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, n_gt_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, n_div_4) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE2;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, a_offset) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, zero) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, qmin) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, qmax) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, strided_cm) {
    TEST_REQUIRES_X86_SSE2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__SSE2_LD128, no_zero_point) {
    TEST_REQUIRES_X86_SSE2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__sse2_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, k_eq_8) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, k_lt_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, k_gt_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, k_div_8) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, n_gt_4) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, n_div_4) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_SSE41;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, a_offset) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(127)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, zero) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(127)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, qmin) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, qmax) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, strided_cm) {
    TEST_REQUIRES_X86_SSE41;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__SSE41_LD128, no_zero_point) {
    TEST_REQUIRES_X86_SSE41;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__sse41_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, k_eq_8) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, strided_cn) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, k_lt_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, k_gt_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, k_div_8) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, n_gt_4) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, n_div_4) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_AVX;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, a_offset) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(127)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, zero) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(127)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, qmin) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, qmax) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, strided_cm) {
    TEST_REQUIRES_X86_AVX;
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__AVX_LD128, no_zero_point) {
    TEST_REQUIRES_X86_AVX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__avx_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, k_eq_8) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, strided_cn) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, k_lt_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, k_gt_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, k_div_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, n_gt_4) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, n_div_4) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, a_offset) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, zero) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, qmin) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, qmax) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, strided_cm) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__XOP_LD128, no_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, k_eq_8) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, strided_cn) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, k_eq_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, k_lt_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, k_lt_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, k_gt_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, k_gt_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, k_div_8) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, k_div_8_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, n_gt_4) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, n_gt_4_strided_cn) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, n_gt_4_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, n_div_4) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, n_div_4_strided_cn) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, n_div_4_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, small_kernel_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, n_gt_4_small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, n_div_4_small_kernel) {
    TEST_REQUIRES_X86_XOP;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, strided_cm_subtile) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, a_offset) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, zero) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, qmin) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, qmax) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, strided_cm) {
    TEST_REQUIRES_X86_XOP;
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, no_a_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, no_b_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__XOP_LD128, no_zero_point) {
    TEST_REQUIRES_X86_XOP;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__xop_ld128, xnn_init_qu8_conv_minmax_fp32_sse2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, k_eq_8) {
    TEST_REQUIRES_X86_AVX2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(8)
      .kr(8)
      .sr(1)
      .m(2)
      .n(8)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, strided_cn) {
    TEST_REQUIRES_X86_AVX2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(8)
      .kr(8)
      .sr(1)
      .m(2)
      .n(8)
      .k(8)
      .cn_stride(11)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, k_eq_8_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 1; n <= 8; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(m)
        .n(8)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 1; n <= 8; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, k_lt_8) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(2)
        .n(8)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, k_lt_8_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, k_gt_8) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(2)
        .n(8)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, k_gt_8_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, k_div_8) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(2)
        .n(8)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, k_div_8_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, n_gt_8) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, n_gt_8_strided_cn) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(11)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, n_gt_8_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, n_div_8) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, n_div_8_strided_cn) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(11)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, n_div_8_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, small_kernel) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(2)
        .n(8)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, small_kernel_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, n_gt_8_small_kernel) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, n_div_8_small_kernel) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, strided_cm_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(11)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, a_offset) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(2)
        .n(8)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, zero) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(2)
          .n(8)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, qmin) {
    TEST_REQUIRES_X86_AVX2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(8)
      .kr(8)
      .sr(1)
      .m(2)
      .n(8)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, qmax) {
    TEST_REQUIRES_X86_AVX2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(8)
      .kr(8)
      .sr(1)
      .m(2)
      .n(8)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, strided_cm) {
    TEST_REQUIRES_X86_AVX2;
    GemmMicrokernelTester()
      .mr(2)
      .nr(8)
      .kr(8)
      .sr(1)
      .m(2)
      .n(8)
      .k(8)
      .cm_stride(11)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, no_a_zero_point) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(2)
        .n(8)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, no_b_zero_point) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(2)
        .n(8)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X8C8__AVX2, no_zero_point) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(2)
        .n(8)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, k_eq_8) {
    TEST_REQUIRES_X86_AVX2;
    GemmMicrokernelTester()
      .mr(3)
      .nr(8)
      .kr(8)
      .sr(1)
      .m(3)
      .n(8)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, strided_cn) {
    TEST_REQUIRES_X86_AVX2;
    GemmMicrokernelTester()
      .mr(3)
      .nr(8)
      .kr(8)
      .sr(1)
      .m(3)
      .n(8)
      .k(8)
      .cn_stride(11)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, k_eq_8_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 1; n <= 8; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(m)
        .n(8)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 1; n <= 8; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(3)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, k_lt_8) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(3)
        .n(8)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, k_lt_8_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, k_gt_8) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(3)
        .n(8)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, k_gt_8_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, k_div_8) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(3)
        .n(8)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, k_div_8_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, n_gt_8) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, n_gt_8_strided_cn) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(11)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, n_gt_8_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, n_div_8) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, n_div_8_strided_cn) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(11)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, n_div_8_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, small_kernel) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(3)
        .n(8)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, small_kernel_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, n_gt_8_small_kernel) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 9; n < 16; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, n_div_8_small_kernel) {
    TEST_REQUIRES_X86_AVX2;
    for (uint32_t n = 16; n <= 24; n += 8) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, strided_cm_subtile) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 8; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(8)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(11)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, a_offset) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(3)
        .n(8)
        .k(k)
        .ks(3)
        .a_offset(127)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, zero) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(8)
          .kr(8)
          .sr(1)
          .m(3)
          .n(8)
          .k(k)
          .ks(3)
          .a_offset(127)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, qmin) {
    TEST_REQUIRES_X86_AVX2;
    GemmMicrokernelTester()
      .mr(3)
      .nr(8)
      .kr(8)
      .sr(1)
      .m(3)
      .n(8)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, qmax) {
    TEST_REQUIRES_X86_AVX2;
    GemmMicrokernelTester()
      .mr(3)
      .nr(8)
      .kr(8)
      .sr(1)
      .m(3)
      .n(8)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, strided_cm) {
    TEST_REQUIRES_X86_AVX2;
    GemmMicrokernelTester()
      .mr(3)
      .nr(8)
      .kr(8)
      .sr(1)
      .m(3)
      .n(8)
      .k(8)
      .cm_stride(11)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, no_a_zero_point) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(3)
        .n(8)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, no_b_zero_point) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(3)
        .n(8)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X8C8__AVX2, no_zero_point) {
    TEST_REQUIRES_X86_AVX2;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(8)
        .kr(8)
        .sr(1)
        .m(3)
        .n(8)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x8c8__avx2, xnn_init_qu8_conv_minmax_fp32_avx2_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, k_eq_8) {
    TEST_REQUIRES_X86_AVX512SKX;
    GemmMicrokernelTester()
      .mr(3)
      .nr(16)
      .kr(8)
      .sr(1)
      .m(3)
      .n(16)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, strided_cn) {
    TEST_REQUIRES_X86_AVX512SKX;
    GemmMicrokernelTester()
      .mr(3)
      .nr(16)
      .kr(8)
      .sr(1)
      .m(3)
      .n(16)
      .k(8)
      .cn_stride(19)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, k_eq_8_subtile) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (uint32_t n = 1; n <= 16; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(16)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, k_eq_8_subtile_m) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(16)
        .kr(8)
        .sr(1)
        .m(m)
        .n(16)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, k_eq_8_subtile_n) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (uint32_t n = 1; n <= 16; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(16)
        .kr(8)
        .sr(1)
        .m(3)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, k_lt_8) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(16)
        .kr(8)
        .sr(1)
        .m(3)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, k_lt_8_subtile) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(16)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, k_gt_8) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(16)
        .kr(8)
        .sr(1)
        .m(3)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, k_gt_8_subtile) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(16)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, k_div_8) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(16)
        .kr(8)
        .sr(1)
        .m(3)
        .n(16)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, k_div_8_subtile) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(16)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, n_gt_16) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(16)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, n_gt_16_strided_cn) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(16)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(19)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, n_gt_16_subtile) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(16)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, n_div_16) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(16)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, n_div_16_strided_cn) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(16)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(19)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, n_div_16_subtile) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(16)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, small_kernel) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(16)
        .kr(8)
        .sr(1)
        .m(3)
        .n(16)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, small_kernel_subtile) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(16)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, n_gt_16_small_kernel) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (uint32_t n = 17; n < 32; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(16)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, n_div_16_small_kernel) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (uint32_t n = 32; n <= 48; n += 16) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(16)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, strided_cm_subtile) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 16; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(16)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(19)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, a_offset) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(16)
        .kr(8)
        .sr(1)
        .m(3)
        .n(16)
        .k(k)
        .ks(3)
        .a_offset(127)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, zero) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(16)
          .kr(8)
          .sr(1)
          .m(3)
          .n(16)
          .k(k)
          .ks(3)
          .a_offset(127)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, qmin) {
    TEST_REQUIRES_X86_AVX512SKX;
    GemmMicrokernelTester()
      .mr(3)
      .nr(16)
      .kr(8)
      .sr(1)
      .m(3)
      .n(16)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, qmax) {
    TEST_REQUIRES_X86_AVX512SKX;
    GemmMicrokernelTester()
      .mr(3)
      .nr(16)
      .kr(8)
      .sr(1)
      .m(3)
      .n(16)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, strided_cm) {
    TEST_REQUIRES_X86_AVX512SKX;
    GemmMicrokernelTester()
      .mr(3)
      .nr(16)
      .kr(8)
      .sr(1)
      .m(3)
      .n(16)
      .k(8)
      .cm_stride(19)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, no_a_zero_point) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(16)
        .kr(8)
        .sr(1)
        .m(3)
        .n(16)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, no_b_zero_point) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(16)
        .kr(8)
        .sr(1)
        .m(3)
        .n(16)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X16C8__AVX512SKX, no_zero_point) {
    TEST_REQUIRES_X86_AVX512SKX;
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(16)
        .kr(8)
        .sr(1)
        .m(3)
        .n(16)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x16c8__avx512skx, xnn_init_qu8_conv_minmax_fp32_avx512_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, k_eq_8) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, strided_cn) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, qmin) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, qmax) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, strided_cm) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD64, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, k_eq_8) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, strided_cn) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 4; m++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 4; m++) {
          GemmMicrokernelTester()
            .mr(4)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(163)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 4; mz++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(4)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(163)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, qmin) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, qmax) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, strided_cm) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(4)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_4X4C2__WASMSIMD_DOT16X2_LD64, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4c2__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, k_eq_8) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, strided_cn) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, qmin) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, qmax) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, strided_cm) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2__WASMSIMD_DOT16X2_LD128, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, k_eq_8) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, strided_cn) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(2)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, qmin) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, qmax) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, strided_cm) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C2__WASMSIMD_DOT16X2_LD128, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, k_eq_8) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, strided_cn) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(127)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(1)
          .m(3)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(127)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, qmin) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, qmax) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, strided_cm) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2__WASMSIMD_DOT16X2_LD128, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, k_eq_8) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(4)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, strided_cn) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(4)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, qmin) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(4)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, qmax) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(4)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, strided_cm) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(2)
      .sr(4)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C2S4__WASMSIMD_DOT16X2_LD128, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, k_eq_8) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(4)
      .m(3)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, strided_cn) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(4)
      .m(3)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(3)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(2)
            .sr(4)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(127)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(2)
          .sr(4)
          .m(3)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(127)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, qmin) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(4)
      .m(3)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, qmax) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(4)
      .m(3)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, strided_cm) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(2)
      .sr(4)
      .m(3)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(3)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C2S4__WASMSIMD_DOT16X2_LD128, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(2)
        .sr(4)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c2s4__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, k_eq_8) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, strided_cn) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, qmin) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, qmax) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, strided_cm) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD64, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, k_eq_8) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, strided_cn) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(127)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(127)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, qmin) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, qmax) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, strided_cm) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD64, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, k_eq_8) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, strided_cn) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, qmin) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, qmax) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, strided_cm) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_DOT16X2_LD128, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, k_eq_8) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, strided_cn) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, qmin) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, qmax) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, strided_cm) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_DOT16X2_LD128, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, k_eq_8) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, strided_cn) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(127)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(3)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(127)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, qmin) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, qmax) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, strided_cm) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(3)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4C8__WASMSIMD_DOT16X2_LD128, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4c8__wasmsimd_dot16x2_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, k_eq_8) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, strided_cn) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 1; m++) {
          GemmMicrokernelTester()
            .mr(1)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(43)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 1; mz++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(1)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(43)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, qmin) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, qmax) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, strided_cm) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(1)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_1X4C8__WASMSIMD_MUL32_LD64, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, k_eq_8) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, strided_cn) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, qmin) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, qmax) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, strided_cm) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD64, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld64, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, k_eq_8) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, strided_cn) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, k_eq_8_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(m)
          .n(n)
          .k(8)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, k_eq_8_subtile_m) {
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(m)
        .n(4)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, k_eq_8_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(n)
        .k(8)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, k_lt_8) {
    for (size_t k = 1; k < 8; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, k_lt_8_subtile) {
    for (size_t k = 1; k < 8; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, k_gt_8) {
    for (size_t k = 9; k < 16; k++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, k_gt_8_subtile) {
    for (size_t k = 9; k < 16; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, k_div_8) {
    for (size_t k = 16; k <= 80; k += 8) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, k_div_8_subtile) {
    for (size_t k = 16; k <= 80; k += 8) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, small_kernel) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, small_kernel_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 40; k += 9) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, strided_cm_subtile) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 2; m++) {
          GemmMicrokernelTester()
            .mr(2)
            .nr(4)
            .kr(8)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, a_offset) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(83)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, zero) {
    for (size_t k = 1; k <= 40; k += 9) {
      for (uint32_t mz = 0; mz < 2; mz++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(8)
          .sr(1)
          .m(2)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(83)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, qmin) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, qmax) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, strided_cm) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(8)
      .sr(1)
      .m(2)
      .n(4)
      .k(8)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, no_a_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, no_b_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_2X4C8__WASMSIMD_MUL32_LD128, no_zero_point) {
    for (size_t k = 1; k <= 40; k += 9) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(8)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4c8__wasmsimd_mul32_ld128, xnn_init_qu8_conv_minmax_fp32_wasmsimd_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASM || XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, k_eq_1) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, strided_cn) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(1)
      .cn_stride(5)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, k_eq_1_subtile) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(1)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, k_eq_1_subtile_m) {
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(m)
        .n(2)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, k_eq_1_subtile_n) {
    for (uint32_t n = 1; n <= 2; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(2)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, k_gt_1_subtile) {
    for (size_t k = 2; k < 10; k++) {
      for (uint32_t n = 1; n <= 2; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(2)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, n_gt_2) {
    for (uint32_t n = 3; n < 4; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, n_gt_2_strided_cn) {
    for (uint32_t n = 3; n < 4; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(5)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, n_gt_2_subtile) {
    for (uint32_t n = 3; n < 4; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(2)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, n_div_2) {
    for (uint32_t n = 4; n <= 6; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, n_div_2_strided_cn) {
    for (uint32_t n = 4; n <= 6; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(5)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, n_div_2_subtile) {
    for (uint32_t n = 4; n <= 6; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(2)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, small_kernel) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(2)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, small_kernel_subtile) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t n = 1; n <= 2; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(2)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, n_gt_2_small_kernel) {
    for (uint32_t n = 3; n < 4; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, n_div_2_small_kernel) {
    for (uint32_t n = 4; n <= 6; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, strided_cm_subtile) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t n = 1; n <= 2; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(2)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(5)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, a_offset) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(2)
        .k(k)
        .ks(3)
        .a_offset(17)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, zero) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(3)
          .n(2)
          .k(k)
          .ks(3)
          .a_offset(17)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, qmin) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(1)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, qmax) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(1)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, strided_cm) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(1)
      .cm_stride(5)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, no_a_zero_point) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(2)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, no_b_zero_point) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(2)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X2__WASM_FMAGIC, no_zero_point) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(2)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASM || XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASM || XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, k_eq_1) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, strided_cn) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(1)
      .cn_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, k_eq_1_subtile) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(1)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, k_eq_1_subtile_m) {
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(m)
        .n(4)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, k_eq_1_subtile_n) {
    for (uint32_t n = 1; n <= 4; n++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, k_gt_1_subtile) {
    for (size_t k = 2; k < 10; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, n_gt_4) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, n_gt_4_strided_cn) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, n_gt_4_subtile) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, n_div_4) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 5; k += 2) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, n_div_4_strided_cn) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 5; k += 2) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .cn_stride(7)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, n_div_4_subtile) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 5; k += 2) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, small_kernel) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, small_kernel_subtile) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .ks(3)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, n_gt_4_small_kernel) {
    for (uint32_t n = 5; n < 8; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, n_div_4_small_kernel) {
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 5; k += 2) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(3)
          .n(n)
          .k(k)
          .ks(3)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, strided_cm_subtile) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t n = 1; n <= 4; n++) {
        for (uint32_t m = 1; m <= 3; m++) {
          GemmMicrokernelTester()
            .mr(3)
            .nr(4)
            .kr(1)
            .sr(1)
            .m(m)
            .n(n)
            .k(k)
            .cm_stride(7)
            .iterations(1)
            .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
        }
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, a_offset) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(17)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, zero) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t mz = 0; mz < 3; mz++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(3)
          .n(4)
          .k(k)
          .ks(3)
          .a_offset(17)
          .zero_index(mz)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, qmin) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(1)
      .qmin(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, qmax) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(1)
      .qmax(128)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, strided_cm) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(1)
      .cm_stride(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, no_a_zero_point) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, no_b_zero_point) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }

  TEST(QU8_IGEMM_MINMAX_FP32_3X4__WASM_FMAGIC, no_zero_point) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .a_zero_point(0)
        .b_zero_point(0)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__wasm_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
#endif  // XNN_ARCH_WASM || XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, k_eq_1) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(3)
    .n(2)
    .k(1)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, strided_cn) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(3)
    .n(2)
    .k(1)
    .cn_stride(5)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 2; n++) {
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(m)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, k_eq_1_subtile_m) {
  for (uint32_t m = 1; m <= 3; m++) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(m)
      .n(2)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, k_eq_1_subtile_n) {
  for (uint32_t n = 1; n <= 2; n++) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(n)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(k)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, n_gt_2) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, n_gt_2_strided_cn) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .cn_stride(5)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, n_gt_2_subtile) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, n_div_2) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, n_div_2_strided_cn) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .cn_stride(5)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, n_div_2_subtile) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, small_kernel) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(k)
      .ks(3)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, small_kernel_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .ks(3)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, n_gt_2_small_kernel) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, n_div_2_small_kernel) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, strided_cm_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .cm_stride(5)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, a_offset) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(k)
      .ks(3)
      .a_offset(17)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, zero) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t mz = 0; mz < 3; mz++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(2)
        .k(k)
        .ks(3)
        .a_offset(17)
        .zero_index(mz)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, qmin) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(3)
    .n(2)
    .k(1)
    .qmin(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, qmax) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(3)
    .n(2)
    .k(1)
    .qmax(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, strided_cm) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(3)
    .n(2)
    .k(1)
    .cm_stride(5)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, no_a_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(k)
      .a_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, no_b_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(k)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_FMAGIC, no_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(k)
      .a_zero_point(0)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, k_eq_1) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(4)
    .n(2)
    .k(1)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, strided_cn) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(4)
    .n(2)
    .k(1)
    .cn_stride(5)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 2; n++) {
    for (uint32_t m = 1; m <= 4; m++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(m)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, k_eq_1_subtile_m) {
  for (uint32_t m = 1; m <= 4; m++) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(m)
      .n(2)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, k_eq_1_subtile_n) {
  for (uint32_t n = 1; n <= 2; n++) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(n)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(2)
      .k(k)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, n_gt_2) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, n_gt_2_strided_cn) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .cn_stride(5)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, n_gt_2_subtile) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, n_div_2) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, n_div_2_strided_cn) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .cn_stride(5)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, n_div_2_subtile) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, small_kernel) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(2)
      .k(k)
      .ks(3)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, small_kernel_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .ks(3)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, n_gt_2_small_kernel) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, n_div_2_small_kernel) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, strided_cm_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .cm_stride(5)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, a_offset) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(2)
      .k(k)
      .ks(3)
      .a_offset(23)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, zero) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t mz = 0; mz < 4; mz++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(2)
        .k(k)
        .ks(3)
        .a_offset(23)
        .zero_index(mz)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, qmin) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(4)
    .n(2)
    .k(1)
    .qmin(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, qmax) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(4)
    .n(2)
    .k(1)
    .qmax(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, strided_cm) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(4)
    .n(2)
    .k(1)
    .cm_stride(5)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, no_a_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(2)
      .k(k)
      .a_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, no_b_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(2)
      .k(k)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_FMAGIC, no_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(2)
      .k(k)
      .a_zero_point(0)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, k_eq_1) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(3)
    .n(4)
    .k(1)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, strided_cn) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(3)
    .n(4)
    .k(1)
    .cn_stride(7)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 4; n++) {
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(m)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, k_eq_1_subtile_m) {
  for (uint32_t m = 1; m <= 3; m++) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(m)
      .n(4)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, k_eq_1_subtile_n) {
  for (uint32_t n = 1; n <= 4; n++) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(n)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(k)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, n_gt_4) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, n_gt_4_strided_cn) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .cn_stride(7)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, n_gt_4_subtile) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, n_div_4) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, n_div_4_strided_cn) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .cn_stride(7)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, n_div_4_subtile) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, small_kernel) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(k)
      .ks(3)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, small_kernel_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .ks(3)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, n_gt_4_small_kernel) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, n_div_4_small_kernel) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, strided_cm_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .cm_stride(7)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, a_offset) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(k)
      .ks(3)
      .a_offset(17)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, zero) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t mz = 0; mz < 3; mz++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(17)
        .zero_index(mz)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, qmin) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(3)
    .n(4)
    .k(1)
    .qmin(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, qmax) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(3)
    .n(4)
    .k(1)
    .qmax(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, strided_cm) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(3)
    .n(4)
    .k(1)
    .cm_stride(7)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, no_a_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(k)
      .a_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, no_b_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(k)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_FMAGIC, no_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(k)
      .a_zero_point(0)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, k_eq_1) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(4)
    .n(4)
    .k(1)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, strided_cn) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(4)
    .n(4)
    .k(1)
    .cn_stride(7)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 4; n++) {
    for (uint32_t m = 1; m <= 4; m++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(m)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, k_eq_1_subtile_m) {
  for (uint32_t m = 1; m <= 4; m++) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(m)
      .n(4)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, k_eq_1_subtile_n) {
  for (uint32_t n = 1; n <= 4; n++) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(n)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(4)
      .k(k)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, n_gt_4) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, n_gt_4_strided_cn) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .cn_stride(7)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, n_gt_4_subtile) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, n_div_4) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, n_div_4_strided_cn) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .cn_stride(7)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, n_div_4_subtile) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, small_kernel) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(4)
      .k(k)
      .ks(3)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, small_kernel_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .ks(3)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, n_gt_4_small_kernel) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, n_div_4_small_kernel) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, strided_cm_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .cm_stride(7)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, a_offset) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(4)
      .k(k)
      .ks(3)
      .a_offset(23)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, zero) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t mz = 0; mz < 4; mz++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(23)
        .zero_index(mz)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, qmin) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(4)
    .n(4)
    .k(1)
    .qmin(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, qmax) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(4)
    .n(4)
    .k(1)
    .qmax(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, strided_cm) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(4)
    .n(4)
    .k(1)
    .cm_stride(7)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, no_a_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(4)
      .k(k)
      .a_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, no_b_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(4)
      .k(k)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_FMAGIC, no_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(4)
      .k(k)
      .a_zero_point(0)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_fmagic, xnn_init_qu8_conv_minmax_fp32_scalar_fmagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, k_eq_1) {
  GemmMicrokernelTester()
    .mr(1)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(1)
    .n(2)
    .k(1)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, strided_cn) {
  GemmMicrokernelTester()
    .mr(1)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(1)
    .n(2)
    .k(1)
    .cn_stride(5)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 2; n++) {
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(m)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, k_eq_1_subtile_m) {
  for (uint32_t m = 1; m <= 1; m++) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(m)
      .n(2)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, k_eq_1_subtile_n) {
  for (uint32_t n = 1; n <= 2; n++) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(1)
      .n(n)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(1)
      .n(2)
      .k(k)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, n_gt_2) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, n_gt_2_strided_cn) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(k)
        .cn_stride(5)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, n_gt_2_subtile) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, n_div_2) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, n_div_2_strided_cn) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(k)
        .cn_stride(5)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, n_div_2_subtile) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, small_kernel) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(1)
      .n(2)
      .k(k)
      .ks(3)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, small_kernel_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .ks(3)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, n_gt_2_small_kernel) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, n_div_2_small_kernel) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, strided_cm_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .cm_stride(5)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, a_offset) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(1)
      .n(2)
      .k(k)
      .ks(3)
      .a_offset(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, zero) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t mz = 0; mz < 1; mz++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(1)
        .n(2)
        .k(k)
        .ks(3)
        .a_offset(7)
        .zero_index(mz)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, qmin) {
  GemmMicrokernelTester()
    .mr(1)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(1)
    .n(2)
    .k(1)
    .qmin(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, qmax) {
  GemmMicrokernelTester()
    .mr(1)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(1)
    .n(2)
    .k(1)
    .qmax(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, strided_cm) {
  GemmMicrokernelTester()
    .mr(1)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(1)
    .n(2)
    .k(1)
    .cm_stride(5)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, no_a_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(1)
      .n(2)
      .k(k)
      .a_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, no_b_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(1)
      .n(2)
      .k(k)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X2__SCALAR_IMAGIC, no_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(1)
      .n(2)
      .k(k)
      .a_zero_point(0)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, k_eq_1) {
  GemmMicrokernelTester()
    .mr(2)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(2)
    .n(2)
    .k(1)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, strided_cn) {
  GemmMicrokernelTester()
    .mr(2)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(2)
    .n(2)
    .k(1)
    .cn_stride(5)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 2; n++) {
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(m)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, k_eq_1_subtile_m) {
  for (uint32_t m = 1; m <= 2; m++) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(m)
      .n(2)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, k_eq_1_subtile_n) {
  for (uint32_t n = 1; n <= 2; n++) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(2)
      .n(n)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(2)
      .n(2)
      .k(k)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, n_gt_2) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(2)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, n_gt_2_strided_cn) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(2)
        .n(n)
        .k(k)
        .cn_stride(5)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, n_gt_2_subtile) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, n_div_2) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(2)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, n_div_2_strided_cn) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(2)
        .n(n)
        .k(k)
        .cn_stride(5)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, n_div_2_subtile) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, small_kernel) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(2)
      .n(2)
      .k(k)
      .ks(3)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, small_kernel_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .ks(3)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, n_gt_2_small_kernel) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(2)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, n_div_2_small_kernel) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(2)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, strided_cm_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .cm_stride(5)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, a_offset) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(2)
      .n(2)
      .k(k)
      .ks(3)
      .a_offset(13)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, zero) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t mz = 0; mz < 2; mz++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(2)
        .n(2)
        .k(k)
        .ks(3)
        .a_offset(13)
        .zero_index(mz)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, qmin) {
  GemmMicrokernelTester()
    .mr(2)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(2)
    .n(2)
    .k(1)
    .qmin(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, qmax) {
  GemmMicrokernelTester()
    .mr(2)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(2)
    .n(2)
    .k(1)
    .qmax(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, strided_cm) {
  GemmMicrokernelTester()
    .mr(2)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(2)
    .n(2)
    .k(1)
    .cm_stride(5)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, no_a_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(2)
      .n(2)
      .k(k)
      .a_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, no_b_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(2)
      .n(2)
      .k(k)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X2__SCALAR_IMAGIC, no_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(2)
      .n(2)
      .k(k)
      .a_zero_point(0)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x2__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, k_eq_1) {
  GemmMicrokernelTester()
    .mr(1)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(1)
    .n(4)
    .k(1)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, strided_cn) {
  GemmMicrokernelTester()
    .mr(1)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(1)
    .n(4)
    .k(1)
    .cn_stride(7)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 4; n++) {
    for (uint32_t m = 1; m <= 1; m++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(m)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, k_eq_1_subtile_m) {
  for (uint32_t m = 1; m <= 1; m++) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(m)
      .n(4)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, k_eq_1_subtile_n) {
  for (uint32_t n = 1; n <= 4; n++) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(1)
      .n(n)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(1)
      .n(4)
      .k(k)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, n_gt_4) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, n_gt_4_strided_cn) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(k)
        .cn_stride(7)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, n_gt_4_subtile) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, n_div_4) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, n_div_4_strided_cn) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(k)
        .cn_stride(7)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, n_div_4_subtile) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, small_kernel) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(1)
      .n(4)
      .k(k)
      .ks(3)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, small_kernel_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .ks(3)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, n_gt_4_small_kernel) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, n_div_4_small_kernel) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(1)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, strided_cm_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 1; m++) {
        GemmMicrokernelTester()
          .mr(1)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .cm_stride(7)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, a_offset) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(1)
      .n(4)
      .k(k)
      .ks(3)
      .a_offset(7)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, zero) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t mz = 0; mz < 1; mz++) {
      GemmMicrokernelTester()
        .mr(1)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(1)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(7)
        .zero_index(mz)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, qmin) {
  GemmMicrokernelTester()
    .mr(1)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(1)
    .n(4)
    .k(1)
    .qmin(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, qmax) {
  GemmMicrokernelTester()
    .mr(1)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(1)
    .n(4)
    .k(1)
    .qmax(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, strided_cm) {
  GemmMicrokernelTester()
    .mr(1)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(1)
    .n(4)
    .k(1)
    .cm_stride(7)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, no_a_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(1)
      .n(4)
      .k(k)
      .a_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, no_b_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(1)
      .n(4)
      .k(k)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_1X4__SCALAR_IMAGIC, no_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(1)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(1)
      .n(4)
      .k(k)
      .a_zero_point(0)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_1x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, k_eq_1) {
  GemmMicrokernelTester()
    .mr(2)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(2)
    .n(4)
    .k(1)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, strided_cn) {
  GemmMicrokernelTester()
    .mr(2)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(2)
    .n(4)
    .k(1)
    .cn_stride(7)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 4; n++) {
    for (uint32_t m = 1; m <= 2; m++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(m)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, k_eq_1_subtile_m) {
  for (uint32_t m = 1; m <= 2; m++) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(m)
      .n(4)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, k_eq_1_subtile_n) {
  for (uint32_t n = 1; n <= 4; n++) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(2)
      .n(n)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(2)
      .n(4)
      .k(k)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, n_gt_4) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(2)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, n_gt_4_strided_cn) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(2)
        .n(n)
        .k(k)
        .cn_stride(7)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, n_gt_4_subtile) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, n_div_4) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(2)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, n_div_4_strided_cn) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(2)
        .n(n)
        .k(k)
        .cn_stride(7)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, n_div_4_subtile) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, small_kernel) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(2)
      .n(4)
      .k(k)
      .ks(3)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, small_kernel_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .ks(3)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, n_gt_4_small_kernel) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(2)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, n_div_4_small_kernel) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(2)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, strided_cm_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 2; m++) {
        GemmMicrokernelTester()
          .mr(2)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .cm_stride(7)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, a_offset) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(2)
      .n(4)
      .k(k)
      .ks(3)
      .a_offset(13)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, zero) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t mz = 0; mz < 2; mz++) {
      GemmMicrokernelTester()
        .mr(2)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(2)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(13)
        .zero_index(mz)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, qmin) {
  GemmMicrokernelTester()
    .mr(2)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(2)
    .n(4)
    .k(1)
    .qmin(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, qmax) {
  GemmMicrokernelTester()
    .mr(2)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(2)
    .n(4)
    .k(1)
    .qmax(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, strided_cm) {
  GemmMicrokernelTester()
    .mr(2)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(2)
    .n(4)
    .k(1)
    .cm_stride(7)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, no_a_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(2)
      .n(4)
      .k(k)
      .a_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, no_b_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(2)
      .n(4)
      .k(k)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_2X4__SCALAR_IMAGIC, no_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(2)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(2)
      .n(4)
      .k(k)
      .a_zero_point(0)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_2x4__scalar_imagic, xnn_init_qu8_conv_minmax_fp32_scalar_imagic_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, k_eq_1) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(3)
    .n(2)
    .k(1)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, strided_cn) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(3)
    .n(2)
    .k(1)
    .cn_stride(5)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 2; n++) {
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(m)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, k_eq_1_subtile_m) {
  for (uint32_t m = 1; m <= 3; m++) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(m)
      .n(2)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, k_eq_1_subtile_n) {
  for (uint32_t n = 1; n <= 2; n++) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(n)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(k)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, n_gt_2) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, n_gt_2_strided_cn) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .cn_stride(5)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, n_gt_2_subtile) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, n_div_2) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, n_div_2_strided_cn) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .cn_stride(5)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, n_div_2_subtile) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, small_kernel) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(k)
      .ks(3)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, small_kernel_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .ks(3)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, n_gt_2_small_kernel) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, n_div_2_small_kernel) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, strided_cm_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .cm_stride(5)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, a_offset) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(k)
      .ks(3)
      .a_offset(17)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, zero) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t mz = 0; mz < 3; mz++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(3)
        .n(2)
        .k(k)
        .ks(3)
        .a_offset(17)
        .zero_index(mz)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, qmin) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(3)
    .n(2)
    .k(1)
    .qmin(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, qmax) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(3)
    .n(2)
    .k(1)
    .qmax(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, strided_cm) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(3)
    .n(2)
    .k(1)
    .cm_stride(5)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, no_a_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(k)
      .a_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, no_b_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(k)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X2__SCALAR_LRINTF, no_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(3)
      .n(2)
      .k(k)
      .a_zero_point(0)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, k_eq_1) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(4)
    .n(2)
    .k(1)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, strided_cn) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(4)
    .n(2)
    .k(1)
    .cn_stride(5)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 2; n++) {
    for (uint32_t m = 1; m <= 4; m++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(m)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, k_eq_1_subtile_m) {
  for (uint32_t m = 1; m <= 4; m++) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(m)
      .n(2)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, k_eq_1_subtile_n) {
  for (uint32_t n = 1; n <= 2; n++) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(n)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(2)
      .k(k)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, n_gt_2) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, n_gt_2_strided_cn) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .cn_stride(5)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, n_gt_2_subtile) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, n_div_2) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, n_div_2_strided_cn) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .cn_stride(5)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, n_div_2_subtile) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, small_kernel) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(2)
      .k(k)
      .ks(3)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, small_kernel_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .ks(3)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, n_gt_2_small_kernel) {
  for (uint32_t n = 3; n < 4; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, n_div_2_small_kernel) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, strided_cm_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 2; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(2)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .cm_stride(5)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, a_offset) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(2)
      .k(k)
      .ks(3)
      .a_offset(23)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, zero) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t mz = 0; mz < 4; mz++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(2)
        .kr(1)
        .sr(1)
        .m(4)
        .n(2)
        .k(k)
        .ks(3)
        .a_offset(23)
        .zero_index(mz)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, qmin) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(4)
    .n(2)
    .k(1)
    .qmin(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, qmax) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(4)
    .n(2)
    .k(1)
    .qmax(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, strided_cm) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(2)
    .kr(1)
    .sr(1)
    .m(4)
    .n(2)
    .k(1)
    .cm_stride(5)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, no_a_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(2)
      .k(k)
      .a_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, no_b_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(2)
      .k(k)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X2__SCALAR_LRINTF, no_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(2)
      .kr(1)
      .sr(1)
      .m(4)
      .n(2)
      .k(k)
      .a_zero_point(0)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x2__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, k_eq_1) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(3)
    .n(4)
    .k(1)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, strided_cn) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(3)
    .n(4)
    .k(1)
    .cn_stride(7)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 4; n++) {
    for (uint32_t m = 1; m <= 3; m++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(m)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, k_eq_1_subtile_m) {
  for (uint32_t m = 1; m <= 3; m++) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(m)
      .n(4)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, k_eq_1_subtile_n) {
  for (uint32_t n = 1; n <= 4; n++) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(n)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(k)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, n_gt_4) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, n_gt_4_strided_cn) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .cn_stride(7)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, n_gt_4_subtile) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, n_div_4) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, n_div_4_strided_cn) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .cn_stride(7)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, n_div_4_subtile) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, small_kernel) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(k)
      .ks(3)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, small_kernel_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .ks(3)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, n_gt_4_small_kernel) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, n_div_4_small_kernel) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, strided_cm_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 3; m++) {
        GemmMicrokernelTester()
          .mr(3)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .cm_stride(7)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, a_offset) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(k)
      .ks(3)
      .a_offset(17)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, zero) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t mz = 0; mz < 3; mz++) {
      GemmMicrokernelTester()
        .mr(3)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(3)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(17)
        .zero_index(mz)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, qmin) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(3)
    .n(4)
    .k(1)
    .qmin(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, qmax) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(3)
    .n(4)
    .k(1)
    .qmax(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, strided_cm) {
  GemmMicrokernelTester()
    .mr(3)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(3)
    .n(4)
    .k(1)
    .cm_stride(7)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, no_a_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(k)
      .a_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, no_b_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(k)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_3X4__SCALAR_LRINTF, no_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(3)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(3)
      .n(4)
      .k(k)
      .a_zero_point(0)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_3x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, k_eq_1) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(4)
    .n(4)
    .k(1)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, strided_cn) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(4)
    .n(4)
    .k(1)
    .cn_stride(7)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 4; n++) {
    for (uint32_t m = 1; m <= 4; m++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(m)
        .n(n)
        .k(1)
        .iterations(1)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, k_eq_1_subtile_m) {
  for (uint32_t m = 1; m <= 4; m++) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(m)
      .n(4)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, k_eq_1_subtile_n) {
  for (uint32_t n = 1; n <= 4; n++) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(n)
      .k(1)
      .iterations(1)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(4)
      .k(k)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, n_gt_4) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, n_gt_4_strided_cn) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .cn_stride(7)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, n_gt_4_subtile) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, n_div_4) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, n_div_4_strided_cn) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .cn_stride(7)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, n_div_4_subtile) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, small_kernel) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(4)
      .k(k)
      .ks(3)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, small_kernel_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .ks(3)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, n_gt_4_small_kernel) {
  for (uint32_t n = 5; n < 8; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, n_div_4_small_kernel) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(n)
        .k(k)
        .ks(3)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, strided_cm_subtile) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t n = 1; n <= 4; n++) {
      for (uint32_t m = 1; m <= 4; m++) {
        GemmMicrokernelTester()
          .mr(4)
          .nr(4)
          .kr(1)
          .sr(1)
          .m(m)
          .n(n)
          .k(k)
          .cm_stride(7)
          .iterations(1)
          .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
      }
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, a_offset) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(4)
      .k(k)
      .ks(3)
      .a_offset(23)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, zero) {
  for (size_t k = 1; k <= 5; k += 2) {
    for (uint32_t mz = 0; mz < 4; mz++) {
      GemmMicrokernelTester()
        .mr(4)
        .nr(4)
        .kr(1)
        .sr(1)
        .m(4)
        .n(4)
        .k(k)
        .ks(3)
        .a_offset(23)
        .zero_index(mz)
        .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
    }
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, qmin) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(4)
    .n(4)
    .k(1)
    .qmin(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, qmax) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(4)
    .n(4)
    .k(1)
    .qmax(128)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, strided_cm) {
  GemmMicrokernelTester()
    .mr(4)
    .nr(4)
    .kr(1)
    .sr(1)
    .m(4)
    .n(4)
    .k(1)
    .cm_stride(7)
    .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, no_a_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(4)
      .k(k)
      .a_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, no_b_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(4)
      .k(k)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}

TEST(QU8_IGEMM_MINMAX_FP32_4X4__SCALAR_LRINTF, no_zero_point) {
  for (size_t k = 1; k <= 5; k += 2) {
    GemmMicrokernelTester()
      .mr(4)
      .nr(4)
      .kr(1)
      .sr(1)
      .m(4)
      .n(4)
      .k(k)
      .a_zero_point(0)
      .b_zero_point(0)
      .Test(xnn_qu8_igemm_minmax_fp32_ukernel_4x4__scalar_lrintf, xnn_init_qu8_conv_minmax_fp32_scalar_lrintf_params, xnn_qu8_requantize_fp32);
  }
}