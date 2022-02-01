// Auto-generated file. Do not edit!
//   Template: src/f32-velu/neon-p6.c.in
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


void xnn_f32_velu_ukernel__neonfma_rr1_p6_x20(
    size_t n,
    const float* x,
    float* y,
    const union xnn_f32_elu_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(n != 0);
  assert(n % sizeof(float) == 0);
  assert(x != NULL);
  assert(y != NULL);

  const float32x4_t vprescale = vld1q_dup_f32(&params->neonfma_rr1_p6.prescale);
  const float32x4_t valpha = vld1q_dup_f32(&params->neonfma_rr1_p6.alpha);
  const float32x4_t vbeta = vld1q_dup_f32(&params->neonfma_rr1_p6.beta);
  const float32x4_t vsat_cutoff = vld1q_dup_f32(&params->neonfma_rr1_p6.sat_cutoff);
  const float32x4_t vmagic_bias = vld1q_dup_f32(&params->neonfma_rr1_p6.magic_bias);
  const float32x4_t vlog2e = vld1q_dup_f32(&params->neonfma_rr1_p6.log2e);
  const float32x4_t vminus_ln2 = vld1q_dup_f32(&params->neonfma_rr1_p6.minus_ln2);
  const float32x4_t vc6 = vld1q_dup_f32(&params->neonfma_rr1_p6.c6);
  const float32x4_t vc5 = vld1q_dup_f32(&params->neonfma_rr1_p6.c5);
  const float32x4_t vc4 = vld1q_dup_f32(&params->neonfma_rr1_p6.c4);
  const float32x4_t vc3 = vld1q_dup_f32(&params->neonfma_rr1_p6.c3);
  const float32x4_t vc2 = vld1q_dup_f32(&params->neonfma_rr1_p6.c2);
  const float32x4_t vone = vmovq_n_f32(1.0f);

  for (; n >= 20 * sizeof(float); n -= 20 * sizeof(float)) {
    float32x4_t vx0123 = vld1q_f32(x); x += 4;
    float32x4_t vx4567 = vld1q_f32(x); x += 4;
    float32x4_t vx89AB = vld1q_f32(x); x += 4;
    float32x4_t vxCDEF = vld1q_f32(x); x += 4;
    float32x4_t vxGHIJ = vld1q_f32(x); x += 4;

    const float32x4_t vz0123 = vmaxq_f32(vmulq_f32(vx0123, vprescale), vsat_cutoff);
    const float32x4_t vz4567 = vmaxq_f32(vmulq_f32(vx4567, vprescale), vsat_cutoff);
    const float32x4_t vz89AB = vmaxq_f32(vmulq_f32(vx89AB, vprescale), vsat_cutoff);
    const float32x4_t vzCDEF = vmaxq_f32(vmulq_f32(vxCDEF, vprescale), vsat_cutoff);
    const float32x4_t vzGHIJ = vmaxq_f32(vmulq_f32(vxGHIJ, vprescale), vsat_cutoff);

    float32x4_t vn0123 = vfmaq_f32(vmagic_bias, vz0123, vlog2e);
    float32x4_t vn4567 = vfmaq_f32(vmagic_bias, vz4567, vlog2e);
    float32x4_t vn89AB = vfmaq_f32(vmagic_bias, vz89AB, vlog2e);
    float32x4_t vnCDEF = vfmaq_f32(vmagic_bias, vzCDEF, vlog2e);
    float32x4_t vnGHIJ = vfmaq_f32(vmagic_bias, vzGHIJ, vlog2e);

    float32x4_t vs0123 = vreinterpretq_f32_s32(vshlq_n_s32(vreinterpretq_s32_f32(vn0123), 23));
    vn0123 = vsubq_f32(vn0123, vmagic_bias);
    float32x4_t vs4567 = vreinterpretq_f32_s32(vshlq_n_s32(vreinterpretq_s32_f32(vn4567), 23));
    vn4567 = vsubq_f32(vn4567, vmagic_bias);
    float32x4_t vs89AB = vreinterpretq_f32_s32(vshlq_n_s32(vreinterpretq_s32_f32(vn89AB), 23));
    vn89AB = vsubq_f32(vn89AB, vmagic_bias);
    float32x4_t vsCDEF = vreinterpretq_f32_s32(vshlq_n_s32(vreinterpretq_s32_f32(vnCDEF), 23));
    vnCDEF = vsubq_f32(vnCDEF, vmagic_bias);
    float32x4_t vsGHIJ = vreinterpretq_f32_s32(vshlq_n_s32(vreinterpretq_s32_f32(vnGHIJ), 23));
    vnGHIJ = vsubq_f32(vnGHIJ, vmagic_bias);

    float32x4_t vt0123 = vfmaq_f32(vz0123, vn0123, vminus_ln2);
    float32x4_t vt4567 = vfmaq_f32(vz4567, vn4567, vminus_ln2);
    float32x4_t vt89AB = vfmaq_f32(vz89AB, vn89AB, vminus_ln2);
    float32x4_t vtCDEF = vfmaq_f32(vzCDEF, vnCDEF, vminus_ln2);
    float32x4_t vtGHIJ = vfmaq_f32(vzGHIJ, vnGHIJ, vminus_ln2);

    float32x4_t vp0123 = vfmaq_f32(vc5, vc6, vt0123);
    float32x4_t vp4567 = vfmaq_f32(vc5, vc6, vt4567);
    float32x4_t vp89AB = vfmaq_f32(vc5, vc6, vt89AB);
    float32x4_t vpCDEF = vfmaq_f32(vc5, vc6, vtCDEF);
    float32x4_t vpGHIJ = vfmaq_f32(vc5, vc6, vtGHIJ);

    vp0123 = vfmaq_f32(vc4, vp0123, vt0123);
    vp4567 = vfmaq_f32(vc4, vp4567, vt4567);
    vp89AB = vfmaq_f32(vc4, vp89AB, vt89AB);
    vpCDEF = vfmaq_f32(vc4, vpCDEF, vtCDEF);
    vpGHIJ = vfmaq_f32(vc4, vpGHIJ, vtGHIJ);

    vp0123 = vfmaq_f32(vc3, vp0123, vt0123);
    vp4567 = vfmaq_f32(vc3, vp4567, vt4567);
    vp89AB = vfmaq_f32(vc3, vp89AB, vt89AB);
    vpCDEF = vfmaq_f32(vc3, vpCDEF, vtCDEF);
    vpGHIJ = vfmaq_f32(vc3, vpGHIJ, vtGHIJ);

    vp0123 = vfmaq_f32(vc2, vp0123, vt0123);
    vp4567 = vfmaq_f32(vc2, vp4567, vt4567);
    vp89AB = vfmaq_f32(vc2, vp89AB, vt89AB);
    vpCDEF = vfmaq_f32(vc2, vpCDEF, vtCDEF);
    vpGHIJ = vfmaq_f32(vc2, vpGHIJ, vtGHIJ);

    vp0123 = vmulq_f32(vp0123, vt0123);
    vp4567 = vmulq_f32(vp4567, vt4567);
    vp89AB = vmulq_f32(vp89AB, vt89AB);
    vpCDEF = vmulq_f32(vpCDEF, vtCDEF);
    vpGHIJ = vmulq_f32(vpGHIJ, vtGHIJ);

    vt0123 = vmulq_f32(vt0123, vs0123);
    vs0123 = vsubq_f32(vs0123, vone);
    vt4567 = vmulq_f32(vt4567, vs4567);
    vs4567 = vsubq_f32(vs4567, vone);
    vt89AB = vmulq_f32(vt89AB, vs89AB);
    vs89AB = vsubq_f32(vs89AB, vone);
    vtCDEF = vmulq_f32(vtCDEF, vsCDEF);
    vsCDEF = vsubq_f32(vsCDEF, vone);
    vtGHIJ = vmulq_f32(vtGHIJ, vsGHIJ);
    vsGHIJ = vsubq_f32(vsGHIJ, vone);

    vp0123 = vfmaq_f32(vt0123, vp0123, vt0123);
    vp4567 = vfmaq_f32(vt4567, vp4567, vt4567);
    vp89AB = vfmaq_f32(vt89AB, vp89AB, vt89AB);
    vpCDEF = vfmaq_f32(vtCDEF, vpCDEF, vtCDEF);
    vpGHIJ = vfmaq_f32(vtGHIJ, vpGHIJ, vtGHIJ);

    const float32x4_t ve0123 = vmulq_f32(vaddq_f32(vp0123, vs0123), valpha);
    const float32x4_t ve4567 = vmulq_f32(vaddq_f32(vp4567, vs4567), valpha);
    const float32x4_t ve89AB = vmulq_f32(vaddq_f32(vp89AB, vs89AB), valpha);
    const float32x4_t veCDEF = vmulq_f32(vaddq_f32(vpCDEF, vsCDEF), valpha);
    const float32x4_t veGHIJ = vmulq_f32(vaddq_f32(vpGHIJ, vsGHIJ), valpha);

    const uint32x4_t vm0123 = vcltq_f32(vx0123, vmovq_n_f32(0.0f));
    vx0123 = vmulq_f32(vx0123, vbeta);
    const uint32x4_t vm4567 = vcltq_f32(vx4567, vmovq_n_f32(0.0f));
    vx4567 = vmulq_f32(vx4567, vbeta);
    const uint32x4_t vm89AB = vcltq_f32(vx89AB, vmovq_n_f32(0.0f));
    vx89AB = vmulq_f32(vx89AB, vbeta);
    const uint32x4_t vmCDEF = vcltq_f32(vxCDEF, vmovq_n_f32(0.0f));
    vxCDEF = vmulq_f32(vxCDEF, vbeta);
    const uint32x4_t vmGHIJ = vcltq_f32(vxGHIJ, vmovq_n_f32(0.0f));
    vxGHIJ = vmulq_f32(vxGHIJ, vbeta);

    const float32x4_t vy0123 = vbslq_f32(vm0123, ve0123, vx0123);
    const float32x4_t vy4567 = vbslq_f32(vm4567, ve4567, vx4567);
    const float32x4_t vy89AB = vbslq_f32(vm89AB, ve89AB, vx89AB);
    const float32x4_t vyCDEF = vbslq_f32(vmCDEF, veCDEF, vxCDEF);
    const float32x4_t vyGHIJ = vbslq_f32(vmGHIJ, veGHIJ, vxGHIJ);

    vst1q_f32(y, vy0123); y += 4;
    vst1q_f32(y, vy4567); y += 4;
    vst1q_f32(y, vy89AB); y += 4;
    vst1q_f32(y, vyCDEF); y += 4;
    vst1q_f32(y, vyGHIJ); y += 4;
  }
  for (; n >= 4 * sizeof(float); n -= 4 * sizeof(float)) {
    float32x4_t vx = vld1q_f32(x); x += 4;

    const float32x4_t vz = vmaxq_f32(vmulq_f32(vx, vprescale), vsat_cutoff);

    float32x4_t vn = vfmaq_f32(vmagic_bias, vz, vlog2e);
    float32x4_t vs = vreinterpretq_f32_s32(vshlq_n_s32(vreinterpretq_s32_f32(vn), 23));
    vn = vsubq_f32(vn, vmagic_bias);

    float32x4_t vt = vfmaq_f32(vz, vn, vminus_ln2);

    float32x4_t vp = vfmaq_f32(vc5, vc6, vt);
    vp = vfmaq_f32(vc4, vp, vt);
    vp = vfmaq_f32(vc3, vp, vt);
    vp = vfmaq_f32(vc2, vp, vt);
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
    float32x4_t vs = vreinterpretq_f32_s32(vshlq_n_s32(vreinterpretq_s32_f32(vn), 23));
    vn = vsubq_f32(vn, vmagic_bias);

    float32x4_t vt = vfmaq_f32(vz, vn, vminus_ln2);

    float32x4_t vp = vfmaq_f32(vc5, vc6, vt);
    vp = vfmaq_f32(vc4, vp, vt);
    vp = vfmaq_f32(vc3, vp, vt);
    vp = vfmaq_f32(vc2, vp, vt);
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
