// Auto-generated file. Do not edit!
//   Template: src/qs8-vadd/sse-mul16-ld64.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <smmintrin.h>

#include <xnnpack/vaddsub.h>


void xnn_qs8_vadd_minmax_ukernel__avx_mul16_ld64_x24(
    size_t n,
    const int8_t* input_a,
    const int8_t* input_b,
    int8_t* output,
    const union xnn_qs8_addsub_minmax_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  const __m128i vbias = _mm_load_si128((const __m128i*) params->sse4_mul16.bias);
  const __m128i va_multiplier_lo = _mm_load_si128((const __m128i*) params->sse4_mul16.a_multiplier_lo);
  const __m128i va_multiplier_hi = _mm_load_si128((const __m128i*) params->sse4_mul16.a_multiplier_hi);
  const __m128i vb_multiplier_lo = _mm_load_si128((const __m128i*) params->sse4_mul16.b_multiplier_lo);
  const __m128i vb_multiplier_hi = _mm_load_si128((const __m128i*) params->sse4_mul16.b_multiplier_hi);
  const __m128i vshift = _mm_cvtsi32_si128((int) params->sse4_mul16.shift);
  const __m128i voutput_zero_point = _mm_load_si128((const __m128i*) params->sse4_mul16.output_zero_point);
  const __m128i voutput_min = _mm_load_si128((const __m128i*) params->sse4_mul16.output_min);
  const __m128i voutput_max = _mm_load_si128((const __m128i*) params->sse4_mul16.output_max);

  for (; n >= 24 * sizeof(int8_t); n -= 24 * sizeof(int8_t)) {
    const __m128i va01234567 = _mm_cvtepi8_epi16(_mm_loadl_epi64((const __m128i*) input_a));
    const __m128i vb01234567 = _mm_cvtepi8_epi16(_mm_loadl_epi64((const __m128i*) input_b));
    const __m128i va89ABCDEF = _mm_cvtepi8_epi16(_mm_loadl_epi64((const __m128i*) (input_a + 8)));
    const __m128i vb89ABCDEF = _mm_cvtepi8_epi16(_mm_loadl_epi64((const __m128i*) (input_b + 8)));
    const __m128i vaGHIJKLMN = _mm_cvtepi8_epi16(_mm_loadl_epi64((const __m128i*) (input_a + 16)));
    const __m128i vbGHIJKLMN = _mm_cvtepi8_epi16(_mm_loadl_epi64((const __m128i*) (input_b + 16)));
    input_a += 24;
    input_b += 24;


    __m128i vaprod01234567hi = _mm_mulhi_epu16(va01234567, va_multiplier_lo);
    __m128i vbprod01234567hi = _mm_mulhi_epu16(vb01234567, vb_multiplier_lo);
    const __m128i vaprod01234567lo = _mm_mullo_epi16(va01234567, va_multiplier_lo);
    const __m128i vbprod01234567lo = _mm_mullo_epi16(vb01234567, vb_multiplier_lo);
    __m128i vaprod89ABCDEFhi = _mm_mulhi_epu16(va89ABCDEF, va_multiplier_lo);
    __m128i vbprod89ABCDEFhi = _mm_mulhi_epu16(vb89ABCDEF, vb_multiplier_lo);
    const __m128i vaprod89ABCDEFlo = _mm_mullo_epi16(va89ABCDEF, va_multiplier_lo);
    const __m128i vbprod89ABCDEFlo = _mm_mullo_epi16(vb89ABCDEF, vb_multiplier_lo);
    __m128i vaprodGHIJKLMNhi = _mm_mulhi_epu16(vaGHIJKLMN, va_multiplier_lo);
    __m128i vbprodGHIJKLMNhi = _mm_mulhi_epu16(vbGHIJKLMN, vb_multiplier_lo);
    const __m128i vaprodGHIJKLMNlo = _mm_mullo_epi16(vaGHIJKLMN, va_multiplier_lo);
    const __m128i vbprodGHIJKLMNlo = _mm_mullo_epi16(vbGHIJKLMN, vb_multiplier_lo);

    vaprod01234567hi = _mm_add_epi16(vaprod01234567hi, _mm_mullo_epi16(va01234567, va_multiplier_hi));
    vbprod01234567hi = _mm_add_epi16(vbprod01234567hi, _mm_mullo_epi16(vb01234567, vb_multiplier_hi));
    vaprod89ABCDEFhi = _mm_add_epi16(vaprod89ABCDEFhi, _mm_mullo_epi16(va89ABCDEF, va_multiplier_hi));
    vbprod89ABCDEFhi = _mm_add_epi16(vbprod89ABCDEFhi, _mm_mullo_epi16(vb89ABCDEF, vb_multiplier_hi));
    vaprodGHIJKLMNhi = _mm_add_epi16(vaprodGHIJKLMNhi, _mm_mullo_epi16(vaGHIJKLMN, va_multiplier_hi));
    vbprodGHIJKLMNhi = _mm_add_epi16(vbprodGHIJKLMNhi, _mm_mullo_epi16(vbGHIJKLMN, vb_multiplier_hi));

    vaprod01234567hi = _mm_sub_epi16(vaprod01234567hi, _mm_and_si128(_mm_srai_epi16(va01234567, 15), va_multiplier_lo));
    vbprod01234567hi = _mm_sub_epi16(vbprod01234567hi, _mm_and_si128(_mm_srai_epi16(vb01234567, 15), vb_multiplier_lo));
    vaprod89ABCDEFhi = _mm_sub_epi16(vaprod89ABCDEFhi, _mm_and_si128(_mm_srai_epi16(va89ABCDEF, 15), va_multiplier_lo));
    vbprod89ABCDEFhi = _mm_sub_epi16(vbprod89ABCDEFhi, _mm_and_si128(_mm_srai_epi16(vb89ABCDEF, 15), vb_multiplier_lo));
    vaprodGHIJKLMNhi = _mm_sub_epi16(vaprodGHIJKLMNhi, _mm_and_si128(_mm_srai_epi16(vaGHIJKLMN, 15), va_multiplier_lo));
    vbprodGHIJKLMNhi = _mm_sub_epi16(vbprodGHIJKLMNhi, _mm_and_si128(_mm_srai_epi16(vbGHIJKLMN, 15), vb_multiplier_lo));

    __m128i vacc0123 = _mm_add_epi32(vbias, _mm_unpacklo_epi16(vaprod01234567lo, vaprod01234567hi));
    __m128i vacc4567 = _mm_add_epi32(vbias, _mm_unpackhi_epi16(vaprod01234567lo, vaprod01234567hi));
    __m128i vacc89AB = _mm_add_epi32(vbias, _mm_unpacklo_epi16(vaprod89ABCDEFlo, vaprod89ABCDEFhi));
    __m128i vaccCDEF = _mm_add_epi32(vbias, _mm_unpackhi_epi16(vaprod89ABCDEFlo, vaprod89ABCDEFhi));
    __m128i vaccGHIJ = _mm_add_epi32(vbias, _mm_unpacklo_epi16(vaprodGHIJKLMNlo, vaprodGHIJKLMNhi));
    __m128i vaccKLMN = _mm_add_epi32(vbias, _mm_unpackhi_epi16(vaprodGHIJKLMNlo, vaprodGHIJKLMNhi));

    vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vbprod01234567lo, vbprod01234567hi));
    vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vbprod01234567lo, vbprod01234567hi));
    vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vbprod89ABCDEFlo, vbprod89ABCDEFhi));
    vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vbprod89ABCDEFlo, vbprod89ABCDEFhi));
    vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vbprodGHIJKLMNlo, vbprodGHIJKLMNhi));
    vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vbprodGHIJKLMNlo, vbprodGHIJKLMNhi));

    vacc0123 = _mm_sra_epi32(vacc0123, vshift);
    vacc4567 = _mm_sra_epi32(vacc4567, vshift);
    vacc89AB = _mm_sra_epi32(vacc89AB, vshift);
    vaccCDEF = _mm_sra_epi32(vaccCDEF, vshift);
    vaccGHIJ = _mm_sra_epi32(vaccGHIJ, vshift);
    vaccKLMN = _mm_sra_epi32(vaccKLMN, vshift);

    __m128i vout01234567 = _mm_adds_epi16(_mm_packs_epi32(vacc0123, vacc4567), voutput_zero_point);
    __m128i vout89ABCDEF = _mm_adds_epi16(_mm_packs_epi32(vacc89AB, vaccCDEF), voutput_zero_point);
    __m128i voutGHIJKLMN = _mm_adds_epi16(_mm_packs_epi32(vaccGHIJ, vaccKLMN), voutput_zero_point);


    __m128i vout0123456789ABCDEF = _mm_packs_epi16(vout01234567, vout89ABCDEF);
    __m128i voutGHIJKLMNGHIJKLMN = _mm_packs_epi16(voutGHIJKLMN, voutGHIJKLMN);

    vout0123456789ABCDEF = _mm_max_epi8(vout0123456789ABCDEF, voutput_min);
    voutGHIJKLMNGHIJKLMN = _mm_max_epi8(voutGHIJKLMNGHIJKLMN, voutput_min);

    vout0123456789ABCDEF = _mm_min_epi8(vout0123456789ABCDEF, voutput_max);
    voutGHIJKLMNGHIJKLMN = _mm_min_epi8(voutGHIJKLMNGHIJKLMN, voutput_max);

    _mm_storeu_si128((__m128i*) output, vout0123456789ABCDEF);
    _mm_storel_epi64((__m128i*) (output + 16), voutGHIJKLMNGHIJKLMN);
    output += 24;
  }
  if XNN_UNLIKELY(n != 0) {
    do {
      const __m128i va01234567 = _mm_cvtepi8_epi16(_mm_loadl_epi64((const __m128i*) input_a));
      const __m128i vb01234567 = _mm_cvtepi8_epi16(_mm_loadl_epi64((const __m128i*) input_b));
      input_a += 8;
      input_b += 8;


      __m128i vaprod01234567hi = _mm_mulhi_epu16(va01234567, va_multiplier_lo);
      __m128i vbprod01234567hi = _mm_mulhi_epu16(vb01234567, vb_multiplier_lo);
      const __m128i vaprod01234567lo = _mm_mullo_epi16(va01234567, va_multiplier_lo);
      const __m128i vbprod01234567lo = _mm_mullo_epi16(vb01234567, vb_multiplier_lo);

      vaprod01234567hi = _mm_add_epi16(vaprod01234567hi, _mm_mullo_epi16(va01234567, va_multiplier_hi));
      vbprod01234567hi = _mm_add_epi16(vbprod01234567hi, _mm_mullo_epi16(vb01234567, vb_multiplier_hi));

      vaprod01234567hi = _mm_sub_epi16(vaprod01234567hi, _mm_and_si128(_mm_srai_epi16(va01234567, 15), va_multiplier_lo));
      vbprod01234567hi = _mm_sub_epi16(vbprod01234567hi, _mm_and_si128(_mm_srai_epi16(vb01234567, 15), vb_multiplier_lo));

      __m128i vacc0123 = _mm_add_epi32(vbias, _mm_unpacklo_epi16(vaprod01234567lo, vaprod01234567hi));
      __m128i vacc4567 = _mm_add_epi32(vbias, _mm_unpackhi_epi16(vaprod01234567lo, vaprod01234567hi));

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vbprod01234567lo, vbprod01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vbprod01234567lo, vbprod01234567hi));

      vacc0123 = _mm_sra_epi32(vacc0123, vshift);
      vacc4567 = _mm_sra_epi32(vacc4567, vshift);

      __m128i vout01234567 = _mm_adds_epi16(_mm_packs_epi32(vacc0123, vacc4567), voutput_zero_point);

      __m128i vout0123456701234567 = _mm_packs_epi16(vout01234567, vout01234567);
      vout0123456701234567 = _mm_max_epi8(vout0123456701234567, voutput_min);
      vout0123456701234567 = _mm_min_epi8(vout0123456701234567, voutput_max);

      if XNN_LIKELY(n >= (8 * sizeof(int8_t))) {
        _mm_storel_epi64((__m128i*) output, vout0123456701234567);
        output += 8;
        n -= 8 * sizeof(int8_t);
      } else {
        if (n & (4 * sizeof(int8_t))) {
          *((uint32_t*) output) = (uint32_t) _mm_cvtsi128_si32(vout0123456701234567);
          vout0123456701234567 = _mm_srli_epi64(vout0123456701234567, 32);
          output += 4;
        }
        if (n & (2 * sizeof(int8_t))) {
          *((uint16_t*) output) = (uint16_t) _mm_extract_epi16(vout0123456701234567, 0);
          vout0123456701234567 = _mm_srli_epi32(vout0123456701234567, 16);
          output += 2;
        }
        if (n & (1 * sizeof(int8_t))) {
          *output = (int8_t) _mm_extract_epi8(vout0123456701234567, 0);
        }
        n = 0;
      }
    } while (n != 0);
  }
}
