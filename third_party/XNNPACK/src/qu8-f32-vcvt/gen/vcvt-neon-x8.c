// Auto-generated file. Do not edit!
//   Template: src/qs8-f32-vcvt/neon.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <arm_neon.h>

#include <xnnpack/common.h>
#include <xnnpack/intrinsics-polyfill.h>
#include <xnnpack/vcvt.h>


void xnn_qu8_f32_vcvt_ukernel__neon_x8(
    size_t n,
    const uint8_t* x,
    float* y,
    const union xnn_qu8_f32_cvt_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(n != 0);
  assert(n % sizeof(uint8_t) == 0);
  assert(x != NULL);
  assert(y != NULL);

  const int16x8_t vminus_zero_point = vreinterpretq_s16_u32(vld1q_dup_u32((const void*) params->neon.minus_zero_point));
  const float32x4_t vscale = vld1q_dup_f32(&params->neon.scale);
  for (; n >= 8 * sizeof(uint8_t); n -= 8 * sizeof(uint8_t)) {
    const uint8x8_t vx = vld1_u8(x); x += 8;

    const int16x8_t vhx = vreinterpretq_s16_u16(vaddw_u8(vreinterpretq_u16_s16(vminus_zero_point), vx));

    const int32x4_t vwx_lo = vmovl_s16(vget_low_s16(vhx));
    const int32x4_t vwx_hi = vmovl_s16(vget_high_s16(vhx));

    float32x4_t vy_lo = vcvtq_f32_s32(vwx_lo);
    float32x4_t vy_hi = vcvtq_f32_s32(vwx_hi);

    vy_lo = vmulq_f32(vy_lo, vscale);
    vy_hi = vmulq_f32(vy_hi, vscale);

    vst1q_f32(y, vy_lo); y += 4;
    vst1q_f32(y, vy_hi); y += 4;
  }
  if XNN_UNLIKELY(n != 0) {
    assert(n >= 1 * sizeof(uint8_t));
    assert(n <= 7 * sizeof(uint8_t));

    const uint8x8_t vx = vld1_u8(x);

    const int16x8_t vhx = vreinterpretq_s16_u16(vaddw_u8(vreinterpretq_u16_s16(vminus_zero_point), vx));

    const int32x4_t vwx_lo = vmovl_s16(vget_low_s16(vhx));
    const int32x4_t vwx_hi = vmovl_s16(vget_high_s16(vhx));

    float32x4_t vy = vcvtq_f32_s32(vwx_lo);
    vy = vmulq_f32(vy, vscale);

    if (n & (4 * sizeof(uint8_t))) {
      vst1q_f32(y, vy); y += 4;
      vy = vcvtq_f32_s32(vwx_hi);
      vy = vmulq_f32(vy, vscale);
    }
    float32x2_t vy_lo = vget_low_f32(vy);
    if (n & (2 * sizeof(uint8_t))) {
      vst1_f32(y, vy_lo); y += 2;
      vy_lo = vget_high_f32(vy);
    }
    if (n & (1 * sizeof(uint8_t))) {
      vst1_lane_f32(y, vy_lo, 0);
    }
  }
}
