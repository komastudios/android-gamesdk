// Auto-generated file. Do not edit!
//   Template: src/f32-vsigmoid/sse-rr2-lut64-p2-div.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <smmintrin.h>

#include <xnnpack/common.h>
#include <xnnpack/vunary.h>


extern XNN_INTERNAL const float xnn_table_exp2minus_k_over_64[64];

void xnn_f32_vsigmoid_ukernel__sse41_rr2_lut64_p2_div_x4(
    size_t n,
    const float* x,
    float* y,
    const union xnn_f32_sigmoid_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(n % sizeof(float) == 0);

  const __m128 vsign_mask = _mm_load_ps(params->sse2_rr2_lut64_p2.sign_mask);
  const __m128 vmagic_bias = _mm_load_ps(params->sse2_rr2_lut64_p2.magic_bias);
  const __m128 vlog2e = _mm_load_ps(params->sse2_rr2_lut64_p2.log2e);
  const __m128i vindex_mask = _mm_load_si128((const __m128i*) params->sse2_rr2_lut64_p2.index_mask);
  const __m128 vminus_ln2_hi = _mm_load_ps(params->sse2_rr2_lut64_p2.minus_ln2_hi);
  const __m128 vminus_ln2_lo = _mm_load_ps(params->sse2_rr2_lut64_p2.minus_ln2_lo);
  const __m128 vc2 = _mm_load_ps(params->sse2_rr2_lut64_p2.c2);
  const __m128 vone = _mm_load_ps(params->sse2_rr2_lut64_p2.one);
  const __m128 vdenorm_cutoff = _mm_load_ps(params->sse2_rr2_lut64_p2.denorm_cutoff);

  for (; n >= 4 * sizeof(float); n -= 4 * sizeof(float)) {
    const __m128 vx = _mm_loadu_ps(x);
    x += 4;

    const __m128 vz = _mm_or_ps(vx, vsign_mask);

    __m128 vn = _mm_add_ps(_mm_mul_ps(vz, vlog2e), vmagic_bias);
    const __m128i ve = _mm_slli_epi32(_mm_castps_si128(vn), 17);

    const __m128i vidx = _mm_slli_epi32(_mm_and_si128(_mm_castps_si128(vn), vindex_mask), 2);
    #if XNN_ARCH_X86_64
      const uint64_t vidx_lo = (uint64_t) _mm_cvtsi128_si64(vidx);
      const uint64_t vidx_hi = (uint64_t) _mm_extract_epi64(vidx, 1);
      const __m128i vl_ll   = _mm_cvtsi32_si128(*((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) vidx_lo)));
      const __m128i vl_hl = _mm_cvtsi32_si128(*((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) vidx_hi)));
      const __m128i vl_lo = _mm_insert_epi32(vl_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) (vidx_lo >> 32))), 1);
      const __m128i vl_hi = _mm_insert_epi32(vl_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) (vidx_hi >> 32))), 1);
    #else  // !XNN_ARCH_X86_64
      const __m128i vl_ll = _mm_cvtsi32_si128(*((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) _mm_cvtsi128_si32(vidx))));
      const __m128i vl_hl = _mm_cvtsi32_si128(*((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) _mm_extract_epi16(vidx, 4))));
      const __m128i vl_lo = _mm_insert_epi32(vl_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) _mm_extract_epi16(vidx, 2))), 1);
      const __m128i vl_hi = _mm_insert_epi32(vl_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) _mm_extract_epi16(vidx, 6))), 1);
    #endif  // XNN_ARCH_X86_64
    const __m128i vl = _mm_unpacklo_epi64(vl_lo, vl_hi);

    const __m128 vs = _mm_castsi128_ps(_mm_add_epi32(vl, ve));
    vn = _mm_sub_ps(vn, vmagic_bias);

    __m128 vt = _mm_add_ps(vz, _mm_mul_ps(vn, vminus_ln2_hi));
    vt = _mm_add_ps(vt, _mm_mul_ps(vn, vminus_ln2_lo));

    __m128 vp = _mm_mul_ps(vt, vc2);
    vp = _mm_add_ps(vt, _mm_mul_ps(vp, vt));

    const __m128 vy = _mm_add_ps(vs, _mm_mul_ps(vs, vp));

    __m128 vf = _mm_div_ps(vy, _mm_add_ps(vy, vone));
    vf = _mm_andnot_ps(_mm_cmplt_ps(vz, vdenorm_cutoff), vf);
    vf = _mm_blendv_ps(_mm_sub_ps(vone, vf), vf, vx);

    _mm_storeu_ps(y, vf);
    y += 4;
  }
  if XNN_UNLIKELY(n != 0) {
    const __m128 vx = _mm_loadu_ps(x);

    const __m128 vz = _mm_or_ps(vx, vsign_mask);

    __m128 vn = _mm_add_ps(_mm_mul_ps(vz, vlog2e), vmagic_bias);
    const __m128i ve = _mm_slli_epi32(_mm_castps_si128(vn), 17);

    const __m128i vidx = _mm_slli_epi32(_mm_and_si128(_mm_castps_si128(vn), vindex_mask), 2);
    #if XNN_ARCH_X86_64
      const uint64_t vidx_lo = (uint64_t) _mm_cvtsi128_si64(vidx);
      const uint64_t vidx_hi = (uint64_t) _mm_extract_epi64(vidx, 1);
      const __m128i vl_ll   = _mm_cvtsi32_si128(*((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) vidx_lo)));
      const __m128i vl_hl = _mm_cvtsi32_si128(*((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) vidx_hi)));
      const __m128i vl_lo = _mm_insert_epi32(vl_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) (vidx_lo >> 32))), 1);
      const __m128i vl_hi = _mm_insert_epi32(vl_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) (vidx_hi >> 32))), 1);
    #else  // !XNN_ARCH_X86_64
      const __m128i vl_ll = _mm_cvtsi32_si128(*((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) _mm_cvtsi128_si32(vidx))));
      const __m128i vl_hl = _mm_cvtsi32_si128(*((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) _mm_extract_epi16(vidx, 4))));
      const __m128i vl_lo = _mm_insert_epi32(vl_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) _mm_extract_epi16(vidx, 2))), 1);
      const __m128i vl_hi = _mm_insert_epi32(vl_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_64 + (uint32_t) _mm_extract_epi16(vidx, 6))), 1);
    #endif  // XNN_ARCH_X86_64
    const __m128i vl = _mm_unpacklo_epi64(vl_lo, vl_hi);

    const __m128 vs = _mm_castsi128_ps(_mm_add_epi32(vl, ve));
    vn = _mm_sub_ps(vn, vmagic_bias);

    __m128 vt = _mm_add_ps(vz, _mm_mul_ps(vn, vminus_ln2_hi));
    vt = _mm_add_ps(vt, _mm_mul_ps(vn, vminus_ln2_lo));

    __m128 vp = _mm_mul_ps(vt, vc2);
    vp = _mm_add_ps(vt, _mm_mul_ps(vp, vt));

    const __m128 vy = _mm_add_ps(vs, _mm_mul_ps(vs, vp));

    __m128 vf = _mm_div_ps(vy, _mm_add_ps(vy, vone));
    vf = _mm_andnot_ps(_mm_cmplt_ps(vz, vdenorm_cutoff), vf);
    vf = _mm_blendv_ps(_mm_sub_ps(vone, vf), vf, vx);

    if (n & (2 * sizeof(float))) {
      _mm_storel_pi((__m64*) y, vf);
      vf = _mm_movehl_ps(vf, vf);
      y += 2;
    }
    if (n & (1 * sizeof(float))) {
      _mm_store_ss(y, vf);
    }
  }
}
