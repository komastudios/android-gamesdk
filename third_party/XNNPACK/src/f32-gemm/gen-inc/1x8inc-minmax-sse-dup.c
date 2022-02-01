// Auto-generated file. Do not edit!
//   Template: src/f32-gemm/sse-dup.c.in
//   Generator: tools/xngen
//
// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <xmmintrin.h>

#include <xnnpack/gemm.h>


void xnn_f32_gemminc_minmax_ukernel_1x8__sse_dup(
    size_t mr,
    size_t nc,
    size_t kc,
    const float*restrict a,
    size_t a_stride,
    const float*restrict w,
    float*restrict c,
    size_t cm_stride,
    size_t cn_stride,
    const float*restrict acc,
    const union xnn_f32_minmax_params params[restrict XNN_MIN_ELEMENTS(1)])
{
  assert(mr != 0);
  assert(mr <= 1);
  assert(nc != 0);
  assert(kc != 0);
  assert(kc % sizeof(float) == 0);
  assert(a != NULL);
  assert(w != NULL);
  assert(c != NULL);
  assert(acc != NULL);

  const float* a0 = a;
  float* c0 = c;

  do {
    __m128 vacc0x0123 = _mm_load_ps(acc + 0);
    __m128 vacc0x4567 = _mm_load_ps(acc + 4);
    acc += 8;

    size_t k = kc;
    while (k >= 4 * sizeof(float)) {
      const __m128 va0 = _mm_loadu_ps(a0);
      a0 += 4;


      const __m128 va0c0000 = _mm_shuffle_ps(va0, va0, _MM_SHUFFLE(0, 0, 0, 0));

      const __m128 vb0123c0 = _mm_load_ps(w + 0);
      const __m128 vb4567c0 = _mm_load_ps(w + 4);

      vacc0x0123 = _mm_add_ps(vacc0x0123, _mm_mul_ps(va0c0000, vb0123c0));
      vacc0x4567 = _mm_add_ps(vacc0x4567, _mm_mul_ps(va0c0000, vb4567c0));

      const __m128 va0c1111 = _mm_shuffle_ps(va0, va0, _MM_SHUFFLE(1, 1, 1, 1));

      const __m128 vb0123c1 = _mm_load_ps(w + 8);
      const __m128 vb4567c1 = _mm_load_ps(w + 12);

      vacc0x0123 = _mm_add_ps(vacc0x0123, _mm_mul_ps(va0c1111, vb0123c1));
      vacc0x4567 = _mm_add_ps(vacc0x4567, _mm_mul_ps(va0c1111, vb4567c1));

      const __m128 va0c2222 = _mm_shuffle_ps(va0, va0, _MM_SHUFFLE(2, 2, 2, 2));

      const __m128 vb0123c2 = _mm_load_ps(w + 16);
      const __m128 vb4567c2 = _mm_load_ps(w + 20);

      vacc0x0123 = _mm_add_ps(vacc0x0123, _mm_mul_ps(va0c2222, vb0123c2));
      vacc0x4567 = _mm_add_ps(vacc0x4567, _mm_mul_ps(va0c2222, vb4567c2));

      const __m128 va0c3333 = _mm_shuffle_ps(va0, va0, _MM_SHUFFLE(3, 3, 3, 3));

      const __m128 vb0123c3 = _mm_load_ps(w + 24);
      const __m128 vb4567c3 = _mm_load_ps(w + 28);

      vacc0x0123 = _mm_add_ps(vacc0x0123, _mm_mul_ps(va0c3333, vb0123c3));
      vacc0x4567 = _mm_add_ps(vacc0x4567, _mm_mul_ps(va0c3333, vb4567c3));

      w += 32;
      k -= 4 * sizeof(float);
    }
    if XNN_UNLIKELY(k != 0) {
      do {
        const __m128 va0 = _mm_load1_ps(a0);
        a0 += 1;

        const __m128 vb0123 = _mm_load_ps(w);
        const __m128 vb4567 = _mm_load_ps(w + 4);
        w += 8;

        vacc0x0123 = _mm_add_ps(vacc0x0123, _mm_mul_ps(va0, vb0123));
        vacc0x4567 = _mm_add_ps(vacc0x4567, _mm_mul_ps(va0, vb4567));

        k -= sizeof(float);
      } while (k != 0);
    }

    const __m128 vmax = _mm_load_ps(params->sse.max);
    vacc0x0123 = _mm_min_ps(vacc0x0123, vmax);
    vacc0x4567 = _mm_min_ps(vacc0x4567, vmax);

    const __m128 vmin = _mm_load_ps(params->sse.min);
    vacc0x0123 = _mm_max_ps(vacc0x0123, vmin);
    vacc0x4567 = _mm_max_ps(vacc0x4567, vmin);

    if XNN_LIKELY(nc >= 8) {
      _mm_storeu_ps(c0, vacc0x0123);
      _mm_storeu_ps(c0 + 4, vacc0x4567);
      c0 = (float*) ((uintptr_t) c0 + cn_stride);

      a0 = (const float*) ((uintptr_t) a0 - kc);

      nc -= 8;
    } else {
      if (nc & 4) {
        _mm_storeu_ps(c0, vacc0x0123);

        vacc0x0123 = vacc0x4567;

        c0 += 4;
      }
      if (nc & 2) {
        _mm_storel_pi((__m64*) c0, vacc0x0123);

        vacc0x0123 = _mm_movehl_ps(vacc0x0123, vacc0x0123);

        c0 += 2;
      }
      if (nc & 1) {
        _mm_store_ss(c0, vacc0x0123);
      }

      nc = 0;
    }
  } while (nc != 0);
}
