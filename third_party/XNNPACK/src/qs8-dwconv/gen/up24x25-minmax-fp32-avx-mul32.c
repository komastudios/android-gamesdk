// Auto-generated file. Do not edit!
//   Template: src/qs8-dwconv/unipass-sse-mul32.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <immintrin.h>

#include <xnnpack/dwconv.h>
#include <xnnpack/intrinsics-polyfill.h>


void xnn_qs8_dwconv_minmax_fp32_ukernel_up24x25__avx_mul32(
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


      const __m128i vi0x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i0));
      const __m128i vk0x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 0 * sizeof(int8_t))));
      const __m128i vi0x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i0 + 4));
      const __m128i vk0x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 4 * sizeof(int8_t))));
      const __m128i vi0x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i0 + 8));
      const __m128i vk0x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 8 * sizeof(int8_t))));
      const __m128i vi0xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i0 + 12));
      const __m128i vk0xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 12 * sizeof(int8_t))));
      const __m128i vi0xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i0 + 16));
      const __m128i vk0xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 16 * sizeof(int8_t))));
      const __m128i vi0xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i0 + 20));
      const __m128i vk0xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 20 * sizeof(int8_t))));
      i0 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi0x0123, vk0x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi0x4567, vk0x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi0x89AB, vk0x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi0xCDEF, vk0xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi0xGHIJ, vk0xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi0xKLMN, vk0xKLMN));

      const __m128i vi1x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i1));
      const __m128i vk1x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 24 * sizeof(int8_t))));
      const __m128i vi1x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i1 + 4));
      const __m128i vk1x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 28 * sizeof(int8_t))));
      const __m128i vi1x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i1 + 8));
      const __m128i vk1x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 32 * sizeof(int8_t))));
      const __m128i vi1xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i1 + 12));
      const __m128i vk1xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 36 * sizeof(int8_t))));
      const __m128i vi1xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i1 + 16));
      const __m128i vk1xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 40 * sizeof(int8_t))));
      const __m128i vi1xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i1 + 20));
      const __m128i vk1xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 44 * sizeof(int8_t))));
      i1 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi1x0123, vk1x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi1x4567, vk1x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi1x89AB, vk1x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi1xCDEF, vk1xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi1xGHIJ, vk1xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi1xKLMN, vk1xKLMN));

      const __m128i vi2x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i2));
      const __m128i vk2x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 48 * sizeof(int8_t))));
      const __m128i vi2x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i2 + 4));
      const __m128i vk2x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 52 * sizeof(int8_t))));
      const __m128i vi2x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i2 + 8));
      const __m128i vk2x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 56 * sizeof(int8_t))));
      const __m128i vi2xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i2 + 12));
      const __m128i vk2xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 60 * sizeof(int8_t))));
      const __m128i vi2xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i2 + 16));
      const __m128i vk2xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 64 * sizeof(int8_t))));
      const __m128i vi2xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i2 + 20));
      const __m128i vk2xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 68 * sizeof(int8_t))));
      i2 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi2x0123, vk2x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi2x4567, vk2x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi2x89AB, vk2x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi2xCDEF, vk2xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi2xGHIJ, vk2xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi2xKLMN, vk2xKLMN));

      const __m128i vi3x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i3));
      const __m128i vk3x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 72 * sizeof(int8_t))));
      const __m128i vi3x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i3 + 4));
      const __m128i vk3x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 76 * sizeof(int8_t))));
      const __m128i vi3x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i3 + 8));
      const __m128i vk3x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 80 * sizeof(int8_t))));
      const __m128i vi3xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i3 + 12));
      const __m128i vk3xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 84 * sizeof(int8_t))));
      const __m128i vi3xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i3 + 16));
      const __m128i vk3xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 88 * sizeof(int8_t))));
      const __m128i vi3xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i3 + 20));
      const __m128i vk3xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 92 * sizeof(int8_t))));
      i3 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi3x0123, vk3x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi3x4567, vk3x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi3x89AB, vk3x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi3xCDEF, vk3xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi3xGHIJ, vk3xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi3xKLMN, vk3xKLMN));

      const __m128i vi4x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i4));
      const __m128i vk4x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 96 * sizeof(int8_t))));
      const __m128i vi4x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i4 + 4));
      const __m128i vk4x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 100 * sizeof(int8_t))));
      const __m128i vi4x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i4 + 8));
      const __m128i vk4x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 104 * sizeof(int8_t))));
      const __m128i vi4xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i4 + 12));
      const __m128i vk4xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 108 * sizeof(int8_t))));
      const __m128i vi4xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i4 + 16));
      const __m128i vk4xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 112 * sizeof(int8_t))));
      const __m128i vi4xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i4 + 20));
      const __m128i vk4xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 116 * sizeof(int8_t))));
      i4 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi4x0123, vk4x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi4x4567, vk4x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi4x89AB, vk4x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi4xCDEF, vk4xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi4xGHIJ, vk4xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi4xKLMN, vk4xKLMN));

      const __m128i vi5x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i5));
      const __m128i vk5x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 120 * sizeof(int8_t))));
      const __m128i vi5x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i5 + 4));
      const __m128i vk5x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 124 * sizeof(int8_t))));
      const __m128i vi5x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i5 + 8));
      const __m128i vk5x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 128 * sizeof(int8_t))));
      const __m128i vi5xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i5 + 12));
      const __m128i vk5xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 132 * sizeof(int8_t))));
      const __m128i vi5xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i5 + 16));
      const __m128i vk5xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 136 * sizeof(int8_t))));
      const __m128i vi5xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i5 + 20));
      const __m128i vk5xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 140 * sizeof(int8_t))));
      i5 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi5x0123, vk5x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi5x4567, vk5x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi5x89AB, vk5x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi5xCDEF, vk5xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi5xGHIJ, vk5xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi5xKLMN, vk5xKLMN));

      const __m128i vi6x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i6));
      const __m128i vk6x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 144 * sizeof(int8_t))));
      const __m128i vi6x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i6 + 4));
      const __m128i vk6x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 148 * sizeof(int8_t))));
      const __m128i vi6x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i6 + 8));
      const __m128i vk6x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 152 * sizeof(int8_t))));
      const __m128i vi6xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i6 + 12));
      const __m128i vk6xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 156 * sizeof(int8_t))));
      const __m128i vi6xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i6 + 16));
      const __m128i vk6xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 160 * sizeof(int8_t))));
      const __m128i vi6xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i6 + 20));
      const __m128i vk6xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 164 * sizeof(int8_t))));
      i6 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi6x0123, vk6x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi6x4567, vk6x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi6x89AB, vk6x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi6xCDEF, vk6xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi6xGHIJ, vk6xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi6xKLMN, vk6xKLMN));

      const __m128i vi7x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i7));
      const __m128i vk7x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 168 * sizeof(int8_t))));
      const __m128i vi7x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i7 + 4));
      const __m128i vk7x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 172 * sizeof(int8_t))));
      const __m128i vi7x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i7 + 8));
      const __m128i vk7x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 176 * sizeof(int8_t))));
      const __m128i vi7xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i7 + 12));
      const __m128i vk7xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 180 * sizeof(int8_t))));
      const __m128i vi7xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i7 + 16));
      const __m128i vk7xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 184 * sizeof(int8_t))));
      const __m128i vi7xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i7 + 20));
      const __m128i vk7xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 188 * sizeof(int8_t))));
      i7 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi7x0123, vk7x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi7x4567, vk7x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi7x89AB, vk7x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi7xCDEF, vk7xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi7xGHIJ, vk7xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi7xKLMN, vk7xKLMN));

      const __m128i vi8x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i8));
      const __m128i vk8x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 192 * sizeof(int8_t))));
      const __m128i vi8x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i8 + 4));
      const __m128i vk8x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 196 * sizeof(int8_t))));
      const __m128i vi8x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i8 + 8));
      const __m128i vk8x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 200 * sizeof(int8_t))));
      const __m128i vi8xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i8 + 12));
      const __m128i vk8xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 204 * sizeof(int8_t))));
      const __m128i vi8xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i8 + 16));
      const __m128i vk8xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 208 * sizeof(int8_t))));
      const __m128i vi8xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i8 + 20));
      const __m128i vk8xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 212 * sizeof(int8_t))));
      i8 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi8x0123, vk8x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi8x4567, vk8x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi8x89AB, vk8x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi8xCDEF, vk8xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi8xGHIJ, vk8xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi8xKLMN, vk8xKLMN));

      const __m128i vi9x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i9));
      const __m128i vk9x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 216 * sizeof(int8_t))));
      const __m128i vi9x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i9 + 4));
      const __m128i vk9x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 220 * sizeof(int8_t))));
      const __m128i vi9x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i9 + 8));
      const __m128i vk9x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 224 * sizeof(int8_t))));
      const __m128i vi9xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i9 + 12));
      const __m128i vk9xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 228 * sizeof(int8_t))));
      const __m128i vi9xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i9 + 16));
      const __m128i vk9xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 232 * sizeof(int8_t))));
      const __m128i vi9xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i9 + 20));
      const __m128i vk9xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 236 * sizeof(int8_t))));
      i9 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi9x0123, vk9x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi9x4567, vk9x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi9x89AB, vk9x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi9xCDEF, vk9xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi9xGHIJ, vk9xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi9xKLMN, vk9xKLMN));

      const __m128i vi10x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i10));
      const __m128i vk10x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 240 * sizeof(int8_t))));
      const __m128i vi10x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i10 + 4));
      const __m128i vk10x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 244 * sizeof(int8_t))));
      const __m128i vi10x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i10 + 8));
      const __m128i vk10x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 248 * sizeof(int8_t))));
      const __m128i vi10xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i10 + 12));
      const __m128i vk10xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 252 * sizeof(int8_t))));
      const __m128i vi10xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i10 + 16));
      const __m128i vk10xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 256 * sizeof(int8_t))));
      const __m128i vi10xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i10 + 20));
      const __m128i vk10xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 260 * sizeof(int8_t))));
      i10 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi10x0123, vk10x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi10x4567, vk10x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi10x89AB, vk10x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi10xCDEF, vk10xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi10xGHIJ, vk10xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi10xKLMN, vk10xKLMN));

      const __m128i vi11x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i11));
      const __m128i vk11x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 264 * sizeof(int8_t))));
      const __m128i vi11x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i11 + 4));
      const __m128i vk11x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 268 * sizeof(int8_t))));
      const __m128i vi11x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i11 + 8));
      const __m128i vk11x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 272 * sizeof(int8_t))));
      const __m128i vi11xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i11 + 12));
      const __m128i vk11xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 276 * sizeof(int8_t))));
      const __m128i vi11xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i11 + 16));
      const __m128i vk11xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 280 * sizeof(int8_t))));
      const __m128i vi11xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i11 + 20));
      const __m128i vk11xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 284 * sizeof(int8_t))));
      i11 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi11x0123, vk11x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi11x4567, vk11x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi11x89AB, vk11x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi11xCDEF, vk11xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi11xGHIJ, vk11xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi11xKLMN, vk11xKLMN));

      const __m128i vi12x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i12));
      const __m128i vk12x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 288 * sizeof(int8_t))));
      const __m128i vi12x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i12 + 4));
      const __m128i vk12x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 292 * sizeof(int8_t))));
      const __m128i vi12x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i12 + 8));
      const __m128i vk12x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 296 * sizeof(int8_t))));
      const __m128i vi12xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i12 + 12));
      const __m128i vk12xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 300 * sizeof(int8_t))));
      const __m128i vi12xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i12 + 16));
      const __m128i vk12xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 304 * sizeof(int8_t))));
      const __m128i vi12xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i12 + 20));
      const __m128i vk12xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 308 * sizeof(int8_t))));
      i12 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi12x0123, vk12x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi12x4567, vk12x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi12x89AB, vk12x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi12xCDEF, vk12xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi12xGHIJ, vk12xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi12xKLMN, vk12xKLMN));

      const __m128i vi13x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i13));
      const __m128i vk13x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 312 * sizeof(int8_t))));
      const __m128i vi13x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i13 + 4));
      const __m128i vk13x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 316 * sizeof(int8_t))));
      const __m128i vi13x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i13 + 8));
      const __m128i vk13x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 320 * sizeof(int8_t))));
      const __m128i vi13xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i13 + 12));
      const __m128i vk13xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 324 * sizeof(int8_t))));
      const __m128i vi13xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i13 + 16));
      const __m128i vk13xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 328 * sizeof(int8_t))));
      const __m128i vi13xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i13 + 20));
      const __m128i vk13xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 332 * sizeof(int8_t))));
      i13 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi13x0123, vk13x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi13x4567, vk13x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi13x89AB, vk13x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi13xCDEF, vk13xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi13xGHIJ, vk13xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi13xKLMN, vk13xKLMN));

      const __m128i vi14x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i14));
      const __m128i vk14x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 336 * sizeof(int8_t))));
      const __m128i vi14x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i14 + 4));
      const __m128i vk14x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 340 * sizeof(int8_t))));
      const __m128i vi14x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i14 + 8));
      const __m128i vk14x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 344 * sizeof(int8_t))));
      const __m128i vi14xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i14 + 12));
      const __m128i vk14xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 348 * sizeof(int8_t))));
      const __m128i vi14xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i14 + 16));
      const __m128i vk14xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 352 * sizeof(int8_t))));
      const __m128i vi14xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i14 + 20));
      const __m128i vk14xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 356 * sizeof(int8_t))));
      i14 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi14x0123, vk14x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi14x4567, vk14x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi14x89AB, vk14x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi14xCDEF, vk14xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi14xGHIJ, vk14xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi14xKLMN, vk14xKLMN));

      const __m128i vi15x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i15));
      const __m128i vk15x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 360 * sizeof(int8_t))));
      const __m128i vi15x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i15 + 4));
      const __m128i vk15x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 364 * sizeof(int8_t))));
      const __m128i vi15x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i15 + 8));
      const __m128i vk15x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 368 * sizeof(int8_t))));
      const __m128i vi15xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i15 + 12));
      const __m128i vk15xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 372 * sizeof(int8_t))));
      const __m128i vi15xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i15 + 16));
      const __m128i vk15xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 376 * sizeof(int8_t))));
      const __m128i vi15xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i15 + 20));
      const __m128i vk15xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 380 * sizeof(int8_t))));
      i15 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi15x0123, vk15x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi15x4567, vk15x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi15x89AB, vk15x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi15xCDEF, vk15xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi15xGHIJ, vk15xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi15xKLMN, vk15xKLMN));

      const __m128i vi16x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i16));
      const __m128i vk16x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 384 * sizeof(int8_t))));
      const __m128i vi16x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i16 + 4));
      const __m128i vk16x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 388 * sizeof(int8_t))));
      const __m128i vi16x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i16 + 8));
      const __m128i vk16x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 392 * sizeof(int8_t))));
      const __m128i vi16xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i16 + 12));
      const __m128i vk16xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 396 * sizeof(int8_t))));
      const __m128i vi16xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i16 + 16));
      const __m128i vk16xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 400 * sizeof(int8_t))));
      const __m128i vi16xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i16 + 20));
      const __m128i vk16xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 404 * sizeof(int8_t))));
      i16 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi16x0123, vk16x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi16x4567, vk16x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi16x89AB, vk16x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi16xCDEF, vk16xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi16xGHIJ, vk16xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi16xKLMN, vk16xKLMN));

      const __m128i vi17x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i17));
      const __m128i vk17x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 408 * sizeof(int8_t))));
      const __m128i vi17x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i17 + 4));
      const __m128i vk17x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 412 * sizeof(int8_t))));
      const __m128i vi17x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i17 + 8));
      const __m128i vk17x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 416 * sizeof(int8_t))));
      const __m128i vi17xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i17 + 12));
      const __m128i vk17xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 420 * sizeof(int8_t))));
      const __m128i vi17xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i17 + 16));
      const __m128i vk17xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 424 * sizeof(int8_t))));
      const __m128i vi17xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i17 + 20));
      const __m128i vk17xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 428 * sizeof(int8_t))));
      i17 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi17x0123, vk17x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi17x4567, vk17x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi17x89AB, vk17x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi17xCDEF, vk17xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi17xGHIJ, vk17xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi17xKLMN, vk17xKLMN));

      const __m128i vi18x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i18));
      const __m128i vk18x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 432 * sizeof(int8_t))));
      const __m128i vi18x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i18 + 4));
      const __m128i vk18x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 436 * sizeof(int8_t))));
      const __m128i vi18x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i18 + 8));
      const __m128i vk18x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 440 * sizeof(int8_t))));
      const __m128i vi18xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i18 + 12));
      const __m128i vk18xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 444 * sizeof(int8_t))));
      const __m128i vi18xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i18 + 16));
      const __m128i vk18xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 448 * sizeof(int8_t))));
      const __m128i vi18xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i18 + 20));
      const __m128i vk18xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 452 * sizeof(int8_t))));
      i18 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi18x0123, vk18x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi18x4567, vk18x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi18x89AB, vk18x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi18xCDEF, vk18xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi18xGHIJ, vk18xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi18xKLMN, vk18xKLMN));

      const __m128i vi19x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i19));
      const __m128i vk19x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 456 * sizeof(int8_t))));
      const __m128i vi19x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i19 + 4));
      const __m128i vk19x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 460 * sizeof(int8_t))));
      const __m128i vi19x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i19 + 8));
      const __m128i vk19x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 464 * sizeof(int8_t))));
      const __m128i vi19xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i19 + 12));
      const __m128i vk19xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 468 * sizeof(int8_t))));
      const __m128i vi19xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i19 + 16));
      const __m128i vk19xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 472 * sizeof(int8_t))));
      const __m128i vi19xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i19 + 20));
      const __m128i vk19xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 476 * sizeof(int8_t))));
      i19 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi19x0123, vk19x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi19x4567, vk19x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi19x89AB, vk19x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi19xCDEF, vk19xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi19xGHIJ, vk19xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi19xKLMN, vk19xKLMN));

      const __m128i vi20x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i20));
      const __m128i vk20x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 480 * sizeof(int8_t))));
      const __m128i vi20x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i20 + 4));
      const __m128i vk20x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 484 * sizeof(int8_t))));
      const __m128i vi20x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i20 + 8));
      const __m128i vk20x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 488 * sizeof(int8_t))));
      const __m128i vi20xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i20 + 12));
      const __m128i vk20xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 492 * sizeof(int8_t))));
      const __m128i vi20xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i20 + 16));
      const __m128i vk20xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 496 * sizeof(int8_t))));
      const __m128i vi20xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i20 + 20));
      const __m128i vk20xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 500 * sizeof(int8_t))));
      i20 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi20x0123, vk20x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi20x4567, vk20x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi20x89AB, vk20x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi20xCDEF, vk20xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi20xGHIJ, vk20xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi20xKLMN, vk20xKLMN));

      const __m128i vi21x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i21));
      const __m128i vk21x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 504 * sizeof(int8_t))));
      const __m128i vi21x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i21 + 4));
      const __m128i vk21x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 508 * sizeof(int8_t))));
      const __m128i vi21x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i21 + 8));
      const __m128i vk21x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 512 * sizeof(int8_t))));
      const __m128i vi21xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i21 + 12));
      const __m128i vk21xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 516 * sizeof(int8_t))));
      const __m128i vi21xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i21 + 16));
      const __m128i vk21xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 520 * sizeof(int8_t))));
      const __m128i vi21xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i21 + 20));
      const __m128i vk21xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 524 * sizeof(int8_t))));
      i21 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi21x0123, vk21x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi21x4567, vk21x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi21x89AB, vk21x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi21xCDEF, vk21xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi21xGHIJ, vk21xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi21xKLMN, vk21xKLMN));

      const __m128i vi22x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i22));
      const __m128i vk22x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 528 * sizeof(int8_t))));
      const __m128i vi22x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i22 + 4));
      const __m128i vk22x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 532 * sizeof(int8_t))));
      const __m128i vi22x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i22 + 8));
      const __m128i vk22x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 536 * sizeof(int8_t))));
      const __m128i vi22xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i22 + 12));
      const __m128i vk22xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 540 * sizeof(int8_t))));
      const __m128i vi22xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i22 + 16));
      const __m128i vk22xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 544 * sizeof(int8_t))));
      const __m128i vi22xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i22 + 20));
      const __m128i vk22xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 548 * sizeof(int8_t))));
      i22 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi22x0123, vk22x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi22x4567, vk22x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi22x89AB, vk22x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi22xCDEF, vk22xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi22xGHIJ, vk22xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi22xKLMN, vk22xKLMN));

      const __m128i vi23x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i23));
      const __m128i vk23x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 552 * sizeof(int8_t))));
      const __m128i vi23x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i23 + 4));
      const __m128i vk23x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 556 * sizeof(int8_t))));
      const __m128i vi23x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i23 + 8));
      const __m128i vk23x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 560 * sizeof(int8_t))));
      const __m128i vi23xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i23 + 12));
      const __m128i vk23xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 564 * sizeof(int8_t))));
      const __m128i vi23xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i23 + 16));
      const __m128i vk23xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 568 * sizeof(int8_t))));
      const __m128i vi23xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i23 + 20));
      const __m128i vk23xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 572 * sizeof(int8_t))));
      i23 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi23x0123, vk23x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi23x4567, vk23x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi23x89AB, vk23x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi23xCDEF, vk23xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi23xGHIJ, vk23xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi23xKLMN, vk23xKLMN));

      const __m128i vi24x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i24));
      const __m128i vk24x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 576 * sizeof(int8_t))));
      const __m128i vi24x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32(i24 + 4));
      const __m128i vk24x4567 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 580 * sizeof(int8_t))));
      const __m128i vi24x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32(i24 + 8));
      const __m128i vk24x89AB = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 584 * sizeof(int8_t))));
      const __m128i vi24xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32(i24 + 12));
      const __m128i vk24xCDEF = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 588 * sizeof(int8_t))));
      const __m128i vi24xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32(i24 + 16));
      const __m128i vk24xGHIJ = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 592 * sizeof(int8_t))));
      const __m128i vi24xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32(i24 + 20));
      const __m128i vk24xKLMN = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 596 * sizeof(int8_t))));
      i24 += 24;

      vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi24x0123, vk24x0123));
      vacc4567 = _mm_add_epi32(vacc4567, _mm_mullo_epi32(vi24x4567, vk24x4567));
      vacc89AB = _mm_add_epi32(vacc89AB, _mm_mullo_epi32(vi24x89AB, vk24x89AB));
      vaccCDEF = _mm_add_epi32(vaccCDEF, _mm_mullo_epi32(vi24xCDEF, vk24xCDEF));
      vaccGHIJ = _mm_add_epi32(vaccGHIJ, _mm_mullo_epi32(vi24xGHIJ, vk24xGHIJ));
      vaccKLMN = _mm_add_epi32(vaccKLMN, _mm_mullo_epi32(vi24xKLMN, vk24xKLMN));

      w = (const void*) ((uintptr_t) w + 24 * sizeof(int32_t) + 600 * sizeof(int8_t));

      __m128 vscaled0123 = _mm_cvtepi32_ps(vacc0123);
      __m128 vscaled4567 = _mm_cvtepi32_ps(vacc4567);
      __m128 vscaled89AB = _mm_cvtepi32_ps(vacc89AB);
      __m128 vscaledCDEF = _mm_cvtepi32_ps(vaccCDEF);
      __m128 vscaledGHIJ = _mm_cvtepi32_ps(vaccGHIJ);
      __m128 vscaledKLMN = _mm_cvtepi32_ps(vaccKLMN);

      const __m128 vscale = _mm_load_ps(params->fp32_sse4.scale);
      vscaled0123 = _mm_mul_ps(vscaled0123, vscale);
      vscaled4567 = _mm_mul_ps(vscaled4567, vscale);
      vscaled89AB = _mm_mul_ps(vscaled89AB, vscale);
      vscaledCDEF = _mm_mul_ps(vscaledCDEF, vscale);
      vscaledGHIJ = _mm_mul_ps(vscaledGHIJ, vscale);
      vscaledKLMN = _mm_mul_ps(vscaledKLMN, vscale);

      const __m128 voutput_max_less_zero_point = _mm_load_ps(params->fp32_sse4.output_max_less_zero_point);
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

      const __m128i voutput_zero_point = _mm_load_si128((const __m128i*) params->fp32_sse4.output_zero_point);
      __m128i vout01234567 = _mm_adds_epi16(_mm_packs_epi32(vacc0123, vacc4567), voutput_zero_point);
      __m128i vout89ABCDEF = _mm_adds_epi16(_mm_packs_epi32(vacc89AB, vaccCDEF), voutput_zero_point);
      __m128i voutGHIJKLMN = _mm_adds_epi16(_mm_packs_epi32(vaccGHIJ, vaccKLMN), voutput_zero_point);

      const __m128i voutput_min = _mm_load_si128((const __m128i*) params->fp32_sse4.output_min);
      __m128i vout0123456789ABCDEF = _mm_packs_epi16(vout01234567, vout89ABCDEF);
      vout0123456789ABCDEF = _mm_max_epi8(vout0123456789ABCDEF, voutput_min);
      __m128i voutGHIJKLMNGHIJKLMN = _mm_packs_epi16(voutGHIJKLMN, voutGHIJKLMN);
      voutGHIJKLMNGHIJKLMN = _mm_max_epi8(voutGHIJKLMNGHIJKLMN, voutput_min);

      _mm_storeu_si128((__m128i*) output, vout0123456789ABCDEF);
      _mm_storel_epi64((__m128i*) (output + 16), voutGHIJKLMNGHIJKLMN);
      output += 24;
    }
    if XNN_UNLIKELY(c != 0) {
      const int8_t* k = (const int8_t*) ((const int32_t*) w + 24);
      do {
        __m128i vacc0123 = _mm_loadu_si128((const __m128i*) w);

        const __m128i vi0x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i0));
        const __m128i vk0x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(k));
        i0 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi0x0123, vk0x0123));
        const __m128i vi1x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i1));
        const __m128i vk1x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 24)));
        i1 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi1x0123, vk1x0123));
        const __m128i vi2x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i2));
        const __m128i vk2x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 48)));
        i2 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi2x0123, vk2x0123));
        const __m128i vi3x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i3));
        const __m128i vk3x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 72)));
        i3 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi3x0123, vk3x0123));
        const __m128i vi4x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i4));
        const __m128i vk4x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 96)));
        i4 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi4x0123, vk4x0123));
        const __m128i vi5x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i5));
        const __m128i vk5x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 120)));
        i5 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi5x0123, vk5x0123));
        const __m128i vi6x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i6));
        const __m128i vk6x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 144)));
        i6 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi6x0123, vk6x0123));
        const __m128i vi7x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i7));
        const __m128i vk7x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 168)));
        i7 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi7x0123, vk7x0123));
        const __m128i vi8x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i8));
        const __m128i vk8x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 192)));
        i8 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi8x0123, vk8x0123));
        const __m128i vi9x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i9));
        const __m128i vk9x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 216)));
        i9 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi9x0123, vk9x0123));
        const __m128i vi10x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i10));
        const __m128i vk10x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 240)));
        i10 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi10x0123, vk10x0123));
        const __m128i vi11x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i11));
        const __m128i vk11x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 264)));
        i11 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi11x0123, vk11x0123));
        const __m128i vi12x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i12));
        const __m128i vk12x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 288)));
        i12 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi12x0123, vk12x0123));
        const __m128i vi13x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i13));
        const __m128i vk13x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 312)));
        i13 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi13x0123, vk13x0123));
        const __m128i vi14x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i14));
        const __m128i vk14x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 336)));
        i14 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi14x0123, vk14x0123));
        const __m128i vi15x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i15));
        const __m128i vk15x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 360)));
        i15 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi15x0123, vk15x0123));
        const __m128i vi16x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i16));
        const __m128i vk16x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 384)));
        i16 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi16x0123, vk16x0123));
        const __m128i vi17x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i17));
        const __m128i vk17x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 408)));
        i17 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi17x0123, vk17x0123));
        const __m128i vi18x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i18));
        const __m128i vk18x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 432)));
        i18 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi18x0123, vk18x0123));
        const __m128i vi19x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i19));
        const __m128i vk19x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 456)));
        i19 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi19x0123, vk19x0123));
        const __m128i vi20x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i20));
        const __m128i vk20x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 480)));
        i20 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi20x0123, vk20x0123));
        const __m128i vi21x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i21));
        const __m128i vk21x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 504)));
        i21 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi21x0123, vk21x0123));
        const __m128i vi22x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i22));
        const __m128i vk22x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 528)));
        i22 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi22x0123, vk22x0123));
        const __m128i vi23x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i23));
        const __m128i vk23x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 552)));
        i23 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi23x0123, vk23x0123));
        const __m128i vi24x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32(i24));
        const __m128i vk24x0123 = _mm_cvtepi8_epi32(_mm_loadu_si32((const void*) (k + 576)));
        i24 += 4;

        vacc0123 = _mm_add_epi32(vacc0123, _mm_mullo_epi32(vi24x0123, vk24x0123));

        k += 4;

        __m128 vscaled0123 = _mm_cvtepi32_ps(vacc0123);
        vscaled0123 = _mm_mul_ps(vscaled0123, _mm_load_ps(params->fp32_sse4.scale));
        vscaled0123 = _mm_min_ps(vscaled0123, _mm_load_ps(params->fp32_sse4.output_max_less_zero_point));
        vacc0123 = _mm_cvtps_epi32(vscaled0123);

        w = (const void*) ((const int32_t*) w + 4);

        const __m128i voutput_zero_point = _mm_load_si128((const __m128i*) params->fp32_sse4.output_zero_point);
        __m128i vout0123 = _mm_adds_epi16(_mm_packs_epi32(vacc0123, vacc0123), voutput_zero_point);

        vout0123 = _mm_packs_epi16(vout0123, vout0123);
        vout0123 = _mm_max_epi8(vout0123, _mm_load_si128((const __m128i*) params->fp32_sse4.output_min));

        if XNN_LIKELY(c >= 4) {
          _mm_storeu_si32(output, vout0123);
          output += 4;
          c -= 4;
        } else {
          if (c & 2) {
            *((uint16_t*) output) = (uint16_t) _mm_extract_epi16(vout0123, 0);
            vout0123 = _mm_srli_epi32(vout0123, 16);
            output += 2;
          }
          if (c & 1) {
            *output = (int8_t) _mm_extract_epi8(vout0123, 0);
            output += 1;
          }
          c = 0;
        }
      } while (c != 0);
    }

    output = (int8_t*) ((uintptr_t) output + output_increment);
  } while (--output_width != 0);
}
