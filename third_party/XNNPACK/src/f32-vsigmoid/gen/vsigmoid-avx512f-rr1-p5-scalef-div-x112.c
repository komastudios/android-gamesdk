// Auto-generated file. Do not edit!
//   Template: src/f32-vsigmoid/avx512f-rr1-p5-scalef.c.in
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


void xnn_f32_vsigmoid_ukernel__avx512f_rr1_p5_scalef_div_x112(
    size_t n,
    const float* x,
    float* y,
    const union xnn_f32_sigmoid_params params[restrict XNN_MIN_ELEMENTS(1)])
{
  assert(n % sizeof(float) == 0);

  const __m512i vsign_mask = _mm512_set1_epi32((int) params->avx512_rr1_p5.sign_mask);
  const __m512 vlog2e = _mm512_set1_ps(params->avx512_rr1_p5.log2e);
  const __m512 vminus_ln2 = _mm512_set1_ps(params->avx512_rr1_p5.minus_ln2);
  const __m512 vc5 = _mm512_set1_ps(params->avx512_rr1_p5.c5);
  const __m512 vc4 = _mm512_set1_ps(params->avx512_rr1_p5.c4);
  const __m512 vc3 = _mm512_set1_ps(params->avx512_rr1_p5.c3);
  const __m512 vc2 = _mm512_set1_ps(params->avx512_rr1_p5.c2);
  const __m512 vc1 = _mm512_set1_ps(params->avx512_rr1_p5.c1);
  const __m512 vone = _mm512_set1_ps(params->avx512_rr1_p5.one);

  for (; n >= 112 * sizeof(float); n -= 112 * sizeof(float)) {
    const __m512 vx0 = _mm512_loadu_ps(x);
    const __m512 vx1 = _mm512_loadu_ps(x + 16);
    const __m512 vx2 = _mm512_loadu_ps(x + 32);
    const __m512 vx3 = _mm512_loadu_ps(x + 48);
    const __m512 vx4 = _mm512_loadu_ps(x + 64);
    const __m512 vx5 = _mm512_loadu_ps(x + 80);
    const __m512 vx6 = _mm512_loadu_ps(x + 96);
    x += 112;

    const __m512 vz0 = _mm512_castsi512_ps(_mm512_or_epi32(_mm512_castps_si512(vx0), vsign_mask));
    const __m512 vz1 = _mm512_castsi512_ps(_mm512_or_epi32(_mm512_castps_si512(vx1), vsign_mask));
    const __m512 vz2 = _mm512_castsi512_ps(_mm512_or_epi32(_mm512_castps_si512(vx2), vsign_mask));
    const __m512 vz3 = _mm512_castsi512_ps(_mm512_or_epi32(_mm512_castps_si512(vx3), vsign_mask));
    const __m512 vz4 = _mm512_castsi512_ps(_mm512_or_epi32(_mm512_castps_si512(vx4), vsign_mask));
    const __m512 vz5 = _mm512_castsi512_ps(_mm512_or_epi32(_mm512_castps_si512(vx5), vsign_mask));
    const __m512 vz6 = _mm512_castsi512_ps(_mm512_or_epi32(_mm512_castps_si512(vx6), vsign_mask));

    __m512 vn0 = _mm512_mul_ps(vz0, vlog2e);
    __m512 vn1 = _mm512_mul_ps(vz1, vlog2e);
    __m512 vn2 = _mm512_mul_ps(vz2, vlog2e);
    __m512 vn3 = _mm512_mul_ps(vz3, vlog2e);
    __m512 vn4 = _mm512_mul_ps(vz4, vlog2e);
    __m512 vn5 = _mm512_mul_ps(vz5, vlog2e);
    __m512 vn6 = _mm512_mul_ps(vz6, vlog2e);

    vn0 = _mm512_roundscale_ps(vn0, 0);
    vn1 = _mm512_roundscale_ps(vn1, 0);
    vn2 = _mm512_roundscale_ps(vn2, 0);
    vn3 = _mm512_roundscale_ps(vn3, 0);
    vn4 = _mm512_roundscale_ps(vn4, 0);
    vn5 = _mm512_roundscale_ps(vn5, 0);
    vn6 = _mm512_roundscale_ps(vn6, 0);

    __m512 vt0 = _mm512_fmadd_ps(vn0, vminus_ln2, vz0);
    __m512 vt1 = _mm512_fmadd_ps(vn1, vminus_ln2, vz1);
    __m512 vt2 = _mm512_fmadd_ps(vn2, vminus_ln2, vz2);
    __m512 vt3 = _mm512_fmadd_ps(vn3, vminus_ln2, vz3);
    __m512 vt4 = _mm512_fmadd_ps(vn4, vminus_ln2, vz4);
    __m512 vt5 = _mm512_fmadd_ps(vn5, vminus_ln2, vz5);
    __m512 vt6 = _mm512_fmadd_ps(vn6, vminus_ln2, vz6);

    __m512 vp0 = _mm512_fmadd_ps(vc5, vt0, vc4);
    __m512 vp1 = _mm512_fmadd_ps(vc5, vt1, vc4);
    __m512 vp2 = _mm512_fmadd_ps(vc5, vt2, vc4);
    __m512 vp3 = _mm512_fmadd_ps(vc5, vt3, vc4);
    __m512 vp4 = _mm512_fmadd_ps(vc5, vt4, vc4);
    __m512 vp5 = _mm512_fmadd_ps(vc5, vt5, vc4);
    __m512 vp6 = _mm512_fmadd_ps(vc5, vt6, vc4);

    vp0 = _mm512_fmadd_ps(vp0, vt0, vc3);
    vp1 = _mm512_fmadd_ps(vp1, vt1, vc3);
    vp2 = _mm512_fmadd_ps(vp2, vt2, vc3);
    vp3 = _mm512_fmadd_ps(vp3, vt3, vc3);
    vp4 = _mm512_fmadd_ps(vp4, vt4, vc3);
    vp5 = _mm512_fmadd_ps(vp5, vt5, vc3);
    vp6 = _mm512_fmadd_ps(vp6, vt6, vc3);

    vp0 = _mm512_fmadd_ps(vp0, vt0, vc2);
    vp1 = _mm512_fmadd_ps(vp1, vt1, vc2);
    vp2 = _mm512_fmadd_ps(vp2, vt2, vc2);
    vp3 = _mm512_fmadd_ps(vp3, vt3, vc2);
    vp4 = _mm512_fmadd_ps(vp4, vt4, vc2);
    vp5 = _mm512_fmadd_ps(vp5, vt5, vc2);
    vp6 = _mm512_fmadd_ps(vp6, vt6, vc2);

    vp0 = _mm512_fmadd_ps(vp0, vt0, vc1);
    vp1 = _mm512_fmadd_ps(vp1, vt1, vc1);
    vp2 = _mm512_fmadd_ps(vp2, vt2, vc1);
    vp3 = _mm512_fmadd_ps(vp3, vt3, vc1);
    vp4 = _mm512_fmadd_ps(vp4, vt4, vc1);
    vp5 = _mm512_fmadd_ps(vp5, vt5, vc1);
    vp6 = _mm512_fmadd_ps(vp6, vt6, vc1);

    vp0 = _mm512_fmadd_ps(vp0, vt0, vone);
    vp1 = _mm512_fmadd_ps(vp1, vt1, vone);
    vp2 = _mm512_fmadd_ps(vp2, vt2, vone);
    vp3 = _mm512_fmadd_ps(vp3, vt3, vone);
    vp4 = _mm512_fmadd_ps(vp4, vt4, vone);
    vp5 = _mm512_fmadd_ps(vp5, vt5, vone);
    vp6 = _mm512_fmadd_ps(vp6, vt6, vone);

    const __m512 ve0 = _mm512_scalef_ps(vp0, vn0);
    const __m512 ve1 = _mm512_scalef_ps(vp1, vn1);
    const __m512 ve2 = _mm512_scalef_ps(vp2, vn2);
    const __m512 ve3 = _mm512_scalef_ps(vp3, vn3);
    const __m512 ve4 = _mm512_scalef_ps(vp4, vn4);
    const __m512 ve5 = _mm512_scalef_ps(vp5, vn5);
    const __m512 ve6 = _mm512_scalef_ps(vp6, vn6);

    const __m512 vd0 = _mm512_add_ps(ve0, vone);
    const __m512 vd1 = _mm512_add_ps(ve1, vone);
    const __m512 vd2 = _mm512_add_ps(ve2, vone);
    const __m512 vd3 = _mm512_add_ps(ve3, vone);
    const __m512 vd4 = _mm512_add_ps(ve4, vone);
    const __m512 vd5 = _mm512_add_ps(ve5, vone);
    const __m512 vd6 = _mm512_add_ps(ve6, vone);

    __m512 vf0 = _mm512_div_ps(ve0, vd0);
    __m512 vf1 = _mm512_div_ps(ve1, vd1);
    __m512 vf2 = _mm512_div_ps(ve2, vd2);
    __m512 vf3 = _mm512_div_ps(ve3, vd3);
    __m512 vf4 = _mm512_div_ps(ve4, vd4);
    __m512 vf5 = _mm512_div_ps(ve5, vd5);
    __m512 vf6 = _mm512_div_ps(ve6, vd6);

    vf0 = _mm512_mask_sub_ps(vf0, _mm512_testn_epi32_mask(_mm512_castps_si512(vx0), vsign_mask), vone, vf0);
    vf1 = _mm512_mask_sub_ps(vf1, _mm512_testn_epi32_mask(_mm512_castps_si512(vx1), vsign_mask), vone, vf1);
    vf2 = _mm512_mask_sub_ps(vf2, _mm512_testn_epi32_mask(_mm512_castps_si512(vx2), vsign_mask), vone, vf2);
    vf3 = _mm512_mask_sub_ps(vf3, _mm512_testn_epi32_mask(_mm512_castps_si512(vx3), vsign_mask), vone, vf3);
    vf4 = _mm512_mask_sub_ps(vf4, _mm512_testn_epi32_mask(_mm512_castps_si512(vx4), vsign_mask), vone, vf4);
    vf5 = _mm512_mask_sub_ps(vf5, _mm512_testn_epi32_mask(_mm512_castps_si512(vx5), vsign_mask), vone, vf5);
    vf6 = _mm512_mask_sub_ps(vf6, _mm512_testn_epi32_mask(_mm512_castps_si512(vx6), vsign_mask), vone, vf6);

    _mm512_storeu_ps(y, vf0);
    _mm512_storeu_ps(y + 16, vf1);
    _mm512_storeu_ps(y + 32, vf2);
    _mm512_storeu_ps(y + 48, vf3);
    _mm512_storeu_ps(y + 64, vf4);
    _mm512_storeu_ps(y + 80, vf5);
    _mm512_storeu_ps(y + 96, vf6);
    y += 112;
  }
  for (; n >= 16 * sizeof(float); n -= 16 * sizeof(float)) {
    const __m512 vx = _mm512_loadu_ps(x);
    x += 16;

    const __m512 vz = _mm512_castsi512_ps(_mm512_or_epi32(_mm512_castps_si512(vx), vsign_mask));

    const __m512 vn = _mm512_roundscale_ps(_mm512_mul_ps(vz, vlog2e), 0);

    __m512 vt = _mm512_fmadd_ps(vn, vminus_ln2, vz);

    __m512 vp = _mm512_fmadd_ps(vc5, vt, vc4);
    vp = _mm512_fmadd_ps(vp, vt, vc3);
    vp = _mm512_fmadd_ps(vp, vt, vc2);
    vp = _mm512_fmadd_ps(vp, vt, vc1);
    vp = _mm512_fmadd_ps(vp, vt, vone);

    const __m512 ve = _mm512_scalef_ps(vp, vn);
    const __m512 vd = _mm512_add_ps(ve, vone);

    __m512 vf = _mm512_div_ps(ve, vd);

    vf = _mm512_mask_sub_ps(vf, _mm512_testn_epi32_mask(_mm512_castps_si512(vx), vsign_mask), vone, vf);

    _mm512_storeu_ps(y, vf);
    y += 16;
  }
  if XNN_UNLIKELY(n != 0) {
    assert(n >= 1 * sizeof(float));
    assert(n <= 15 * sizeof(float));

    // Prepare mask for valid 32-bit elements (depends on n).
    n >>= 2 /* log2(sizeof(float)) */;
    const __mmask16 vmask = _cvtu32_mask16((uint16_t) ((uint32_t) (UINT32_C(1) << n) - UINT32_C(1)));

    const __m512 vx = _mm512_maskz_loadu_ps(vmask, x);
    const __m512 vz = _mm512_castsi512_ps(_mm512_or_epi32(_mm512_castps_si512(vx), vsign_mask));

    const __m512 vn = _mm512_roundscale_ps(_mm512_mul_ps(vz, vlog2e), 0);

    __m512 vt = _mm512_fmadd_ps(vn, vminus_ln2, vz);

    __m512 vp = _mm512_fmadd_ps(vc5, vt, vc4);
    vp = _mm512_fmadd_ps(vp, vt, vc3);
    vp = _mm512_fmadd_ps(vp, vt, vc2);
    vp = _mm512_fmadd_ps(vp, vt, vc1);
    vp = _mm512_fmadd_ps(vp, vt, vone);

    const __m512 ve = _mm512_scalef_ps(vp, vn);
    const __m512 vd = _mm512_add_ps(ve, vone);

    __m512 vf = _mm512_div_ps(ve, vd);

    vf = _mm512_mask_sub_ps(vf, _mm512_testn_epi32_mask(_mm512_castps_si512(vx), vsign_mask), vone, vf);

    _mm512_mask_storeu_ps(y, vmask, vf);
  }
}
