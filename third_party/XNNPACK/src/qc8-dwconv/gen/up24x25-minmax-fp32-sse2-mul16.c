// Auto-generated file. Do not edit!
//   Template: src/qs8-dwconv/unipass-sse-mul16.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <emmintrin.h>

#include <xnnpack/dwconv.h>


void xnn_qc8_dwconv_minmax_fp32_ukernel_up24x25__sse2_mul16(
    size_t channels,
    size_t output_width,
    const int8_t** input,
    const void* weights,
    int8_t* output,
    size_t input_stride,
    size_t output_increment,
    size_t input_offset,
    const int8_t* zero,
    const union xnn_qs8_minmax_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(channels != 0);
  assert(output_width != 0);

  do {
    const int8_t* i0 = input[0];
    assert(i0 != NULL);
    if XNN_UNPREDICTABLE(i0 != zero) {
      i0 = (const int8_t*) ((uintptr_t) i0 + input_offset);
    }
    const int8_t* i1 = input[1];
    assert(i1 != NULL);
    if XNN_UNPREDICTABLE(i1 != zero) {
      i1 = (const int8_t*) ((uintptr_t) i1 + input_offset);
    }
    const int8_t* i2 = input[2];
    assert(i2 != NULL);
    if XNN_UNPREDICTABLE(i2 != zero) {
      i2 = (const int8_t*) ((uintptr_t) i2 + input_offset);
    }
    const int8_t* i3 = input[3];
    assert(i3 != NULL);
    if XNN_UNPREDICTABLE(i3 != zero) {
      i3 = (const int8_t*) ((uintptr_t) i3 + input_offset);
    }
    const int8_t* i4 = input[4];
    assert(i4 != NULL);
    if XNN_UNPREDICTABLE(i4 != zero) {
      i4 = (const int8_t*) ((uintptr_t) i4 + input_offset);
    }
    const int8_t* i5 = input[5];
    assert(i5 != NULL);
    if XNN_UNPREDICTABLE(i5 != zero) {
      i5 = (const int8_t*) ((uintptr_t) i5 + input_offset);
    }
    const int8_t* i6 = input[6];
    assert(i6 != NULL);
    if XNN_UNPREDICTABLE(i6 != zero) {
      i6 = (const int8_t*) ((uintptr_t) i6 + input_offset);
    }
    const int8_t* i7 = input[7];
    assert(i7 != NULL);
    if XNN_UNPREDICTABLE(i7 != zero) {
      i7 = (const int8_t*) ((uintptr_t) i7 + input_offset);
    }
    const int8_t* i8 = input[8];
    assert(i8 != NULL);
    if XNN_UNPREDICTABLE(i8 != zero) {
      i8 = (const int8_t*) ((uintptr_t) i8 + input_offset);
    }
    const int8_t* i9 = input[9];
    assert(i9 != NULL);
    if XNN_UNPREDICTABLE(i9 != zero) {
      i9 = (const int8_t*) ((uintptr_t) i9 + input_offset);
    }
    const int8_t* i10 = input[10];
    assert(i10 != NULL);
    if XNN_UNPREDICTABLE(i10 != zero) {
      i10 = (const int8_t*) ((uintptr_t) i10 + input_offset);
    }
    const int8_t* i11 = input[11];
    assert(i11 != NULL);
    if XNN_UNPREDICTABLE(i11 != zero) {
      i11 = (const int8_t*) ((uintptr_t) i11 + input_offset);
    }
    const int8_t* i12 = input[12];
    assert(i12 != NULL);
    if XNN_UNPREDICTABLE(i12 != zero) {
      i12 = (const int8_t*) ((uintptr_t) i12 + input_offset);
    }
    const int8_t* i13 = input[13];
    assert(i13 != NULL);
    if XNN_UNPREDICTABLE(i13 != zero) {
      i13 = (const int8_t*) ((uintptr_t) i13 + input_offset);
    }
    const int8_t* i14 = input[14];
    assert(i14 != NULL);
    if XNN_UNPREDICTABLE(i14 != zero) {
      i14 = (const int8_t*) ((uintptr_t) i14 + input_offset);
    }
    const int8_t* i15 = input[15];
    assert(i15 != NULL);
    if XNN_UNPREDICTABLE(i15 != zero) {
      i15 = (const int8_t*) ((uintptr_t) i15 + input_offset);
    }
    const int8_t* i16 = input[16];
    assert(i16 != NULL);
    if XNN_UNPREDICTABLE(i16 != zero) {
      i16 = (const int8_t*) ((uintptr_t) i16 + input_offset);
    }
    const int8_t* i17 = input[17];
    assert(i17 != NULL);
    if XNN_UNPREDICTABLE(i17 != zero) {
      i17 = (const int8_t*) ((uintptr_t) i17 + input_offset);
    }
    const int8_t* i18 = input[18];
    assert(i18 != NULL);
    if XNN_UNPREDICTABLE(i18 != zero) {
      i18 = (const int8_t*) ((uintptr_t) i18 + input_offset);
    }
    const int8_t* i19 = input[19];
    assert(i19 != NULL);
    if XNN_UNPREDICTABLE(i19 != zero) {
      i19 = (const int8_t*) ((uintptr_t) i19 + input_offset);
    }
    const int8_t* i20 = input[20];
    assert(i20 != NULL);
    if XNN_UNPREDICTABLE(i20 != zero) {
      i20 = (const int8_t*) ((uintptr_t) i20 + input_offset);
    }
    const int8_t* i21 = input[21];
    assert(i21 != NULL);
    if XNN_UNPREDICTABLE(i21 != zero) {
      i21 = (const int8_t*) ((uintptr_t) i21 + input_offset);
    }
    const int8_t* i22 = input[22];
    assert(i22 != NULL);
    if XNN_UNPREDICTABLE(i22 != zero) {
      i22 = (const int8_t*) ((uintptr_t) i22 + input_offset);
    }
    const int8_t* i23 = input[23];
    assert(i23 != NULL);
    if XNN_UNPREDICTABLE(i23 != zero) {
      i23 = (const int8_t*) ((uintptr_t) i23 + input_offset);
    }
    const int8_t* i24 = input[24];
    assert(i24 != NULL);
    if XNN_UNPREDICTABLE(i24 != zero) {
      i24 = (const int8_t*) ((uintptr_t) i24 + input_offset);
    }
    input = (const int8_t**) ((uintptr_t) input + input_stride);

    size_t c = channels;
    const void* w = weights;
    for (; c >= 24; c -= 24) {
      __m128i vacc0123 = _mm_loadu_si128((const __m128i*) w);
      __m128i vacc4567 = _mm_loadu_si128((const __m128i*) ((const int32_t*) w + 4));
      __m128i vacc89AB = _mm_loadu_si128((const __m128i*) ((const int32_t*) w + 8));
      __m128i vaccCDEF = _mm_loadu_si128((const __m128i*) ((const int32_t*) w + 12));
      __m128i vaccGHIJ = _mm_loadu_si128((const __m128i*) ((const int32_t*) w + 16));
      __m128i vaccKLMN = _mm_loadu_si128((const __m128i*) ((const int32_t*) w + 20));


      const __m128i vi0x01234567 = _mm_loadl_epi64((const __m128i*) i0);
      const __m128i vk0x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 0 * sizeof(int8_t)));
      const __m128i vi0x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i0 + 8));
      const __m128i vk0x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 8 * sizeof(int8_t)));
      const __m128i vi0xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i0 + 16));
      const __m128i vk0xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 16 * sizeof(int8_t)));
      i0 += 24;

      const __m128i vxi0x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi0x01234567, vi0x01234567), 8);
      const __m128i vxk0x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk0x01234567, vk0x01234567), 8);
      const __m128i vxi0x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi0x89ABCDEF, vi0x89ABCDEF), 8);
      const __m128i vxk0x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk0x89ABCDEF, vk0x89ABCDEF), 8);
      const __m128i vxi0xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi0xGHIJKLMN, vi0xGHIJKLMN), 8);
      const __m128i vxk0xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk0xGHIJKLMN, vk0xGHIJKLMN), 8);

      const __m128i vprod0x01234567lo = _mm_mullo_epi16(vxi0x01234567, vxk0x01234567);
      const __m128i vprod0x01234567hi = _mm_mulhi_epi16(vxi0x01234567, vxk0x01234567);
      const __m128i vprod0x89ABCDEFlo = _mm_mullo_epi16(vxi0x89ABCDEF, vxk0x89ABCDEF);
      const __m128i vprod0x89ABCDEFhi = _mm_mulhi_epi16(vxi0x89ABCDEF, vxk0x89ABCDEF);
      const __m128i vprod0xGHIJKLMNlo = _mm_mullo_epi16(vxi0xGHIJKLMN, vxk0xGHIJKLMN);
      const __m128i vprod0xGHIJKLMNhi = _mm_mulhi_epi16(vxi0xGHIJKLMN, vxk0xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod0x01234567lo, vprod0x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod0x01234567lo, vprod0x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod0x89ABCDEFlo, vprod0x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod0x89ABCDEFlo, vprod0x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod0xGHIJKLMNlo, vprod0xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod0xGHIJKLMNlo, vprod0xGHIJKLMNhi));

      const __m128i vi1x01234567 = _mm_loadl_epi64((const __m128i*) i1);
      const __m128i vk1x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 24 * sizeof(int8_t)));
      const __m128i vi1x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i1 + 8));
      const __m128i vk1x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 32 * sizeof(int8_t)));
      const __m128i vi1xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i1 + 16));
      const __m128i vk1xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 40 * sizeof(int8_t)));
      i1 += 24;

      const __m128i vxi1x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi1x01234567, vi1x01234567), 8);
      const __m128i vxk1x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk1x01234567, vk1x01234567), 8);
      const __m128i vxi1x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi1x89ABCDEF, vi1x89ABCDEF), 8);
      const __m128i vxk1x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk1x89ABCDEF, vk1x89ABCDEF), 8);
      const __m128i vxi1xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi1xGHIJKLMN, vi1xGHIJKLMN), 8);
      const __m128i vxk1xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk1xGHIJKLMN, vk1xGHIJKLMN), 8);

      const __m128i vprod1x01234567lo = _mm_mullo_epi16(vxi1x01234567, vxk1x01234567);
      const __m128i vprod1x01234567hi = _mm_mulhi_epi16(vxi1x01234567, vxk1x01234567);
      const __m128i vprod1x89ABCDEFlo = _mm_mullo_epi16(vxi1x89ABCDEF, vxk1x89ABCDEF);
      const __m128i vprod1x89ABCDEFhi = _mm_mulhi_epi16(vxi1x89ABCDEF, vxk1x89ABCDEF);
      const __m128i vprod1xGHIJKLMNlo = _mm_mullo_epi16(vxi1xGHIJKLMN, vxk1xGHIJKLMN);
      const __m128i vprod1xGHIJKLMNhi = _mm_mulhi_epi16(vxi1xGHIJKLMN, vxk1xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod1x01234567lo, vprod1x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod1x01234567lo, vprod1x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod1x89ABCDEFlo, vprod1x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod1x89ABCDEFlo, vprod1x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod1xGHIJKLMNlo, vprod1xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod1xGHIJKLMNlo, vprod1xGHIJKLMNhi));

      const __m128i vi2x01234567 = _mm_loadl_epi64((const __m128i*) i2);
      const __m128i vk2x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 48 * sizeof(int8_t)));
      const __m128i vi2x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i2 + 8));
      const __m128i vk2x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 56 * sizeof(int8_t)));
      const __m128i vi2xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i2 + 16));
      const __m128i vk2xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 64 * sizeof(int8_t)));
      i2 += 24;

      const __m128i vxi2x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi2x01234567, vi2x01234567), 8);
      const __m128i vxk2x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk2x01234567, vk2x01234567), 8);
      const __m128i vxi2x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi2x89ABCDEF, vi2x89ABCDEF), 8);
      const __m128i vxk2x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk2x89ABCDEF, vk2x89ABCDEF), 8);
      const __m128i vxi2xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi2xGHIJKLMN, vi2xGHIJKLMN), 8);
      const __m128i vxk2xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk2xGHIJKLMN, vk2xGHIJKLMN), 8);

      const __m128i vprod2x01234567lo = _mm_mullo_epi16(vxi2x01234567, vxk2x01234567);
      const __m128i vprod2x01234567hi = _mm_mulhi_epi16(vxi2x01234567, vxk2x01234567);
      const __m128i vprod2x89ABCDEFlo = _mm_mullo_epi16(vxi2x89ABCDEF, vxk2x89ABCDEF);
      const __m128i vprod2x89ABCDEFhi = _mm_mulhi_epi16(vxi2x89ABCDEF, vxk2x89ABCDEF);
      const __m128i vprod2xGHIJKLMNlo = _mm_mullo_epi16(vxi2xGHIJKLMN, vxk2xGHIJKLMN);
      const __m128i vprod2xGHIJKLMNhi = _mm_mulhi_epi16(vxi2xGHIJKLMN, vxk2xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod2x01234567lo, vprod2x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod2x01234567lo, vprod2x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod2x89ABCDEFlo, vprod2x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod2x89ABCDEFlo, vprod2x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod2xGHIJKLMNlo, vprod2xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod2xGHIJKLMNlo, vprod2xGHIJKLMNhi));

      const __m128i vi3x01234567 = _mm_loadl_epi64((const __m128i*) i3);
      const __m128i vk3x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 72 * sizeof(int8_t)));
      const __m128i vi3x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i3 + 8));
      const __m128i vk3x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 80 * sizeof(int8_t)));
      const __m128i vi3xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i3 + 16));
      const __m128i vk3xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 88 * sizeof(int8_t)));
      i3 += 24;

      const __m128i vxi3x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi3x01234567, vi3x01234567), 8);
      const __m128i vxk3x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk3x01234567, vk3x01234567), 8);
      const __m128i vxi3x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi3x89ABCDEF, vi3x89ABCDEF), 8);
      const __m128i vxk3x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk3x89ABCDEF, vk3x89ABCDEF), 8);
      const __m128i vxi3xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi3xGHIJKLMN, vi3xGHIJKLMN), 8);
      const __m128i vxk3xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk3xGHIJKLMN, vk3xGHIJKLMN), 8);

      const __m128i vprod3x01234567lo = _mm_mullo_epi16(vxi3x01234567, vxk3x01234567);
      const __m128i vprod3x01234567hi = _mm_mulhi_epi16(vxi3x01234567, vxk3x01234567);
      const __m128i vprod3x89ABCDEFlo = _mm_mullo_epi16(vxi3x89ABCDEF, vxk3x89ABCDEF);
      const __m128i vprod3x89ABCDEFhi = _mm_mulhi_epi16(vxi3x89ABCDEF, vxk3x89ABCDEF);
      const __m128i vprod3xGHIJKLMNlo = _mm_mullo_epi16(vxi3xGHIJKLMN, vxk3xGHIJKLMN);
      const __m128i vprod3xGHIJKLMNhi = _mm_mulhi_epi16(vxi3xGHIJKLMN, vxk3xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod3x01234567lo, vprod3x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod3x01234567lo, vprod3x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod3x89ABCDEFlo, vprod3x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod3x89ABCDEFlo, vprod3x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod3xGHIJKLMNlo, vprod3xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod3xGHIJKLMNlo, vprod3xGHIJKLMNhi));

      const __m128i vi4x01234567 = _mm_loadl_epi64((const __m128i*) i4);
      const __m128i vk4x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 96 * sizeof(int8_t)));
      const __m128i vi4x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i4 + 8));
      const __m128i vk4x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 104 * sizeof(int8_t)));
      const __m128i vi4xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i4 + 16));
      const __m128i vk4xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 112 * sizeof(int8_t)));
      i4 += 24;

      const __m128i vxi4x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi4x01234567, vi4x01234567), 8);
      const __m128i vxk4x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk4x01234567, vk4x01234567), 8);
      const __m128i vxi4x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi4x89ABCDEF, vi4x89ABCDEF), 8);
      const __m128i vxk4x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk4x89ABCDEF, vk4x89ABCDEF), 8);
      const __m128i vxi4xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi4xGHIJKLMN, vi4xGHIJKLMN), 8);
      const __m128i vxk4xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk4xGHIJKLMN, vk4xGHIJKLMN), 8);

      const __m128i vprod4x01234567lo = _mm_mullo_epi16(vxi4x01234567, vxk4x01234567);
      const __m128i vprod4x01234567hi = _mm_mulhi_epi16(vxi4x01234567, vxk4x01234567);
      const __m128i vprod4x89ABCDEFlo = _mm_mullo_epi16(vxi4x89ABCDEF, vxk4x89ABCDEF);
      const __m128i vprod4x89ABCDEFhi = _mm_mulhi_epi16(vxi4x89ABCDEF, vxk4x89ABCDEF);
      const __m128i vprod4xGHIJKLMNlo = _mm_mullo_epi16(vxi4xGHIJKLMN, vxk4xGHIJKLMN);
      const __m128i vprod4xGHIJKLMNhi = _mm_mulhi_epi16(vxi4xGHIJKLMN, vxk4xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod4x01234567lo, vprod4x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod4x01234567lo, vprod4x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod4x89ABCDEFlo, vprod4x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod4x89ABCDEFlo, vprod4x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod4xGHIJKLMNlo, vprod4xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod4xGHIJKLMNlo, vprod4xGHIJKLMNhi));

      const __m128i vi5x01234567 = _mm_loadl_epi64((const __m128i*) i5);
      const __m128i vk5x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 120 * sizeof(int8_t)));
      const __m128i vi5x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i5 + 8));
      const __m128i vk5x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 128 * sizeof(int8_t)));
      const __m128i vi5xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i5 + 16));
      const __m128i vk5xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 136 * sizeof(int8_t)));
      i5 += 24;

      const __m128i vxi5x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi5x01234567, vi5x01234567), 8);
      const __m128i vxk5x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk5x01234567, vk5x01234567), 8);
      const __m128i vxi5x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi5x89ABCDEF, vi5x89ABCDEF), 8);
      const __m128i vxk5x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk5x89ABCDEF, vk5x89ABCDEF), 8);
      const __m128i vxi5xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi5xGHIJKLMN, vi5xGHIJKLMN), 8);
      const __m128i vxk5xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk5xGHIJKLMN, vk5xGHIJKLMN), 8);

      const __m128i vprod5x01234567lo = _mm_mullo_epi16(vxi5x01234567, vxk5x01234567);
      const __m128i vprod5x01234567hi = _mm_mulhi_epi16(vxi5x01234567, vxk5x01234567);
      const __m128i vprod5x89ABCDEFlo = _mm_mullo_epi16(vxi5x89ABCDEF, vxk5x89ABCDEF);
      const __m128i vprod5x89ABCDEFhi = _mm_mulhi_epi16(vxi5x89ABCDEF, vxk5x89ABCDEF);
      const __m128i vprod5xGHIJKLMNlo = _mm_mullo_epi16(vxi5xGHIJKLMN, vxk5xGHIJKLMN);
      const __m128i vprod5xGHIJKLMNhi = _mm_mulhi_epi16(vxi5xGHIJKLMN, vxk5xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod5x01234567lo, vprod5x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod5x01234567lo, vprod5x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod5x89ABCDEFlo, vprod5x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod5x89ABCDEFlo, vprod5x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod5xGHIJKLMNlo, vprod5xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod5xGHIJKLMNlo, vprod5xGHIJKLMNhi));

      const __m128i vi6x01234567 = _mm_loadl_epi64((const __m128i*) i6);
      const __m128i vk6x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 144 * sizeof(int8_t)));
      const __m128i vi6x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i6 + 8));
      const __m128i vk6x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 152 * sizeof(int8_t)));
      const __m128i vi6xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i6 + 16));
      const __m128i vk6xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 160 * sizeof(int8_t)));
      i6 += 24;

      const __m128i vxi6x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi6x01234567, vi6x01234567), 8);
      const __m128i vxk6x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk6x01234567, vk6x01234567), 8);
      const __m128i vxi6x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi6x89ABCDEF, vi6x89ABCDEF), 8);
      const __m128i vxk6x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk6x89ABCDEF, vk6x89ABCDEF), 8);
      const __m128i vxi6xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi6xGHIJKLMN, vi6xGHIJKLMN), 8);
      const __m128i vxk6xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk6xGHIJKLMN, vk6xGHIJKLMN), 8);

      const __m128i vprod6x01234567lo = _mm_mullo_epi16(vxi6x01234567, vxk6x01234567);
      const __m128i vprod6x01234567hi = _mm_mulhi_epi16(vxi6x01234567, vxk6x01234567);
      const __m128i vprod6x89ABCDEFlo = _mm_mullo_epi16(vxi6x89ABCDEF, vxk6x89ABCDEF);
      const __m128i vprod6x89ABCDEFhi = _mm_mulhi_epi16(vxi6x89ABCDEF, vxk6x89ABCDEF);
      const __m128i vprod6xGHIJKLMNlo = _mm_mullo_epi16(vxi6xGHIJKLMN, vxk6xGHIJKLMN);
      const __m128i vprod6xGHIJKLMNhi = _mm_mulhi_epi16(vxi6xGHIJKLMN, vxk6xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod6x01234567lo, vprod6x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod6x01234567lo, vprod6x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod6x89ABCDEFlo, vprod6x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod6x89ABCDEFlo, vprod6x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod6xGHIJKLMNlo, vprod6xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod6xGHIJKLMNlo, vprod6xGHIJKLMNhi));

      const __m128i vi7x01234567 = _mm_loadl_epi64((const __m128i*) i7);
      const __m128i vk7x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 168 * sizeof(int8_t)));
      const __m128i vi7x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i7 + 8));
      const __m128i vk7x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 176 * sizeof(int8_t)));
      const __m128i vi7xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i7 + 16));
      const __m128i vk7xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 184 * sizeof(int8_t)));
      i7 += 24;

      const __m128i vxi7x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi7x01234567, vi7x01234567), 8);
      const __m128i vxk7x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk7x01234567, vk7x01234567), 8);
      const __m128i vxi7x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi7x89ABCDEF, vi7x89ABCDEF), 8);
      const __m128i vxk7x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk7x89ABCDEF, vk7x89ABCDEF), 8);
      const __m128i vxi7xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi7xGHIJKLMN, vi7xGHIJKLMN), 8);
      const __m128i vxk7xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk7xGHIJKLMN, vk7xGHIJKLMN), 8);

      const __m128i vprod7x01234567lo = _mm_mullo_epi16(vxi7x01234567, vxk7x01234567);
      const __m128i vprod7x01234567hi = _mm_mulhi_epi16(vxi7x01234567, vxk7x01234567);
      const __m128i vprod7x89ABCDEFlo = _mm_mullo_epi16(vxi7x89ABCDEF, vxk7x89ABCDEF);
      const __m128i vprod7x89ABCDEFhi = _mm_mulhi_epi16(vxi7x89ABCDEF, vxk7x89ABCDEF);
      const __m128i vprod7xGHIJKLMNlo = _mm_mullo_epi16(vxi7xGHIJKLMN, vxk7xGHIJKLMN);
      const __m128i vprod7xGHIJKLMNhi = _mm_mulhi_epi16(vxi7xGHIJKLMN, vxk7xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod7x01234567lo, vprod7x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod7x01234567lo, vprod7x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod7x89ABCDEFlo, vprod7x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod7x89ABCDEFlo, vprod7x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod7xGHIJKLMNlo, vprod7xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod7xGHIJKLMNlo, vprod7xGHIJKLMNhi));

      const __m128i vi8x01234567 = _mm_loadl_epi64((const __m128i*) i8);
      const __m128i vk8x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 192 * sizeof(int8_t)));
      const __m128i vi8x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i8 + 8));
      const __m128i vk8x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 200 * sizeof(int8_t)));
      const __m128i vi8xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i8 + 16));
      const __m128i vk8xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 208 * sizeof(int8_t)));
      i8 += 24;

      const __m128i vxi8x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi8x01234567, vi8x01234567), 8);
      const __m128i vxk8x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk8x01234567, vk8x01234567), 8);
      const __m128i vxi8x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi8x89ABCDEF, vi8x89ABCDEF), 8);
      const __m128i vxk8x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk8x89ABCDEF, vk8x89ABCDEF), 8);
      const __m128i vxi8xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi8xGHIJKLMN, vi8xGHIJKLMN), 8);
      const __m128i vxk8xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk8xGHIJKLMN, vk8xGHIJKLMN), 8);

      const __m128i vprod8x01234567lo = _mm_mullo_epi16(vxi8x01234567, vxk8x01234567);
      const __m128i vprod8x01234567hi = _mm_mulhi_epi16(vxi8x01234567, vxk8x01234567);
      const __m128i vprod8x89ABCDEFlo = _mm_mullo_epi16(vxi8x89ABCDEF, vxk8x89ABCDEF);
      const __m128i vprod8x89ABCDEFhi = _mm_mulhi_epi16(vxi8x89ABCDEF, vxk8x89ABCDEF);
      const __m128i vprod8xGHIJKLMNlo = _mm_mullo_epi16(vxi8xGHIJKLMN, vxk8xGHIJKLMN);
      const __m128i vprod8xGHIJKLMNhi = _mm_mulhi_epi16(vxi8xGHIJKLMN, vxk8xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod8x01234567lo, vprod8x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod8x01234567lo, vprod8x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod8x89ABCDEFlo, vprod8x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod8x89ABCDEFlo, vprod8x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod8xGHIJKLMNlo, vprod8xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod8xGHIJKLMNlo, vprod8xGHIJKLMNhi));

      const __m128i vi9x01234567 = _mm_loadl_epi64((const __m128i*) i9);
      const __m128i vk9x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 216 * sizeof(int8_t)));
      const __m128i vi9x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i9 + 8));
      const __m128i vk9x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 224 * sizeof(int8_t)));
      const __m128i vi9xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i9 + 16));
      const __m128i vk9xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 232 * sizeof(int8_t)));
      i9 += 24;

      const __m128i vxi9x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi9x01234567, vi9x01234567), 8);
      const __m128i vxk9x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk9x01234567, vk9x01234567), 8);
      const __m128i vxi9x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi9x89ABCDEF, vi9x89ABCDEF), 8);
      const __m128i vxk9x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk9x89ABCDEF, vk9x89ABCDEF), 8);
      const __m128i vxi9xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi9xGHIJKLMN, vi9xGHIJKLMN), 8);
      const __m128i vxk9xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk9xGHIJKLMN, vk9xGHIJKLMN), 8);

      const __m128i vprod9x01234567lo = _mm_mullo_epi16(vxi9x01234567, vxk9x01234567);
      const __m128i vprod9x01234567hi = _mm_mulhi_epi16(vxi9x01234567, vxk9x01234567);
      const __m128i vprod9x89ABCDEFlo = _mm_mullo_epi16(vxi9x89ABCDEF, vxk9x89ABCDEF);
      const __m128i vprod9x89ABCDEFhi = _mm_mulhi_epi16(vxi9x89ABCDEF, vxk9x89ABCDEF);
      const __m128i vprod9xGHIJKLMNlo = _mm_mullo_epi16(vxi9xGHIJKLMN, vxk9xGHIJKLMN);
      const __m128i vprod9xGHIJKLMNhi = _mm_mulhi_epi16(vxi9xGHIJKLMN, vxk9xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod9x01234567lo, vprod9x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod9x01234567lo, vprod9x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod9x89ABCDEFlo, vprod9x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod9x89ABCDEFlo, vprod9x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod9xGHIJKLMNlo, vprod9xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod9xGHIJKLMNlo, vprod9xGHIJKLMNhi));

      const __m128i vi10x01234567 = _mm_loadl_epi64((const __m128i*) i10);
      const __m128i vk10x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 240 * sizeof(int8_t)));
      const __m128i vi10x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i10 + 8));
      const __m128i vk10x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 248 * sizeof(int8_t)));
      const __m128i vi10xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i10 + 16));
      const __m128i vk10xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 256 * sizeof(int8_t)));
      i10 += 24;

      const __m128i vxi10x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi10x01234567, vi10x01234567), 8);
      const __m128i vxk10x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk10x01234567, vk10x01234567), 8);
      const __m128i vxi10x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi10x89ABCDEF, vi10x89ABCDEF), 8);
      const __m128i vxk10x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk10x89ABCDEF, vk10x89ABCDEF), 8);
      const __m128i vxi10xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi10xGHIJKLMN, vi10xGHIJKLMN), 8);
      const __m128i vxk10xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk10xGHIJKLMN, vk10xGHIJKLMN), 8);

      const __m128i vprod10x01234567lo = _mm_mullo_epi16(vxi10x01234567, vxk10x01234567);
      const __m128i vprod10x01234567hi = _mm_mulhi_epi16(vxi10x01234567, vxk10x01234567);
      const __m128i vprod10x89ABCDEFlo = _mm_mullo_epi16(vxi10x89ABCDEF, vxk10x89ABCDEF);
      const __m128i vprod10x89ABCDEFhi = _mm_mulhi_epi16(vxi10x89ABCDEF, vxk10x89ABCDEF);
      const __m128i vprod10xGHIJKLMNlo = _mm_mullo_epi16(vxi10xGHIJKLMN, vxk10xGHIJKLMN);
      const __m128i vprod10xGHIJKLMNhi = _mm_mulhi_epi16(vxi10xGHIJKLMN, vxk10xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod10x01234567lo, vprod10x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod10x01234567lo, vprod10x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod10x89ABCDEFlo, vprod10x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod10x89ABCDEFlo, vprod10x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod10xGHIJKLMNlo, vprod10xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod10xGHIJKLMNlo, vprod10xGHIJKLMNhi));

      const __m128i vi11x01234567 = _mm_loadl_epi64((const __m128i*) i11);
      const __m128i vk11x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 264 * sizeof(int8_t)));
      const __m128i vi11x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i11 + 8));
      const __m128i vk11x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 272 * sizeof(int8_t)));
      const __m128i vi11xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i11 + 16));
      const __m128i vk11xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 280 * sizeof(int8_t)));
      i11 += 24;

      const __m128i vxi11x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi11x01234567, vi11x01234567), 8);
      const __m128i vxk11x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk11x01234567, vk11x01234567), 8);
      const __m128i vxi11x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi11x89ABCDEF, vi11x89ABCDEF), 8);
      const __m128i vxk11x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk11x89ABCDEF, vk11x89ABCDEF), 8);
      const __m128i vxi11xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi11xGHIJKLMN, vi11xGHIJKLMN), 8);
      const __m128i vxk11xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk11xGHIJKLMN, vk11xGHIJKLMN), 8);

      const __m128i vprod11x01234567lo = _mm_mullo_epi16(vxi11x01234567, vxk11x01234567);
      const __m128i vprod11x01234567hi = _mm_mulhi_epi16(vxi11x01234567, vxk11x01234567);
      const __m128i vprod11x89ABCDEFlo = _mm_mullo_epi16(vxi11x89ABCDEF, vxk11x89ABCDEF);
      const __m128i vprod11x89ABCDEFhi = _mm_mulhi_epi16(vxi11x89ABCDEF, vxk11x89ABCDEF);
      const __m128i vprod11xGHIJKLMNlo = _mm_mullo_epi16(vxi11xGHIJKLMN, vxk11xGHIJKLMN);
      const __m128i vprod11xGHIJKLMNhi = _mm_mulhi_epi16(vxi11xGHIJKLMN, vxk11xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod11x01234567lo, vprod11x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod11x01234567lo, vprod11x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod11x89ABCDEFlo, vprod11x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod11x89ABCDEFlo, vprod11x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod11xGHIJKLMNlo, vprod11xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod11xGHIJKLMNlo, vprod11xGHIJKLMNhi));

      const __m128i vi12x01234567 = _mm_loadl_epi64((const __m128i*) i12);
      const __m128i vk12x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 288 * sizeof(int8_t)));
      const __m128i vi12x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i12 + 8));
      const __m128i vk12x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 296 * sizeof(int8_t)));
      const __m128i vi12xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i12 + 16));
      const __m128i vk12xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 304 * sizeof(int8_t)));
      i12 += 24;

      const __m128i vxi12x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi12x01234567, vi12x01234567), 8);
      const __m128i vxk12x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk12x01234567, vk12x01234567), 8);
      const __m128i vxi12x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi12x89ABCDEF, vi12x89ABCDEF), 8);
      const __m128i vxk12x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk12x89ABCDEF, vk12x89ABCDEF), 8);
      const __m128i vxi12xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi12xGHIJKLMN, vi12xGHIJKLMN), 8);
      const __m128i vxk12xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk12xGHIJKLMN, vk12xGHIJKLMN), 8);

      const __m128i vprod12x01234567lo = _mm_mullo_epi16(vxi12x01234567, vxk12x01234567);
      const __m128i vprod12x01234567hi = _mm_mulhi_epi16(vxi12x01234567, vxk12x01234567);
      const __m128i vprod12x89ABCDEFlo = _mm_mullo_epi16(vxi12x89ABCDEF, vxk12x89ABCDEF);
      const __m128i vprod12x89ABCDEFhi = _mm_mulhi_epi16(vxi12x89ABCDEF, vxk12x89ABCDEF);
      const __m128i vprod12xGHIJKLMNlo = _mm_mullo_epi16(vxi12xGHIJKLMN, vxk12xGHIJKLMN);
      const __m128i vprod12xGHIJKLMNhi = _mm_mulhi_epi16(vxi12xGHIJKLMN, vxk12xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod12x01234567lo, vprod12x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod12x01234567lo, vprod12x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod12x89ABCDEFlo, vprod12x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod12x89ABCDEFlo, vprod12x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod12xGHIJKLMNlo, vprod12xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod12xGHIJKLMNlo, vprod12xGHIJKLMNhi));

      const __m128i vi13x01234567 = _mm_loadl_epi64((const __m128i*) i13);
      const __m128i vk13x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 312 * sizeof(int8_t)));
      const __m128i vi13x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i13 + 8));
      const __m128i vk13x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 320 * sizeof(int8_t)));
      const __m128i vi13xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i13 + 16));
      const __m128i vk13xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 328 * sizeof(int8_t)));
      i13 += 24;

      const __m128i vxi13x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi13x01234567, vi13x01234567), 8);
      const __m128i vxk13x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk13x01234567, vk13x01234567), 8);
      const __m128i vxi13x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi13x89ABCDEF, vi13x89ABCDEF), 8);
      const __m128i vxk13x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk13x89ABCDEF, vk13x89ABCDEF), 8);
      const __m128i vxi13xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi13xGHIJKLMN, vi13xGHIJKLMN), 8);
      const __m128i vxk13xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk13xGHIJKLMN, vk13xGHIJKLMN), 8);

      const __m128i vprod13x01234567lo = _mm_mullo_epi16(vxi13x01234567, vxk13x01234567);
      const __m128i vprod13x01234567hi = _mm_mulhi_epi16(vxi13x01234567, vxk13x01234567);
      const __m128i vprod13x89ABCDEFlo = _mm_mullo_epi16(vxi13x89ABCDEF, vxk13x89ABCDEF);
      const __m128i vprod13x89ABCDEFhi = _mm_mulhi_epi16(vxi13x89ABCDEF, vxk13x89ABCDEF);
      const __m128i vprod13xGHIJKLMNlo = _mm_mullo_epi16(vxi13xGHIJKLMN, vxk13xGHIJKLMN);
      const __m128i vprod13xGHIJKLMNhi = _mm_mulhi_epi16(vxi13xGHIJKLMN, vxk13xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod13x01234567lo, vprod13x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod13x01234567lo, vprod13x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod13x89ABCDEFlo, vprod13x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod13x89ABCDEFlo, vprod13x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod13xGHIJKLMNlo, vprod13xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod13xGHIJKLMNlo, vprod13xGHIJKLMNhi));

      const __m128i vi14x01234567 = _mm_loadl_epi64((const __m128i*) i14);
      const __m128i vk14x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 336 * sizeof(int8_t)));
      const __m128i vi14x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i14 + 8));
      const __m128i vk14x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 344 * sizeof(int8_t)));
      const __m128i vi14xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i14 + 16));
      const __m128i vk14xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 352 * sizeof(int8_t)));
      i14 += 24;

      const __m128i vxi14x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi14x01234567, vi14x01234567), 8);
      const __m128i vxk14x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk14x01234567, vk14x01234567), 8);
      const __m128i vxi14x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi14x89ABCDEF, vi14x89ABCDEF), 8);
      const __m128i vxk14x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk14x89ABCDEF, vk14x89ABCDEF), 8);
      const __m128i vxi14xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi14xGHIJKLMN, vi14xGHIJKLMN), 8);
      const __m128i vxk14xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk14xGHIJKLMN, vk14xGHIJKLMN), 8);

      const __m128i vprod14x01234567lo = _mm_mullo_epi16(vxi14x01234567, vxk14x01234567);
      const __m128i vprod14x01234567hi = _mm_mulhi_epi16(vxi14x01234567, vxk14x01234567);
      const __m128i vprod14x89ABCDEFlo = _mm_mullo_epi16(vxi14x89ABCDEF, vxk14x89ABCDEF);
      const __m128i vprod14x89ABCDEFhi = _mm_mulhi_epi16(vxi14x89ABCDEF, vxk14x89ABCDEF);
      const __m128i vprod14xGHIJKLMNlo = _mm_mullo_epi16(vxi14xGHIJKLMN, vxk14xGHIJKLMN);
      const __m128i vprod14xGHIJKLMNhi = _mm_mulhi_epi16(vxi14xGHIJKLMN, vxk14xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod14x01234567lo, vprod14x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod14x01234567lo, vprod14x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod14x89ABCDEFlo, vprod14x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod14x89ABCDEFlo, vprod14x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod14xGHIJKLMNlo, vprod14xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod14xGHIJKLMNlo, vprod14xGHIJKLMNhi));

      const __m128i vi15x01234567 = _mm_loadl_epi64((const __m128i*) i15);
      const __m128i vk15x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 360 * sizeof(int8_t)));
      const __m128i vi15x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i15 + 8));
      const __m128i vk15x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 368 * sizeof(int8_t)));
      const __m128i vi15xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i15 + 16));
      const __m128i vk15xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 376 * sizeof(int8_t)));
      i15 += 24;

      const __m128i vxi15x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi15x01234567, vi15x01234567), 8);
      const __m128i vxk15x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk15x01234567, vk15x01234567), 8);
      const __m128i vxi15x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi15x89ABCDEF, vi15x89ABCDEF), 8);
      const __m128i vxk15x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk15x89ABCDEF, vk15x89ABCDEF), 8);
      const __m128i vxi15xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi15xGHIJKLMN, vi15xGHIJKLMN), 8);
      const __m128i vxk15xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk15xGHIJKLMN, vk15xGHIJKLMN), 8);

      const __m128i vprod15x01234567lo = _mm_mullo_epi16(vxi15x01234567, vxk15x01234567);
      const __m128i vprod15x01234567hi = _mm_mulhi_epi16(vxi15x01234567, vxk15x01234567);
      const __m128i vprod15x89ABCDEFlo = _mm_mullo_epi16(vxi15x89ABCDEF, vxk15x89ABCDEF);
      const __m128i vprod15x89ABCDEFhi = _mm_mulhi_epi16(vxi15x89ABCDEF, vxk15x89ABCDEF);
      const __m128i vprod15xGHIJKLMNlo = _mm_mullo_epi16(vxi15xGHIJKLMN, vxk15xGHIJKLMN);
      const __m128i vprod15xGHIJKLMNhi = _mm_mulhi_epi16(vxi15xGHIJKLMN, vxk15xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod15x01234567lo, vprod15x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod15x01234567lo, vprod15x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod15x89ABCDEFlo, vprod15x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod15x89ABCDEFlo, vprod15x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod15xGHIJKLMNlo, vprod15xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod15xGHIJKLMNlo, vprod15xGHIJKLMNhi));

      const __m128i vi16x01234567 = _mm_loadl_epi64((const __m128i*) i16);
      const __m128i vk16x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 384 * sizeof(int8_t)));
      const __m128i vi16x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i16 + 8));
      const __m128i vk16x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 392 * sizeof(int8_t)));
      const __m128i vi16xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i16 + 16));
      const __m128i vk16xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 400 * sizeof(int8_t)));
      i16 += 24;

      const __m128i vxi16x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi16x01234567, vi16x01234567), 8);
      const __m128i vxk16x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk16x01234567, vk16x01234567), 8);
      const __m128i vxi16x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi16x89ABCDEF, vi16x89ABCDEF), 8);
      const __m128i vxk16x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk16x89ABCDEF, vk16x89ABCDEF), 8);
      const __m128i vxi16xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi16xGHIJKLMN, vi16xGHIJKLMN), 8);
      const __m128i vxk16xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk16xGHIJKLMN, vk16xGHIJKLMN), 8);

      const __m128i vprod16x01234567lo = _mm_mullo_epi16(vxi16x01234567, vxk16x01234567);
      const __m128i vprod16x01234567hi = _mm_mulhi_epi16(vxi16x01234567, vxk16x01234567);
      const __m128i vprod16x89ABCDEFlo = _mm_mullo_epi16(vxi16x89ABCDEF, vxk16x89ABCDEF);
      const __m128i vprod16x89ABCDEFhi = _mm_mulhi_epi16(vxi16x89ABCDEF, vxk16x89ABCDEF);
      const __m128i vprod16xGHIJKLMNlo = _mm_mullo_epi16(vxi16xGHIJKLMN, vxk16xGHIJKLMN);
      const __m128i vprod16xGHIJKLMNhi = _mm_mulhi_epi16(vxi16xGHIJKLMN, vxk16xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod16x01234567lo, vprod16x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod16x01234567lo, vprod16x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod16x89ABCDEFlo, vprod16x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod16x89ABCDEFlo, vprod16x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod16xGHIJKLMNlo, vprod16xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod16xGHIJKLMNlo, vprod16xGHIJKLMNhi));

      const __m128i vi17x01234567 = _mm_loadl_epi64((const __m128i*) i17);
      const __m128i vk17x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 408 * sizeof(int8_t)));
      const __m128i vi17x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i17 + 8));
      const __m128i vk17x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 416 * sizeof(int8_t)));
      const __m128i vi17xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i17 + 16));
      const __m128i vk17xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 424 * sizeof(int8_t)));
      i17 += 24;

      const __m128i vxi17x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi17x01234567, vi17x01234567), 8);
      const __m128i vxk17x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk17x01234567, vk17x01234567), 8);
      const __m128i vxi17x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi17x89ABCDEF, vi17x89ABCDEF), 8);
      const __m128i vxk17x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk17x89ABCDEF, vk17x89ABCDEF), 8);
      const __m128i vxi17xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi17xGHIJKLMN, vi17xGHIJKLMN), 8);
      const __m128i vxk17xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk17xGHIJKLMN, vk17xGHIJKLMN), 8);

      const __m128i vprod17x01234567lo = _mm_mullo_epi16(vxi17x01234567, vxk17x01234567);
      const __m128i vprod17x01234567hi = _mm_mulhi_epi16(vxi17x01234567, vxk17x01234567);
      const __m128i vprod17x89ABCDEFlo = _mm_mullo_epi16(vxi17x89ABCDEF, vxk17x89ABCDEF);
      const __m128i vprod17x89ABCDEFhi = _mm_mulhi_epi16(vxi17x89ABCDEF, vxk17x89ABCDEF);
      const __m128i vprod17xGHIJKLMNlo = _mm_mullo_epi16(vxi17xGHIJKLMN, vxk17xGHIJKLMN);
      const __m128i vprod17xGHIJKLMNhi = _mm_mulhi_epi16(vxi17xGHIJKLMN, vxk17xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod17x01234567lo, vprod17x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod17x01234567lo, vprod17x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod17x89ABCDEFlo, vprod17x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod17x89ABCDEFlo, vprod17x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod17xGHIJKLMNlo, vprod17xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod17xGHIJKLMNlo, vprod17xGHIJKLMNhi));

      const __m128i vi18x01234567 = _mm_loadl_epi64((const __m128i*) i18);
      const __m128i vk18x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 432 * sizeof(int8_t)));
      const __m128i vi18x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i18 + 8));
      const __m128i vk18x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 440 * sizeof(int8_t)));
      const __m128i vi18xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i18 + 16));
      const __m128i vk18xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 448 * sizeof(int8_t)));
      i18 += 24;

      const __m128i vxi18x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi18x01234567, vi18x01234567), 8);
      const __m128i vxk18x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk18x01234567, vk18x01234567), 8);
      const __m128i vxi18x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi18x89ABCDEF, vi18x89ABCDEF), 8);
      const __m128i vxk18x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk18x89ABCDEF, vk18x89ABCDEF), 8);
      const __m128i vxi18xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi18xGHIJKLMN, vi18xGHIJKLMN), 8);
      const __m128i vxk18xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk18xGHIJKLMN, vk18xGHIJKLMN), 8);

      const __m128i vprod18x01234567lo = _mm_mullo_epi16(vxi18x01234567, vxk18x01234567);
      const __m128i vprod18x01234567hi = _mm_mulhi_epi16(vxi18x01234567, vxk18x01234567);
      const __m128i vprod18x89ABCDEFlo = _mm_mullo_epi16(vxi18x89ABCDEF, vxk18x89ABCDEF);
      const __m128i vprod18x89ABCDEFhi = _mm_mulhi_epi16(vxi18x89ABCDEF, vxk18x89ABCDEF);
      const __m128i vprod18xGHIJKLMNlo = _mm_mullo_epi16(vxi18xGHIJKLMN, vxk18xGHIJKLMN);
      const __m128i vprod18xGHIJKLMNhi = _mm_mulhi_epi16(vxi18xGHIJKLMN, vxk18xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod18x01234567lo, vprod18x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod18x01234567lo, vprod18x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod18x89ABCDEFlo, vprod18x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod18x89ABCDEFlo, vprod18x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod18xGHIJKLMNlo, vprod18xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod18xGHIJKLMNlo, vprod18xGHIJKLMNhi));

      const __m128i vi19x01234567 = _mm_loadl_epi64((const __m128i*) i19);
      const __m128i vk19x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 456 * sizeof(int8_t)));
      const __m128i vi19x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i19 + 8));
      const __m128i vk19x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 464 * sizeof(int8_t)));
      const __m128i vi19xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i19 + 16));
      const __m128i vk19xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 472 * sizeof(int8_t)));
      i19 += 24;

      const __m128i vxi19x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi19x01234567, vi19x01234567), 8);
      const __m128i vxk19x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk19x01234567, vk19x01234567), 8);
      const __m128i vxi19x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi19x89ABCDEF, vi19x89ABCDEF), 8);
      const __m128i vxk19x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk19x89ABCDEF, vk19x89ABCDEF), 8);
      const __m128i vxi19xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi19xGHIJKLMN, vi19xGHIJKLMN), 8);
      const __m128i vxk19xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk19xGHIJKLMN, vk19xGHIJKLMN), 8);

      const __m128i vprod19x01234567lo = _mm_mullo_epi16(vxi19x01234567, vxk19x01234567);
      const __m128i vprod19x01234567hi = _mm_mulhi_epi16(vxi19x01234567, vxk19x01234567);
      const __m128i vprod19x89ABCDEFlo = _mm_mullo_epi16(vxi19x89ABCDEF, vxk19x89ABCDEF);
      const __m128i vprod19x89ABCDEFhi = _mm_mulhi_epi16(vxi19x89ABCDEF, vxk19x89ABCDEF);
      const __m128i vprod19xGHIJKLMNlo = _mm_mullo_epi16(vxi19xGHIJKLMN, vxk19xGHIJKLMN);
      const __m128i vprod19xGHIJKLMNhi = _mm_mulhi_epi16(vxi19xGHIJKLMN, vxk19xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod19x01234567lo, vprod19x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod19x01234567lo, vprod19x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod19x89ABCDEFlo, vprod19x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod19x89ABCDEFlo, vprod19x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod19xGHIJKLMNlo, vprod19xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod19xGHIJKLMNlo, vprod19xGHIJKLMNhi));

      const __m128i vi20x01234567 = _mm_loadl_epi64((const __m128i*) i20);
      const __m128i vk20x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 480 * sizeof(int8_t)));
      const __m128i vi20x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i20 + 8));
      const __m128i vk20x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 488 * sizeof(int8_t)));
      const __m128i vi20xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i20 + 16));
      const __m128i vk20xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 496 * sizeof(int8_t)));
      i20 += 24;

      const __m128i vxi20x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi20x01234567, vi20x01234567), 8);
      const __m128i vxk20x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk20x01234567, vk20x01234567), 8);
      const __m128i vxi20x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi20x89ABCDEF, vi20x89ABCDEF), 8);
      const __m128i vxk20x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk20x89ABCDEF, vk20x89ABCDEF), 8);
      const __m128i vxi20xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi20xGHIJKLMN, vi20xGHIJKLMN), 8);
      const __m128i vxk20xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk20xGHIJKLMN, vk20xGHIJKLMN), 8);

      const __m128i vprod20x01234567lo = _mm_mullo_epi16(vxi20x01234567, vxk20x01234567);
      const __m128i vprod20x01234567hi = _mm_mulhi_epi16(vxi20x01234567, vxk20x01234567);
      const __m128i vprod20x89ABCDEFlo = _mm_mullo_epi16(vxi20x89ABCDEF, vxk20x89ABCDEF);
      const __m128i vprod20x89ABCDEFhi = _mm_mulhi_epi16(vxi20x89ABCDEF, vxk20x89ABCDEF);
      const __m128i vprod20xGHIJKLMNlo = _mm_mullo_epi16(vxi20xGHIJKLMN, vxk20xGHIJKLMN);
      const __m128i vprod20xGHIJKLMNhi = _mm_mulhi_epi16(vxi20xGHIJKLMN, vxk20xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod20x01234567lo, vprod20x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod20x01234567lo, vprod20x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod20x89ABCDEFlo, vprod20x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod20x89ABCDEFlo, vprod20x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod20xGHIJKLMNlo, vprod20xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod20xGHIJKLMNlo, vprod20xGHIJKLMNhi));

      const __m128i vi21x01234567 = _mm_loadl_epi64((const __m128i*) i21);
      const __m128i vk21x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 504 * sizeof(int8_t)));
      const __m128i vi21x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i21 + 8));
      const __m128i vk21x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 512 * sizeof(int8_t)));
      const __m128i vi21xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i21 + 16));
      const __m128i vk21xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 520 * sizeof(int8_t)));
      i21 += 24;

      const __m128i vxi21x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi21x01234567, vi21x01234567), 8);
      const __m128i vxk21x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk21x01234567, vk21x01234567), 8);
      const __m128i vxi21x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi21x89ABCDEF, vi21x89ABCDEF), 8);
      const __m128i vxk21x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk21x89ABCDEF, vk21x89ABCDEF), 8);
      const __m128i vxi21xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi21xGHIJKLMN, vi21xGHIJKLMN), 8);
      const __m128i vxk21xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk21xGHIJKLMN, vk21xGHIJKLMN), 8);

      const __m128i vprod21x01234567lo = _mm_mullo_epi16(vxi21x01234567, vxk21x01234567);
      const __m128i vprod21x01234567hi = _mm_mulhi_epi16(vxi21x01234567, vxk21x01234567);
      const __m128i vprod21x89ABCDEFlo = _mm_mullo_epi16(vxi21x89ABCDEF, vxk21x89ABCDEF);
      const __m128i vprod21x89ABCDEFhi = _mm_mulhi_epi16(vxi21x89ABCDEF, vxk21x89ABCDEF);
      const __m128i vprod21xGHIJKLMNlo = _mm_mullo_epi16(vxi21xGHIJKLMN, vxk21xGHIJKLMN);
      const __m128i vprod21xGHIJKLMNhi = _mm_mulhi_epi16(vxi21xGHIJKLMN, vxk21xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod21x01234567lo, vprod21x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod21x01234567lo, vprod21x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod21x89ABCDEFlo, vprod21x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod21x89ABCDEFlo, vprod21x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod21xGHIJKLMNlo, vprod21xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod21xGHIJKLMNlo, vprod21xGHIJKLMNhi));

      const __m128i vi22x01234567 = _mm_loadl_epi64((const __m128i*) i22);
      const __m128i vk22x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 528 * sizeof(int8_t)));
      const __m128i vi22x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i22 + 8));
      const __m128i vk22x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 536 * sizeof(int8_t)));
      const __m128i vi22xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i22 + 16));
      const __m128i vk22xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 544 * sizeof(int8_t)));
      i22 += 24;

      const __m128i vxi22x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi22x01234567, vi22x01234567), 8);
      const __m128i vxk22x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk22x01234567, vk22x01234567), 8);
      const __m128i vxi22x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi22x89ABCDEF, vi22x89ABCDEF), 8);
      const __m128i vxk22x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk22x89ABCDEF, vk22x89ABCDEF), 8);
      const __m128i vxi22xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi22xGHIJKLMN, vi22xGHIJKLMN), 8);
      const __m128i vxk22xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk22xGHIJKLMN, vk22xGHIJKLMN), 8);

      const __m128i vprod22x01234567lo = _mm_mullo_epi16(vxi22x01234567, vxk22x01234567);
      const __m128i vprod22x01234567hi = _mm_mulhi_epi16(vxi22x01234567, vxk22x01234567);
      const __m128i vprod22x89ABCDEFlo = _mm_mullo_epi16(vxi22x89ABCDEF, vxk22x89ABCDEF);
      const __m128i vprod22x89ABCDEFhi = _mm_mulhi_epi16(vxi22x89ABCDEF, vxk22x89ABCDEF);
      const __m128i vprod22xGHIJKLMNlo = _mm_mullo_epi16(vxi22xGHIJKLMN, vxk22xGHIJKLMN);
      const __m128i vprod22xGHIJKLMNhi = _mm_mulhi_epi16(vxi22xGHIJKLMN, vxk22xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod22x01234567lo, vprod22x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod22x01234567lo, vprod22x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod22x89ABCDEFlo, vprod22x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod22x89ABCDEFlo, vprod22x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod22xGHIJKLMNlo, vprod22xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod22xGHIJKLMNlo, vprod22xGHIJKLMNhi));

      const __m128i vi23x01234567 = _mm_loadl_epi64((const __m128i*) i23);
      const __m128i vk23x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 552 * sizeof(int8_t)));
      const __m128i vi23x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i23 + 8));
      const __m128i vk23x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 560 * sizeof(int8_t)));
      const __m128i vi23xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i23 + 16));
      const __m128i vk23xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 568 * sizeof(int8_t)));
      i23 += 24;

      const __m128i vxi23x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi23x01234567, vi23x01234567), 8);
      const __m128i vxk23x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk23x01234567, vk23x01234567), 8);
      const __m128i vxi23x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi23x89ABCDEF, vi23x89ABCDEF), 8);
      const __m128i vxk23x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk23x89ABCDEF, vk23x89ABCDEF), 8);
      const __m128i vxi23xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi23xGHIJKLMN, vi23xGHIJKLMN), 8);
      const __m128i vxk23xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk23xGHIJKLMN, vk23xGHIJKLMN), 8);

      const __m128i vprod23x01234567lo = _mm_mullo_epi16(vxi23x01234567, vxk23x01234567);
      const __m128i vprod23x01234567hi = _mm_mulhi_epi16(vxi23x01234567, vxk23x01234567);
      const __m128i vprod23x89ABCDEFlo = _mm_mullo_epi16(vxi23x89ABCDEF, vxk23x89ABCDEF);
      const __m128i vprod23x89ABCDEFhi = _mm_mulhi_epi16(vxi23x89ABCDEF, vxk23x89ABCDEF);
      const __m128i vprod23xGHIJKLMNlo = _mm_mullo_epi16(vxi23xGHIJKLMN, vxk23xGHIJKLMN);
      const __m128i vprod23xGHIJKLMNhi = _mm_mulhi_epi16(vxi23xGHIJKLMN, vxk23xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod23x01234567lo, vprod23x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod23x01234567lo, vprod23x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod23x89ABCDEFlo, vprod23x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod23x89ABCDEFlo, vprod23x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod23xGHIJKLMNlo, vprod23xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod23xGHIJKLMNlo, vprod23xGHIJKLMNhi));

      const __m128i vi24x01234567 = _mm_loadl_epi64((const __m128i*) i24);
      const __m128i vk24x01234567 = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 576 * sizeof(int8_t)));
      const __m128i vi24x89ABCDEF = _mm_loadl_epi64((const __m128i*) (i24 + 8));
      const __m128i vk24x89ABCDEF = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 584 * sizeof(int8_t)));
      const __m128i vi24xGHIJKLMN = _mm_loadl_epi64((const __m128i*) (i24 + 16));
      const __m128i vk24xGHIJKLMN = _mm_loadl_epi64((const __m128i*) ((uintptr_t) w + 24 * sizeof(int32_t) + 592 * sizeof(int8_t)));
      i24 += 24;

      const __m128i vxi24x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi24x01234567, vi24x01234567), 8);
      const __m128i vxk24x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk24x01234567, vk24x01234567), 8);
      const __m128i vxi24x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vi24x89ABCDEF, vi24x89ABCDEF), 8);
      const __m128i vxk24x89ABCDEF = _mm_srai_epi16(_mm_unpacklo_epi8(vk24x89ABCDEF, vk24x89ABCDEF), 8);
      const __m128i vxi24xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vi24xGHIJKLMN, vi24xGHIJKLMN), 8);
      const __m128i vxk24xGHIJKLMN = _mm_srai_epi16(_mm_unpacklo_epi8(vk24xGHIJKLMN, vk24xGHIJKLMN), 8);

      const __m128i vprod24x01234567lo = _mm_mullo_epi16(vxi24x01234567, vxk24x01234567);
      const __m128i vprod24x01234567hi = _mm_mulhi_epi16(vxi24x01234567, vxk24x01234567);
      const __m128i vprod24x89ABCDEFlo = _mm_mullo_epi16(vxi24x89ABCDEF, vxk24x89ABCDEF);
      const __m128i vprod24x89ABCDEFhi = _mm_mulhi_epi16(vxi24x89ABCDEF, vxk24x89ABCDEF);
      const __m128i vprod24xGHIJKLMNlo = _mm_mullo_epi16(vxi24xGHIJKLMN, vxk24xGHIJKLMN);
      const __m128i vprod24xGHIJKLMNhi = _mm_mulhi_epi16(vxi24xGHIJKLMN, vxk24xGHIJKLMN);

      vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod24x01234567lo, vprod24x01234567hi));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod24x01234567lo, vprod24x01234567hi));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_unpacklo_epi16(vprod24x89ABCDEFlo, vprod24x89ABCDEFhi));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_unpackhi_epi16(vprod24x89ABCDEFlo, vprod24x89ABCDEFhi));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_unpacklo_epi16(vprod24xGHIJKLMNlo, vprod24xGHIJKLMNhi));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_unpackhi_epi16(vprod24xGHIJKLMNlo, vprod24xGHIJKLMNhi));

      w = (const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 600 * sizeof(int8_t));

      __m128 vscaled0123 = _mm_cvtepi32_ps(vacc0123);
      __m128 vscaled4567 = _mm_cvtepi32_ps(vacc4567);
      __m128 vscaled89AB = _mm_cvtepi32_ps(vacc89AB);
      __m128 vscaledCDEF = _mm_cvtepi32_ps(vaccCDEF);
      __m128 vscaledGHIJ = _mm_cvtepi32_ps(vaccGHIJ);
      __m128 vscaledKLMN = _mm_cvtepi32_ps(vaccKLMN);

      const __m128 vscale0123 = _mm_loadu_ps((const float*) w);
      const __m128 vscale4567 = _mm_loadu_ps((const float*) w + 4);
      const __m128 vscale89AB = _mm_loadu_ps((const float*) w + 8);
      const __m128 vscaleCDEF = _mm_loadu_ps((const float*) w + 12);
      const __m128 vscaleGHIJ = _mm_loadu_ps((const float*) w + 16);
      const __m128 vscaleKLMN = _mm_loadu_ps((const float*) w + 20);
      w = (const void*) ((const float*) w + 24);
      vscaled0123 = _mm_mul_ps(vscaled0123, vscale0123);
      vscaled4567 = _mm_mul_ps(vscaled4567, vscale4567);
      vscaled89AB = _mm_mul_ps(vscaled89AB, vscale89AB);
      vscaledCDEF = _mm_mul_ps(vscaledCDEF, vscaleCDEF);
      vscaledGHIJ = _mm_mul_ps(vscaledGHIJ, vscaleGHIJ);
      vscaledKLMN = _mm_mul_ps(vscaledKLMN, vscaleKLMN);

      const __m128 voutput_max_less_zero_point = _mm_load_ps(params->sse2.output_max_less_zero_point);
      vscaled0123 = _mm_min_ps(vscaled0123, voutput_max_less_zero_point);
      vscaled4567 = _mm_min_ps(vscaled4567, voutput_max_less_zero_point);
      vscaled89AB = _mm_min_ps(vscaled89AB, voutput_max_less_zero_point);
      vscaledCDEF = _mm_min_ps(vscaledCDEF, voutput_max_less_zero_point);
      vscaledGHIJ = _mm_min_ps(vscaledGHIJ, voutput_max_less_zero_point);
      vscaledKLMN = _mm_min_ps(vscaledKLMN, voutput_max_less_zero_point);

      vacc0123 = _mm_cvtps_epi32(vscaled0123);
      vacc4567 = _mm_cvtps_epi32(vscaled4567);
      vacc89AB = _mm_cvtps_epi32(vscaled89AB);
      vaccCDEF = _mm_cvtps_epi32(vscaledCDEF);
      vaccGHIJ = _mm_cvtps_epi32(vscaledGHIJ);
      vaccKLMN = _mm_cvtps_epi32(vscaledKLMN);

      const __m128i voutput_zero_point = _mm_load_si128((const __m128i*) params->sse2.output_zero_point);
      __m128i vout01234567 = _mm_adds_epi16(_mm_packs_epi32(vacc0123, vacc4567), voutput_zero_point);
      __m128i vout89ABCDEF = _mm_adds_epi16(_mm_packs_epi32(vacc89AB, vaccCDEF), voutput_zero_point);
      __m128i voutGHIJKLMN = _mm_adds_epi16(_mm_packs_epi32(vaccGHIJ, vaccKLMN), voutput_zero_point);

      const __m128i voutput_min = _mm_load_si128((const __m128i*) params->sse2.output_min);
      vout01234567 = _mm_max_epi16(vout01234567, voutput_min);
      vout89ABCDEF = _mm_max_epi16(vout89ABCDEF, voutput_min);
      voutGHIJKLMN = _mm_max_epi16(voutGHIJKLMN, voutput_min);

      __m128i vout0123456789ABCDEF = _mm_packs_epi16(vout01234567, vout89ABCDEF);
      __m128i voutGHIJKLMNGHIJKLMN = _mm_packs_epi16(voutGHIJKLMN, voutGHIJKLMN);


      _mm_storeu_si128((__m128i*) output, vout0123456789ABCDEF);
      _mm_storel_epi64((__m128i*) (output + 16), voutGHIJKLMNGHIJKLMN);
      output += 24;
    }
    if XNN_UNLIKELY(c != 0) {
      const int8_t* k = (const int8_t*) ((const int32_t*) w + 24);
      do {
        __m128i vacc0123 = _mm_loadu_si128((const __m128i*) w);
        __m128i vacc4567 = _mm_loadu_si128((const __m128i*) ((const int32_t*) w + 4));


        const __m128i vi0x01234567 = _mm_loadl_epi64((const __m128i*) i0);
        const __m128i vk0x01234567 = _mm_loadl_epi64((const __m128i*) k);
        i0 += 8;

        const __m128i vxi0x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi0x01234567, vi0x01234567), 8);
        const __m128i vxk0x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk0x01234567, vk0x01234567), 8);

        const __m128i vprod0x01234567lo = _mm_mullo_epi16(vxi0x01234567, vxk0x01234567);
        const __m128i vprod0x01234567hi = _mm_mulhi_epi16(vxi0x01234567, vxk0x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod0x01234567lo, vprod0x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod0x01234567lo, vprod0x01234567hi));

        const __m128i vi1x01234567 = _mm_loadl_epi64((const __m128i*) i1);
        const __m128i vk1x01234567 = _mm_loadl_epi64((const __m128i*) (k + 24));
        i1 += 8;

        const __m128i vxi1x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi1x01234567, vi1x01234567), 8);
        const __m128i vxk1x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk1x01234567, vk1x01234567), 8);

        const __m128i vprod1x01234567lo = _mm_mullo_epi16(vxi1x01234567, vxk1x01234567);
        const __m128i vprod1x01234567hi = _mm_mulhi_epi16(vxi1x01234567, vxk1x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod1x01234567lo, vprod1x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod1x01234567lo, vprod1x01234567hi));

        const __m128i vi2x01234567 = _mm_loadl_epi64((const __m128i*) i2);
        const __m128i vk2x01234567 = _mm_loadl_epi64((const __m128i*) (k + 48));
        i2 += 8;

        const __m128i vxi2x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi2x01234567, vi2x01234567), 8);
        const __m128i vxk2x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk2x01234567, vk2x01234567), 8);

        const __m128i vprod2x01234567lo = _mm_mullo_epi16(vxi2x01234567, vxk2x01234567);
        const __m128i vprod2x01234567hi = _mm_mulhi_epi16(vxi2x01234567, vxk2x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod2x01234567lo, vprod2x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod2x01234567lo, vprod2x01234567hi));

        const __m128i vi3x01234567 = _mm_loadl_epi64((const __m128i*) i3);
        const __m128i vk3x01234567 = _mm_loadl_epi64((const __m128i*) (k + 72));
        i3 += 8;

        const __m128i vxi3x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi3x01234567, vi3x01234567), 8);
        const __m128i vxk3x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk3x01234567, vk3x01234567), 8);

        const __m128i vprod3x01234567lo = _mm_mullo_epi16(vxi3x01234567, vxk3x01234567);
        const __m128i vprod3x01234567hi = _mm_mulhi_epi16(vxi3x01234567, vxk3x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod3x01234567lo, vprod3x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod3x01234567lo, vprod3x01234567hi));

        const __m128i vi4x01234567 = _mm_loadl_epi64((const __m128i*) i4);
        const __m128i vk4x01234567 = _mm_loadl_epi64((const __m128i*) (k + 96));
        i4 += 8;

        const __m128i vxi4x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi4x01234567, vi4x01234567), 8);
        const __m128i vxk4x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk4x01234567, vk4x01234567), 8);

        const __m128i vprod4x01234567lo = _mm_mullo_epi16(vxi4x01234567, vxk4x01234567);
        const __m128i vprod4x01234567hi = _mm_mulhi_epi16(vxi4x01234567, vxk4x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod4x01234567lo, vprod4x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod4x01234567lo, vprod4x01234567hi));

        const __m128i vi5x01234567 = _mm_loadl_epi64((const __m128i*) i5);
        const __m128i vk5x01234567 = _mm_loadl_epi64((const __m128i*) (k + 120));
        i5 += 8;

        const __m128i vxi5x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi5x01234567, vi5x01234567), 8);
        const __m128i vxk5x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk5x01234567, vk5x01234567), 8);

        const __m128i vprod5x01234567lo = _mm_mullo_epi16(vxi5x01234567, vxk5x01234567);
        const __m128i vprod5x01234567hi = _mm_mulhi_epi16(vxi5x01234567, vxk5x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod5x01234567lo, vprod5x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod5x01234567lo, vprod5x01234567hi));

        const __m128i vi6x01234567 = _mm_loadl_epi64((const __m128i*) i6);
        const __m128i vk6x01234567 = _mm_loadl_epi64((const __m128i*) (k + 144));
        i6 += 8;

        const __m128i vxi6x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi6x01234567, vi6x01234567), 8);
        const __m128i vxk6x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk6x01234567, vk6x01234567), 8);

        const __m128i vprod6x01234567lo = _mm_mullo_epi16(vxi6x01234567, vxk6x01234567);
        const __m128i vprod6x01234567hi = _mm_mulhi_epi16(vxi6x01234567, vxk6x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod6x01234567lo, vprod6x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod6x01234567lo, vprod6x01234567hi));

        const __m128i vi7x01234567 = _mm_loadl_epi64((const __m128i*) i7);
        const __m128i vk7x01234567 = _mm_loadl_epi64((const __m128i*) (k + 168));
        i7 += 8;

        const __m128i vxi7x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi7x01234567, vi7x01234567), 8);
        const __m128i vxk7x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk7x01234567, vk7x01234567), 8);

        const __m128i vprod7x01234567lo = _mm_mullo_epi16(vxi7x01234567, vxk7x01234567);
        const __m128i vprod7x01234567hi = _mm_mulhi_epi16(vxi7x01234567, vxk7x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod7x01234567lo, vprod7x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod7x01234567lo, vprod7x01234567hi));

        const __m128i vi8x01234567 = _mm_loadl_epi64((const __m128i*) i8);
        const __m128i vk8x01234567 = _mm_loadl_epi64((const __m128i*) (k + 192));
        i8 += 8;

        const __m128i vxi8x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi8x01234567, vi8x01234567), 8);
        const __m128i vxk8x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk8x01234567, vk8x01234567), 8);

        const __m128i vprod8x01234567lo = _mm_mullo_epi16(vxi8x01234567, vxk8x01234567);
        const __m128i vprod8x01234567hi = _mm_mulhi_epi16(vxi8x01234567, vxk8x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod8x01234567lo, vprod8x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod8x01234567lo, vprod8x01234567hi));

        const __m128i vi9x01234567 = _mm_loadl_epi64((const __m128i*) i9);
        const __m128i vk9x01234567 = _mm_loadl_epi64((const __m128i*) (k + 216));
        i9 += 8;

        const __m128i vxi9x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi9x01234567, vi9x01234567), 8);
        const __m128i vxk9x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk9x01234567, vk9x01234567), 8);

        const __m128i vprod9x01234567lo = _mm_mullo_epi16(vxi9x01234567, vxk9x01234567);
        const __m128i vprod9x01234567hi = _mm_mulhi_epi16(vxi9x01234567, vxk9x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod9x01234567lo, vprod9x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod9x01234567lo, vprod9x01234567hi));

        const __m128i vi10x01234567 = _mm_loadl_epi64((const __m128i*) i10);
        const __m128i vk10x01234567 = _mm_loadl_epi64((const __m128i*) (k + 240));
        i10 += 8;

        const __m128i vxi10x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi10x01234567, vi10x01234567), 8);
        const __m128i vxk10x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk10x01234567, vk10x01234567), 8);

        const __m128i vprod10x01234567lo = _mm_mullo_epi16(vxi10x01234567, vxk10x01234567);
        const __m128i vprod10x01234567hi = _mm_mulhi_epi16(vxi10x01234567, vxk10x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod10x01234567lo, vprod10x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod10x01234567lo, vprod10x01234567hi));

        const __m128i vi11x01234567 = _mm_loadl_epi64((const __m128i*) i11);
        const __m128i vk11x01234567 = _mm_loadl_epi64((const __m128i*) (k + 264));
        i11 += 8;

        const __m128i vxi11x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi11x01234567, vi11x01234567), 8);
        const __m128i vxk11x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk11x01234567, vk11x01234567), 8);

        const __m128i vprod11x01234567lo = _mm_mullo_epi16(vxi11x01234567, vxk11x01234567);
        const __m128i vprod11x01234567hi = _mm_mulhi_epi16(vxi11x01234567, vxk11x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod11x01234567lo, vprod11x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod11x01234567lo, vprod11x01234567hi));

        const __m128i vi12x01234567 = _mm_loadl_epi64((const __m128i*) i12);
        const __m128i vk12x01234567 = _mm_loadl_epi64((const __m128i*) (k + 288));
        i12 += 8;

        const __m128i vxi12x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi12x01234567, vi12x01234567), 8);
        const __m128i vxk12x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk12x01234567, vk12x01234567), 8);

        const __m128i vprod12x01234567lo = _mm_mullo_epi16(vxi12x01234567, vxk12x01234567);
        const __m128i vprod12x01234567hi = _mm_mulhi_epi16(vxi12x01234567, vxk12x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod12x01234567lo, vprod12x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod12x01234567lo, vprod12x01234567hi));

        const __m128i vi13x01234567 = _mm_loadl_epi64((const __m128i*) i13);
        const __m128i vk13x01234567 = _mm_loadl_epi64((const __m128i*) (k + 312));
        i13 += 8;

        const __m128i vxi13x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi13x01234567, vi13x01234567), 8);
        const __m128i vxk13x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk13x01234567, vk13x01234567), 8);

        const __m128i vprod13x01234567lo = _mm_mullo_epi16(vxi13x01234567, vxk13x01234567);
        const __m128i vprod13x01234567hi = _mm_mulhi_epi16(vxi13x01234567, vxk13x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod13x01234567lo, vprod13x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod13x01234567lo, vprod13x01234567hi));

        const __m128i vi14x01234567 = _mm_loadl_epi64((const __m128i*) i14);
        const __m128i vk14x01234567 = _mm_loadl_epi64((const __m128i*) (k + 336));
        i14 += 8;

        const __m128i vxi14x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi14x01234567, vi14x01234567), 8);
        const __m128i vxk14x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk14x01234567, vk14x01234567), 8);

        const __m128i vprod14x01234567lo = _mm_mullo_epi16(vxi14x01234567, vxk14x01234567);
        const __m128i vprod14x01234567hi = _mm_mulhi_epi16(vxi14x01234567, vxk14x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod14x01234567lo, vprod14x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod14x01234567lo, vprod14x01234567hi));

        const __m128i vi15x01234567 = _mm_loadl_epi64((const __m128i*) i15);
        const __m128i vk15x01234567 = _mm_loadl_epi64((const __m128i*) (k + 360));
        i15 += 8;

        const __m128i vxi15x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi15x01234567, vi15x01234567), 8);
        const __m128i vxk15x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk15x01234567, vk15x01234567), 8);

        const __m128i vprod15x01234567lo = _mm_mullo_epi16(vxi15x01234567, vxk15x01234567);
        const __m128i vprod15x01234567hi = _mm_mulhi_epi16(vxi15x01234567, vxk15x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod15x01234567lo, vprod15x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod15x01234567lo, vprod15x01234567hi));

        const __m128i vi16x01234567 = _mm_loadl_epi64((const __m128i*) i16);
        const __m128i vk16x01234567 = _mm_loadl_epi64((const __m128i*) (k + 384));
        i16 += 8;

        const __m128i vxi16x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi16x01234567, vi16x01234567), 8);
        const __m128i vxk16x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk16x01234567, vk16x01234567), 8);

        const __m128i vprod16x01234567lo = _mm_mullo_epi16(vxi16x01234567, vxk16x01234567);
        const __m128i vprod16x01234567hi = _mm_mulhi_epi16(vxi16x01234567, vxk16x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod16x01234567lo, vprod16x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod16x01234567lo, vprod16x01234567hi));

        const __m128i vi17x01234567 = _mm_loadl_epi64((const __m128i*) i17);
        const __m128i vk17x01234567 = _mm_loadl_epi64((const __m128i*) (k + 408));
        i17 += 8;

        const __m128i vxi17x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi17x01234567, vi17x01234567), 8);
        const __m128i vxk17x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk17x01234567, vk17x01234567), 8);

        const __m128i vprod17x01234567lo = _mm_mullo_epi16(vxi17x01234567, vxk17x01234567);
        const __m128i vprod17x01234567hi = _mm_mulhi_epi16(vxi17x01234567, vxk17x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod17x01234567lo, vprod17x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod17x01234567lo, vprod17x01234567hi));

        const __m128i vi18x01234567 = _mm_loadl_epi64((const __m128i*) i18);
        const __m128i vk18x01234567 = _mm_loadl_epi64((const __m128i*) (k + 432));
        i18 += 8;

        const __m128i vxi18x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi18x01234567, vi18x01234567), 8);
        const __m128i vxk18x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk18x01234567, vk18x01234567), 8);

        const __m128i vprod18x01234567lo = _mm_mullo_epi16(vxi18x01234567, vxk18x01234567);
        const __m128i vprod18x01234567hi = _mm_mulhi_epi16(vxi18x01234567, vxk18x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod18x01234567lo, vprod18x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod18x01234567lo, vprod18x01234567hi));

        const __m128i vi19x01234567 = _mm_loadl_epi64((const __m128i*) i19);
        const __m128i vk19x01234567 = _mm_loadl_epi64((const __m128i*) (k + 456));
        i19 += 8;

        const __m128i vxi19x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi19x01234567, vi19x01234567), 8);
        const __m128i vxk19x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk19x01234567, vk19x01234567), 8);

        const __m128i vprod19x01234567lo = _mm_mullo_epi16(vxi19x01234567, vxk19x01234567);
        const __m128i vprod19x01234567hi = _mm_mulhi_epi16(vxi19x01234567, vxk19x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod19x01234567lo, vprod19x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod19x01234567lo, vprod19x01234567hi));

        const __m128i vi20x01234567 = _mm_loadl_epi64((const __m128i*) i20);
        const __m128i vk20x01234567 = _mm_loadl_epi64((const __m128i*) (k + 480));
        i20 += 8;

        const __m128i vxi20x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi20x01234567, vi20x01234567), 8);
        const __m128i vxk20x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk20x01234567, vk20x01234567), 8);

        const __m128i vprod20x01234567lo = _mm_mullo_epi16(vxi20x01234567, vxk20x01234567);
        const __m128i vprod20x01234567hi = _mm_mulhi_epi16(vxi20x01234567, vxk20x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod20x01234567lo, vprod20x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod20x01234567lo, vprod20x01234567hi));

        const __m128i vi21x01234567 = _mm_loadl_epi64((const __m128i*) i21);
        const __m128i vk21x01234567 = _mm_loadl_epi64((const __m128i*) (k + 504));
        i21 += 8;

        const __m128i vxi21x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi21x01234567, vi21x01234567), 8);
        const __m128i vxk21x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk21x01234567, vk21x01234567), 8);

        const __m128i vprod21x01234567lo = _mm_mullo_epi16(vxi21x01234567, vxk21x01234567);
        const __m128i vprod21x01234567hi = _mm_mulhi_epi16(vxi21x01234567, vxk21x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod21x01234567lo, vprod21x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod21x01234567lo, vprod21x01234567hi));

        const __m128i vi22x01234567 = _mm_loadl_epi64((const __m128i*) i22);
        const __m128i vk22x01234567 = _mm_loadl_epi64((const __m128i*) (k + 528));
        i22 += 8;

        const __m128i vxi22x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi22x01234567, vi22x01234567), 8);
        const __m128i vxk22x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk22x01234567, vk22x01234567), 8);

        const __m128i vprod22x01234567lo = _mm_mullo_epi16(vxi22x01234567, vxk22x01234567);
        const __m128i vprod22x01234567hi = _mm_mulhi_epi16(vxi22x01234567, vxk22x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod22x01234567lo, vprod22x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod22x01234567lo, vprod22x01234567hi));

        const __m128i vi23x01234567 = _mm_loadl_epi64((const __m128i*) i23);
        const __m128i vk23x01234567 = _mm_loadl_epi64((const __m128i*) (k + 552));
        i23 += 8;

        const __m128i vxi23x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi23x01234567, vi23x01234567), 8);
        const __m128i vxk23x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk23x01234567, vk23x01234567), 8);

        const __m128i vprod23x01234567lo = _mm_mullo_epi16(vxi23x01234567, vxk23x01234567);
        const __m128i vprod23x01234567hi = _mm_mulhi_epi16(vxi23x01234567, vxk23x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod23x01234567lo, vprod23x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod23x01234567lo, vprod23x01234567hi));

        const __m128i vi24x01234567 = _mm_loadl_epi64((const __m128i*) i24);
        const __m128i vk24x01234567 = _mm_loadl_epi64((const __m128i*) (k + 576));
        i24 += 8;

        const __m128i vxi24x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vi24x01234567, vi24x01234567), 8);
        const __m128i vxk24x01234567 = _mm_srai_epi16(_mm_unpacklo_epi8(vk24x01234567, vk24x01234567), 8);

        const __m128i vprod24x01234567lo = _mm_mullo_epi16(vxi24x01234567, vxk24x01234567);
        const __m128i vprod24x01234567hi = _mm_mulhi_epi16(vxi24x01234567, vxk24x01234567);

        vacc0123 = _mm_add_epi32(vacc0123, _mm_unpacklo_epi16(vprod24x01234567lo, vprod24x01234567hi));
        vacc4567 = _mm_add_epi32(vacc4567, _mm_unpackhi_epi16(vprod24x01234567lo, vprod24x01234567hi));

        k += 8;

        __m128 vscaled0123 = _mm_cvtepi32_ps(vacc0123);
        __m128 vscaled4567 = _mm_cvtepi32_ps(vacc4567);

        const __m128 vscale0123 = _mm_loadu_ps((const float*) ((uintptr_t) w + 24 * sizeof(int32_t) + 600 * sizeof(int8_t)));
        const __m128 vscale4567 = _mm_loadu_ps((const float*) ((uintptr_t) w + 24 * sizeof(int32_t) + 600 * sizeof(int8_t) + 4 * sizeof(float)));
        vscaled0123 = _mm_mul_ps(vscaled0123, vscale0123);
        vscaled4567 = _mm_mul_ps(vscaled4567, vscale4567);

        const __m128 voutput_max_less_zero_point = _mm_load_ps(params->sse2.output_max_less_zero_point);
        vscaled0123 = _mm_min_ps(vscaled0123, voutput_max_less_zero_point);
        vscaled4567 = _mm_min_ps(vscaled4567, voutput_max_less_zero_point);

        vacc0123 = _mm_cvtps_epi32(vscaled0123);
        vacc4567 = _mm_cvtps_epi32(vscaled4567);

        w = (const void*) ((const int32_t*) w + 8);

        const __m128i voutput_zero_point = _mm_load_si128((const __m128i*) params->sse2.output_zero_point);
        __m128i vout01234567 = _mm_adds_epi16(_mm_packs_epi32(vacc0123, vacc4567), voutput_zero_point);

        vout01234567 = _mm_max_epi16(vout01234567, _mm_load_si128((const __m128i*) params->sse2.output_min));

        __m128i vout0123456701234567 = _mm_packs_epi16(vout01234567, vout01234567);


        if XNN_LIKELY(c >= 8) {
          _mm_storel_epi64((__m128i*) output, vout0123456701234567);
          output += 8;
          c -= 8;
        } else {
          if (c & 4) {
            *((uint32_t*) output) = (uint32_t) _mm_cvtsi128_si32(vout0123456701234567);
            vout0123456701234567 = _mm_srli_epi64(vout0123456701234567, 32);
            output += 4;
          }
          if (c & 2) {
            *((uint16_t*) output) = (uint16_t) _mm_extract_epi16(vout0123456701234567, 0);
            vout0123456701234567 = _mm_srli_epi32(vout0123456701234567, 16);
            output += 2;
          }
          if (c & 1) {
            *output = (int8_t) _mm_cvtsi128_si32(vout0123456701234567);
            output += 1;
          }
          c = 0;
        }
      } while (c != 0);
    }

    output = (int8_t*) ((uintptr_t) output + output_increment);
  } while (--output_width != 0);
}
