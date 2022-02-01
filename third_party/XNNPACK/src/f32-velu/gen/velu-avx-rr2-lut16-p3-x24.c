// Auto-generated file. Do not edit!
//   Template: src/f32-velu/avx-rr2-lut16-p3.c.in
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


extern XNN_INTERNAL const int xnn_table_exp2minus_k_over_16[16];

void xnn_f32_velu_ukernel__avx_rr2_lut16_p3_x24(
    size_t n,
    const float* x,
    float* y,
    const union xnn_f32_elu_params params[restrict XNN_MIN_ELEMENTS(1)])
{
  assert(n % sizeof(float) == 0);

  const __m256 vprescale = _mm256_load_ps(params->avx_rr2_lut16_p3.prescale);
  const __m256 valpha = _mm256_load_ps(params->avx_rr2_lut16_p3.alpha);
  const __m256 vbeta = _mm256_load_ps(params->avx_rr2_lut16_p3.beta);
  const __m256 vsat_cutoff = _mm256_load_ps(params->avx_rr2_lut16_p3.sat_cutoff);
  const __m256 vmagic_bias = _mm256_load_ps(params->avx_rr2_lut16_p3.magic_bias);
  const __m256 vlog2e = _mm256_load_ps(params->avx_rr2_lut16_p3.log2e);
  const __m256 vindex_mask = _mm256_load_ps((const float*) params->avx_rr2_lut16_p3.index_mask);
  const __m256 vminus_ln2_hi = _mm256_load_ps(params->avx_rr2_lut16_p3.minus_ln2_hi);
  const __m256 vminus_ln2_lo = _mm256_load_ps(params->avx_rr2_lut16_p3.minus_ln2_lo);
  const __m256 vc3 = _mm256_load_ps(params->avx_rr2_lut16_p3.c3);
  const __m256 vc2 = _mm256_load_ps(params->avx_rr2_lut16_p3.c2);
  const __m256 vone = _mm256_load_ps(params->avx_rr2_lut16_p3.one);

  for (; n >= 24 * sizeof(float); n -= 24 * sizeof(float)) {
    __m256 vx0 = _mm256_loadu_ps(x);
    __m256 vx1 = _mm256_loadu_ps(x + 8);
    __m256 vx2 = _mm256_loadu_ps(x + 16);
    x += 24;

    const __m256 vz0 = _mm256_max_ps(vsat_cutoff, _mm256_mul_ps(vx0, vprescale));
    const __m256 vz1 = _mm256_max_ps(vsat_cutoff, _mm256_mul_ps(vx1, vprescale));
    const __m256 vz2 = _mm256_max_ps(vsat_cutoff, _mm256_mul_ps(vx2, vprescale));

    __m256 vn0 = _mm256_add_ps(_mm256_mul_ps(vz0, vlog2e), vmagic_bias);
    __m256 vn1 = _mm256_add_ps(_mm256_mul_ps(vz1, vlog2e), vmagic_bias);
    __m256 vn2 = _mm256_add_ps(_mm256_mul_ps(vz2, vlog2e), vmagic_bias);

    const __m256 vidx0 = _mm256_and_ps(vn0, vindex_mask);

    const __m128i vidx0_lo = _mm_slli_epi32(_mm_castps_si128(_mm256_castps256_ps128(vidx0)), 2);
    const __m128i vidx0_hi = _mm_slli_epi32(_mm_castps_si128(_mm256_extractf128_ps(vidx0, 1)), 2);
    #if XNN_ARCH_X86_64
      const uint64_t vidx0_ll = (uint64_t) _mm_cvtsi128_si64(vidx0_lo);
      const uint64_t vidx0_lh = (uint64_t) _mm_extract_epi64(vidx0_lo, 1);
      const uint64_t vidx0_hl = (uint64_t) _mm_cvtsi128_si64(vidx0_hi);
      const uint64_t vidx0_hh = (uint64_t) _mm_extract_epi64(vidx0_hi, 1);
      __m128i vl0_ll = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx0_ll));
      __m128i vl0_lh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx0_lh));
      __m128i vl0_hl = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx0_hl));
      __m128i vl0_hh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx0_hh));
      vl0_ll = _mm_insert_epi32(vl0_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx0_ll >> 32))), 1);
      vl0_lh = _mm_insert_epi32(vl0_lh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx0_lh >> 32))), 1);
      vl0_hl = _mm_insert_epi32(vl0_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx0_hl >> 32))), 1);
      vl0_hh = _mm_insert_epi32(vl0_hh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx0_hh >> 32))), 1);
    #else
      __m128i vl0_ll = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_cvtsi128_si32(vidx0_lo)));
      __m128i vl0_lh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx0_lo, 2)));
      __m128i vl0_hl = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_cvtsi128_si32(vidx0_hi)));
      __m128i vl0_hh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx0_hi, 2)));
      vl0_ll = _mm_insert_epi32(vl0_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx0_lo, 1))), 1);
      vl0_lh = _mm_insert_epi32(vl0_lh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx0_lo, 3))), 1);
      vl0_hl = _mm_insert_epi32(vl0_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx0_hi, 1))), 1);
      vl0_hh = _mm_insert_epi32(vl0_hh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx0_hi, 3))), 1);
    #endif
    const __m128i vl0_lo = _mm_unpacklo_epi64(vl0_ll, vl0_lh);
    const __m128i vl0_hi = _mm_unpacklo_epi64(vl0_hl, vl0_hh);
    const __m256 vidx1 = _mm256_and_ps(vn1, vindex_mask);

    const __m128i vidx1_lo = _mm_slli_epi32(_mm_castps_si128(_mm256_castps256_ps128(vidx1)), 2);
    const __m128i vidx1_hi = _mm_slli_epi32(_mm_castps_si128(_mm256_extractf128_ps(vidx1, 1)), 2);
    #if XNN_ARCH_X86_64
      const uint64_t vidx1_ll = (uint64_t) _mm_cvtsi128_si64(vidx1_lo);
      const uint64_t vidx1_lh = (uint64_t) _mm_extract_epi64(vidx1_lo, 1);
      const uint64_t vidx1_hl = (uint64_t) _mm_cvtsi128_si64(vidx1_hi);
      const uint64_t vidx1_hh = (uint64_t) _mm_extract_epi64(vidx1_hi, 1);
      __m128i vl1_ll = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx1_ll));
      __m128i vl1_lh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx1_lh));
      __m128i vl1_hl = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx1_hl));
      __m128i vl1_hh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx1_hh));
      vl1_ll = _mm_insert_epi32(vl1_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx1_ll >> 32))), 1);
      vl1_lh = _mm_insert_epi32(vl1_lh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx1_lh >> 32))), 1);
      vl1_hl = _mm_insert_epi32(vl1_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx1_hl >> 32))), 1);
      vl1_hh = _mm_insert_epi32(vl1_hh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx1_hh >> 32))), 1);
    #else
      __m128i vl1_ll = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_cvtsi128_si32(vidx1_lo)));
      __m128i vl1_lh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx1_lo, 2)));
      __m128i vl1_hl = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_cvtsi128_si32(vidx1_hi)));
      __m128i vl1_hh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx1_hi, 2)));
      vl1_ll = _mm_insert_epi32(vl1_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx1_lo, 1))), 1);
      vl1_lh = _mm_insert_epi32(vl1_lh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx1_lo, 3))), 1);
      vl1_hl = _mm_insert_epi32(vl1_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx1_hi, 1))), 1);
      vl1_hh = _mm_insert_epi32(vl1_hh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx1_hi, 3))), 1);
    #endif
    const __m128i vl1_lo = _mm_unpacklo_epi64(vl1_ll, vl1_lh);
    const __m128i vl1_hi = _mm_unpacklo_epi64(vl1_hl, vl1_hh);
    const __m256 vidx2 = _mm256_and_ps(vn2, vindex_mask);

    const __m128i vidx2_lo = _mm_slli_epi32(_mm_castps_si128(_mm256_castps256_ps128(vidx2)), 2);
    const __m128i vidx2_hi = _mm_slli_epi32(_mm_castps_si128(_mm256_extractf128_ps(vidx2, 1)), 2);
    #if XNN_ARCH_X86_64
      const uint64_t vidx2_ll = (uint64_t) _mm_cvtsi128_si64(vidx2_lo);
      const uint64_t vidx2_lh = (uint64_t) _mm_extract_epi64(vidx2_lo, 1);
      const uint64_t vidx2_hl = (uint64_t) _mm_cvtsi128_si64(vidx2_hi);
      const uint64_t vidx2_hh = (uint64_t) _mm_extract_epi64(vidx2_hi, 1);
      __m128i vl2_ll = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx2_ll));
      __m128i vl2_lh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx2_lh));
      __m128i vl2_hl = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx2_hl));
      __m128i vl2_hh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx2_hh));
      vl2_ll = _mm_insert_epi32(vl2_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx2_ll >> 32))), 1);
      vl2_lh = _mm_insert_epi32(vl2_lh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx2_lh >> 32))), 1);
      vl2_hl = _mm_insert_epi32(vl2_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx2_hl >> 32))), 1);
      vl2_hh = _mm_insert_epi32(vl2_hh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx2_hh >> 32))), 1);
    #else
      __m128i vl2_ll = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_cvtsi128_si32(vidx2_lo)));
      __m128i vl2_lh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx2_lo, 2)));
      __m128i vl2_hl = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_cvtsi128_si32(vidx2_hi)));
      __m128i vl2_hh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx2_hi, 2)));
      vl2_ll = _mm_insert_epi32(vl2_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx2_lo, 1))), 1);
      vl2_lh = _mm_insert_epi32(vl2_lh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx2_lo, 3))), 1);
      vl2_hl = _mm_insert_epi32(vl2_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx2_hi, 1))), 1);
      vl2_hh = _mm_insert_epi32(vl2_hh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx2_hi, 3))), 1);
    #endif
    const __m128i vl2_lo = _mm_unpacklo_epi64(vl2_ll, vl2_lh);
    const __m128i vl2_hi = _mm_unpacklo_epi64(vl2_hl, vl2_hh);

    const __m128i ven0_lo = _mm_slli_epi32(_mm_castps_si128(_mm256_castps256_ps128(vn0)), 19);
    const __m128i ven0_hi = _mm_slli_epi32(_mm_castps_si128(_mm256_extractf128_ps(vn0, 1)), 19);
    vn0 = _mm256_sub_ps(vn0, vmagic_bias);
    const __m128 vs0_lo = _mm_castsi128_ps(_mm_add_epi32(vl0_lo, ven0_lo));
    const __m128 vs0_hi = _mm_castsi128_ps(_mm_add_epi32(vl0_hi, ven0_hi));
    const __m128i ven1_lo = _mm_slli_epi32(_mm_castps_si128(_mm256_castps256_ps128(vn1)), 19);
    const __m128i ven1_hi = _mm_slli_epi32(_mm_castps_si128(_mm256_extractf128_ps(vn1, 1)), 19);
    vn1 = _mm256_sub_ps(vn1, vmagic_bias);
    const __m128 vs1_lo = _mm_castsi128_ps(_mm_add_epi32(vl1_lo, ven1_lo));
    const __m128 vs1_hi = _mm_castsi128_ps(_mm_add_epi32(vl1_hi, ven1_hi));
    const __m128i ven2_lo = _mm_slli_epi32(_mm_castps_si128(_mm256_castps256_ps128(vn2)), 19);
    const __m128i ven2_hi = _mm_slli_epi32(_mm_castps_si128(_mm256_extractf128_ps(vn2, 1)), 19);
    vn2 = _mm256_sub_ps(vn2, vmagic_bias);
    const __m128 vs2_lo = _mm_castsi128_ps(_mm_add_epi32(vl2_lo, ven2_lo));
    const __m128 vs2_hi = _mm_castsi128_ps(_mm_add_epi32(vl2_hi, ven2_hi));

    __m256 vt0 = _mm256_add_ps(_mm256_mul_ps(vn0, vminus_ln2_hi), vz0);
    __m256 vt1 = _mm256_add_ps(_mm256_mul_ps(vn1, vminus_ln2_hi), vz1);
    __m256 vt2 = _mm256_add_ps(_mm256_mul_ps(vn2, vminus_ln2_hi), vz2);

    vt0 = _mm256_add_ps(_mm256_mul_ps(vn0, vminus_ln2_lo), vt0);
    __m256 vs0 = _mm256_insertf128_ps(_mm256_castps128_ps256(vs0_lo), vs0_hi, 1);
    vt1 = _mm256_add_ps(_mm256_mul_ps(vn1, vminus_ln2_lo), vt1);
    __m256 vs1 = _mm256_insertf128_ps(_mm256_castps128_ps256(vs1_lo), vs1_hi, 1);
    vt2 = _mm256_add_ps(_mm256_mul_ps(vn2, vminus_ln2_lo), vt2);
    __m256 vs2 = _mm256_insertf128_ps(_mm256_castps128_ps256(vs2_lo), vs2_hi, 1);

    __m256 vp0 = _mm256_add_ps(_mm256_mul_ps(vc3, vt0), vc2);
    __m256 vp1 = _mm256_add_ps(_mm256_mul_ps(vc3, vt1), vc2);
    __m256 vp2 = _mm256_add_ps(_mm256_mul_ps(vc3, vt2), vc2);

    vp0 = _mm256_mul_ps(vp0, vt0);
    vp1 = _mm256_mul_ps(vp1, vt1);
    vp2 = _mm256_mul_ps(vp2, vt2);

    vt0 = _mm256_mul_ps(vt0, vs0);
    vs0 = _mm256_sub_ps(vs0, vone);
    vt1 = _mm256_mul_ps(vt1, vs1);
    vs1 = _mm256_sub_ps(vs1, vone);
    vt2 = _mm256_mul_ps(vt2, vs2);
    vs2 = _mm256_sub_ps(vs2, vone);

    vp0 = _mm256_add_ps(_mm256_mul_ps(vp0, vt0), vt0);
    vp1 = _mm256_add_ps(_mm256_mul_ps(vp1, vt1), vt1);
    vp2 = _mm256_add_ps(_mm256_mul_ps(vp2, vt2), vt2);

    const __m256 ve0 = _mm256_mul_ps(_mm256_add_ps(vp0, vs0), valpha);
    vx0 = _mm256_mul_ps(vx0, vbeta);
    const __m256 ve1 = _mm256_mul_ps(_mm256_add_ps(vp1, vs1), valpha);
    vx1 = _mm256_mul_ps(vx1, vbeta);
    const __m256 ve2 = _mm256_mul_ps(_mm256_add_ps(vp2, vs2), valpha);
    vx2 = _mm256_mul_ps(vx2, vbeta);

    const __m256 vy0 = _mm256_blendv_ps(vx0, ve0, vx0);
    const __m256 vy1 = _mm256_blendv_ps(vx1, ve1, vx1);
    const __m256 vy2 = _mm256_blendv_ps(vx2, ve2, vx2);

    _mm256_storeu_ps(y, vy0);
    _mm256_storeu_ps(y + 8, vy1);
    _mm256_storeu_ps(y + 16, vy2);
    y += 24;
  }
  for (; n >= 8 * sizeof(float); n -= 8 * sizeof(float)) {
    __m256 vx = _mm256_loadu_ps(x);
    x += 8;

    const __m256 vz = _mm256_max_ps(vsat_cutoff, _mm256_mul_ps(vx, vprescale));

    __m256 vn = _mm256_add_ps(_mm256_mul_ps(vz, vlog2e), vmagic_bias);

    const __m256 vidx = _mm256_and_ps(vn, vindex_mask);

    const __m128i vidx_lo = _mm_slli_epi32(_mm_castps_si128(_mm256_castps256_ps128(vidx)), 2);
    const __m128i vidx_hi = _mm_slli_epi32(_mm_castps_si128(_mm256_extractf128_ps(vidx, 1)), 2);
    #if XNN_ARCH_X86_64
      const uint64_t vidx_ll = (uint64_t) _mm_cvtsi128_si64(vidx_lo);
      const uint64_t vidx_lh = (uint64_t) _mm_extract_epi64(vidx_lo, 1);
      const uint64_t vidx_hl = (uint64_t) _mm_cvtsi128_si64(vidx_hi);
      const uint64_t vidx_hh = (uint64_t) _mm_extract_epi64(vidx_hi, 1);
      __m128i vl_ll = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx_ll));
      __m128i vl_lh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx_lh));
      __m128i vl_hl = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx_hl));
      __m128i vl_hh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx_hh));
      vl_ll = _mm_insert_epi32(vl_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx_ll >> 32))), 1);
      vl_lh = _mm_insert_epi32(vl_lh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx_lh >> 32))), 1);
      vl_hl = _mm_insert_epi32(vl_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx_hl >> 32))), 1);
      vl_hh = _mm_insert_epi32(vl_hh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx_hh >> 32))), 1);
    #else
      __m128i vl_ll = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_cvtsi128_si32(vidx_lo)));
      __m128i vl_lh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx_lo, 2)));
      __m128i vl_hl = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_cvtsi128_si32(vidx_hi)));
      __m128i vl_hh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx_hi, 2)));
      vl_ll = _mm_insert_epi32(vl_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx_lo, 1))), 1);
      vl_lh = _mm_insert_epi32(vl_lh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx_lo, 3))), 1);
      vl_hl = _mm_insert_epi32(vl_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx_hi, 1))), 1);
      vl_hh = _mm_insert_epi32(vl_hh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx_hi, 3))), 1);
    #endif
    const __m128i ven_lo = _mm_slli_epi32(_mm_castps_si128(_mm256_castps256_ps128(vn)), 19);
    const __m128i ven_hi = _mm_slli_epi32(_mm_castps_si128(_mm256_extractf128_ps(vn, 1)), 19);

    const __m128i vl_lo = _mm_unpacklo_epi64(vl_ll, vl_lh);
    const __m128i vl_hi = _mm_unpacklo_epi64(vl_hl, vl_hh);

    vn = _mm256_sub_ps(vn, vmagic_bias);
    const __m128 vs_lo = _mm_castsi128_ps(_mm_add_epi32(vl_lo, ven_lo));
    const __m128 vs_hi = _mm_castsi128_ps(_mm_add_epi32(vl_hi, ven_hi));

    __m256 vt = _mm256_add_ps(_mm256_mul_ps(vn, vminus_ln2_hi), vz);
    vt = _mm256_add_ps(_mm256_mul_ps(vn, vminus_ln2_lo), vt);
    __m256 vs = _mm256_insertf128_ps(_mm256_castps128_ps256(vs_lo), vs_hi, 1);

    __m256 vp = _mm256_add_ps(_mm256_mul_ps(vc3, vt), vc2);
    vp = _mm256_mul_ps(vp, vt);

    vt = _mm256_mul_ps(vt, vs);
    vs = _mm256_sub_ps(vs, vone);
    vp = _mm256_add_ps(_mm256_mul_ps(vp, vt), vt);

    const __m256 ve = _mm256_mul_ps(_mm256_add_ps(vp, vs), valpha);
    vx = _mm256_mul_ps(vx, vbeta);
    const __m256 vy = _mm256_blendv_ps(vx, ve, vx);

    _mm256_storeu_ps(y, vy);
    y += 8;
  }
  if XNN_UNLIKELY(n != 0) {
    assert(n >= 1 * sizeof(float));
    assert(n <= 7 * sizeof(float));
    const __m256i vmask = _mm256_loadu_si256((const __m256i*) ((uintptr_t) &params->avx_rr2_lut16_p3.mask_table[7] - n));

    __m256 vx = _mm256_maskload_ps(x, vmask);

    const __m256 vz = _mm256_max_ps(vsat_cutoff, _mm256_mul_ps(vx, vprescale));

    __m256 vn = _mm256_add_ps(_mm256_mul_ps(vz, vlog2e), vmagic_bias);

    const __m256 vidx = _mm256_and_ps(vn, vindex_mask);

    const __m128i vidx_lo = _mm_slli_epi32(_mm_castps_si128(_mm256_castps256_ps128(vidx)), 2);
    const __m128i vidx_hi = _mm_slli_epi32(_mm_castps_si128(_mm256_extractf128_ps(vidx, 1)), 2);
    #if XNN_ARCH_X86_64
      const uint64_t vidx_ll = (uint64_t) _mm_cvtsi128_si64(vidx_lo);
      const uint64_t vidx_lh = (uint64_t) _mm_extract_epi64(vidx_lo, 1);
      const uint64_t vidx_hl = (uint64_t) _mm_cvtsi128_si64(vidx_hi);
      const uint64_t vidx_hh = (uint64_t) _mm_extract_epi64(vidx_hi, 1);
      __m128i vl_ll = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx_ll));
      __m128i vl_lh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx_lh));
      __m128i vl_hl = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx_hl));
      __m128i vl_hh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) vidx_hh));
      vl_ll = _mm_insert_epi32(vl_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx_ll >> 32))), 1);
      vl_lh = _mm_insert_epi32(vl_lh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx_lh >> 32))), 1);
      vl_hl = _mm_insert_epi32(vl_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx_hl >> 32))), 1);
      vl_hh = _mm_insert_epi32(vl_hh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) (vidx_hh >> 32))), 1);
    #else
      __m128i vl_ll = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_cvtsi128_si32(vidx_lo)));
      __m128i vl_lh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx_lo, 2)));
      __m128i vl_hl = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_cvtsi128_si32(vidx_hi)));
      __m128i vl_hh = _mm_loadu_si32((const void*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx_hi, 2)));
      vl_ll = _mm_insert_epi32(vl_ll, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx_lo, 1))), 1);
      vl_lh = _mm_insert_epi32(vl_lh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx_lo, 3))), 1);
      vl_hl = _mm_insert_epi32(vl_hl, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx_hi, 1))), 1);
      vl_hh = _mm_insert_epi32(vl_hh, *((const int*) ((uintptr_t) xnn_table_exp2minus_k_over_16 + (uint32_t) _mm_extract_epi32(vidx_hi, 3))), 1);
    #endif
    const __m128i ven_lo = _mm_slli_epi32(_mm_castps_si128(_mm256_castps256_ps128(vn)), 19);
    const __m128i ven_hi = _mm_slli_epi32(_mm_castps_si128(_mm256_extractf128_ps(vn, 1)), 19);

    const __m128i vl_lo = _mm_unpacklo_epi64(vl_ll, vl_lh);
    const __m128i vl_hi = _mm_unpacklo_epi64(vl_hl, vl_hh);

    vn = _mm256_sub_ps(vn, vmagic_bias);
    const __m128 vs_lo = _mm_castsi128_ps(_mm_add_epi32(vl_lo, ven_lo));
    const __m128 vs_hi = _mm_castsi128_ps(_mm_add_epi32(vl_hi, ven_hi));

    __m256 vt = _mm256_add_ps(_mm256_mul_ps(vn, vminus_ln2_hi), vz);
    vt = _mm256_add_ps(_mm256_mul_ps(vn, vminus_ln2_lo), vt);
    __m256 vs = _mm256_insertf128_ps(_mm256_castps128_ps256(vs_lo), vs_hi, 1);

    __m256 vp = _mm256_add_ps(_mm256_mul_ps(vc3, vt), vc2);
    vp = _mm256_mul_ps(vp, vt);

    vt = _mm256_mul_ps(vt, vs);
    vs = _mm256_sub_ps(vs, vone);
    vp = _mm256_add_ps(_mm256_mul_ps(vp, vt), vt);

    const __m256 ve = _mm256_mul_ps(_mm256_add_ps(vp, vs), valpha);
    vx = _mm256_mul_ps(vx, vbeta);
    const __m256 vy = _mm256_blendv_ps(vx, ve, vx);

    __m128 vy_lo = _mm256_castps256_ps128(vy);
    if (n & (4 * sizeof(float))) {
      _mm_storeu_ps(y, vy_lo);
      vy_lo = _mm256_extractf128_ps(vy, 1);
      y += 4;
    }
    if (n & (2 * sizeof(float))) {
      _mm_storel_pi((__m64*) y, vy_lo);
      vy_lo = _mm_movehl_ps(vy_lo, vy_lo);
      y += 2;
    }
    if (n & (1 * sizeof(float))) {
      _mm_store_ss(y, vy_lo);
    }
  }
}
