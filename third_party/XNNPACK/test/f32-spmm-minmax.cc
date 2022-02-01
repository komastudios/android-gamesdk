// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.
//
// Auto-generated file. Do not edit!
//   Specification: test/f32-spmm-minmax.yaml
//   Generator: tools/generate-spmm-test.py


#include <gtest/gtest.h>

#include <xnnpack/common.h>
#include <xnnpack/isa-checks.h>

#include <xnnpack/spmm.h>
#include "spmm-microkernel-tester.h"


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_4X1__NEON, k_eq_1) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON, k_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON, m_lt_4) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON, m_div_4) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON, m_gt_4) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_4X1__NEON_PIPELINED, k_eq_1) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_PIPELINED, k_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_PIPELINED, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_PIPELINED, m_lt_4) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_PIPELINED, m_div_4) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_PIPELINED, m_gt_4) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_PIPELINED, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_PIPELINED, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_PIPELINED, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_PIPELINED, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_PIPELINED, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, k_eq_2) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, k_lt_2) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, k_gt_2) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, k_div_2) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, m_lt_4) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, m_div_4) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, m_gt_4) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEON_X2, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_8X1__NEON, k_eq_1) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON, k_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON, m_lt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON, m_div_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON, m_gt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_8X1__NEON_PIPELINED, k_eq_1) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_PIPELINED, k_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_PIPELINED, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_PIPELINED, m_lt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_PIPELINED, m_div_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_PIPELINED, m_gt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_PIPELINED, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_PIPELINED, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_PIPELINED, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_PIPELINED, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_PIPELINED, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, k_eq_2) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, k_lt_2) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, k_gt_2) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, k_div_2) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, m_lt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, m_div_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, m_gt_8) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEON_X2, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_12X1__NEON, k_eq_1) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(12)
      .nr(1)
      .m(12)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_12x1__neon, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_12X1__NEON, k_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(12)
        .nr(1)
        .m(12)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_12x1__neon, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEON, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(1)
          .m(12)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEON, m_lt_12) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 12; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(12)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_12x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEON, m_div_12) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 24; m <= 36; m += 12) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(12)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_12x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEON, m_gt_12) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 13; m < 24; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(12)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_12x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEON, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(1)
          .m(24)
          .n(n)
          .k(k)
          .output_stride(29)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEON, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(1)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_12x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEON, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(1)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_12x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEON, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(1)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEON, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(1)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_16X1__NEON, k_eq_1) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON, k_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON, m_lt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON, m_div_16) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON, m_gt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_16X1__NEON_PIPELINED, k_eq_1) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_PIPELINED, k_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_PIPELINED, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_PIPELINED, m_lt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_PIPELINED, m_div_16) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_PIPELINED, m_gt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_PIPELINED, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_PIPELINED, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_PIPELINED, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_PIPELINED, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_PIPELINED, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, k_eq_2) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, k_lt_2) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, k_gt_2) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, k_div_2) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, m_lt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, m_div_16) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, m_gt_16) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEON_X2, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_32X1__NEON, k_eq_1) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON, k_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON, m_lt_32) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON, m_div_32) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON, m_gt_32) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_32X1__NEON_PIPELINED, k_eq_1) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_PIPELINED, k_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_PIPELINED, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_PIPELINED, m_lt_32) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_PIPELINED, m_div_32) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_PIPELINED, m_gt_32) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_PIPELINED, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_PIPELINED, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_PIPELINED, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_PIPELINED, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_PIPELINED, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, k_eq_2) {
    TEST_REQUIRES_ARM_NEON;
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, k_lt_2) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, k_gt_2) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, k_div_2) {
    TEST_REQUIRES_ARM_NEON;
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, n_gt_1) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, m_lt_32) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, m_div_32) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, m_gt_32) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, output_stride) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, qmin) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, qmax) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, half_sparse) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEON_X2, zero_weights) {
    TEST_REQUIRES_ARM_NEON;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neon_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_4X1__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA, m_lt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA, m_div_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA, m_gt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(4)
      .nr(2)
      .m(4)
      .n(2)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, k_eq_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n <= 2; n++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(2)
        .m(4)
        .n(n)
        .k(1)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(2)
        .m(4)
        .n(2)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, k_gt_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      for (uint32_t n = 1; n <= 2; n++) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(2)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, n_gt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 3; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(2)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, n_div_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 4; n <= 6; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(2)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, m_lt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, m_div_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, m_gt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(2)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(2)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(2)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(2)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X2__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(2)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM64


#if XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(4)
      .nr(4)
      .m(4)
      .n(4)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, k_eq_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n <= 4; n++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(4)
        .m(4)
        .n(n)
        .k(1)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(4)
        .m(4)
        .n(4)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, k_gt_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(4)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, n_gt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 5; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(4)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, n_div_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(4)
          .m(4)
          .n(n)
          .k(k)
          .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, m_lt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, m_div_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, m_gt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(4)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(4)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(4)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(4)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X4__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(4)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_PIPELINED, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_PIPELINED, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_PIPELINED, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_PIPELINED, m_lt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_PIPELINED, m_div_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_PIPELINED, m_gt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_PIPELINED, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_PIPELINED, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_PIPELINED, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_PIPELINED, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_PIPELINED, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, k_eq_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, k_lt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, k_gt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, k_div_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, m_lt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, m_div_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, m_gt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__NEONFMA_X2, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_8X1__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA, m_lt_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA, m_div_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA, m_gt_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(8)
      .nr(2)
      .m(8)
      .n(2)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, k_eq_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n <= 2; n++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(2)
        .m(8)
        .n(n)
        .k(1)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(2)
        .m(8)
        .n(2)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, k_gt_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      for (uint32_t n = 1; n <= 2; n++) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(2)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, n_gt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 3; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(2)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, n_div_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 4; n <= 6; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(2)
          .m(8)
          .n(n)
          .k(k)
          .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, m_lt_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, m_div_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, m_gt_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(2)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(2)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(2)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(2)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X2__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(2)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM64


#if XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(8)
      .nr(4)
      .m(8)
      .n(4)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, k_eq_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n <= 4; n++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(4)
        .m(8)
        .n(n)
        .k(1)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(4)
        .m(8)
        .n(4)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, k_gt_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(4)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, n_gt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 5; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(4)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, n_div_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(4)
          .m(8)
          .n(n)
          .k(k)
          .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, m_lt_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, m_div_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, m_gt_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(4)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(4)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(4)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(4)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X4__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(4)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_PIPELINED, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_PIPELINED, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_PIPELINED, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_PIPELINED, m_lt_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_PIPELINED, m_div_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_PIPELINED, m_gt_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_PIPELINED, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_PIPELINED, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_PIPELINED, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_PIPELINED, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_PIPELINED, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, k_eq_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, k_lt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, k_gt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, k_div_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, m_lt_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, m_div_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, m_gt_8) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__NEONFMA_X2, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_12X1__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(12)
      .nr(1)
      .m(12)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_12x1__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_12X1__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(12)
        .nr(1)
        .m(12)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_12x1__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEONFMA, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(1)
          .m(12)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEONFMA, m_lt_12) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 12; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(12)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_12x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEONFMA, m_div_12) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 24; m <= 36; m += 12) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(12)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_12x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEONFMA, m_gt_12) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 13; m < 24; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(12)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_12x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(1)
          .m(24)
          .n(n)
          .k(k)
          .output_stride(29)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(1)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_12x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(1)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_12x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(1)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X1__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(1)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(12)
      .nr(2)
      .m(12)
      .n(2)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, k_eq_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n <= 2; n++) {
      SpMMMicrokernelTester()
        .mr(12)
        .nr(2)
        .m(12)
        .n(n)
        .k(1)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(12)
        .nr(2)
        .m(12)
        .n(2)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, k_gt_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      for (uint32_t n = 1; n <= 2; n++) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(2)
          .m(12)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, n_gt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 3; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(2)
          .m(12)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, n_div_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 4; n <= 6; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(2)
          .m(12)
          .n(n)
          .k(k)
          .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, m_lt_12) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 12; m++) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(12)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, m_div_12) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 24; m <= 36; m += 12) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(12)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, m_gt_12) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 13; m < 24; m++) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(12)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(2)
          .m(24)
          .n(n)
          .k(k)
          .output_stride(29)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(2)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(2)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(2)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X2__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(2)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM64


