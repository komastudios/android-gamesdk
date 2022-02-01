// Auto-generated file. Do not edit!
//   Template: src/f32-raddextexp/avx2-p5.c.in
//   Generator: tools/xngen
//
// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>
#include <math.h>

#include <immintrin.h>

#include <xnnpack/raddextexp.h>


static const int32_t mask_table[14] = {-1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0};

void xnn_f32_raddextexp_ukernel__avx2_p5_x80(
    size_t elements,
    const float* x,
    float* sum)
{
  assert(elements % sizeof(float) == 0);

  const __m256 vlog2e = _mm256_set1_ps(0x1.715476p+0f);
  const __m256 vminus_ln2_hi = _mm256_set1_ps(-0x1.62E43p-1f);
  const __m256 vminus_ln2_lo = _mm256_set1_ps(0x1.05C61p-29f);

  // The smallest elements such that 2**elements is considered non-negligible.
  // For smaller elements, 2**elements is replaced with zero.
  const __m256 vmin_exponent = _mm256_set1_ps(-127.0f);
  const __m256 vmagic_bias = _mm256_set1_ps(0x1.8000FEp23f);
  const __m256 vminus_inf = _mm256_set1_ps(-INFINITY);

  const __m256 vc0 = _mm256_set1_ps(1.0f);
  const __m256 vc1 = _mm256_set1_ps(0x1.FFFFF6p-1f);
  const __m256 vc2 = _mm256_set1_ps(0x1.FFFDC6p-2f);
  const __m256 vc3 = _mm256_set1_ps(0x1.555A80p-3f);
  const __m256 vc4 = _mm256_set1_ps(0x1.573A1Ap-5f);
  const __m256 vc5 = _mm256_set1_ps(0x1.0F9F9Cp-7f);

  __m256 vaccv0 = _mm256_setzero_ps();
  __m256 vacce0 = vminus_inf;
  for (; elements >= 80 * sizeof(float); elements -= 80 * sizeof(float)) {
    // Load 80 (10x8) inputs at a time.
    const __m256 vx0 = _mm256_loadu_ps(x);
    const __m256 vx1 = _mm256_loadu_ps(x + 8);
    const __m256 vx2 = _mm256_loadu_ps(x + 16);
    const __m256 vx3 = _mm256_loadu_ps(x + 24);
    const __m256 vx4 = _mm256_loadu_ps(x + 32);
    const __m256 vx5 = _mm256_loadu_ps(x + 40);
    const __m256 vx6 = _mm256_loadu_ps(x + 48);
    const __m256 vx7 = _mm256_loadu_ps(x + 56);
    const __m256 vx8 = _mm256_loadu_ps(x + 64);
    const __m256 vx9 = _mm256_loadu_ps(x + 72);
    x += 80;

    // Compute reduced argument elements := round(x / log(2)).
    const __m256 vn0 = _mm256_round_ps(_mm256_mul_ps(vx0, vlog2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    const __m256 vn1 = _mm256_round_ps(_mm256_mul_ps(vx1, vlog2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    const __m256 vn2 = _mm256_round_ps(_mm256_mul_ps(vx2, vlog2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    const __m256 vn3 = _mm256_round_ps(_mm256_mul_ps(vx3, vlog2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    const __m256 vn4 = _mm256_round_ps(_mm256_mul_ps(vx4, vlog2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    const __m256 vn5 = _mm256_round_ps(_mm256_mul_ps(vx5, vlog2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    const __m256 vn6 = _mm256_round_ps(_mm256_mul_ps(vx6, vlog2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    const __m256 vn7 = _mm256_round_ps(_mm256_mul_ps(vx7, vlog2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    const __m256 vn8 = _mm256_round_ps(_mm256_mul_ps(vx8, vlog2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    const __m256 vn9 = _mm256_round_ps(_mm256_mul_ps(vx9, vlog2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);

    // Compute reduced argument t := x - elements * log(2).
    // Use Cody-Waite range reduction method (note two constants to represent log(2)) to improve accuracy.
    __m256 vt0 = _mm256_fmadd_ps(vn0, vminus_ln2_hi, vx0);
    __m256 vt1 = _mm256_fmadd_ps(vn1, vminus_ln2_hi, vx1);
    __m256 vt2 = _mm256_fmadd_ps(vn2, vminus_ln2_hi, vx2);
    __m256 vt3 = _mm256_fmadd_ps(vn3, vminus_ln2_hi, vx3);
    __m256 vt4 = _mm256_fmadd_ps(vn4, vminus_ln2_hi, vx4);
    __m256 vt5 = _mm256_fmadd_ps(vn5, vminus_ln2_hi, vx5);
    __m256 vt6 = _mm256_fmadd_ps(vn6, vminus_ln2_hi, vx6);
    __m256 vt7 = _mm256_fmadd_ps(vn7, vminus_ln2_hi, vx7);
    __m256 vt8 = _mm256_fmadd_ps(vn8, vminus_ln2_hi, vx8);
    __m256 vt9 = _mm256_fmadd_ps(vn9, vminus_ln2_hi, vx9);

    vt0 = _mm256_fmadd_ps(vn0, vminus_ln2_lo, vt0);
    vt1 = _mm256_fmadd_ps(vn1, vminus_ln2_lo, vt1);
    vt2 = _mm256_fmadd_ps(vn2, vminus_ln2_lo, vt2);
    vt3 = _mm256_fmadd_ps(vn3, vminus_ln2_lo, vt3);
    vt4 = _mm256_fmadd_ps(vn4, vminus_ln2_lo, vt4);
    vt5 = _mm256_fmadd_ps(vn5, vminus_ln2_lo, vt5);
    vt6 = _mm256_fmadd_ps(vn6, vminus_ln2_lo, vt6);
    vt7 = _mm256_fmadd_ps(vn7, vminus_ln2_lo, vt7);
    vt8 = _mm256_fmadd_ps(vn8, vminus_ln2_lo, vt8);
    vt9 = _mm256_fmadd_ps(vn9, vminus_ln2_lo, vt9);

    // Compute degree-5 polynomial approximation for exp(t) on [-log(2)/2, log(2)/2].
    __m256 vp0 = _mm256_fmadd_ps(vc5, vt0, vc4);
    __m256 vp1 = _mm256_fmadd_ps(vc5, vt1, vc4);
    __m256 vp2 = _mm256_fmadd_ps(vc5, vt2, vc4);
    __m256 vp3 = _mm256_fmadd_ps(vc5, vt3, vc4);
    __m256 vp4 = _mm256_fmadd_ps(vc5, vt4, vc4);
    __m256 vp5 = _mm256_fmadd_ps(vc5, vt5, vc4);
    __m256 vp6 = _mm256_fmadd_ps(vc5, vt6, vc4);
    __m256 vp7 = _mm256_fmadd_ps(vc5, vt7, vc4);
    __m256 vp8 = _mm256_fmadd_ps(vc5, vt8, vc4);
    __m256 vp9 = _mm256_fmadd_ps(vc5, vt9, vc4);

    vp0 = _mm256_fmadd_ps(vp0, vt0, vc3);
    vp1 = _mm256_fmadd_ps(vp1, vt1, vc3);
    vp2 = _mm256_fmadd_ps(vp2, vt2, vc3);
    vp3 = _mm256_fmadd_ps(vp3, vt3, vc3);
    vp4 = _mm256_fmadd_ps(vp4, vt4, vc3);
    vp5 = _mm256_fmadd_ps(vp5, vt5, vc3);
    vp6 = _mm256_fmadd_ps(vp6, vt6, vc3);
    vp7 = _mm256_fmadd_ps(vp7, vt7, vc3);
    vp8 = _mm256_fmadd_ps(vp8, vt8, vc3);
    vp9 = _mm256_fmadd_ps(vp9, vt9, vc3);

    vp0 = _mm256_fmadd_ps(vp0, vt0, vc2);
    vp1 = _mm256_fmadd_ps(vp1, vt1, vc2);
    vp2 = _mm256_fmadd_ps(vp2, vt2, vc2);
    vp3 = _mm256_fmadd_ps(vp3, vt3, vc2);
    vp4 = _mm256_fmadd_ps(vp4, vt4, vc2);
    vp5 = _mm256_fmadd_ps(vp5, vt5, vc2);
    vp6 = _mm256_fmadd_ps(vp6, vt6, vc2);
    vp7 = _mm256_fmadd_ps(vp7, vt7, vc2);
    vp8 = _mm256_fmadd_ps(vp8, vt8, vc2);
    vp9 = _mm256_fmadd_ps(vp9, vt9, vc2);

    vp0 = _mm256_fmadd_ps(vp0, vt0, vc1);
    vp1 = _mm256_fmadd_ps(vp1, vt1, vc1);
    vp2 = _mm256_fmadd_ps(vp2, vt2, vc1);
    vp3 = _mm256_fmadd_ps(vp3, vt3, vc1);
    vp4 = _mm256_fmadd_ps(vp4, vt4, vc1);
    vp5 = _mm256_fmadd_ps(vp5, vt5, vc1);
    vp6 = _mm256_fmadd_ps(vp6, vt6, vc1);
    vp7 = _mm256_fmadd_ps(vp7, vt7, vc1);
    vp8 = _mm256_fmadd_ps(vp8, vt8, vc1);
    vp9 = _mm256_fmadd_ps(vp9, vt9, vc1);

    vp0 = _mm256_fmadd_ps(vp0, vt0, vc0);
    vp1 = _mm256_fmadd_ps(vp1, vt1, vc0);
    vp2 = _mm256_fmadd_ps(vp2, vt2, vc0);
    vp3 = _mm256_fmadd_ps(vp3, vt3, vc0);
    vp4 = _mm256_fmadd_ps(vp4, vt4, vc0);
    vp5 = _mm256_fmadd_ps(vp5, vt5, vc0);
    vp6 = _mm256_fmadd_ps(vp6, vt6, vc0);
    vp7 = _mm256_fmadd_ps(vp7, vt7, vc0);
    vp8 = _mm256_fmadd_ps(vp8, vt8, vc0);
    vp9 = _mm256_fmadd_ps(vp9, vt9, vc0);

    // Accumulate "extended" floating-point numbers in ("mantissa", "exponent") representation where
    //  - vnX is "exponent"
    //  - vpX is "mantissa"
    //
    // exp2(ae) * av + exp2(be) * bv =
    //   = exp2(max(ae, be)) * exp2(ae - max(ae, be)) * av + exp2(max(ae, be)) * exp2(be - max(ae, be)) * bv
    //   = exp2(max_e) * (exp2(ae - max_e) * av + exp2(be - max_e) * bv)
    //   = exp2(max_e) * (exp2(delta_ae) * av + exp2(delta_be) * bv)
    //
    // For computational efficiency we may add several "extended" floating-point numbers at a time.
    __m256 vmax_e0 = _mm256_max_ps(vacce0, vn0);
    vmax_e0 = _mm256_max_ps(vmax_e0, vn1);
    vmax_e0 = _mm256_max_ps(vmax_e0, vn2);
    vmax_e0 = _mm256_max_ps(vmax_e0, vn3);
    vmax_e0 = _mm256_max_ps(vmax_e0, vn4);
    vmax_e0 = _mm256_max_ps(vmax_e0, vn5);
    vmax_e0 = _mm256_max_ps(vmax_e0, vn6);
    vmax_e0 = _mm256_max_ps(vmax_e0, vn7);
    vmax_e0 = _mm256_max_ps(vmax_e0, vn8);
    vmax_e0 = _mm256_max_ps(vmax_e0, vn9);

    // For computational efficiency, replace exp2(delta_e) with 0.0f when delta_e <= -127.0.
    // This replacement is done in two steps:
    // 1. Clamp minimum delta_e at -127.0.
    // 2. Map delta_e to scale factor 0.0 when delta_e == -127.0
    const __m256 vdelta_acce0 = _mm256_max_ps(_mm256_sub_ps(vacce0, vmax_e0), vmin_exponent);
    const __m256 vdelta_e0 = _mm256_max_ps(_mm256_sub_ps(vn0, vmax_e0), vmin_exponent);
    const __m256 vdelta_e1 = _mm256_max_ps(_mm256_sub_ps(vn1, vmax_e0), vmin_exponent);
    const __m256 vdelta_e2 = _mm256_max_ps(_mm256_sub_ps(vn2, vmax_e0), vmin_exponent);
    const __m256 vdelta_e3 = _mm256_max_ps(_mm256_sub_ps(vn3, vmax_e0), vmin_exponent);
    const __m256 vdelta_e4 = _mm256_max_ps(_mm256_sub_ps(vn4, vmax_e0), vmin_exponent);
    const __m256 vdelta_e5 = _mm256_max_ps(_mm256_sub_ps(vn5, vmax_e0), vmin_exponent);
    const __m256 vdelta_e6 = _mm256_max_ps(_mm256_sub_ps(vn6, vmax_e0), vmin_exponent);
    const __m256 vdelta_e7 = _mm256_max_ps(_mm256_sub_ps(vn7, vmax_e0), vmin_exponent);
    const __m256 vdelta_e8 = _mm256_max_ps(_mm256_sub_ps(vn8, vmax_e0), vmin_exponent);
    const __m256 vdelta_e9 = _mm256_max_ps(_mm256_sub_ps(vn9, vmax_e0), vmin_exponent);

    // Convert delta-exponents into scale factors:
    // - s = exp2(delta_e) when delta_e > -127.0
    // - s = 0.0 when delta_e <= -127.0
    //
    // Note: delta-exponents can not exceed 0.0, thus scale factors can not exceed 1.0.
    const __m256 vaccs0 = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_acce0, vmagic_bias)), 23));
    const __m256 vs0 = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_e0, vmagic_bias)), 23));
    const __m256 vs1 = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_e1, vmagic_bias)), 23));
    const __m256 vs2 = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_e2, vmagic_bias)), 23));
    const __m256 vs3 = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_e3, vmagic_bias)), 23));
    const __m256 vs4 = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_e4, vmagic_bias)), 23));
    const __m256 vs5 = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_e5, vmagic_bias)), 23));
    const __m256 vs6 = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_e6, vmagic_bias)), 23));
    const __m256 vs7 = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_e7, vmagic_bias)), 23));
    const __m256 vs8 = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_e8, vmagic_bias)), 23));
    const __m256 vs9 = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_e9, vmagic_bias)), 23));

    // Update accumulated "mantissa" and "exponent" values
    vaccv0 = _mm256_mul_ps(vaccv0, vaccs0);
    vaccv0 = _mm256_fmadd_ps(vp0, vs0, vaccv0);
    vaccv0 = _mm256_fmadd_ps(vp1, vs1, vaccv0);
    vaccv0 = _mm256_fmadd_ps(vp2, vs2, vaccv0);
    vaccv0 = _mm256_fmadd_ps(vp3, vs3, vaccv0);
    vaccv0 = _mm256_fmadd_ps(vp4, vs4, vaccv0);
    vaccv0 = _mm256_fmadd_ps(vp5, vs5, vaccv0);
    vaccv0 = _mm256_fmadd_ps(vp6, vs6, vaccv0);
    vaccv0 = _mm256_fmadd_ps(vp7, vs7, vaccv0);
    vaccv0 = _mm256_fmadd_ps(vp8, vs8, vaccv0);
    vaccv0 = _mm256_fmadd_ps(vp9, vs9, vaccv0);

    vacce0 = vmax_e0;
  }

  // Reduce partial sums of "extended" floating-point numbers into a single "extended" SIMD vector of sums.
  __m256 vaccv = vaccv0;
  __m256 vacce = vacce0;

  for (; elements >= 8 * sizeof(float); elements -= 8 * sizeof(float)) {
    // Load 8 inputs at a time.
    const __m256 vx = _mm256_loadu_ps(x);
    x += 8;

    // Compute reduced argument elements := round(x / log(2)).
    const __m256 vn = _mm256_round_ps(_mm256_mul_ps(vx, vlog2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);

    // Compute reduced argument t := x - elements * log(2).
    // Use Cody-Waite range reduction method (note two constants to represent log(2)) to improve accuracy.
    __m256 vt = _mm256_fmadd_ps(vn, vminus_ln2_hi, vx);
    vt = _mm256_fmadd_ps(vn, vminus_ln2_lo, vt);

    // Compute degree-5 polynomial approximation for exp(t) on [-log(2)/2, log(2)/2].
    __m256 vp = _mm256_fmadd_ps(vc5, vt, vc4);
    vp = _mm256_fmadd_ps(vp, vt, vc3);
    vp = _mm256_fmadd_ps(vp, vt, vc2);
    vp = _mm256_fmadd_ps(vp, vt, vc1);
    vp = _mm256_fmadd_ps(vp, vt, vc0);

    // Accumulate "extended" floating-point numbers in ("mantissa", "exponent") representation.
    const __m256 vmax_e = _mm256_max_ps(vacce, vn);

    // For computational efficiency, clamp minimum exp2(delta_e) at -127.0. It will be mapped to 0.0 scale factor later.
    const __m256 vdelta_acce = _mm256_max_ps(_mm256_sub_ps(vacce, vmax_e), vmin_exponent);
    const __m256 vdelta_e = _mm256_max_ps(_mm256_sub_ps(vn, vmax_e), vmin_exponent);

    // Convert exponents into scale factors.
    const __m256 vaccs = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_acce, vmagic_bias)), 23));
    const __m256 vs = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_e, vmagic_bias)), 23));

    // Update accumulated "mantissa" and "exponent" values.
    vaccv = _mm256_mul_ps(vaccv, vaccs);
    vaccv = _mm256_fmadd_ps(vp, vs, vaccv);

    vacce = vmax_e;
  }
  if XNN_UNLIKELY(elements != 0) {
    assert(elements >= 1 * sizeof(float));
    assert(elements <= 7 * sizeof(float));
    const __m256i vmask = _mm256_loadu_si256((const __m256i*) ((uintptr_t) &mask_table[7] - elements));

    // Load up to 7 inputs at a time.
    const __m256 vx = _mm256_maskload_ps(x, vmask);

    // Compute reduced argument elements := round(x / log(2)).
    __m256 vn = _mm256_round_ps(_mm256_mul_ps(vx, vlog2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);

    // Compute reduced argument t := x - elements * log(2).
    // Use Cody-Waite range reduction method (note two constants to represent log(2)) to improve accuracy.
    __m256 vt = _mm256_fmadd_ps(vn, vminus_ln2_hi, vx);
    vt = _mm256_fmadd_ps(vn, vminus_ln2_lo, vt);

    // Correct reduced argument elements for masked out elements.
    vn = _mm256_blendv_ps(vacce, vn, _mm256_castsi256_ps(vmask));

    // Compute degree-5 polynomial approximation for exp(t) on [-log(2)/2, log(2)/2].
    __m256 vp = _mm256_fmadd_ps(vc5, vt, vc4);
    vp = _mm256_fmadd_ps(vp, vt, vc3);
    vp = _mm256_fmadd_ps(vp, vt, vc2);
    vp = _mm256_fmadd_ps(vp, vt, vc1);
    vp = _mm256_fmadd_ps(vp, vt, vc0);
    vp = _mm256_and_ps(vp, _mm256_castsi256_ps(vmask));

    // Accumulate "extended" floating-point numbers in ("mantissa", "exponent") representation.
    const __m256 vmax_e = _mm256_max_ps(vacce, vn);

    // For computational efficiency, clamp minimum exp2(delta_e) at -127.0. It will be mapped to 0.0 scale factor later.
    const __m256 vdelta_e = _mm256_max_ps(_mm256_sub_ps(vn, vmax_e), vmin_exponent);
    const __m256 vdelta_acce = _mm256_max_ps(_mm256_sub_ps(vacce, vmax_e), vmin_exponent);

    // Convert exponents into scale factors.
    const __m256 vs = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_e, vmagic_bias)), 23));
    const __m256 vaccs = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_acce, vmagic_bias)), 23));

    // Update accumulated "mantissa" and "exponent" values.
    vaccv = _mm256_mul_ps(vaccv, vaccs);
    vaccv = _mm256_fmadd_ps(vp, vs, vaccv);

    vacce = vmax_e;
  }

  // Reduce partial sums of "extended" floating-point numbers into a single "extended" floating-point sum.
  __m256 vmax_acce = _mm256_max_ps(vacce, _mm256_permute2f128_ps(vacce, vacce, 1));
  vmax_acce = _mm256_max_ps(vmax_acce, _mm256_shuffle_ps(vmax_acce, vmax_acce, _MM_SHUFFLE(1, 0, 3, 2)));
  vmax_acce = _mm256_max_ps(vmax_acce, _mm256_shuffle_ps(vmax_acce, vmax_acce, _MM_SHUFFLE(2, 3, 0, 1)));
  const __m256 vdelta_acce = _mm256_max_ps(_mm256_sub_ps(vacce, vmax_acce), vmin_exponent);
  const __m256 vaccs = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_add_ps(vdelta_acce, vmagic_bias)), 23));

  vaccv = _mm256_mul_ps(vaccv, vaccs);
  __m128 vaccv_sum = _mm_add_ps(_mm256_castps256_ps128(vaccv), _mm256_extractf128_ps(vaccv, 1));
  vaccv_sum = _mm_add_ps(vaccv_sum, _mm_movehl_ps(vaccv_sum, vaccv_sum));
  vaccv_sum = _mm_add_ss(vaccv_sum, _mm_movehdup_ps(vaccv_sum));

  _mm_store_ss(&sum[0], vaccv_sum);
  _mm_store_ss(&sum[1], _mm256_castps256_ps128(vmax_acce));

  _mm256_zeroupper();
}
