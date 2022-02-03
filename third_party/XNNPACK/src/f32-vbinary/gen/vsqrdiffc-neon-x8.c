// Auto-generated file. Do not edit!
//   Template: src/f32-vbinary/vopc-neon.c.in
//   Generator: tools/xngen
//
// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <arm_neon.h>

#include <xnnpack/common.h>
#include <xnnpack/vbinary.h>


void xnn_f32_vsqrdiffc_ukernel__neon_x8(
    size_t n,
    const float* a,
    const float* b,
    float* y,
    const union xnn_f32_default_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(n != 0);
  assert(n % sizeof(float) == 0);
  assert(a != NULL);
  assert(b != NULL);
  assert(y != NULL);


  const float32x4_t vb = vld1q_dup_f32(b);
  for (; n >= 8 * sizeof(float); n -= 8 * sizeof(float)) {
    const float32x4_t va0123 = vld1q_f32(a); a += 4;
    const float32x4_t va4567 = vld1q_f32(a); a += 4;

    float32x4_t vy0123 = vsubq_f32(va0123, vb);
    float32x4_t vy4567 = vsubq_f32(va4567, vb);

    vy0123 = vmulq_f32(vy0123, vy0123);
    vy4567 = vmulq_f32(vy4567, vy4567);


    vst1q_f32(y, vy0123); y += 4;
    vst1q_f32(y, vy4567); y += 4;
  }
  for (; n >= 4 * sizeof(float); n -= 4 * sizeof(float)) {
    const float32x4_t va0123 = vld1q_f32(a); a += 4;

    float32x4_t vy0123 = vsubq_f32(va0123, vb);
    vy0123 = vmulq_f32(vy0123, vy0123);
    vst1q_f32(y, vy0123); y += 4;
  }
  if XNN_UNLIKELY(n != 0) {
    const float32x4_t va0123 = vld1q_f32(a);

    float32x4_t vy0123 = vsubq_f32(va0123, vb);
    vy0123 = vmulq_f32(vy0123, vy0123);

    float32x2_t vy01 = vget_low_f32(vy0123);
    if (n & (2 * sizeof(float))) {
      vst1_f32(y, vy01); y += 2;
      vy01 = vget_high_f32(vy0123);
    }
    if (n & (1 * sizeof(float))) {
      vst1_lane_f32(y, vy01, 0);
    }
  }
}