#if XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(12)
      .nr(4)
      .m(12)
      .n(4)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, k_eq_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n <= 4; n++) {
      SpMMMicrokernelTester()
        .mr(12)
        .nr(4)
        .m(12)
        .n(n)
        .k(1)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(12)
        .nr(4)
        .m(12)
        .n(4)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, k_gt_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(4)
          .m(12)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, n_gt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 5; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(4)
          .m(12)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, n_div_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(4)
          .m(12)
          .n(n)
          .k(k)
          .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, m_lt_12) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 12; m++) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(12)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, m_div_12) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 24; m <= 36; m += 12) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(12)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, m_gt_12) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 13; m < 24; m++) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(12)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(4)
          .m(24)
          .n(n)
          .k(k)
          .output_stride(29)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(4)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(4)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(4)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_12X4__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(12)
          .nr(4)
          .m(24)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_12x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_16X1__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA, m_lt_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA, m_div_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA, m_gt_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(16)
      .nr(2)
      .m(16)
      .n(2)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, k_eq_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n <= 2; n++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(2)
        .m(16)
        .n(n)
        .k(1)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(2)
        .m(16)
        .n(2)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, k_gt_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      for (uint32_t n = 1; n <= 2; n++) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(2)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, n_gt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 3; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(2)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, n_div_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 4; n <= 6; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(2)
          .m(16)
          .n(n)
          .k(k)
          .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, m_lt_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, m_div_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, m_gt_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(2)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(2)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(2)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(2)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X2__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(2)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM64


