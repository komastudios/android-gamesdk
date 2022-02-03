// Auto-generated file. Do not edit!
//   Template: src/f32-vclamp/avx.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <immintrin.h>

#include <xnnpack/common.h>
#include <xnnpack/vunary.h>


void xnn_f32_vclamp_ukernel__avx_x8(
    size_t n,
    const float* x,
    float* y,
    const union xnn_f32_minmax_params params[restrict XNN_MIN_ELEMENTS(1)])
{
  assert(n != 0);
  assert(n % sizeof(float) == 0);
  assert(x != NULL);
  assert(y != NULL);

  const __m256 vy_min = _mm256_load_ps(params->avx.min);
  const __m256 vy_max = _mm256_load_ps(params->avx.max);

  for (; n >= 8 * sizeof(float); n -= 8 * sizeof(float)) {
    __m256 vacc01234567 = _mm256_loadu_ps(x);
    x += 8;

    vacc01234567 = _mm256_max_ps(vacc01234567, vy_min);

    vacc01234567 = _mm256_min_ps(vacc01234567, vy_max);

    _mm256_storeu_ps(y, vacc01234567);
    y += 8;
  }
  if XNN_UNLIKELY(n != 0) {
    assert(n >= 1 * sizeof(float));
    assert(n <= 7 * sizeof(float));
    const __m256i vmask = _mm256_loadu_si256((const __m256i*) ((uintptr_t) &params->avx.mask_table[7] - n));

    __m256 vacc = _mm256_maskload_ps(x, vmask);
    vacc = _mm256_max_ps(vacc, vy_min);
    vacc = _mm256_min_ps(vacc, vy_max);

    __m128 vacc_lo = _mm256_castps256_ps128(vacc);
    if (n & (4 * sizeof(float))) {
      _mm_storeu_ps(y, vacc_lo);
      vacc_lo = _mm256_extractf128_ps(vacc, 1);
      y += 4;
    }
    if (n & (2 * sizeof(float))) {
      _mm_storel_pi((__m64*) y, vacc_lo);
      vacc_lo = _mm_movehl_ps(vacc_lo, vacc_lo);
      y += 2;
    }
    if (n & (1 * sizeof(float))) {
      _mm_store_ss(y, vacc_lo);
    }
  }
}
