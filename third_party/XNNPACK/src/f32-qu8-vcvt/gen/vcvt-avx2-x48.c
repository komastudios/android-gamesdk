// Auto-generated file. Do not edit!
//   Template: src/f32-qs8-vcvt/avx2.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <immintrin.h>

#include <xnnpack/common.h>
#include <xnnpack/vcvt.h>


void xnn_f32_qu8_vcvt_ukernel__avx2_x48(
    size_t n,
    const float* x,
    uint8_t* y,
    const union xnn_f32_qu8_cvt_params params[restrict XNN_MIN_ELEMENTS(1)])
{
  assert(n != 0);
  assert(n % sizeof(float) == 0);
  assert(x != NULL);
  assert(y != NULL);

  const __m256 vscale = _mm256_load_ps(params->avx2.scale);
  const __m256 voutput_max_less_zero_point = _mm256_load_ps(params->avx2.output_max_less_zero_point);
  const __m256i voutput_zero_point = _mm256_load_si256((const __m256i*) params->avx2.output_zero_point);
  const __m256i vshuffle_mask = _mm256_load_si256((const __m256i*) params->avx2.shuffle_mask);
  const __m256i voutput_min = _mm256_load_si256((const __m256i*) params->avx2.output_min);

  for (; n >= 48 * sizeof(float); n -= 48 * sizeof(float)) {
    __m256 vx01 = _mm256_loadu_ps(x);
    __m256 vx23 = _mm256_loadu_ps(x + 8);
    __m256 vx45 = _mm256_loadu_ps(x + 16);
    __m256 vx67 = _mm256_loadu_ps(x + 24);
    __m256 vx89 = _mm256_loadu_ps(x + 32);
    __m256 vxAB = _mm256_loadu_ps(x + 40);
    x += 48;

    vx01 = _mm256_mul_ps(vx01, vscale);
    vx23 = _mm256_mul_ps(vx23, vscale);
    vx45 = _mm256_mul_ps(vx45, vscale);
    vx67 = _mm256_mul_ps(vx67, vscale);
    vx89 = _mm256_mul_ps(vx89, vscale);
    vxAB = _mm256_mul_ps(vxAB, vscale);

    vx01 = _mm256_min_ps(vx01, voutput_max_less_zero_point);
    vx23 = _mm256_min_ps(vx23, voutput_max_less_zero_point);
    vx45 = _mm256_min_ps(vx45, voutput_max_less_zero_point);
    vx67 = _mm256_min_ps(vx67, voutput_max_less_zero_point);
    vx89 = _mm256_min_ps(vx89, voutput_max_less_zero_point);
    vxAB = _mm256_min_ps(vxAB, voutput_max_less_zero_point);

    const __m256i vacc01 = _mm256_cvtps_epi32(vx01);
    const __m256i vacc23 = _mm256_cvtps_epi32(vx23);
    const __m256i vacc45 = _mm256_cvtps_epi32(vx45);
    const __m256i vacc67 = _mm256_cvtps_epi32(vx67);
    const __m256i vacc89 = _mm256_cvtps_epi32(vx89);
    const __m256i vaccAB = _mm256_cvtps_epi32(vxAB);

    __m256i vacc0213 = _mm256_packs_epi32(vacc01, vacc23);
    __m256i vacc4657 = _mm256_packs_epi32(vacc45, vacc67);
    __m256i vacc8A9B = _mm256_packs_epi32(vacc89, vaccAB);

    vacc0213 = _mm256_adds_epi16(vacc0213, voutput_zero_point);
    vacc4657 = _mm256_adds_epi16(vacc4657, voutput_zero_point);
    vacc8A9B = _mm256_adds_epi16(vacc8A9B, voutput_zero_point);

    const __m256i vy02461357 = _mm256_packus_epi16(vacc0213, vacc4657);
    const __m128i vy8A9B = _mm_packus_epi16(_mm256_castsi256_si128(vacc8A9B), _mm256_extracti128_si256(vacc8A9B, 1));

    __m256i vy01234567 = _mm256_permutevar8x32_epi32(vy02461357, vshuffle_mask);
    __m128i vy89AB = _mm_shuffle_epi32(vy8A9B, _MM_SHUFFLE(3, 1, 2, 0));

    vy01234567 = _mm256_max_epu8(vy01234567, voutput_min);
    vy89AB = _mm_max_epu8(vy89AB, _mm256_castsi256_si128(voutput_min));

    _mm256_storeu_si256((__m256i*) y, vy01234567);
    _mm_storeu_si128((__m128i*) (y + 32), vy89AB);
    y += 48;
  }
  for (; n >= 8 * sizeof(float); n -= 8 * sizeof(float)) {
    __m256 vx = _mm256_loadu_ps(x);
    vx = _mm256_mul_ps(vx, vscale);
    vx = _mm256_min_ps(vx, voutput_max_less_zero_point);
    x += 8;

    const __m256i vacc = _mm256_cvtps_epi32(vx);

    __m128i vy = _mm_packs_epi32(_mm256_castsi256_si128(vacc), _mm256_extracti128_si256(vacc, 1));
    vy = _mm_adds_epi16(vy, _mm256_castsi256_si128(voutput_zero_point));
    vy = _mm_packus_epi16(vy, vy);
    vy = _mm_max_epu8(vy, _mm256_castsi256_si128(voutput_min));

    _mm_storel_epi64((__m128i*) y, vy);
    y += 8;
  }
  if XNN_UNLIKELY(n != 0) {
    assert(n >= 1 * sizeof(float));
    assert(n <= 7 * sizeof(float));
    const __m256i vmask = _mm256_loadu_si256((const __m256i*) ((uintptr_t) &params->avx2.mask_table[7] - n));

    __m256 vx = _mm256_maskload_ps(x, vmask);
    vx = _mm256_mul_ps(vx, vscale);
    vx = _mm256_min_ps(vx, voutput_max_less_zero_point);

    const __m256i vacc = _mm256_cvtps_epi32(vx);

    __m128i vy = _mm_packs_epi32(_mm256_castsi256_si128(vacc), _mm256_extracti128_si256(vacc, 1));
    vy = _mm_adds_epi16(vy, _mm256_castsi256_si128(voutput_zero_point));
    vy = _mm_packus_epi16(vy, vy);
    vy = _mm_max_epu8(vy, _mm256_castsi256_si128(voutput_min));

    if (n & (4 * sizeof(float))) {
      *((uint32_t*) y) = (uint32_t) _mm_cvtsi128_si32(vy);
      y += 4;
      vy = _mm_srli_epi64(vy, 32);
    }
    if (n & (2 * sizeof(float))) {
      *((uint16_t*) y) = (uint16_t) _mm_extract_epi16(vy, 0);
      y += 2;
      vy = _mm_srli_epi32(vy, 16);
    }
    if (n & (1 * sizeof(float))) {
      *y = (uint8_t) _mm_extract_epi8(vy, 0);
    }
  }
}
