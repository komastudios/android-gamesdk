// Auto-generated file. Do not edit!
//   Template: src/f32-velu/avx512f-rr1-p6.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <immintrin.h>

#include <xnnpack/common.h>
#include <xnnpack/intrinsics-polyfill.h>
#include <xnnpack/vunary.h>


void xnn_f32_velu_ukernel__avx512f_rr1_p6_x48(
    size_t n,
    const float* x,
    float* y,
    const union xnn_f32_elu_params params[restrict XNN_MIN_ELEMENTS(1)])
{
  assert(n != 0);
  assert(n % sizeof(float) == 0);

  const __m512 vprescale = _mm512_set1_ps(params->avx512_rr1_p6.prescale);
  const __m512 valpha = _mm512_set1_ps(params->avx512_rr1_p6.alpha);
  const __m512 vbeta = _mm512_set1_ps(params->avx512_rr1_p6.beta);
  const __m512 vsat_cutoff = _mm512_set1_ps(params->avx512_rr1_p6.sat_cutoff);
  const __m512 vmagic_bias = _mm512_set1_ps(params->avx512_rr1_p6.magic_bias);
  const __m512 vlog2e = _mm512_set1_ps(params->avx512_rr1_p6.log2e);
  const __m512 vminus_ln2 = _mm512_set1_ps(params->avx512_rr1_p6.minus_ln2);
  const __m512 vc6 = _mm512_set1_ps(params->avx512_rr1_p6.c6);
  const __m512 vc5 = _mm512_set1_ps(params->avx512_rr1_p6.c5);
  const __m512 vc4 = _mm512_set1_ps(params->avx512_rr1_p6.c4);
  const __m512 vc3 = _mm512_set1_ps(params->avx512_rr1_p6.c3);
  const __m512 vc2 = _mm512_set1_ps(params->avx512_rr1_p6.c2);

  for (; n >= 48 * sizeof(float); n -= 48 * sizeof(float)) {
    __m512 vx0 = _mm512_loadu_ps(x);
    __m512 vx1 = _mm512_loadu_ps(x + 16);
    __m512 vx2 = _mm512_loadu_ps(x + 32);
    x += 48;

    const __m512 vz0 = _mm512_max_ps(vsat_cutoff, _mm512_mul_ps(vx0, vprescale));
    const __m512 vz1 = _mm512_max_ps(vsat_cutoff, _mm512_mul_ps(vx1, vprescale));
    const __m512 vz2 = _mm512_max_ps(vsat_cutoff, _mm512_mul_ps(vx2, vprescale));

    __m512 vn0 = _mm512_fmadd_ps(vz0, vlog2e, vmagic_bias);
    __m512 vn1 = _mm512_fmadd_ps(vz1, vlog2e, vmagic_bias);
    __m512 vn2 = _mm512_fmadd_ps(vz2, vlog2e, vmagic_bias);

    __m512 vs0 = _mm512_castsi512_ps(_mm512_slli_epi32(_mm512_castps_si512(vn0), 23));
    vn0 = _mm512_sub_ps(vn0, vmagic_bias);
    __m512 vs1 = _mm512_castsi512_ps(_mm512_slli_epi32(_mm512_castps_si512(vn1), 23));
    vn1 = _mm512_sub_ps(vn1, vmagic_bias);
    __m512 vs2 = _mm512_castsi512_ps(_mm512_slli_epi32(_mm512_castps_si512(vn2), 23));
    vn2 = _mm512_sub_ps(vn2, vmagic_bias);

    __m512 vt0 = _mm512_fmadd_ps(vn0, vminus_ln2, vz0);
    __m512 vt1 = _mm512_fmadd_ps(vn1, vminus_ln2, vz1);
    __m512 vt2 = _mm512_fmadd_ps(vn2, vminus_ln2, vz2);

    __m512 vp0 = _mm512_fmadd_ps(vc6, vt0, vc5);
    __m512 vp1 = _mm512_fmadd_ps(vc6, vt1, vc5);
    __m512 vp2 = _mm512_fmadd_ps(vc6, vt2, vc5);

    vp0 = _mm512_fmadd_ps(vp0, vt0, vc4);
    vp1 = _mm512_fmadd_ps(vp1, vt1, vc4);
    vp2 = _mm512_fmadd_ps(vp2, vt2, vc4);

    vp0 = _mm512_fmadd_ps(vp0, vt0, vc3);
    vp1 = _mm512_fmadd_ps(vp1, vt1, vc3);
    vp2 = _mm512_fmadd_ps(vp2, vt2, vc3);

    vp0 = _mm512_fmadd_ps(vp0, vt0, vc2);
    vp1 = _mm512_fmadd_ps(vp1, vt1, vc2);
    vp2 = _mm512_fmadd_ps(vp2, vt2, vc2);

    vp0 = _mm512_mul_ps(vp0, vt0);
    vt0 = _mm512_mul_ps(vt0, vs0);
    vp1 = _mm512_mul_ps(vp1, vt1);
    vt1 = _mm512_mul_ps(vt1, vs1);
    vp2 = _mm512_mul_ps(vp2, vt2);
    vt2 = _mm512_mul_ps(vt2, vs2);

    vs0 = _mm512_fmsub_ps(vs0, valpha, valpha);
    vs1 = _mm512_fmsub_ps(vs1, valpha, valpha);
    vs2 = _mm512_fmsub_ps(vs2, valpha, valpha);

    vp0 = _mm512_fmadd_ps(vp0, vt0, vt0);
    vp1 = _mm512_fmadd_ps(vp1, vt1, vt1);
    vp2 = _mm512_fmadd_ps(vp2, vt2, vt2);

    const __m512 vzero = _mm512_setzero_ps();
    __m512 vy0 = _mm512_fmadd_ps(vp0, valpha, vs0);
    const __mmask16 vsign0 = _mm512_cmp_ps_mask(vx0, vzero, _CMP_NLT_US);
    __m512 vy1 = _mm512_fmadd_ps(vp1, valpha, vs1);
    const __mmask16 vsign1 = _mm512_cmp_ps_mask(vx1, vzero, _CMP_NLT_US);
    __m512 vy2 = _mm512_fmadd_ps(vp2, valpha, vs2);
    const __mmask16 vsign2 = _mm512_cmp_ps_mask(vx2, vzero, _CMP_NLT_US);

    vy0 = _mm512_mask_mul_ps(vy0, vsign0, vx0, vbeta);
    vy1 = _mm512_mask_mul_ps(vy1, vsign1, vx1, vbeta);
    vy2 = _mm512_mask_mul_ps(vy2, vsign2, vx2, vbeta);

    _mm512_storeu_ps(y, vy0);
    _mm512_storeu_ps(y + 16, vy1);
    _mm512_storeu_ps(y + 32, vy2);
    y += 48;
  }
  for (; n >= 16 * sizeof(float); n -= 16 * sizeof(float)) {
    __m512 vx = _mm512_loadu_ps(x);
    x += 16;

    const __m512 vz = _mm512_max_ps(vsat_cutoff, _mm512_mul_ps(vx, vprescale));
    const __mmask16 vsign = _mm512_cmp_ps_mask(vx, _mm512_setzero_ps(), _CMP_NLT_US);

    __m512 vn = _mm512_fmadd_ps(vz, vlog2e, vmagic_bias);
    __m512 vs = _mm512_castsi512_ps(_mm512_slli_epi32(_mm512_castps_si512(vn), 23));
    vn = _mm512_sub_ps(vn, vmagic_bias);

    __m512 vt = _mm512_fmadd_ps(vn, vminus_ln2, vz);

    __m512 vp = _mm512_fmadd_ps(vc6, vt, vc5);
    vp = _mm512_fmadd_ps(vp, vt, vc4);
    vp = _mm512_fmadd_ps(vp, vt, vc3);
    vp = _mm512_fmadd_ps(vp, vt, vc2);
    vp = _mm512_mul_ps(vp, vt);

    vt = _mm512_mul_ps(vt, vs);
    vs = _mm512_fmsub_ps(vs, valpha, valpha);
    vp = _mm512_fmadd_ps(vp, vt, vt);
    __m512 vy = _mm512_fmadd_ps(vp, valpha, vs);

    vy = _mm512_mask_mul_ps(vy, vsign, vx, vbeta);

    _mm512_storeu_ps(y, vy);
    y += 16;
  }
  if XNN_UNLIKELY(n != 0) {
    assert(n >= 1 * sizeof(float));
    assert(n <= 15 * sizeof(float));
    // Prepare mask for valid 32-bit elements (depends on n).
    n >>= 2 /* log2(sizeof(float)) */;
    const __mmask16 vmask = _cvtu32_mask16((uint16_t) ((uint32_t) (UINT32_C(1) << n) - UINT32_C(1)));

    __m512 vx = _mm512_maskz_loadu_ps(vmask, x);

    const __m512 vz = _mm512_max_ps(vsat_cutoff, _mm512_mul_ps(vx, vprescale));
    const __mmask16 vsign = _mm512_cmp_ps_mask(vx, _mm512_setzero_ps(), _CMP_NLT_US);

    __m512 vn = _mm512_fmadd_ps(vz, vlog2e, vmagic_bias);
    __m512 vs = _mm512_castsi512_ps(_mm512_slli_epi32(_mm512_castps_si512(vn), 23));
    vn = _mm512_sub_ps(vn, vmagic_bias);

    __m512 vt = _mm512_fmadd_ps(vn, vminus_ln2, vz);

    __m512 vp = _mm512_fmadd_ps(vc6, vt, vc5);
    vp = _mm512_fmadd_ps(vp, vt, vc4);
    vp = _mm512_fmadd_ps(vp, vt, vc3);
    vp = _mm512_fmadd_ps(vp, vt, vc2);
    vp = _mm512_mul_ps(vp, vt);

    vt = _mm512_mul_ps(vt, vs);
    vs = _mm512_fmsub_ps(vs, valpha, valpha);
    vp = _mm512_fmadd_ps(vp, vt, vt);
    __m512 vy = _mm512_fmadd_ps(vp, valpha, vs);

    vy = _mm512_mask_mul_ps(vy, vsign, vx, vbeta);

    _mm512_mask_storeu_ps(y, vmask, vy);
  }
}
