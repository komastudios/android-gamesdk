// Auto-generated file. Do not edit!
//   Template: src/qs8-igemm/MRx4c2-sse.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <smmintrin.h>

#include <xnnpack/igemm.h>
#include <xnnpack/math.h>


void xnn_qc8_igemm_minmax_fp32_ukernel_1x4c2__sse41_ld64(
    size_t mr,
    size_t nc,
    size_t kc,
    size_t ks,
    const int8_t** restrict a,
    const void* restrict w,
    int8_t* restrict c,
    size_t cm_stride,
    size_t cn_stride,
    size_t a_offset,
    const int8_t* zero,
    const union xnn_qs8_minmax_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(mr != 0);
  assert(mr <= 1);
  assert(nc != 0);
  assert(kc != 0);
  assert(ks != 0);
  assert(ks % (1 * sizeof(void*)) == 0);
  assert(a_offset % sizeof(int8_t) == 0);
  assert(a != NULL);
  assert(w != NULL);
  assert(c != NULL);

  kc = round_up_po2(kc, 2);
  int8_t* c0 = c;

  do {
    __m128i vacc0x0123 = _mm_loadu_si128((const __m128i*) w);
    w = (const void*) ((const int32_t*) w + 4);

    size_t p = ks;
    do {
      const int8_t* restrict a0 = a[0];
      if XNN_UNPREDICTABLE(a0 != zero) {
        a0 = (const int8_t*) ((uintptr_t) a0 + a_offset);
      }
      a += 1;

      size_t k = kc;
      while (k >= 8 * sizeof(int8_t)) {
        const __m128i va0 = _mm_loadl_epi64((const __m128i*) a0);
        const __m128i vxa0 = _mm_cvtepi8_epi16(va0);
        a0 += 8;

        const __m128i vb0 = _mm_loadl_epi64((const __m128i*) w);
        const __m128i vxb0 = _mm_cvtepi8_epi16(vb0);

        vacc0x0123 = _mm_add_epi32(vacc0x0123,
          _mm_madd_epi16(_mm_shuffle_epi32(vxa0, _MM_SHUFFLE(0, 0, 0, 0)), vxb0));
        const __m128i vb1 = _mm_loadl_epi64((const __m128i*) ((const int8_t*) w + 8));
        const __m128i vxb1 = _mm_cvtepi8_epi16(vb1);

        vacc0x0123 = _mm_add_epi32(vacc0x0123,
          _mm_madd_epi16(_mm_shuffle_epi32(vxa0, _MM_SHUFFLE(1, 1, 1, 1)), vxb1));
        const __m128i vb2 = _mm_loadl_epi64((const __m128i*) ((const int8_t*) w + 16));
        const __m128i vxb2 = _mm_cvtepi8_epi16(vb2);

        vacc0x0123 = _mm_add_epi32(vacc0x0123,
          _mm_madd_epi16(_mm_shuffle_epi32(vxa0, _MM_SHUFFLE(2, 2, 2, 2)), vxb2));
        const __m128i vb3 = _mm_loadl_epi64((const __m128i*) ((const int8_t*) w + 24));
        const __m128i vxb3 = _mm_cvtepi8_epi16(vb3);

        vacc0x0123 = _mm_add_epi32(vacc0x0123,
          _mm_madd_epi16(_mm_shuffle_epi32(vxa0, _MM_SHUFFLE(3, 3, 3, 3)), vxb3));

        w = (const void*) ((const int8_t*) w + 32);
        k -= 8 * sizeof(int8_t);
      }
      if (k != 0) {
        const __m128i va0 = _mm_loadl_epi64((const __m128i*) a0);
        const __m128i vxa0 = _mm_cvtepi8_epi16(va0);
        a0 = (const int8_t*) ((uintptr_t) a0 + k);

        const __m128i vb0 = _mm_loadl_epi64((const __m128i*) w);
        w = (const void*) ((const int8_t*) w + 8);
        const __m128i vxb0 = _mm_cvtepi8_epi16(vb0);

        vacc0x0123 = _mm_add_epi32(vacc0x0123,
          _mm_madd_epi16(_mm_shuffle_epi32(vxa0, _MM_SHUFFLE(0, 0, 0, 0)), vxb0));

        if (k > 2 * sizeof(int8_t)) {
          const __m128i vb1 = _mm_loadl_epi64((const __m128i*) w);
          w = (const void*) ((const int8_t*) w + 8);
          const __m128i vxb1 = _mm_cvtepi8_epi16(vb1);

          vacc0x0123 = _mm_add_epi32(vacc0x0123,
            _mm_madd_epi16(_mm_shuffle_epi32(vxa0, _MM_SHUFFLE(1, 1, 1, 1)), vxb1));

          if (k > 4 * sizeof(int8_t)) {
            const __m128i vb2 = _mm_loadl_epi64((const __m128i*) w);
            w = (const void*) ((const int8_t*) w + 8);
            const __m128i vxb2 = _mm_cvtepi8_epi16(vb2);

            vacc0x0123 = _mm_add_epi32(vacc0x0123,
              _mm_madd_epi16(_mm_shuffle_epi32(vxa0, _MM_SHUFFLE(2, 2, 2, 2)), vxb2));
          }
        }
      }
      p -= 1 * sizeof(void*);
    } while (p != 0);

    __m128 vscaled0x0123 = _mm_cvtepi32_ps(vacc0x0123);

    const __m128 vscale0123 = _mm_loadu_ps((const float*) w);
    w = (const void*) ((const float*) w + 4);
    vscaled0x0123 = _mm_mul_ps(vscaled0x0123, vscale0123);

    const __m128 voutput_max_less_zero_point = _mm_load_ps(params->sse4.output_max_less_zero_point);
    vscaled0x0123 = _mm_min_ps(vscaled0x0123, voutput_max_less_zero_point);

    vacc0x0123 = _mm_cvtps_epi32(vscaled0x0123);

    const __m128i voutput_zero_point = _mm_load_si128((const __m128i*) params->sse4.output_zero_point);
    __m128i vacc00x0123 = _mm_adds_epi16(_mm_packs_epi32(vacc0x0123, vacc0x0123), voutput_zero_point);


    __m128i vout = _mm_packs_epi16(vacc00x0123, vacc00x0123);

    vout = _mm_max_epi8(vout, _mm_load_si128((const __m128i*) params->sse4.output_min));

    if (nc >= 4) {
      *((uint32_t*) c0) = (uint32_t) _mm_cvtsi128_si32(vout);
      c0 = (int8_t*) ((uintptr_t) c0 + cn_stride);

      a = (const int8_t**restrict) ((uintptr_t) a - ks);

      nc -= 4;
    } else {
      if (nc & 2) {
        *((uint16_t*) c0) = (uint16_t) _mm_extract_epi16(vout, 0);
        c0 += 2;
        vout = _mm_srli_epi32(vout, 16);
      }
      if (nc & 1) {
        *c0 = (int8_t) _mm_extract_epi8(vout, 0);
      }

      nc = 0;
    }
  } while (nc != 0);
}
