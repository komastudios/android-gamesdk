// Auto-generated file. Do not edit!
//   Template: src/qs8-dwconv/unipass-avx2-mul16-vpunpck.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <immintrin.h>

#include <xnnpack/dwconv.h>


void xnn_qs8_dwconv_minmax_fp32_ukernel_up32x9__avx2_mul16_vpunpck(
    size_t channels,
    size_t output_width,
    const int8_t** input,
    const void* weights,
    int8_t* output,
    size_t input_stride,
    size_t output_increment,
    size_t input_offset,
    const int8_t* zero,
    const union xnn_qs8_conv_minmax_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
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
    input = (const int8_t**) ((uintptr_t) input + input_stride);

    size_t c = channels;
    const void* w = weights;
    for (; c >= 32; c -= 32) {
      __m256i vacc01234567 = _mm256_loadu_si256((const __m256i*) w);
      __m256i vacc89ABCDEF = _mm256_loadu_si256((const __m256i*) ((uintptr_t) w + 8 * sizeof(int32_t)));
      __m256i vaccGHIJKLMN = _mm256_loadu_si256((const __m256i*) ((uintptr_t) w + 16 * sizeof(int32_t)));
      __m256i vaccOPQRSTUV = _mm256_loadu_si256((const __m256i*) ((uintptr_t) w + 24 * sizeof(int32_t)));

      __m256i vacc012389AB = _mm256_inserti128_si256(vacc01234567, _mm256_castsi256_si128(vacc89ABCDEF), 1);
      __m256i vacc4567CDEF = _mm256_permute2x128_si256(vacc01234567, vacc89ABCDEF, 0x31);
      __m256i vaccGHIJOPQR = _mm256_inserti128_si256(vaccGHIJKLMN, _mm256_castsi256_si128(vaccOPQRSTUV), 1);
      __m256i vaccKLMNSTUV = _mm256_permute2x128_si256(vaccGHIJKLMN, vaccOPQRSTUV, 0x31);


      const __m256i vi0x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i0));
      const __m256i vk0x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 0 * sizeof(int8_t))));
      const __m256i vi0xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (i0 + 16)));
      const __m256i vk0xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 16 * sizeof(int8_t))));
      i0 += 32;

      const __m256i vprod0x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi0x0123456789ABCDEF, vk0x0123456789ABCDEF);
      const __m256i vprod0x0123456789ABCDEFhi = _mm256_srai_epi16(vprod0x0123456789ABCDEFlo, 15);
      const __m256i vprod0xGHIJKLMNOPQRSTUVlo =  _mm256_mullo_epi16(vi0xGHIJKLMNOPQRSTUV, vk0xGHIJKLMNOPQRSTUV);
      const __m256i vprod0xGHIJKLMNOPQRSTUVhi = _mm256_srai_epi16(vprod0xGHIJKLMNOPQRSTUVlo, 15);

      vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod0x0123456789ABCDEFlo, vprod0x0123456789ABCDEFhi));
      vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod0x0123456789ABCDEFlo, vprod0x0123456789ABCDEFhi));
      vaccGHIJOPQR = _mm256_add_epi32(vaccGHIJOPQR, _mm256_unpacklo_epi16(vprod0xGHIJKLMNOPQRSTUVlo, vprod0xGHIJKLMNOPQRSTUVhi));
      vaccKLMNSTUV = _mm256_add_epi32(vaccKLMNSTUV, _mm256_unpackhi_epi16(vprod0xGHIJKLMNOPQRSTUVlo, vprod0xGHIJKLMNOPQRSTUVhi));

      const __m256i vi1x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i1));
      const __m256i vk1x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 32 * sizeof(int8_t))));
      const __m256i vi1xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (i1 + 16)));
      const __m256i vk1xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 48 * sizeof(int8_t))));
      i1 += 32;

      const __m256i vprod1x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi1x0123456789ABCDEF, vk1x0123456789ABCDEF);
      const __m256i vprod1x0123456789ABCDEFhi = _mm256_srai_epi16(vprod1x0123456789ABCDEFlo, 15);
      const __m256i vprod1xGHIJKLMNOPQRSTUVlo =  _mm256_mullo_epi16(vi1xGHIJKLMNOPQRSTUV, vk1xGHIJKLMNOPQRSTUV);
      const __m256i vprod1xGHIJKLMNOPQRSTUVhi = _mm256_srai_epi16(vprod1xGHIJKLMNOPQRSTUVlo, 15);

      vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod1x0123456789ABCDEFlo, vprod1x0123456789ABCDEFhi));
      vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod1x0123456789ABCDEFlo, vprod1x0123456789ABCDEFhi));
      vaccGHIJOPQR = _mm256_add_epi32(vaccGHIJOPQR, _mm256_unpacklo_epi16(vprod1xGHIJKLMNOPQRSTUVlo, vprod1xGHIJKLMNOPQRSTUVhi));
      vaccKLMNSTUV = _mm256_add_epi32(vaccKLMNSTUV, _mm256_unpackhi_epi16(vprod1xGHIJKLMNOPQRSTUVlo, vprod1xGHIJKLMNOPQRSTUVhi));

      const __m256i vi2x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i2));
      const __m256i vk2x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 64 * sizeof(int8_t))));
      const __m256i vi2xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (i2 + 16)));
      const __m256i vk2xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 80 * sizeof(int8_t))));
      i2 += 32;

      const __m256i vprod2x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi2x0123456789ABCDEF, vk2x0123456789ABCDEF);
      const __m256i vprod2x0123456789ABCDEFhi = _mm256_srai_epi16(vprod2x0123456789ABCDEFlo, 15);
      const __m256i vprod2xGHIJKLMNOPQRSTUVlo =  _mm256_mullo_epi16(vi2xGHIJKLMNOPQRSTUV, vk2xGHIJKLMNOPQRSTUV);
      const __m256i vprod2xGHIJKLMNOPQRSTUVhi = _mm256_srai_epi16(vprod2xGHIJKLMNOPQRSTUVlo, 15);

      vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod2x0123456789ABCDEFlo, vprod2x0123456789ABCDEFhi));
      vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod2x0123456789ABCDEFlo, vprod2x0123456789ABCDEFhi));
      vaccGHIJOPQR = _mm256_add_epi32(vaccGHIJOPQR, _mm256_unpacklo_epi16(vprod2xGHIJKLMNOPQRSTUVlo, vprod2xGHIJKLMNOPQRSTUVhi));
      vaccKLMNSTUV = _mm256_add_epi32(vaccKLMNSTUV, _mm256_unpackhi_epi16(vprod2xGHIJKLMNOPQRSTUVlo, vprod2xGHIJKLMNOPQRSTUVhi));

      const __m256i vi3x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i3));
      const __m256i vk3x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 96 * sizeof(int8_t))));
      const __m256i vi3xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (i3 + 16)));
      const __m256i vk3xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 112 * sizeof(int8_t))));
      i3 += 32;

      const __m256i vprod3x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi3x0123456789ABCDEF, vk3x0123456789ABCDEF);
      const __m256i vprod3x0123456789ABCDEFhi = _mm256_srai_epi16(vprod3x0123456789ABCDEFlo, 15);
      const __m256i vprod3xGHIJKLMNOPQRSTUVlo =  _mm256_mullo_epi16(vi3xGHIJKLMNOPQRSTUV, vk3xGHIJKLMNOPQRSTUV);
      const __m256i vprod3xGHIJKLMNOPQRSTUVhi = _mm256_srai_epi16(vprod3xGHIJKLMNOPQRSTUVlo, 15);

      vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod3x0123456789ABCDEFlo, vprod3x0123456789ABCDEFhi));
      vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod3x0123456789ABCDEFlo, vprod3x0123456789ABCDEFhi));
      vaccGHIJOPQR = _mm256_add_epi32(vaccGHIJOPQR, _mm256_unpacklo_epi16(vprod3xGHIJKLMNOPQRSTUVlo, vprod3xGHIJKLMNOPQRSTUVhi));
      vaccKLMNSTUV = _mm256_add_epi32(vaccKLMNSTUV, _mm256_unpackhi_epi16(vprod3xGHIJKLMNOPQRSTUVlo, vprod3xGHIJKLMNOPQRSTUVhi));

      const __m256i vi4x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i4));
      const __m256i vk4x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 128 * sizeof(int8_t))));
      const __m256i vi4xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (i4 + 16)));
      const __m256i vk4xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 144 * sizeof(int8_t))));
      i4 += 32;

      const __m256i vprod4x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi4x0123456789ABCDEF, vk4x0123456789ABCDEF);
      const __m256i vprod4x0123456789ABCDEFhi = _mm256_srai_epi16(vprod4x0123456789ABCDEFlo, 15);
      const __m256i vprod4xGHIJKLMNOPQRSTUVlo =  _mm256_mullo_epi16(vi4xGHIJKLMNOPQRSTUV, vk4xGHIJKLMNOPQRSTUV);
      const __m256i vprod4xGHIJKLMNOPQRSTUVhi = _mm256_srai_epi16(vprod4xGHIJKLMNOPQRSTUVlo, 15);

      vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod4x0123456789ABCDEFlo, vprod4x0123456789ABCDEFhi));
      vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod4x0123456789ABCDEFlo, vprod4x0123456789ABCDEFhi));
      vaccGHIJOPQR = _mm256_add_epi32(vaccGHIJOPQR, _mm256_unpacklo_epi16(vprod4xGHIJKLMNOPQRSTUVlo, vprod4xGHIJKLMNOPQRSTUVhi));
      vaccKLMNSTUV = _mm256_add_epi32(vaccKLMNSTUV, _mm256_unpackhi_epi16(vprod4xGHIJKLMNOPQRSTUVlo, vprod4xGHIJKLMNOPQRSTUVhi));

      const __m256i vi5x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i5));
      const __m256i vk5x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 160 * sizeof(int8_t))));
      const __m256i vi5xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (i5 + 16)));
      const __m256i vk5xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 176 * sizeof(int8_t))));
      i5 += 32;

      const __m256i vprod5x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi5x0123456789ABCDEF, vk5x0123456789ABCDEF);
      const __m256i vprod5x0123456789ABCDEFhi = _mm256_srai_epi16(vprod5x0123456789ABCDEFlo, 15);
      const __m256i vprod5xGHIJKLMNOPQRSTUVlo =  _mm256_mullo_epi16(vi5xGHIJKLMNOPQRSTUV, vk5xGHIJKLMNOPQRSTUV);
      const __m256i vprod5xGHIJKLMNOPQRSTUVhi = _mm256_srai_epi16(vprod5xGHIJKLMNOPQRSTUVlo, 15);

      vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod5x0123456789ABCDEFlo, vprod5x0123456789ABCDEFhi));
      vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod5x0123456789ABCDEFlo, vprod5x0123456789ABCDEFhi));
      vaccGHIJOPQR = _mm256_add_epi32(vaccGHIJOPQR, _mm256_unpacklo_epi16(vprod5xGHIJKLMNOPQRSTUVlo, vprod5xGHIJKLMNOPQRSTUVhi));
      vaccKLMNSTUV = _mm256_add_epi32(vaccKLMNSTUV, _mm256_unpackhi_epi16(vprod5xGHIJKLMNOPQRSTUVlo, vprod5xGHIJKLMNOPQRSTUVhi));

      const __m256i vi6x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i6));
      const __m256i vk6x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 192 * sizeof(int8_t))));
      const __m256i vi6xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (i6 + 16)));
      const __m256i vk6xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 208 * sizeof(int8_t))));
      i6 += 32;

      const __m256i vprod6x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi6x0123456789ABCDEF, vk6x0123456789ABCDEF);
      const __m256i vprod6x0123456789ABCDEFhi = _mm256_srai_epi16(vprod6x0123456789ABCDEFlo, 15);
      const __m256i vprod6xGHIJKLMNOPQRSTUVlo =  _mm256_mullo_epi16(vi6xGHIJKLMNOPQRSTUV, vk6xGHIJKLMNOPQRSTUV);
      const __m256i vprod6xGHIJKLMNOPQRSTUVhi = _mm256_srai_epi16(vprod6xGHIJKLMNOPQRSTUVlo, 15);

      vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod6x0123456789ABCDEFlo, vprod6x0123456789ABCDEFhi));
      vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod6x0123456789ABCDEFlo, vprod6x0123456789ABCDEFhi));
      vaccGHIJOPQR = _mm256_add_epi32(vaccGHIJOPQR, _mm256_unpacklo_epi16(vprod6xGHIJKLMNOPQRSTUVlo, vprod6xGHIJKLMNOPQRSTUVhi));
      vaccKLMNSTUV = _mm256_add_epi32(vaccKLMNSTUV, _mm256_unpackhi_epi16(vprod6xGHIJKLMNOPQRSTUVlo, vprod6xGHIJKLMNOPQRSTUVhi));

      const __m256i vi7x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i7));
      const __m256i vk7x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 224 * sizeof(int8_t))));
      const __m256i vi7xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (i7 + 16)));
      const __m256i vk7xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 240 * sizeof(int8_t))));
      i7 += 32;

      const __m256i vprod7x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi7x0123456789ABCDEF, vk7x0123456789ABCDEF);
      const __m256i vprod7x0123456789ABCDEFhi = _mm256_srai_epi16(vprod7x0123456789ABCDEFlo, 15);
      const __m256i vprod7xGHIJKLMNOPQRSTUVlo =  _mm256_mullo_epi16(vi7xGHIJKLMNOPQRSTUV, vk7xGHIJKLMNOPQRSTUV);
      const __m256i vprod7xGHIJKLMNOPQRSTUVhi = _mm256_srai_epi16(vprod7xGHIJKLMNOPQRSTUVlo, 15);

      vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod7x0123456789ABCDEFlo, vprod7x0123456789ABCDEFhi));
      vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod7x0123456789ABCDEFlo, vprod7x0123456789ABCDEFhi));
      vaccGHIJOPQR = _mm256_add_epi32(vaccGHIJOPQR, _mm256_unpacklo_epi16(vprod7xGHIJKLMNOPQRSTUVlo, vprod7xGHIJKLMNOPQRSTUVhi));
      vaccKLMNSTUV = _mm256_add_epi32(vaccKLMNSTUV, _mm256_unpackhi_epi16(vprod7xGHIJKLMNOPQRSTUVlo, vprod7xGHIJKLMNOPQRSTUVhi));

      const __m256i vi8x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i8));
      const __m256i vk8x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 256 * sizeof(int8_t))));
      const __m256i vi8xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (i8 + 16)));
      const __m256i vk8xGHIJKLMNOPQRSTUV = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) ((uintptr_t) w + 32 * sizeof(int32_t) + 272 * sizeof(int8_t))));
      i8 += 32;

      const __m256i vprod8x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi8x0123456789ABCDEF, vk8x0123456789ABCDEF);
      const __m256i vprod8x0123456789ABCDEFhi = _mm256_srai_epi16(vprod8x0123456789ABCDEFlo, 15);
      const __m256i vprod8xGHIJKLMNOPQRSTUVlo =  _mm256_mullo_epi16(vi8xGHIJKLMNOPQRSTUV, vk8xGHIJKLMNOPQRSTUV);
      const __m256i vprod8xGHIJKLMNOPQRSTUVhi = _mm256_srai_epi16(vprod8xGHIJKLMNOPQRSTUVlo, 15);

      vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod8x0123456789ABCDEFlo, vprod8x0123456789ABCDEFhi));
      vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod8x0123456789ABCDEFlo, vprod8x0123456789ABCDEFhi));
      vaccGHIJOPQR = _mm256_add_epi32(vaccGHIJOPQR, _mm256_unpacklo_epi16(vprod8xGHIJKLMNOPQRSTUVlo, vprod8xGHIJKLMNOPQRSTUVhi));
      vaccKLMNSTUV = _mm256_add_epi32(vaccKLMNSTUV, _mm256_unpackhi_epi16(vprod8xGHIJKLMNOPQRSTUVlo, vprod8xGHIJKLMNOPQRSTUVhi));

      w = (const void*) ((uintptr_t) w + 32 * sizeof(int32_t) + 288 * sizeof(int8_t));

      vacc01234567 = _mm256_inserti128_si256(vacc012389AB, _mm256_castsi256_si128(vacc4567CDEF), 1);
      vacc89ABCDEF = _mm256_permute2x128_si256(vacc012389AB, vacc4567CDEF, 0x31);
      vaccGHIJKLMN = _mm256_inserti128_si256(vaccGHIJOPQR, _mm256_castsi256_si128(vaccKLMNSTUV), 1);
      vaccOPQRSTUV = _mm256_permute2x128_si256(vaccGHIJOPQR, vaccKLMNSTUV, 0x31);

      __m256 vfpacc01234567 = _mm256_cvtepi32_ps(vacc01234567);
      __m256 vfpacc89ABCDEF = _mm256_cvtepi32_ps(vacc89ABCDEF);
      __m256 vfpaccGHIJKLMN = _mm256_cvtepi32_ps(vaccGHIJKLMN);
      __m256 vfpaccOPQRSTUV = _mm256_cvtepi32_ps(vaccOPQRSTUV);

      const __m256 vscale = _mm256_load_ps(params->fp32_avx2.scale);
      vfpacc01234567 = _mm256_mul_ps(vfpacc01234567, vscale);
      vfpacc89ABCDEF = _mm256_mul_ps(vfpacc89ABCDEF, vscale);
      vfpaccGHIJKLMN = _mm256_mul_ps(vfpaccGHIJKLMN, vscale);
      vfpaccOPQRSTUV = _mm256_mul_ps(vfpaccOPQRSTUV, vscale);

      const __m256 voutput_max_less_zero_point = _mm256_load_ps(params->fp32_avx2.output_max_less_zero_point);
      vfpacc01234567 = _mm256_min_ps(vfpacc01234567, voutput_max_less_zero_point);
      vfpacc89ABCDEF = _mm256_min_ps(vfpacc89ABCDEF, voutput_max_less_zero_point);
      vfpaccGHIJKLMN = _mm256_min_ps(vfpaccGHIJKLMN, voutput_max_less_zero_point);
      vfpaccOPQRSTUV = _mm256_min_ps(vfpaccOPQRSTUV, voutput_max_less_zero_point);

      vacc01234567 = _mm256_cvtps_epi32(vfpacc01234567);
      vacc89ABCDEF = _mm256_cvtps_epi32(vfpacc89ABCDEF);
      vaccGHIJKLMN = _mm256_cvtps_epi32(vfpaccGHIJKLMN);
      vaccOPQRSTUV = _mm256_cvtps_epi32(vfpaccOPQRSTUV);

      const __m256i voutput_zero_point = _mm256_load_si256((const __m256i*) params->fp32_avx2.output_zero_point);
      const __m256i vout012389AB4567CDEF = _mm256_adds_epi16(_mm256_packs_epi32(vacc01234567, vacc89ABCDEF), voutput_zero_point);
      const __m256i voutGHIJOPQRKLMNSTUV = _mm256_adds_epi16(_mm256_packs_epi32(vaccGHIJKLMN, vaccOPQRSTUV), voutput_zero_point);

      __m128i vout0123456789ABCDEF = _mm_shuffle_epi32(_mm_packs_epi16(_mm256_castsi256_si128(vout012389AB4567CDEF), _mm256_extracti128_si256(vout012389AB4567CDEF, 1)), _MM_SHUFFLE(3, 1, 2, 0));
      __m128i voutGHIJKLMNOPQRSTUV = _mm_shuffle_epi32(_mm_packs_epi16(_mm256_castsi256_si128(voutGHIJOPQRKLMNSTUV), _mm256_extracti128_si256(voutGHIJOPQRKLMNSTUV, 1)), _MM_SHUFFLE(3, 1, 2, 0));

      const __m128i voutput_min = _mm_load_si128((const __m128i*) params->fp32_avx2.output_min);
      vout0123456789ABCDEF = _mm_max_epi8(vout0123456789ABCDEF, voutput_min);
      voutGHIJKLMNOPQRSTUV = _mm_max_epi8(voutGHIJKLMNOPQRSTUV, voutput_min);

      _mm_storeu_si128((__m128i*) output, vout0123456789ABCDEF);
      _mm_storeu_si128((__m128i*) (output + 16), voutGHIJKLMNOPQRSTUV);
      output += 32;
    }
    if XNN_UNLIKELY(c != 0) {
      const int8_t* k = (const int8_t*) ((uintptr_t) w + 32 * sizeof(int32_t));
      do {
        __m256i vacc01234567 = _mm256_loadu_si256((const __m256i*) w);
        __m256i vacc89ABCDEF = _mm256_loadu_si256((const __m256i*) ((uintptr_t) w + 8 * sizeof(int32_t)));

        __m256i vacc012389AB = _mm256_inserti128_si256(vacc01234567, _mm256_castsi256_si128(vacc89ABCDEF), 1);
        __m256i vacc4567CDEF = _mm256_permute2x128_si256(vacc01234567, vacc89ABCDEF, 0x31);


        const __m256i vi0x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i0));
        const __m256i vk0x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) k));
        i0 += 16;

        const __m256i vprod0x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi0x0123456789ABCDEF, vk0x0123456789ABCDEF);
        const __m256i vprod0x0123456789ABCDEFhi = _mm256_srai_epi16(vprod0x0123456789ABCDEFlo, 15);

        vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod0x0123456789ABCDEFlo, vprod0x0123456789ABCDEFhi));
        vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod0x0123456789ABCDEFlo, vprod0x0123456789ABCDEFhi));

        const __m256i vi1x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i1));
        const __m256i vk1x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (k + 32)));
        i1 += 16;

        const __m256i vprod1x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi1x0123456789ABCDEF, vk1x0123456789ABCDEF);
        const __m256i vprod1x0123456789ABCDEFhi = _mm256_srai_epi16(vprod1x0123456789ABCDEFlo, 15);

        vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod1x0123456789ABCDEFlo, vprod1x0123456789ABCDEFhi));
        vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod1x0123456789ABCDEFlo, vprod1x0123456789ABCDEFhi));

        const __m256i vi2x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i2));
        const __m256i vk2x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (k + 64)));
        i2 += 16;

        const __m256i vprod2x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi2x0123456789ABCDEF, vk2x0123456789ABCDEF);
        const __m256i vprod2x0123456789ABCDEFhi = _mm256_srai_epi16(vprod2x0123456789ABCDEFlo, 15);

        vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod2x0123456789ABCDEFlo, vprod2x0123456789ABCDEFhi));
        vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod2x0123456789ABCDEFlo, vprod2x0123456789ABCDEFhi));

        const __m256i vi3x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i3));
        const __m256i vk3x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (k + 96)));
        i3 += 16;

        const __m256i vprod3x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi3x0123456789ABCDEF, vk3x0123456789ABCDEF);
        const __m256i vprod3x0123456789ABCDEFhi = _mm256_srai_epi16(vprod3x0123456789ABCDEFlo, 15);

        vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod3x0123456789ABCDEFlo, vprod3x0123456789ABCDEFhi));
        vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod3x0123456789ABCDEFlo, vprod3x0123456789ABCDEFhi));

        const __m256i vi4x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i4));
        const __m256i vk4x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (k + 128)));
        i4 += 16;

        const __m256i vprod4x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi4x0123456789ABCDEF, vk4x0123456789ABCDEF);
        const __m256i vprod4x0123456789ABCDEFhi = _mm256_srai_epi16(vprod4x0123456789ABCDEFlo, 15);

        vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod4x0123456789ABCDEFlo, vprod4x0123456789ABCDEFhi));
        vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod4x0123456789ABCDEFlo, vprod4x0123456789ABCDEFhi));

        const __m256i vi5x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i5));
        const __m256i vk5x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (k + 160)));
        i5 += 16;

        const __m256i vprod5x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi5x0123456789ABCDEF, vk5x0123456789ABCDEF);
        const __m256i vprod5x0123456789ABCDEFhi = _mm256_srai_epi16(vprod5x0123456789ABCDEFlo, 15);

        vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod5x0123456789ABCDEFlo, vprod5x0123456789ABCDEFhi));
        vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod5x0123456789ABCDEFlo, vprod5x0123456789ABCDEFhi));

        const __m256i vi6x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i6));
        const __m256i vk6x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (k + 192)));
        i6 += 16;

        const __m256i vprod6x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi6x0123456789ABCDEF, vk6x0123456789ABCDEF);
        const __m256i vprod6x0123456789ABCDEFhi = _mm256_srai_epi16(vprod6x0123456789ABCDEFlo, 15);

        vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod6x0123456789ABCDEFlo, vprod6x0123456789ABCDEFhi));
        vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod6x0123456789ABCDEFlo, vprod6x0123456789ABCDEFhi));

        const __m256i vi7x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i7));
        const __m256i vk7x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (k + 224)));
        i7 += 16;

        const __m256i vprod7x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi7x0123456789ABCDEF, vk7x0123456789ABCDEF);
        const __m256i vprod7x0123456789ABCDEFhi = _mm256_srai_epi16(vprod7x0123456789ABCDEFlo, 15);

        vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod7x0123456789ABCDEFlo, vprod7x0123456789ABCDEFhi));
        vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod7x0123456789ABCDEFlo, vprod7x0123456789ABCDEFhi));

        const __m256i vi8x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) i8));
        const __m256i vk8x0123456789ABCDEF = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i*) (k + 256)));
        i8 += 16;

        const __m256i vprod8x0123456789ABCDEFlo =  _mm256_mullo_epi16(vi8x0123456789ABCDEF, vk8x0123456789ABCDEF);
        const __m256i vprod8x0123456789ABCDEFhi = _mm256_srai_epi16(vprod8x0123456789ABCDEFlo, 15);

        vacc012389AB = _mm256_add_epi32(vacc012389AB, _mm256_unpacklo_epi16(vprod8x0123456789ABCDEFlo, vprod8x0123456789ABCDEFhi));
        vacc4567CDEF = _mm256_add_epi32(vacc4567CDEF, _mm256_unpackhi_epi16(vprod8x0123456789ABCDEFlo, vprod8x0123456789ABCDEFhi));

        vacc01234567 = _mm256_inserti128_si256(vacc012389AB, _mm256_castsi256_si128(vacc4567CDEF), 1);
        vacc89ABCDEF = _mm256_permute2x128_si256(vacc012389AB, vacc4567CDEF, 0x31);

        k += 16;

        __m256 vfpacc01234567 = _mm256_cvtepi32_ps(vacc01234567);
        __m256 vfpacc89ABCDEF = _mm256_cvtepi32_ps(vacc89ABCDEF);

        const __m256 vscale = _mm256_load_ps(params->fp32_avx2.scale);
        vfpacc01234567 = _mm256_mul_ps(vfpacc01234567, vscale);
        vfpacc89ABCDEF = _mm256_mul_ps(vfpacc89ABCDEF, vscale);

        const __m256 voutput_max_less_zero_point = _mm256_load_ps(params->fp32_avx2.output_max_less_zero_point);
        vfpacc01234567 = _mm256_min_ps(vfpacc01234567, voutput_max_less_zero_point);
        vfpacc89ABCDEF = _mm256_min_ps(vfpacc89ABCDEF, voutput_max_less_zero_point);

        vacc01234567 = _mm256_cvtps_epi32(vfpacc01234567);
        vacc89ABCDEF = _mm256_cvtps_epi32(vfpacc89ABCDEF);

        w = (const void*) ((uintptr_t) w + 16 * sizeof(int32_t));

        const __m128i voutput_zero_point = _mm_load_si128((const __m128i*) params->fp32_avx2.output_zero_point);
        __m128i vout01234567 = _mm_adds_epi16(_mm_packs_epi32(_mm256_castsi256_si128(vacc01234567), _mm256_extracti128_si256(vacc01234567, 1)), voutput_zero_point);
        __m128i vout89ABCDEF = _mm_adds_epi16(_mm_packs_epi32(_mm256_castsi256_si128(vacc89ABCDEF), _mm256_extracti128_si256(vacc89ABCDEF, 1)), voutput_zero_point);

        const __m128i voutput_min = _mm_load_si128((const __m128i*) params->fp32_avx2.output_min);

        __m128i vout0123456789ABCDEF = _mm_packs_epi16(vout01234567, vout89ABCDEF);
        vout0123456789ABCDEF = _mm_max_epi8(vout0123456789ABCDEF, voutput_min);

        if XNN_LIKELY(c >= 16) {
          _mm_storeu_si128((__m128i*) output, vout0123456789ABCDEF);
          output += 16;
          c -= 16;
        } else {
          if (c & 8) {
            _mm_storel_epi64((__m128i*) output, vout0123456789ABCDEF);
            vout0123456789ABCDEF = _mm_unpackhi_epi64(vout0123456789ABCDEF, vout0123456789ABCDEF);
            output += 8;
          }
          if (c & 4) {
            *((uint32_t*) output) = (uint32_t) _mm_cvtsi128_si32(vout0123456789ABCDEF);
            vout0123456789ABCDEF = _mm_srli_epi64(vout0123456789ABCDEF, 32);
            output += 4;
          }
          if (c & 2) {
            *((uint16_t*) output) = (uint16_t) _mm_extract_epi16(vout0123456789ABCDEF, 0);
            vout0123456789ABCDEF = _mm_srli_epi32(vout0123456789ABCDEF, 16);
            output += 2;
          }
          if (c & 1) {
            *output = (int8_t) _mm_extract_epi8(vout0123456789ABCDEF, 0);
            output += 1;
          }
          c = 0;
        }
      } while (c != 0);
    }

    output = (int8_t*) ((uintptr_t) output + output_increment);
  } while (--output_width != 0);
}