#if XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(16)
      .nr(4)
      .m(16)
      .n(4)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, k_eq_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n <= 4; n++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(4)
        .m(16)
        .n(n)
        .k(1)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(4)
        .m(16)
        .n(4)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, k_gt_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(4)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, n_gt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 5; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(4)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, n_div_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(4)
          .m(16)
          .n(n)
          .k(k)
          .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, m_lt_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, m_div_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, m_gt_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(4)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(4)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(4)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(4)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X4__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(4)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_PIPELINED, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_PIPELINED, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_PIPELINED, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_PIPELINED, m_lt_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_PIPELINED, m_div_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_PIPELINED, m_gt_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_PIPELINED, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_PIPELINED, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_PIPELINED, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_PIPELINED, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_PIPELINED, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, k_eq_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, k_lt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, k_gt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, k_div_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, m_lt_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, m_div_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, m_gt_16) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__NEONFMA_X2, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_32X1__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA, m_lt_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA, m_div_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA, m_gt_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(32)
      .nr(2)
      .m(32)
      .n(2)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, k_eq_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n <= 2; n++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(2)
        .m(32)
        .n(n)
        .k(1)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(2)
        .m(32)
        .n(2)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, k_gt_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      for (uint32_t n = 1; n <= 2; n++) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(2)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, n_gt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 3; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(2)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, n_div_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 4; n <= 6; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(2)
          .m(32)
          .n(n)
          .k(k)
          .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, m_lt_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, m_div_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, m_gt_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 3) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(2)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(2)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(2)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(2)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(2)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X2__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(2)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x2__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM64


