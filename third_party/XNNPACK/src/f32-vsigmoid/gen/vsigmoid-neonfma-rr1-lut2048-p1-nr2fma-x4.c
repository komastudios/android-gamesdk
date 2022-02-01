// Auto-generated file. Do not edit!
//   Template: src/f32-vsigmoid/neon-lut2048-p1.c.in
//   Generator: tools/xngen
//
// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <arm_neon.h>

#include <xnnpack/common.h>
#include <xnnpack/vunary.h>


extern XNN_INTERNAL const float xnn_table_exp2minus_k_over_2048[2048];

void xnn_f32_vsigmoid_ukernel__neonfma_rr1_lut2048_p1_nr2fma_x4(
    size_t n,
    const float* x,
    float* y,
    const union xnn_f32_sigmoid_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(n % sizeof(float) == 0);

  const float32x4_t vmagic_bias = vld1q_dup_f32(&params->neonfma_rr1_lut2048_p1.magic_bias);
  const float32x4_t vminus_log2e = vld1q_dup_f32(&params->neonfma_rr1_lut2048_p1.minus_log2e);
  const int32x4_t vindex_mask = vmovq_n_s32(INT32_C(0x7FF));
  const float32x4_t vln2 = vld1q_dup_f32(&params->neonfma_rr1_lut2048_p1.ln2);
  const float32x4_t vc1 = vld1q_dup_f32(&params->neonfma_rr1_lut2048_p1.c1);
  const float32x4_t vone = vmovq_n_f32(1.0f);
  const float32x4_t vdenorm_cutoff = vld1q_dup_f32(&params->neonfma_rr1_lut2048_p1.denorm_cutoff);

  for (; n >= 4 * sizeof(float); n -= 4 * sizeof(float)) {
    const float32x4_t vx = vld1q_f32(x); x += 4;

    const float32x4_t vz = vabsq_f32(vx);

    float32x4_t vn = vfmaq_f32(vmagic_bias, vz, vminus_log2e);
    const int32x4_t ve = vshlq_n_s32(vreinterpretq_s32_f32(vn), 12);

    const uint64x2_t vidx = vreinterpretq_u64_s32(vandq_s32(vreinterpretq_s32_f32(vn), vindex_mask));
    const uint64_t vidx_lo = vgetq_lane_u64(vidx, 0);
    const uint64_t vidx_hi = vgetq_lane_u64(vidx, 1);
    float32x2_t vl_lo = vld1_dup_f32(&xnn_table_exp2minus_k_over_2048[(uint32_t) vidx_lo]);
    float32x2_t vl_hi = vld1_dup_f32(&xnn_table_exp2minus_k_over_2048[(uint32_t) vidx_hi]);
    vl_lo = vld1_lane_f32(&xnn_table_exp2minus_k_over_2048[(uint32_t) (vidx_lo >> 32)], vl_lo, 1);
    vl_hi = vld1_lane_f32(&xnn_table_exp2minus_k_over_2048[(uint32_t) (vidx_hi >> 32)], vl_hi, 1);
    const float32x4_t vl = vcombine_f32(vl_lo, vl_hi);

    const float32x4_t vs = vreinterpretq_f32_s32(vaddq_s32(vreinterpretq_s32_f32(vl), ve));
    vn = vsubq_f32(vn, vmagic_bias);
    float32x4_t vt = vfmaq_f32(vz, vn, vln2);

    const float32x4_t vp = vmulq_f32(vt, vc1);

    const float32x4_t vy = vfmaq_f32(vs, vs, vp);
    const float32x4_t vd = vaddq_f32(vy, vone);

    float32x4_t vr = vrecpeq_f32(vd);
    vr = vfmaq_f32(vr, vr, vfmsq_f32(vone, vr, vd));
    vr = vfmaq_f32(vr, vr, vfmsq_f32(vone, vr, vd));

    float32x4_t vf = vmulq_f32(vy, vr);
    vf = vreinterpretq_f32_u32(vbicq_u32(vreinterpretq_u32_f32(vf), vcagtq_f32(vx, vdenorm_cutoff)));
    const uint32x4_t vm = vcltq_f32(vx, vmovq_n_f32(0.0f));
    vf = vbslq_f32(vm, vf, vsubq_f32(vone, vf));

    vst1q_f32(y, vf); y += 4;
  }
  if XNN_UNLIKELY(n != 0) {
    const float32x4_t vx = vld1q_f32(x);

    const float32x4_t vz = vabsq_f32(vx);

    float32x4_t vn = vfmaq_f32(vmagic_bias, vz, vminus_log2e);
    const int32x4_t ve = vshlq_n_s32(vreinterpretq_s32_f32(vn), 12);

    const uint64x2_t vidx = vreinterpretq_u64_s32(vandq_s32(vreinterpretq_s32_f32(vn), vindex_mask));
    const uint64_t vidx_lo = vgetq_lane_u64(vidx, 0);
    const uint64_t vidx_hi = vgetq_lane_u64(vidx, 1);
    float32x2_t vl_lo = vld1_dup_f32(&xnn_table_exp2minus_k_over_2048[(uint32_t) vidx_lo]);
    float32x2_t vl_hi = vld1_dup_f32(&xnn_table_exp2minus_k_over_2048[(uint32_t) vidx_hi]);
    vl_lo = vld1_lane_f32(&xnn_table_exp2minus_k_over_2048[(uint32_t) (vidx_lo >> 32)], vl_lo, 1);
    vl_hi = vld1_lane_f32(&xnn_table_exp2minus_k_over_2048[(uint32_t) (vidx_hi >> 32)], vl_hi, 1);
    const float32x4_t vl = vcombine_f32(vl_lo, vl_hi);

    const float32x4_t vs = vreinterpretq_f32_s32(vaddq_s32(vreinterpretq_s32_f32(vl), ve));
    vn = vsubq_f32(vn, vmagic_bias);
    float32x4_t vt = vfmaq_f32(vz, vn, vln2);

    const float32x4_t vp = vmulq_f32(vt, vc1);

    const float32x4_t vy = vfmaq_f32(vs, vs, vp);
    const float32x4_t vd = vaddq_f32(vy, vone);

    float32x4_t vr = vrecpeq_f32(vd);
    vr = vfmaq_f32(vr, vr, vfmsq_f32(vone, vr, vd));
    vr = vfmaq_f32(vr, vr, vfmsq_f32(vone, vr, vd));

    float32x4_t vf = vmulq_f32(vy, vr);
    vf = vreinterpretq_f32_u32(vbicq_u32(vreinterpretq_u32_f32(vf), vcagtq_f32(vx, vdenorm_cutoff)));
    const uint32x4_t vm = vcltq_f32(vx, vmovq_n_f32(0.0f));
    vf = vbslq_f32(vm, vf, vsubq_f32(vone, vf));

    float32x2_t vf_lo = vget_low_f32(vf);
    if (n & (2 * sizeof(float))) {
      vst1_f32(y, vf_lo); y += 2;
      vf_lo = vget_high_f32(vf);
    }
    if (n & (1 * sizeof(float))) {
      vst1_lane_f32(y, vf_lo, 0);
    }
  }
}
