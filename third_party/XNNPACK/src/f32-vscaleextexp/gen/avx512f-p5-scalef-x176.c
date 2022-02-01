// Auto-generated file. Do not edit!
//   Template: src/f32-vscaleextexp/avx512f-p5-scalef.c.in
//   Generator: tools/xngen
//
// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <immintrin.h>

#include <xnnpack/common.h>
#include <xnnpack/intrinsics-polyfill.h>
#include <xnnpack/vscaleextexp.h>


void xnn_f32_vscaleextexp_ukernel__avx512f_p5_scalef_x176(
    size_t elements,
    const float* x,
    float* y,
    float scale_value,
    float scale_exp)
{
  assert(elements % sizeof(float) == 0);

  const __m512 vlog2e = _mm512_set1_ps(0x1.715476p+0f);
  const __m512 vminus_ln2_hi = _mm512_set1_ps(-0x1.62E43p-1f);
  const __m512 vminus_ln2_lo = _mm512_set1_ps(0x1.05C61p-29f);

  const __m512 vc0 = _mm512_set1_ps(1.0f);
  const __m512 vc1 = _mm512_set1_ps(0x1.FFFFF6p-1f);
  const __m512 vc2 = _mm512_set1_ps(0x1.FFFDC6p-2f);
  const __m512 vc3 = _mm512_set1_ps(0x1.555A80p-3f);
  const __m512 vc4 = _mm512_set1_ps(0x1.573A1Ap-5f);
  const __m512 vc5 = _mm512_set1_ps(0x1.0F9F9Cp-7f);

  const __m512 vscalev = _mm512_set1_ps(scale_value);
  const __m512 vscalee = _mm512_set1_ps(scale_exp);

  for (; elements >= 176 * sizeof(float); elements -= 176 * sizeof(float)) {
    // Load 176 (11x16) inputs at a time.
    const __m512 vx0 = _mm512_loadu_ps(x);
    const __m512 vx1 = _mm512_loadu_ps(x + 16);
    const __m512 vx2 = _mm512_loadu_ps(x + 32);
    const __m512 vx3 = _mm512_loadu_ps(x + 48);
    const __m512 vx4 = _mm512_loadu_ps(x + 64);
    const __m512 vx5 = _mm512_loadu_ps(x + 80);
    const __m512 vx6 = _mm512_loadu_ps(x + 96);
    const __m512 vx7 = _mm512_loadu_ps(x + 112);
    const __m512 vx8 = _mm512_loadu_ps(x + 128);
    const __m512 vx9 = _mm512_loadu_ps(x + 144);
    const __m512 vx10 = _mm512_loadu_ps(x + 160);
    x += 176;

    // Compute reduced argument elements := round(x / log(2)).
    const __m512 vn0 = _mm512_roundscale_ps(_mm512_mul_ps(vx0, vlog2e), 0);
    const __m512 vn1 = _mm512_roundscale_ps(_mm512_mul_ps(vx1, vlog2e), 0);
    const __m512 vn2 = _mm512_roundscale_ps(_mm512_mul_ps(vx2, vlog2e), 0);
    const __m512 vn3 = _mm512_roundscale_ps(_mm512_mul_ps(vx3, vlog2e), 0);
    const __m512 vn4 = _mm512_roundscale_ps(_mm512_mul_ps(vx4, vlog2e), 0);
    const __m512 vn5 = _mm512_roundscale_ps(_mm512_mul_ps(vx5, vlog2e), 0);
    const __m512 vn6 = _mm512_roundscale_ps(_mm512_mul_ps(vx6, vlog2e), 0);
    const __m512 vn7 = _mm512_roundscale_ps(_mm512_mul_ps(vx7, vlog2e), 0);
    const __m512 vn8 = _mm512_roundscale_ps(_mm512_mul_ps(vx8, vlog2e), 0);
    const __m512 vn9 = _mm512_roundscale_ps(_mm512_mul_ps(vx9, vlog2e), 0);
    const __m512 vn10 = _mm512_roundscale_ps(_mm512_mul_ps(vx10, vlog2e), 0);

    // Compute reduced argument t := x - elements * log(2).
    // Use Cody-Waite range reduction method (note two constants to represent log(2)) to improve accuracy.
    __m512 vt0 = _mm512_fmadd_ps(vn0, vminus_ln2_hi, vx0);
    __m512 vt1 = _mm512_fmadd_ps(vn1, vminus_ln2_hi, vx1);
    __m512 vt2 = _mm512_fmadd_ps(vn2, vminus_ln2_hi, vx2);
    __m512 vt3 = _mm512_fmadd_ps(vn3, vminus_ln2_hi, vx3);
    __m512 vt4 = _mm512_fmadd_ps(vn4, vminus_ln2_hi, vx4);
    __m512 vt5 = _mm512_fmadd_ps(vn5, vminus_ln2_hi, vx5);
    __m512 vt6 = _mm512_fmadd_ps(vn6, vminus_ln2_hi, vx6);
    __m512 vt7 = _mm512_fmadd_ps(vn7, vminus_ln2_hi, vx7);
    __m512 vt8 = _mm512_fmadd_ps(vn8, vminus_ln2_hi, vx8);
    __m512 vt9 = _mm512_fmadd_ps(vn9, vminus_ln2_hi, vx9);
    __m512 vt10 = _mm512_fmadd_ps(vn10, vminus_ln2_hi, vx10);

    vt0 = _mm512_fmadd_ps(vn0, vminus_ln2_lo, vt0);
    vt1 = _mm512_fmadd_ps(vn1, vminus_ln2_lo, vt1);
    vt2 = _mm512_fmadd_ps(vn2, vminus_ln2_lo, vt2);
    vt3 = _mm512_fmadd_ps(vn3, vminus_ln2_lo, vt3);
    vt4 = _mm512_fmadd_ps(vn4, vminus_ln2_lo, vt4);
    vt5 = _mm512_fmadd_ps(vn5, vminus_ln2_lo, vt5);
    vt6 = _mm512_fmadd_ps(vn6, vminus_ln2_lo, vt6);
    vt7 = _mm512_fmadd_ps(vn7, vminus_ln2_lo, vt7);
    vt8 = _mm512_fmadd_ps(vn8, vminus_ln2_lo, vt8);
    vt9 = _mm512_fmadd_ps(vn9, vminus_ln2_lo, vt9);
    vt10 = _mm512_fmadd_ps(vn10, vminus_ln2_lo, vt10);

    // Compute degree-5 polynomial approximation for exp(t) on [-log(2)/2, log(2)/2].
    __m512 vp0 = _mm512_fmadd_ps(vc5, vt0, vc4);
    __m512 vp1 = _mm512_fmadd_ps(vc5, vt1, vc4);
    __m512 vp2 = _mm512_fmadd_ps(vc5, vt2, vc4);
    __m512 vp3 = _mm512_fmadd_ps(vc5, vt3, vc4);
    __m512 vp4 = _mm512_fmadd_ps(vc5, vt4, vc4);
    __m512 vp5 = _mm512_fmadd_ps(vc5, vt5, vc4);
    __m512 vp6 = _mm512_fmadd_ps(vc5, vt6, vc4);
    __m512 vp7 = _mm512_fmadd_ps(vc5, vt7, vc4);
    __m512 vp8 = _mm512_fmadd_ps(vc5, vt8, vc4);
    __m512 vp9 = _mm512_fmadd_ps(vc5, vt9, vc4);
    __m512 vp10 = _mm512_fmadd_ps(vc5, vt10, vc4);

    vp0 = _mm512_fmadd_ps(vp0, vt0, vc3);
    vp1 = _mm512_fmadd_ps(vp1, vt1, vc3);
    vp2 = _mm512_fmadd_ps(vp2, vt2, vc3);
    vp3 = _mm512_fmadd_ps(vp3, vt3, vc3);
    vp4 = _mm512_fmadd_ps(vp4, vt4, vc3);
    vp5 = _mm512_fmadd_ps(vp5, vt5, vc3);
    vp6 = _mm512_fmadd_ps(vp6, vt6, vc3);
    vp7 = _mm512_fmadd_ps(vp7, vt7, vc3);
    vp8 = _mm512_fmadd_ps(vp8, vt8, vc3);
    vp9 = _mm512_fmadd_ps(vp9, vt9, vc3);
    vp10 = _mm512_fmadd_ps(vp10, vt10, vc3);

    vp0 = _mm512_fmadd_ps(vp0, vt0, vc2);
    vp1 = _mm512_fmadd_ps(vp1, vt1, vc2);
    vp2 = _mm512_fmadd_ps(vp2, vt2, vc2);
    vp3 = _mm512_fmadd_ps(vp3, vt3, vc2);
    vp4 = _mm512_fmadd_ps(vp4, vt4, vc2);
    vp5 = _mm512_fmadd_ps(vp5, vt5, vc2);
    vp6 = _mm512_fmadd_ps(vp6, vt6, vc2);
    vp7 = _mm512_fmadd_ps(vp7, vt7, vc2);
    vp8 = _mm512_fmadd_ps(vp8, vt8, vc2);
    vp9 = _mm512_fmadd_ps(vp9, vt9, vc2);
    vp10 = _mm512_fmadd_ps(vp10, vt10, vc2);

    vp0 = _mm512_fmadd_ps(vp0, vt0, vc1);
    vp1 = _mm512_fmadd_ps(vp1, vt1, vc1);
    vp2 = _mm512_fmadd_ps(vp2, vt2, vc1);
    vp3 = _mm512_fmadd_ps(vp3, vt3, vc1);
    vp4 = _mm512_fmadd_ps(vp4, vt4, vc1);
    vp5 = _mm512_fmadd_ps(vp5, vt5, vc1);
    vp6 = _mm512_fmadd_ps(vp6, vt6, vc1);
    vp7 = _mm512_fmadd_ps(vp7, vt7, vc1);
    vp8 = _mm512_fmadd_ps(vp8, vt8, vc1);
    vp9 = _mm512_fmadd_ps(vp9, vt9, vc1);
    vp10 = _mm512_fmadd_ps(vp10, vt10, vc1);

    vp0 = _mm512_fmadd_ps(vp0, vt0, vc0);
    vp1 = _mm512_fmadd_ps(vp1, vt1, vc0);
    vp2 = _mm512_fmadd_ps(vp2, vt2, vc0);
    vp3 = _mm512_fmadd_ps(vp3, vt3, vc0);
    vp4 = _mm512_fmadd_ps(vp4, vt4, vc0);
    vp5 = _mm512_fmadd_ps(vp5, vt5, vc0);
    vp6 = _mm512_fmadd_ps(vp6, vt6, vc0);
    vp7 = _mm512_fmadd_ps(vp7, vt7, vc0);
    vp8 = _mm512_fmadd_ps(vp8, vt8, vc0);
    vp9 = _mm512_fmadd_ps(vp9, vt9, vc0);
    vp10 = _mm512_fmadd_ps(vp10, vt10, vc0);

    // Multiply "extended" floating-point numbers in ("mantissa", "exponent") representation where
    //  - vnX is "exponent"
    //  - vpX is "mantissa"
    //
    // exp2(ae) * av * exp2(be) * bv =
    //   = exp2(ae + be) * (av * bv)
    __m512 vf0 = _mm512_mul_ps(vp0, vscalev);
    __m512 vf1 = _mm512_mul_ps(vp1, vscalev);
    __m512 vf2 = _mm512_mul_ps(vp2, vscalev);
    __m512 vf3 = _mm512_mul_ps(vp3, vscalev);
    __m512 vf4 = _mm512_mul_ps(vp4, vscalev);
    __m512 vf5 = _mm512_mul_ps(vp5, vscalev);
    __m512 vf6 = _mm512_mul_ps(vp6, vscalev);
    __m512 vf7 = _mm512_mul_ps(vp7, vscalev);
    __m512 vf8 = _mm512_mul_ps(vp8, vscalev);
    __m512 vf9 = _mm512_mul_ps(vp9, vscalev);
    __m512 vf10 = _mm512_mul_ps(vp10, vscalev);

    const __m512 ve0 = _mm512_add_ps(vn0, vscalee);
    const __m512 ve1 = _mm512_add_ps(vn1, vscalee);
    const __m512 ve2 = _mm512_add_ps(vn2, vscalee);
    const __m512 ve3 = _mm512_add_ps(vn3, vscalee);
    const __m512 ve4 = _mm512_add_ps(vn4, vscalee);
    const __m512 ve5 = _mm512_add_ps(vn5, vscalee);
    const __m512 ve6 = _mm512_add_ps(vn6, vscalee);
    const __m512 ve7 = _mm512_add_ps(vn7, vscalee);
    const __m512 ve8 = _mm512_add_ps(vn8, vscalee);
    const __m512 ve9 = _mm512_add_ps(vn9, vscalee);
    const __m512 ve10 = _mm512_add_ps(vn10, vscalee);

    // Multiply "mantissa" by the exp2("exponent").
    vf0 = _mm512_scalef_ps(vf0, ve0);
    vf1 = _mm512_scalef_ps(vf1, ve1);
    vf2 = _mm512_scalef_ps(vf2, ve2);
    vf3 = _mm512_scalef_ps(vf3, ve3);
    vf4 = _mm512_scalef_ps(vf4, ve4);
    vf5 = _mm512_scalef_ps(vf5, ve5);
    vf6 = _mm512_scalef_ps(vf6, ve6);
    vf7 = _mm512_scalef_ps(vf7, ve7);
    vf8 = _mm512_scalef_ps(vf8, ve8);
    vf9 = _mm512_scalef_ps(vf9, ve9);
    vf10 = _mm512_scalef_ps(vf10, ve10);

    // Store 128 (8x16) results at a time.
    _mm512_storeu_ps(y, vf0);
    _mm512_storeu_ps(y + 0, vf0);
    _mm512_storeu_ps(y + 16, vf1);
    _mm512_storeu_ps(y + 32, vf2);
    _mm512_storeu_ps(y + 48, vf3);
    _mm512_storeu_ps(y + 64, vf4);
    _mm512_storeu_ps(y + 80, vf5);
    _mm512_storeu_ps(y + 96, vf6);
    _mm512_storeu_ps(y + 112, vf7);
    _mm512_storeu_ps(y + 128, vf8);
    _mm512_storeu_ps(y + 144, vf9);
    _mm512_storeu_ps(y + 160, vf10);
    y += 176;
  }

  for (; elements >= 16 * sizeof(float); elements -= 16 * sizeof(float)) {
    // Load 16 inputs at a time.
    const __m512 vx = _mm512_loadu_ps(x);
    x += 16;

    // Compute reduced argument elements := round(x / log(2)).
    const __m512 vn = _mm512_roundscale_ps(_mm512_mul_ps(vx, vlog2e), 0);

    // Compute reduced argument t := x - elements * log(2).
    // Use Cody-Waite range reduction method (note two constants to represent log(2)) to improve accuracy.
    __m512 vt = _mm512_fmadd_ps(vn, vminus_ln2_hi, vx);
    vt = _mm512_fmadd_ps(vn, vminus_ln2_lo, vt);

    // Compute degree-5 polynomial approximation for exp(t) on [-log(2)/2, log(2)/2].
    __m512 vp = _mm512_fmadd_ps(vc5, vt, vc4);
    vp = _mm512_fmadd_ps(vp, vt, vc3);
    vp = _mm512_fmadd_ps(vp, vt, vc2);
    vp = _mm512_fmadd_ps(vp, vt, vc1);
    vp = _mm512_fmadd_ps(vp, vt, vc0);

    // Multiply "extended" floating-point numbers in ("mantissa", "exponent") representation.
    __m512 vf = _mm512_mul_ps(vp, vscalev);
    const __m512 ve = _mm512_add_ps(vn, vscalee);

    // Multiply "mantissa" by the exp2("exponent").
    vf = _mm512_scalef_ps(vf, ve);

    // Store 16 results at a time.
    _mm512_storeu_ps(y, vf);
    y += 16;
  }
  if XNN_UNLIKELY(elements != 0) {
    // Prepare mask for valid 32-bit elements (depends on elements).
    elements >>= 2 /* log2(sizeof(float)) */;
    const __mmask16 vmask = _cvtu32_mask16((uint16_t) ((uint32_t) (UINT32_C(1) << elements) - UINT32_C(1)));

    // Load up to 15 inputs at a time.
    const __m512 vx = _mm512_maskz_loadu_ps(vmask, x);

    // Compute reduced argument elements := round(x / log(2)).
    const __m512 vn = _mm512_roundscale_ps(_mm512_mul_ps(vx, vlog2e), 0);

    // Compute reduced argument t := x - elements * log(2).
    // Use Cody-Waite range reduction method (note two constants to represent log(2)) to improve accuracy.
    __m512 vt = _mm512_fmadd_ps(vn, vminus_ln2_hi, vx);
    vt = _mm512_fmadd_ps(vn, vminus_ln2_lo, vt);

    // Compute degree-5 polynomial approximation for exp(t) on [-log(2)/2, log(2)/2].
    __m512 vp = _mm512_fmadd_ps(vc5, vt, vc4);
    vp = _mm512_fmadd_ps(vp, vt, vc3);
    vp = _mm512_fmadd_ps(vp, vt, vc2);
    vp = _mm512_fmadd_ps(vp, vt, vc1);
    vp = _mm512_fmadd_ps(vp, vt, vc0);

    // Multiply "extended" floating-point numbers in ("mantissa", "exponent") representation.
    __m512 vf = _mm512_mul_ps(vp, vscalev);
    const __m512 ve = _mm512_add_ps(vn, vscalee);

    // Multiply "mantissa" by the exp2("exponent").
    vf = _mm512_scalef_ps(vf, ve);

    // Store up to 15 results at a time.
    _mm512_mask_storeu_ps(y, vmask, vf);
  }
  _mm256_zeroupper();
}
