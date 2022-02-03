// Auto-generated file. Do not edit!
//   Template: src/f32-raddstoreexpminusmax/scalar-rr2-lut64-p2.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <xnnpack/common.h>
#include <xnnpack/raddstoreexpminusmax.h>

#include <fp16/bitcasts.h>


// Note redefine as uint32[] to avoid redundant bitcasts.
extern XNN_INTERNAL const uint32_t xnn_table_exp2_k_over_64[64];

void xnn_f32_raddstoreexpminusmax_ukernel__scalar_rr2_lut64_p2_x4_acc4(
    size_t elements,
    const float* input,
    const float* max,
    float* output,
    float* sum,
    const union xnn_f32_expminus_params params[restrict XNN_MIN_ELEMENTS(1)])
{
  assert(elements % sizeof(float) == 0);

  const float vi_max = *max;
  const float vlog2e = params->scalar_rr2_lut64_p2.log2e;
  const float vmagic_bias = params->scalar_rr2_lut64_p2.magic_bias;
  const uint32_t vindex_mask = UINT32_C(0x3F);
  const float vminus_ln2_hi = params->scalar_rr2_lut64_p2.minus_ln2_hi;
  const float vminus_ln2_lo = params->scalar_rr2_lut64_p2.minus_ln2_lo;
  const float vc2 = params->scalar_rr2_lut64_p2.c2;
  const float vdenorm_cutoff = params->scalar_rr2_lut64_p2.denorm_cutoff;

  float vacc0 = 0.0f;
  float vacc1 = 0.0f;
  float vacc2 = 0.0f;
  float vacc3 = 0.0f;
  for (; elements >= 4 * sizeof(float); elements -= 4 * sizeof(float)) {
    // Load 4 inputs at a time.
    const float vi0 = input[0];
    const float vi1 = input[1];
    const float vi2 = input[2];
    const float vi3 = input[3];
    input += 4;

    // Subtract maximum input x := i - i_max. This implies x <= 0.
    const float vx0 = vi0 - vi_max;
    const float vx1 = vi1 - vi_max;
    const float vx2 = vi2 - vi_max;
    const float vx3 = vi3 - vi_max;

    // Compute reduced argument n := round(x * 64 / log(2)).
    // We do it by adding a large number (magic bias), which cause rounding of the result to an integer, then subtracing
    // the large number back. The first addition is combined with multiplication by log2e into a single FMA instruction.
    // The trick with adding large number is valid only within certain bounds (|x * 64 / log(2)| <= 2**22, i.e.
    // |x| <= 0x1.62E43p+15 = 45426.09375), but that is acceptable, because inputs outside of [-87.336540, 0.0]
    // result in denormalized or underflown expf(x). We fixup the result for such inputs at the very end of the
    // algorithm.
    float vn0 = vx0 * vlog2e + vmagic_bias;
    float vn1 = vx1 * vlog2e + vmagic_bias;
    float vn2 = vx2 * vlog2e + vmagic_bias;
    float vn3 = vx3 * vlog2e + vmagic_bias;

    // Create a floating-point number s (scale) such that s := 2**(n / 64) for such inputs that expf(x) is normalized,
    // i.e. -87.33642 <= x <= 0.0. As n has 6 fractional bits, we split s == 2**(n / 64) = 2**e * 2**(n / 64 - e), where
    // e := int(n / 64). We create s in two steps:
    // 1. Fetch 2**(n / 64 - e) = 2**(n % 64) from the table using the 6 low bits of n, as integer. Note that the
    //    fetched values are in the [1.0, 2.0) range, i.e. their floating-point exponent is 0.
    // 2. Adjust fecthed value by addition of e to its floating-point exponent. The result is always a normalized
    //    number, because for -87.33642 <= x <= 0.0 (inputs for which expf(x) is normalized) we have -126 <= e <= 0,
    //    and thus the adjusted exponent is not lower than -126.
    //
    // Extract e from bits 6:14 of n and shift it into bits 23:31 (position of floating-point exponent).
    const uint32_t ve0 = (fp32_to_bits(vn0) & UINT32_C(0xFFFFFFC0)) << 17;
    const uint32_t ve1 = (fp32_to_bits(vn1) & UINT32_C(0xFFFFFFC0)) << 17;
    const uint32_t ve2 = (fp32_to_bits(vn2) & UINT32_C(0xFFFFFFC0)) << 17;
    const uint32_t ve3 = (fp32_to_bits(vn3) & UINT32_C(0xFFFFFFC0)) << 17;

    // Use bits 0:6 bits of n, as integer, as an index for table lookup of l := 2**(n % 64).
    const uint32_t vidx0 = fp32_to_bits(vn0) & vindex_mask;
    const uint32_t vidx1 = fp32_to_bits(vn1) & vindex_mask;
    const uint32_t vidx2 = fp32_to_bits(vn2) & vindex_mask;
    const uint32_t vidx3 = fp32_to_bits(vn3) & vindex_mask;
    // Adjust exponent of the value l fetched from the table to get the final s value.
    const float vs0 = fp32_from_bits(xnn_table_exp2_k_over_64[vidx0] + ve0);
    const float vs1 = fp32_from_bits(xnn_table_exp2_k_over_64[vidx1] + ve1);
    const float vs2 = fp32_from_bits(xnn_table_exp2_k_over_64[vidx2] + ve2);
    const float vs3 = fp32_from_bits(xnn_table_exp2_k_over_64[vidx3] + ve3);

    // Subtract the large number back to get final n := round(x * 64 / log(2)) as a floating-point number.
    vn0 -= vmagic_bias;
    vn1 -= vmagic_bias;
    vn2 -= vmagic_bias;
    vn3 -= vmagic_bias;

    // Compute reduced argument t := x - n * log(2) / 64.
    // Use Cody-Waite range reduction method (note the two constants representing log(2) / 64) to improve accuracy.
    float vt0 = vn0 * vminus_ln2_hi + vx0;
    float vt1 = vn1 * vminus_ln2_hi + vx1;
    float vt2 = vn2 * vminus_ln2_hi + vx2;
    float vt3 = vn3 * vminus_ln2_hi + vx3;

    vt0 = vn0 * vminus_ln2_lo + vt0;
    vt1 = vn1 * vminus_ln2_lo + vt1;
    vt2 = vn2 * vminus_ln2_lo + vt2;
    vt3 = vn3 * vminus_ln2_lo + vt3;

    // Compute degree-2 polynomial approximation for exp(t) on [-log(2)/128, log(2)/128].
    float vp0 = vt0 * vc2;
    float vp1 = vt1 * vc2;
    float vp2 = vt2 * vc2;
    float vp3 = vt3 * vc2;

    vp0 = vp0 * vt0 + vt0;
    vp1 = vp1 * vt1 + vt1;
    vp2 = vp2 * vt2 + vt2;
    vp3 = vp3 * vt3 + vt3;

    // Reconstruct the final f value:
    //   f = s * (1 + t * (1 + t * c2))
    //     = s * (1 + t + t * (t * c2))
    //     = s + s * (t + t * (t * c2))
    //     = s + s * p
    float vf0 = vp0 * vs0 + vs0;
    float vf1 = vp1 * vs1 + vs1;
    float vf2 = vp2 * vs2 + vs2;
    float vf3 = vp3 * vs3 + vs3;

    // For inputs below denormal cutoff, replace output with +0.0f.
    // Note that for NaN inputs, comparison result is false, and outputs are left unchanged.
    if XNN_UNPREDICTABLE(vx0 < vdenorm_cutoff) {
      vf0 = 0.0f;
    }
    if XNN_UNPREDICTABLE(vx1 < vdenorm_cutoff) {
      vf1 = 0.0f;
    }
    if XNN_UNPREDICTABLE(vx2 < vdenorm_cutoff) {
      vf2 = 0.0f;
    }
    if XNN_UNPREDICTABLE(vx3 < vdenorm_cutoff) {
      vf3 = 0.0f;
    }

    // Store 4 outputs at a time.
    output[0] = vf0;
    output[1] = vf1;
    output[2] = vf2;
    output[3] = vf3;
    output += 4;

    // Accumulate computed exponents.
    vacc0 += vf0;
    vacc1 += vf1;
    vacc2 += vf2;
    vacc3 += vf3;
  }
  // Add up all accumulators to vacc0
  vacc0 += vacc1;
  vacc2 += vacc3;
  vacc0 += vacc2;

  float vacc = vacc0;
  for (; elements >= sizeof(float); elements -= sizeof(float)) {
    // Load 1 input at a time.
    const float vi = *input++;

    // Subtract maximum input x := i - i_max. This implies x <= 0.
    const float vx = vi - vi_max;

    // Compute reduced argument n := round(x * 64 / log(2)).
    // We do it by adding a large number (magic bias), which cause rounding of the result to an integer, then subtracing
    // the large number back. The first addition is combined with multiplication by log2e into a single FMA instruction.
    // The trick with adding large number is valid only within certain bounds (|x * 64 / log(2)| <= 2**22, i.e.
    // |x| <= 0x1.62E43p+15 = 45426.09375), but that is acceptable, because inputs outside of [-87.336540, 0.0]
    // result in denormalized or underflown expf(x). We fixup the result for such inputs at the very end of the
    // algorithm.
    float vn = vx * vlog2e + vmagic_bias;

    // Create a floating-point number s (scale) such that s := 2**(n / 64) for such inputs that expf(x) is normalized,
    // i.e. -87.33642 <= x <= 0.0. As n has 6 fractional bits, we split s == 2**(n / 64) = 2**e * 2**(n / 64 - e), where
    // e := int(n / 64). We create s in two steps:
    // 1. Fetch 2**(n / 64 - e) = 2**(n % 64) from the table using the 6 low bits of n, as integer. Note that the
    //    fetched values are in the [1.0, 2.0) range, i.e. their floating-point exponent is 0.
    // 2. Adjust fecthed value by addition of e to its floating-point exponent. The result is always a normalized
    //    number, because for -87.33642 <= x <= 0.0 (inputs for which expf(x) is normalized) we have -126 <= e <= 0,
    //    and thus the adjusted exponent is not lower than -126.
    //
    // Extract e from bits 6:14 of n and shift it into bits 23:31 (position of floating-point exponent).
    const uint32_t ve = (fp32_to_bits(vn) & UINT32_C(0xFFFFFFC0)) << 17;

    // Use bits 0:6 bits of n, as integer, as an index for table lookup of l := 2**(n % 64).
    const uint32_t vidx = fp32_to_bits(vn) & vindex_mask;
    // Adjust exponent of the value l fetched from the table to get the final s value.
    const float vs = fp32_from_bits(xnn_table_exp2_k_over_64[vidx] + ve);

    // Subtract the large number back to get final n := round(x * 64 / log(2)) as a floating-point number.
    vn -= vmagic_bias;

    // Compute reduced argument t := x - n * log(2) / 64.
    // Use Cody-Waite range reduction method (note the two constants representing log(2) / 64) to improve accuracy.
    float vt = vn * vminus_ln2_hi + vx;
    vt = vn * vminus_ln2_lo + vt;

    // Compute degree-2 polynomial approximation for exp(t) on [-log(2)/128, log(2)/128].
    float vp = vt * vc2;
    vp = vp * vt + vt;

    // Reconstruct the final f value:
    //   f = s * (1 + t * (1 + t * c2))
    //     = s * (1 + t + t * (t * c2))
    //     = s + s * (t + t * (t * c2))
    //     = s + s * p
    float vf = vp * vs + vs;

    // For inputs below denormal cutoff, replace output with +0.0f.
    // Note that for NaN inputs, comparison result is false, and outputs are left unchanged.
    if XNN_UNPREDICTABLE(vx < vdenorm_cutoff) {
      vf = 0.0f;
    }

    // Store 1 output at a time.
    *output++ = vf;

    // Accumulate computed exponents.
    vacc += vf;
  }
  *sum = vacc;
}
