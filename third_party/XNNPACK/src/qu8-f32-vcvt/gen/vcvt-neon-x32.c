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


void xnn_qu8_f32_vcvt_ukernel__neon_x32(
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
  for (; n >= 32 * sizeof(uint8_t); n -= 32 * sizeof(uint8_t)) {
    const uint8x8_t vx01234567 = vld1_u8(x); x += 8;
    const uint8x8_t vx89ABCDEF = vld1_u8(x); x += 8;
    const uint8x8_t vxGHIJKLMN = vld1_u8(x); x += 8;
    const uint8x8_t vxOPQRSTUV = vld1_u8(x); x += 8;

    const int16x8_t vhx01234567 = vreinterpretq_s16_u16(vaddw_u8(vreinterpretq_u16_s16(vminus_zero_point), vx01234567));
    const int16x8_t vhx89ABCDEF = vreinterpretq_s16_u16(vaddw_u8(vreinterpretq_u16_s16(vminus_zero_point), vx89ABCDEF));
    const int16x8_t vhxGHIJKLMN = vreinterpretq_s16_u16(vaddw_u8(vreinterpretq_u16_s16(vminus_zero_point), vxGHIJKLMN));
    const int16x8_t vhxOPQRSTUV = vreinterpretq_s16_u16(vaddw_u8(vreinterpretq_u16_s16(vminus_zero_point), vxOPQRSTUV));

    const int32x4_t vwx0123 = vmovl_s16(vget_low_s16(vhx01234567));
    const int32x4_t vwx4567 = vmovl_s16(vget_high_s16(vhx01234567));
    const int32x4_t vwx89AB = vmovl_s16(vget_low_s16(vhx89ABCDEF));
    const int32x4_t vwxCDEF = vmovl_s16(vget_high_s16(vhx89ABCDEF));
    const int32x4_t vwxGHIJ = vmovl_s16(vget_low_s16(vhxGHIJKLMN));
    const int32x4_t vwxKLMN = vmovl_s16(vget_high_s16(vhxGHIJKLMN));
    const int32x4_t vwxOPQR = vmovl_s16(vget_low_s16(vhxOPQRSTUV));
    const int32x4_t vwxSTUV = vmovl_s16(vget_high_s16(vhxOPQRSTUV));

    float32x4_t vy0123 = vcvtq_f32_s32(vwx0123);
    float32x4_t vy4567 = vcvtq_f32_s32(vwx4567);
    float32x4_t vy89AB = vcvtq_f32_s32(vwx89AB);
    float32x4_t vyCDEF = vcvtq_f32_s32(vwxCDEF);
    float32x4_t vyGHIJ = vcvtq_f32_s32(vwxGHIJ);
    float32x4_t vyKLMN = vcvtq_f32_s32(vwxKLMN);
    float32x4_t vyOPQR = vcvtq_f32_s32(vwxOPQR);
    float32x4_t vySTUV = vcvtq_f32_s32(vwxSTUV);

    vy0123 = vmulq_f32(vy0123, vscale);
    vy4567 = vmulq_f32(vy4567, vscale);
    vy89AB = vmulq_f32(vy89AB, vscale);
    vyCDEF = vmulq_f32(vyCDEF, vscale);
    vyGHIJ = vmulq_f32(vyGHIJ, vscale);
    vyKLMN = vmulq_f32(vyKLMN, vscale);
    vyOPQR = vmulq_f32(vyOPQR, vscale);
    vySTUV = vmulq_f32(vySTUV, vscale);

    vst1q_f32(y, vy0123); y += 4;
    vst1q_f32(y, vy4567); y += 4;
    vst1q_f32(y, vy89AB); y += 4;
    vst1q_f32(y, vyCDEF); y += 4;
    vst1q_f32(y, vyGHIJ); y += 4;
    vst1q_f32(y, vyKLMN); y += 4;
    vst1q_f32(y, vyOPQR); y += 4;
    vst1q_f32(y, vySTUV); y += 4;
  }
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