#if XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(32)
      .nr(4)
      .m(32)
      .n(4)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, k_eq_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n <= 4; n++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(4)
        .m(32)
        .n(n)
        .k(1)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(4)
        .m(32)
        .n(4)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, k_gt_1_subtile) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      for (uint32_t n = 1; n <= 4; n++) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(4)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, n_gt_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 5; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(4)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, n_div_4) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 8; n <= 12; n += 4) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(4)
          .m(32)
          .n(n)
          .k(k)
          .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, m_lt_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, m_div_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, m_gt_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 20; n += 5) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(4)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(4)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(4)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(4)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(4)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X4__NEONFMA, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(4)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x4__neonfma, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_PIPELINED, k_eq_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_PIPELINED, k_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_PIPELINED, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_PIPELINED, m_lt_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_PIPELINED, m_div_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_PIPELINED, m_gt_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_PIPELINED, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_PIPELINED, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_PIPELINED, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_PIPELINED, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_PIPELINED, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, k_eq_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, k_lt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, k_gt_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, k_div_2) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, n_gt_1) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, m_lt_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, m_div_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, m_gt_32) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, output_stride) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, qmin) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, qmax) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, half_sparse) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__NEONFMA_X2, zero_weights) {
    TEST_REQUIRES_ARM_NEON_FMA;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__neonfma_x2, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(F32_SPMM_MINMAX_4X1__SSE, k_eq_1) {
    TEST_REQUIRES_X86_SSE;
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__sse, xnn_init_f32_minmax_sse_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__SSE, k_gt_1) {
    TEST_REQUIRES_X86_SSE;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__sse, xnn_init_f32_minmax_sse_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__SSE, n_gt_1) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__SSE, m_lt_4) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__sse, xnn_init_f32_minmax_sse_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__SSE, m_div_4) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__sse, xnn_init_f32_minmax_sse_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__SSE, m_gt_4) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__sse, xnn_init_f32_minmax_sse_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__SSE, output_stride) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__SSE, qmin) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__SSE, qmax) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__SSE, half_sparse) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__SSE, zero_weights) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(F32_SPMM_MINMAX_8X1__SSE, k_eq_1) {
    TEST_REQUIRES_X86_SSE;
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__sse, xnn_init_f32_minmax_sse_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__SSE, k_gt_1) {
    TEST_REQUIRES_X86_SSE;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__sse, xnn_init_f32_minmax_sse_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__SSE, n_gt_1) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__SSE, m_lt_8) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__sse, xnn_init_f32_minmax_sse_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__SSE, m_div_8) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__sse, xnn_init_f32_minmax_sse_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__SSE, m_gt_8) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__sse, xnn_init_f32_minmax_sse_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__SSE, output_stride) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__SSE, qmin) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__SSE, qmax) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__SSE, half_sparse) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__SSE, zero_weights) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(F32_SPMM_MINMAX_16X1__SSE, k_eq_1) {
    TEST_REQUIRES_X86_SSE;
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__sse, xnn_init_f32_minmax_sse_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__SSE, k_gt_1) {
    TEST_REQUIRES_X86_SSE;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__sse, xnn_init_f32_minmax_sse_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__SSE, n_gt_1) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__SSE, m_lt_16) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__sse, xnn_init_f32_minmax_sse_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__SSE, m_div_16) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__sse, xnn_init_f32_minmax_sse_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__SSE, m_gt_16) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__sse, xnn_init_f32_minmax_sse_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__SSE, output_stride) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__SSE, qmin) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__SSE, qmax) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__SSE, half_sparse) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__SSE, zero_weights) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  TEST(F32_SPMM_MINMAX_32X1__SSE, k_eq_1) {
    TEST_REQUIRES_X86_SSE;
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__sse, xnn_init_f32_minmax_sse_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__SSE, k_gt_1) {
    TEST_REQUIRES_X86_SSE;
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__sse, xnn_init_f32_minmax_sse_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__SSE, n_gt_1) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__SSE, m_lt_32) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__sse, xnn_init_f32_minmax_sse_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__SSE, m_div_32) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__sse, xnn_init_f32_minmax_sse_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__SSE, m_gt_32) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__sse, xnn_init_f32_minmax_sse_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__SSE, output_stride) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__SSE, qmin) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__SSE, qmax) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__SSE, half_sparse) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__SSE, zero_weights) {
    TEST_REQUIRES_X86_SSE;
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__sse, xnn_init_f32_minmax_sse_params);
      }
    }
  }
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM, m_lt_4) {
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM, m_div_4) {
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM, m_gt_4) {
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM, m_lt_8) {
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM, m_div_8) {
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM, m_gt_8) {
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM, m_lt_16) {
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM, m_div_16) {
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM, m_gt_16) {
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM, m_lt_32) {
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM, m_div_32) {
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM, m_gt_32) {
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86, m_lt_4) {
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86, m_div_4) {
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86, m_gt_4) {
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86, m_lt_8) {
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86, m_div_8) {
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86, m_gt_8) {
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86, m_lt_16) {
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86, m_div_16) {
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86, m_gt_16) {
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86, m_lt_32) {
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86, m_div_32) {
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86, m_gt_32) {
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, m_lt_4) {
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, m_div_4) {
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, m_gt_4) {
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, m_lt_8) {
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, m_div_8) {
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, m_gt_8) {
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, m_lt_16) {
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, m_div_16) {
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, m_gt_16) {
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, m_lt_32) {
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, m_div_32) {
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, m_gt_32) {
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, m_lt_4) {
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, m_div_4) {
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, m_gt_4) {
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, m_lt_8) {
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, m_div_8) {
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, m_gt_8) {
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, m_lt_16) {
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, m_div_16) {
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, m_gt_16) {
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, m_lt_32) {
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, m_div_32) {
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, m_gt_32) {
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, k_eq_4) {
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(4)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, k_lt_4) {
    for (size_t k = 1; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, k_gt_4) {
    for (size_t k = 5; k < 8; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, k_div_4) {
    for (size_t k = 8; k <= 40; k += 4) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, m_lt_4) {
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, m_div_4) {
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, m_gt_4) {
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_X4, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, k_eq_4) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(4)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, k_lt_4) {
    for (size_t k = 1; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, k_gt_4) {
    for (size_t k = 5; k < 8; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, k_div_4) {
    for (size_t k = 8; k <= 40; k += 4) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, m_lt_8) {
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, m_div_8) {
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, m_gt_8) {
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_X4, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, k_eq_4) {
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(4)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, k_lt_4) {
    for (size_t k = 1; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, k_gt_4) {
    for (size_t k = 5; k < 8; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, k_div_4) {
    for (size_t k = 8; k <= 40; k += 4) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, m_lt_16) {
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, m_div_16) {
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, m_gt_16) {
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_X4, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, k_eq_4) {
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(4)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, k_lt_4) {
    for (size_t k = 1; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, k_gt_4) {
    for (size_t k = 5; k < 8; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, k_div_4) {
    for (size_t k = 8; k <= 40; k += 4) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, m_lt_32) {
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, m_div_32) {
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, m_gt_32) {
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_X4, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, k_eq_4) {
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(4)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, k_lt_4) {
    for (size_t k = 1; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, k_gt_4) {
    for (size_t k = 5; k < 8; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, k_div_4) {
    for (size_t k = 8; k <= 40; k += 4) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, m_lt_4) {
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, m_div_4) {
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, m_gt_4) {
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_X4, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, k_eq_4) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(4)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, k_lt_4) {
    for (size_t k = 1; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, k_gt_4) {
    for (size_t k = 5; k < 8; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, k_div_4) {
    for (size_t k = 8; k <= 40; k += 4) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, m_lt_8) {
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, m_div_8) {
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, m_gt_8) {
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_X4, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, k_eq_4) {
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(4)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, k_lt_4) {
    for (size_t k = 1; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, k_gt_4) {
    for (size_t k = 5; k < 8; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, k_div_4) {
    for (size_t k = 8; k <= 40; k += 4) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, m_lt_16) {
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, m_div_16) {
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, m_gt_16) {
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_X4, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, k_eq_4) {
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(4)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, k_lt_4) {
    for (size_t k = 1; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, k_gt_4) {
    for (size_t k = 5; k < 8; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, k_div_4) {
    for (size_t k = 8; k <= 40; k += 4) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, m_lt_32) {
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, m_div_32) {
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, m_gt_32) {
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 20; k += 5) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_X4, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 20; k += 5) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_x4, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED, m_lt_4) {
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED, m_div_4) {
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED, m_gt_4) {
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED, m_lt_8) {
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED, m_div_8) {
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED, m_gt_8) {
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED, m_lt_16) {
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED, m_div_16) {
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED, m_gt_16) {
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED, m_lt_32) {
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED, m_div_32) {
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED, m_gt_32) {
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED, m_lt_4) {
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED, m_div_4) {
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED, m_gt_4) {
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED, m_lt_8) {
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED, m_div_8) {
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED, m_gt_8) {
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED, m_lt_16) {
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED, m_div_16) {
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED, m_gt_16) {
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED, k_eq_1) {
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED, k_gt_1) {
    for (size_t k = 2; k < 10; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED, m_lt_32) {
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED, m_div_32) {
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED, m_gt_32) {
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 5; k += 2) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, m_lt_4) {
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, m_div_4) {
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, m_gt_4) {
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_ARM_PIPELINED_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, m_lt_8) {
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, m_div_8) {
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, m_gt_8) {
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_ARM_PIPELINED_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, m_lt_16) {
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, m_div_16) {
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, m_gt_16) {
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_ARM_PIPELINED_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, m_lt_32) {
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, m_div_32) {
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, m_gt_32) {
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_ARM_PIPELINED_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_arm_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(4)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, m_lt_4) {
    for (uint32_t m = 1; m < 4; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, m_div_4) {
    for (uint32_t m = 8; m <= 12; m += 4) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, m_gt_4) {
    for (uint32_t m = 5; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(4)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .output_stride(11)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_4X1__WASMSIMD_X86_PIPELINED_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(8)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, m_lt_8) {
    for (uint32_t m = 1; m < 8; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, m_div_8) {
    for (uint32_t m = 16; m <= 24; m += 8) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, m_gt_8) {
    for (uint32_t m = 9; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(8)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .output_stride(19)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_8X1__WASMSIMD_X86_PIPELINED_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(16)
      .nr(1)
      .m(16)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(16)
        .nr(1)
        .m(16)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(16)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, m_lt_16) {
    for (uint32_t m = 1; m < 16; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, m_div_16) {
    for (uint32_t m = 32; m <= 48; m += 16) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, m_gt_16) {
    for (uint32_t m = 17; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(16)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .output_stride(37)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_16X1__WASMSIMD_X86_PIPELINED_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(16)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_16x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


#if XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD
  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, k_eq_2) {
    SpMMMicrokernelTester()
      .mr(32)
      .nr(1)
      .m(32)
      .n(1)
      .k(2)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, k_lt_2) {
    for (size_t k = 1; k < 2; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, k_gt_2) {
    for (size_t k = 3; k < 4; k++) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, k_div_2) {
    for (size_t k = 4; k <= 20; k += 2) {
      SpMMMicrokernelTester()
        .mr(32)
        .nr(1)
        .m(32)
        .n(1)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, n_gt_1) {
    for (uint32_t n = 2; n < 10; n++) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(32)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, m_lt_32) {
    for (uint32_t m = 1; m < 32; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, m_div_32) {
    for (uint32_t m = 64; m <= 96; m += 32) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, m_gt_32) {
    for (uint32_t m = 33; m < 64; m++) {
      for (uint32_t n = 1; n < 10; n += 2) {
        for (size_t k = 1; k <= 10; k += 3) {
          SpMMMicrokernelTester()
            .mr(32)
            .nr(1)
            .m(m)
            .n(n)
            .k(k)
            .sparsity(0.0f)
            .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
        }
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, output_stride) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .output_stride(67)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, qmin) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmin(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, qmax) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .qmax(128)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, half_sparse) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(0.5f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }

  TEST(F32_SPMM_MINMAX_32X1__WASMSIMD_X86_PIPELINED_X2, zero_weights) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 10; k += 3) {
        SpMMMicrokernelTester()
          .mr(32)
          .nr(1)
          .m(64)
          .n(n)
          .k(k)
          .sparsity(1.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_32x1__wasmsimd_x86_pipelined_x2, xnn_init_f32_minmax_wasmsimd_params);
      }
    }
  }
#endif  // XNN_ARCH_WASMSIMD || XNN_ARCH_WASMRELAXEDSIMD


TEST(F32_SPMM_MINMAX_1X1__SCALAR, k_eq_1) {
  SpMMMicrokernelTester()
    .mr(1)
    .nr(1)
    .m(1)
    .n(1)
    .k(1)
    .sparsity(0.0f)
    .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar, xnn_init_f32_minmax_scalar_params);
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    SpMMMicrokernelTester()
      .mr(1)
      .nr(1)
      .m(1)
      .n(1)
      .k(k)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar, xnn_init_f32_minmax_scalar_params);
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR, n_gt_1) {
  for (uint32_t n = 2; n < 10; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(1)
        .nr(1)
        .m(1)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR, m_lt_1) {
  for (uint32_t m = 1; m < 1; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(1)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR, m_div_1) {
  for (uint32_t m = 2; m <= 3; m += 1) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(1)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR, m_gt_1) {
  for (uint32_t m = 2; m < 2; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(1)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR, output_stride) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(1)
        .nr(1)
        .m(2)
        .n(n)
        .k(k)
        .output_stride(5)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR, qmin) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(1)
        .nr(1)
        .m(2)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmin(128)
        .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR, qmax) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(1)
        .nr(1)
        .m(2)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmax(128)
        .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR, half_sparse) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(1)
        .nr(1)
        .m(2)
        .n(n)
        .k(k)
        .sparsity(0.5f)
        .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR, zero_weights) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(1)
        .nr(1)
        .m(2)
        .n(n)
        .k(k)
        .sparsity(1.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR_PIPELINED, k_eq_1) {
  SpMMMicrokernelTester()
    .mr(1)
    .nr(1)
    .m(1)
    .n(1)
    .k(1)
    .sparsity(0.0f)
    .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR_PIPELINED, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    SpMMMicrokernelTester()
      .mr(1)
      .nr(1)
      .m(1)
      .n(1)
      .k(k)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR_PIPELINED, n_gt_1) {
  for (uint32_t n = 2; n < 10; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(1)
        .nr(1)
        .m(1)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR_PIPELINED, m_lt_1) {
  for (uint32_t m = 1; m < 1; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(1)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR_PIPELINED, m_div_1) {
  for (uint32_t m = 2; m <= 3; m += 1) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(1)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR_PIPELINED, m_gt_1) {
  for (uint32_t m = 2; m < 2; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(1)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR_PIPELINED, output_stride) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(1)
        .nr(1)
        .m(2)
        .n(n)
        .k(k)
        .output_stride(5)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR_PIPELINED, qmin) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(1)
        .nr(1)
        .m(2)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmin(128)
        .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR_PIPELINED, qmax) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(1)
        .nr(1)
        .m(2)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmax(128)
        .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR_PIPELINED, half_sparse) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(1)
        .nr(1)
        .m(2)
        .n(n)
        .k(k)
        .sparsity(0.5f)
        .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_1X1__SCALAR_PIPELINED, zero_weights) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(1)
        .nr(1)
        .m(2)
        .n(n)
        .k(k)
        .sparsity(1.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_1x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR, k_eq_1) {
  SpMMMicrokernelTester()
    .mr(2)
    .nr(1)
    .m(2)
    .n(1)
    .k(1)
    .sparsity(0.0f)
    .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar, xnn_init_f32_minmax_scalar_params);
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    SpMMMicrokernelTester()
      .mr(2)
      .nr(1)
      .m(2)
      .n(1)
      .k(k)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar, xnn_init_f32_minmax_scalar_params);
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR, n_gt_1) {
  for (uint32_t n = 2; n < 10; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(2)
        .nr(1)
        .m(2)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR, m_lt_2) {
  for (uint32_t m = 1; m < 2; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(2)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR, m_div_2) {
  for (uint32_t m = 4; m <= 6; m += 2) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(2)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR, m_gt_2) {
  for (uint32_t m = 3; m < 4; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(2)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR, output_stride) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(2)
        .nr(1)
        .m(4)
        .n(n)
        .k(k)
        .output_stride(7)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR, qmin) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(2)
        .nr(1)
        .m(4)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmin(128)
        .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR, qmax) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(2)
        .nr(1)
        .m(4)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmax(128)
        .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR, half_sparse) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(2)
        .nr(1)
        .m(4)
        .n(n)
        .k(k)
        .sparsity(0.5f)
        .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR, zero_weights) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(2)
        .nr(1)
        .m(4)
        .n(n)
        .k(k)
        .sparsity(1.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR_PIPELINED, k_eq_1) {
  SpMMMicrokernelTester()
    .mr(2)
    .nr(1)
    .m(2)
    .n(1)
    .k(1)
    .sparsity(0.0f)
    .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR_PIPELINED, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    SpMMMicrokernelTester()
      .mr(2)
      .nr(1)
      .m(2)
      .n(1)
      .k(k)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR_PIPELINED, n_gt_1) {
  for (uint32_t n = 2; n < 10; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(2)
        .nr(1)
        .m(2)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR_PIPELINED, m_lt_2) {
  for (uint32_t m = 1; m < 2; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(2)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR_PIPELINED, m_div_2) {
  for (uint32_t m = 4; m <= 6; m += 2) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(2)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR_PIPELINED, m_gt_2) {
  for (uint32_t m = 3; m < 4; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(2)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR_PIPELINED, output_stride) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(2)
        .nr(1)
        .m(4)
        .n(n)
        .k(k)
        .output_stride(7)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR_PIPELINED, qmin) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(2)
        .nr(1)
        .m(4)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmin(128)
        .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR_PIPELINED, qmax) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(2)
        .nr(1)
        .m(4)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmax(128)
        .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR_PIPELINED, half_sparse) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(2)
        .nr(1)
        .m(4)
        .n(n)
        .k(k)
        .sparsity(0.5f)
        .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_2X1__SCALAR_PIPELINED, zero_weights) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(2)
        .nr(1)
        .m(4)
        .n(n)
        .k(k)
        .sparsity(1.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_2x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR, k_eq_1) {
  SpMMMicrokernelTester()
    .mr(4)
    .nr(1)
    .m(4)
    .n(1)
    .k(1)
    .sparsity(0.0f)
    .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar, xnn_init_f32_minmax_scalar_params);
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(k)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar, xnn_init_f32_minmax_scalar_params);
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR, n_gt_1) {
  for (uint32_t n = 2; n < 10; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR, m_lt_4) {
  for (uint32_t m = 1; m < 4; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR, m_div_4) {
  for (uint32_t m = 8; m <= 12; m += 4) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR, m_gt_4) {
  for (uint32_t m = 5; m < 8; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR, output_stride) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(8)
        .n(n)
        .k(k)
        .output_stride(11)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR, qmin) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmin(128)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR, qmax) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmax(128)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR, half_sparse) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(0.5f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR, zero_weights) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(1.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR_PIPELINED, k_eq_1) {
  SpMMMicrokernelTester()
    .mr(4)
    .nr(1)
    .m(4)
    .n(1)
    .k(1)
    .sparsity(0.0f)
    .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR_PIPELINED, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    SpMMMicrokernelTester()
      .mr(4)
      .nr(1)
      .m(4)
      .n(1)
      .k(k)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR_PIPELINED, n_gt_1) {
  for (uint32_t n = 2; n < 10; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(4)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR_PIPELINED, m_lt_4) {
  for (uint32_t m = 1; m < 4; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR_PIPELINED, m_div_4) {
  for (uint32_t m = 8; m <= 12; m += 4) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR_PIPELINED, m_gt_4) {
  for (uint32_t m = 5; m < 8; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(4)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR_PIPELINED, output_stride) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(8)
        .n(n)
        .k(k)
        .output_stride(11)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR_PIPELINED, qmin) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmin(128)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR_PIPELINED, qmax) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmax(128)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR_PIPELINED, half_sparse) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(0.5f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_4X1__SCALAR_PIPELINED, zero_weights) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(4)
        .nr(1)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(1.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_4x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR, k_eq_1) {
  SpMMMicrokernelTester()
    .mr(8)
    .nr(1)
    .m(8)
    .n(1)
    .k(1)
    .sparsity(0.0f)
    .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar, xnn_init_f32_minmax_scalar_params);
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(k)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar, xnn_init_f32_minmax_scalar_params);
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR, n_gt_1) {
  for (uint32_t n = 2; n < 10; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR, m_lt_8) {
  for (uint32_t m = 1; m < 8; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR, m_div_8) {
  for (uint32_t m = 16; m <= 24; m += 8) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR, m_gt_8) {
  for (uint32_t m = 9; m < 16; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR, output_stride) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(16)
        .n(n)
        .k(k)
        .output_stride(19)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR, qmin) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmin(128)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR, qmax) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmax(128)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR, half_sparse) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(0.5f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR, zero_weights) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(1.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR_PIPELINED, k_eq_1) {
  SpMMMicrokernelTester()
    .mr(8)
    .nr(1)
    .m(8)
    .n(1)
    .k(1)
    .sparsity(0.0f)
    .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR_PIPELINED, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(1)
      .m(8)
      .n(1)
      .k(k)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR_PIPELINED, n_gt_1) {
  for (uint32_t n = 2; n < 10; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR_PIPELINED, m_lt_8) {
  for (uint32_t m = 1; m < 8; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR_PIPELINED, m_div_8) {
  for (uint32_t m = 16; m <= 24; m += 8) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR_PIPELINED, m_gt_8) {
  for (uint32_t m = 9; m < 16; m++) {
    for (uint32_t n = 1; n < 10; n += 2) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(1)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR_PIPELINED, output_stride) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(16)
        .n(n)
        .k(k)
        .output_stride(19)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR_PIPELINED, qmin) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmin(128)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR_PIPELINED, qmax) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmax(128)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR_PIPELINED, half_sparse) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(0.5f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X1__SCALAR_PIPELINED, zero_weights) {
  for (uint32_t n = 1; n < 10; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(1)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(1.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x1__scalar_pipelined, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, k_eq_1) {
  SpMMMicrokernelTester()
    .mr(8)
    .nr(2)
    .m(8)
    .n(2)
    .k(1)
    .sparsity(0.0f)
    .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 2; n++) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(2)
      .m(8)
      .n(n)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(2)
      .m(8)
      .n(2)
      .k(k)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 2; n++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(2)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, n_gt_2) {
  for (uint32_t n = 3; n < 10; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(2)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, n_div_2) {
  for (uint32_t n = 4; n <= 6; n += 2) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(2)
        .m(8)
        .n(n)
        .k(k)
        .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, m_lt_8) {
  for (uint32_t m = 1; m < 8; m++) {
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(2)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, m_div_8) {
  for (uint32_t m = 16; m <= 24; m += 8) {
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(2)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, m_gt_8) {
  for (uint32_t m = 9; m < 16; m++) {
    for (uint32_t n = 1; n < 10; n += 3) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(2)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, output_stride) {
  for (uint32_t n = 1; n < 10; n += 3) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(2)
        .m(16)
        .n(n)
        .k(k)
        .output_stride(19)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, qmin) {
  for (uint32_t n = 1; n < 10; n += 3) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(2)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmin(128)
        .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, qmax) {
  for (uint32_t n = 1; n < 10; n += 3) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(2)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmax(128)
        .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, half_sparse) {
  for (uint32_t n = 1; n < 10; n += 3) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(2)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(0.5f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X2__SCALAR, zero_weights) {
  for (uint32_t n = 1; n < 10; n += 3) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(2)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(1.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x2__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, k_eq_1) {
  SpMMMicrokernelTester()
    .mr(8)
    .nr(4)
    .m(8)
    .n(4)
    .k(1)
    .sparsity(0.0f)
    .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, k_eq_1_subtile) {
  for (uint32_t n = 1; n <= 4; n++) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(4)
      .m(8)
      .n(n)
      .k(1)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, k_gt_1) {
  for (size_t k = 2; k < 10; k++) {
    SpMMMicrokernelTester()
      .mr(8)
      .nr(4)
      .m(8)
      .n(4)
      .k(k)
      .sparsity(0.0f)
      .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, k_gt_1_subtile) {
  for (size_t k = 2; k < 10; k++) {
    for (uint32_t n = 1; n <= 4; n++) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(4)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, n_gt_4) {
  for (uint32_t n = 5; n < 10; n++) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(4)
        .m(8)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, n_div_4) {
  for (uint32_t n = 8; n <= 12; n += 4) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(4)
        .m(8)
        .n(n)
        .k(k)
        .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, m_lt_8) {
  for (uint32_t m = 1; m < 8; m++) {
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(4)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, m_div_8) {
  for (uint32_t m = 16; m <= 24; m += 8) {
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(4)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, m_gt_8) {
  for (uint32_t m = 9; m < 16; m++) {
    for (uint32_t n = 1; n < 20; n += 5) {
      for (size_t k = 1; k <= 5; k += 2) {
        SpMMMicrokernelTester()
          .mr(8)
          .nr(4)
          .m(m)
          .n(n)
          .k(k)
          .sparsity(0.0f)
          .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
      }
    }
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, output_stride) {
  for (uint32_t n = 1; n < 20; n += 5) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(4)
        .m(16)
        .n(n)
        .k(k)
        .output_stride(19)
        .sparsity(0.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, qmin) {
  for (uint32_t n = 1; n < 20; n += 5) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(4)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmin(128)
        .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, qmax) {
  for (uint32_t n = 1; n < 20; n += 5) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(4)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(0.0f)
        .qmax(128)
        .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, half_sparse) {
  for (uint32_t n = 1; n < 20; n += 5) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(4)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(0.5f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}

TEST(F32_SPMM_MINMAX_8X4__SCALAR, zero_weights) {
  for (uint32_t n = 1; n < 20; n += 5) {
    for (size_t k = 1; k <= 5; k += 2) {
      SpMMMicrokernelTester()
        .mr(8)
        .nr(4)
        .m(16)
        .n(n)
        .k(k)
        .sparsity(1.0f)
        .Test(xnn_f32_spmm_minmax_ukernel_8x4__scalar, xnn_init_f32_minmax_scalar_params);
    }
  }
}