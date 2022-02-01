// Auto-generated file. Do not edit!
//   Template: src/f32-vlrelu/sse.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <emmintrin.h>

#include <xnnpack/common.h>
#include <xnnpack/vunary.h>


void xnn_f32_vlrelu_ukernel__sse2_x8(
    size_t n,
    const float* x,
    float* y,
    const union xnn_f32_lrelu_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(n != 0);
  assert(n % sizeof(float) == 0);

  const __m128 vslope = _mm_load_ps(params->sse.slope);
  for (; n >= 8 * sizeof(float); n -= 8 * sizeof(float)) {
    const __m128 vx0123 = _mm_loadu_ps(x);
    const __m128 vx4567 = _mm_loadu_ps(x + 4);
    x += 8;

    __m128 vacc0123 = _mm_mul_ps(vx0123, vslope);
    const __m128 vmask0123 = _mm_castsi128_ps(_mm_cmpgt_epi32(_mm_setzero_si128(), _mm_castps_si128(vx0123)));
    __m128 vacc4567 = _mm_mul_ps(vx4567, vslope);
    const __m128 vmask4567 = _mm_castsi128_ps(_mm_cmpgt_epi32(_mm_setzero_si128(), _mm_castps_si128(vx4567)));

    vacc0123 = _mm_or_ps(_mm_and_ps(vacc0123, vmask0123), _mm_andnot_ps(vmask0123, vx0123));
    vacc4567 = _mm_or_ps(_mm_and_ps(vacc4567, vmask4567), _mm_andnot_ps(vmask4567, vx4567));

    _mm_storeu_ps(y, vacc0123);
    _mm_storeu_ps(y + 4, vacc4567);
    y += 8;
  }
  for (; n >= 4 * sizeof(float); n -= 4 * sizeof(float)) {
    const __m128 vx = _mm_loadu_ps(x);
    x += 4;

    __m128 vacc = _mm_mul_ps(vx, vslope);
    const __m128 vmask = _mm_castsi128_ps(_mm_cmpgt_epi32(_mm_setzero_si128(), _mm_castps_si128(vx)));
    vacc = _mm_or_ps(_mm_and_ps(vacc, vmask), _mm_andnot_ps(vmask, vx));

    _mm_storeu_ps(y, vacc);
    y += 4;
  }
  if XNN_UNLIKELY(n != 0) {
    const __m128 vx = _mm_loadu_ps(x);

    __m128 vacc = _mm_mul_ps(vx, vslope);
    const __m128 vmask = _mm_castsi128_ps(_mm_cmpgt_epi32(_mm_setzero_si128(), _mm_castps_si128(vx)));
    vacc = _mm_or_ps(_mm_and_ps(vacc, vmask), _mm_andnot_ps(vmask, vx));

    if (n & (2 * sizeof(float))) {
      _mm_storel_pi((__m64*) y, vacc);
      vacc = _mm_movehl_ps(vacc, vacc);
      y += 2;
    }
    if (n & (1 * sizeof(float))) {
      _mm_store_ss(y, vacc);
    }
  }
}
