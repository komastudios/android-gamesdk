// Auto-generated file. Do not edit!
//   Template: src/f32-velu/neon-lut16-p3.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <arm_neon.h>

#include <xnnpack/vunary.h>
#include <xnnpack/common.h>


extern XNN_INTERNAL const int32_t xnn_table_exp2minus_k_over_16[16];

void xnn_f32_velu_ukernel__neonfma_rr1_lut16_p3_x4(
    size_t n,
    const float* x,
    float* y,
    const union xnn_f32_elu_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(n != 0);
  assert(n % sizeof(float) == 0);
  assert(x != NULL);
  assert(y != NULL);

  const float32x4_t vprescale = vld1q_dup_f32(&params->neonfma_rr1_lut16_p3.prescale);
  const float32x4_t valpha = vld1q_dup_f32(&params->neonfma_rr1_lut16_p3.alpha);
  const float32x4_t vbeta = vld1q_dup_f32(&params->neonfma_rr1_lut16_p3.beta);
  const float32x4_t vsat_cutoff = vld1q_dup_f32(&params->neonfma_rr1_lut16_p3.sat_cutoff);
  const float32x4_t vmagic_bias = vld1q_dup_f32(&params->neonfma_rr1_lut16_p3.magic_bias);
  const float32x4_t vlog2e = vld1q_dup_f32(&params->neonfma_rr1_lut16_p3.log2e);
  const int32x4_t vindex_mask = vmovq_n_s32(0xF);
  const float32x4_t vminus_ln2 = vld1q_dup_f32(&params->neonfma_rr1_lut16_p3.minus_ln2);
  const float32x4_t vc3 = vld1q_dup_f32(&params->neonfma_rr1_lut16_p3.c3);
  const float32x4_t vc2 = vld1q_dup_f32(&params->neonfma_rr1_lut16_p3.c2);
  const float32x4_t vone = vmovq_n_f32(1.0f);

  for (; n >= 4 * sizeof(float); n -= 4 * sizeof(float)) {
    float32x4_t vx = vld1q_f32(x); x += 4;

    const float32x4_t vz = vmaxq_f32(vmulq_f32(vx, vprescale), vsat_cutoff);

    float32x4_t vn = vfmaq_f32(vmagic_bias, vz, vlog2e);
    const uint64x2_t vidx = vreinterpretq_u64_s32(vshlq_n_s32(vandq_s32(vreinterpretq_s32_f32(vn), vindex_mask), 2));
    const int32x4_t ven = vshlq_n_s32(vreinterpretq_s32_f32(vn), 19);

    const uint64_t vidx_lo = vgetq_lane_u64(vidx, 0);
    const uint64_t vidx_hi = vgetq_lane_u64(vidx, 1);
    int32x2_t vl_lo = vld1_dup_s32((const int32_t*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx_lo));
    int32x2_t vl_hi = vld1_dup_s32((const int32_t*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx_hi));
    vl_lo = vld1_lane_s32((const int32_t*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx_lo >> 32)), vl_lo, 1);
    vl_hi = vld1_lane_s32((const int32_t*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx_hi >> 32)), vl_hi, 1);

    vn = vsubq_f32(vn, vmagic_bias);
    const int32x4_t vl = vcombine_s32(vl_lo, vl_hi);

    float32x4_t vt = vfmaq_f32(vz, vn, vminus_ln2);
    float32x4_t vs = vreinterpretq_f32_s32(vaddq_s32(vl, ven));

    float32x4_t vp = vfmaq_f32(vc2, vc3, vt);
    vp = vmulq_f32(vp, vt);

    vt = vmulq_f32(vt, vs);
    vs = vsubq_f32(vs, vone);
    vp = vfmaq_f32(vt, vp, vt);
    const float32x4_t ve = vmulq_f32(vaddq_f32(vp, vs), valpha);

    const uint32x4_t vm = vcltq_f32(vx, vmovq_n_f32(0.0f));
    vx = vmulq_f32(vx, vbeta);
    const float32x4_t vy = vbslq_f32(vm, ve, vx);

    vst1q_f32(y, vy); y += 4;
  }
  if XNN_UNLIKELY(n != 0) {
    float32x4_t vx = vld1q_f32(x);

    const float32x4_t vz = vmaxq_f32(vmulq_f32(vx, vprescale), vsat_cutoff);

    float32x4_t vn = vfmaq_f32(vmagic_bias, vz, vlog2e);
    const uint64x2_t vidx = vreinterpretq_u64_s32(vshlq_n_s32(vandq_s32(vreinterpretq_s32_f32(vn), vindex_mask), 2));
    const int32x4_t ven = vshlq_n_s32(vreinterpretq_s32_f32(vn), 19);

    const uint64_t vidx_lo = vgetq_lane_u64(vidx, 0);
    const uint64_t vidx_hi = vgetq_lane_u64(vidx, 1);
    int32x2_t vl_lo = vld1_dup_s32((const int32_t*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx_lo));
    int32x2_t vl_hi = vld1_dup_s32((const int32_t*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx_hi));
    vl_lo = vld1_lane_s32((const int32_t*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx_lo >> 32)), vl_lo, 1);
    vl_hi = vld1_lane_s32((const int32_t*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx_hi >> 32)), vl_hi, 1);

    vn = vsubq_f32(vn, vmagic_bias);
    const int32x4_t vl = vcombine_s32(vl_lo, vl_hi);

    float32x4_t vt = vfmaq_f32(vz, vn, vminus_ln2);
    float32x4_t vs = vreinterpretq_f32_s32(vaddq_s32(vl, ven));

    float32x4_t vp = vfmaq_f32(vc2, vc3, vt);
    vp = vmulq_f32(vp, vt);

    vt = vmulq_f32(vt, vs);
    vs = vsubq_f32(vs, vone);
    vp = vfmaq_f32(vt, vp, vt);
    const float32x4_t ve = vmulq_f32(vaddq_f32(vp, vs), valpha);

    const uint32x4_t vm = vcltq_f32(vx, vmovq_n_f32(0.0f));
    vx = vmulq_f32(vx, vbeta);
    const float32x4_t vy = vbslq_f32(vm, ve, vx);

    float32x2_t vy_lo = vget_low_f32(vy);
    if (n & (2 * sizeof(float))) {
      vst1_f32(y, vy_lo); y += 2;
      vy_lo = vget_high_f32(vy);
    }
    if (n & (1 * sizeof(float))) {
      vst1_lane_f32(y, vy_lo, 0);
    }
  }
}
