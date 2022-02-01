// Auto-generated file. Do not edit!
//   Template: src/x8-lut/avx2.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <immintrin.h>

#include <xnnpack/intrinsics-polyfill.h>
#include <xnnpack/lut.h>
#include <xnnpack/common.h>


void xnn_x8_lut_ukernel__avx2_x64(
    size_t n,
    const uint8_t* x,
    uint8_t* y,
    const uint8_t t[restrict XNN_MIN_ELEMENTS(256)])
{
  assert(n != 0);
  assert(x != NULL);
  assert(y != NULL);

  const __m256i vt0 = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) t));
  const __m256i vt1 = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 16)));
  const __m256i vt2 = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 32)));
  const __m256i vt3 = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 48)));
  const __m256i vt4 = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 64)));
  const __m256i vt5 = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 80)));
  const __m256i vt6 = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 96)));
  const __m256i vt7 = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 112)));
  const __m256i vt8 = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 128)));
  const __m256i vt9 = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 144)));
  const __m256i vtA = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 160)));
  const __m256i vtB = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 176)));
  const __m256i vtC = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 192)));
  const __m256i vtD = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 208)));
  const __m256i vtE = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 224)));
  const __m256i vtF = _mm256_broadcastsi128_si256(_mm_load_si128((const __m128i*) (t + 240)));

  const __m256i vtable0 = vt0;
  const __m256i vtable1 = _mm256_xor_si256(vt0, vt1);
  const __m256i vtable2 = _mm256_xor_si256(vt1, vt2);
  const __m256i vtable3 = _mm256_xor_si256(vt2, vt3);
  const __m256i vtable4 = _mm256_xor_si256(vt3, vt4);
  const __m256i vtable5 = _mm256_xor_si256(vt4, vt5);
  const __m256i vtable6 = _mm256_xor_si256(vt5, vt6);
  const __m256i vtable7 = _mm256_xor_si256(vt6, vt7);
  const __m256i vtable8 = _mm256_xor_si256(_mm256_xor_si256(vt7, vt8), vtable0);
  const __m256i vtable9 = _mm256_xor_si256(_mm256_xor_si256(vt8, vt9), vtable1);
  const __m256i vtableA = _mm256_xor_si256(_mm256_xor_si256(vt9, vtA), vtable2);
  const __m256i vtableB = _mm256_xor_si256(_mm256_xor_si256(vtA, vtB), vtable3);
  const __m256i vtableC = _mm256_xor_si256(_mm256_xor_si256(vtB, vtC), vtable4);
  const __m256i vtableD = _mm256_xor_si256(_mm256_xor_si256(vtC, vtD), vtable5);
  const __m256i vtableE = _mm256_xor_si256(_mm256_xor_si256(vtD, vtE), vtable6);
  const __m256i vtableF = _mm256_xor_si256(_mm256_xor_si256(vtE, vtF), vtable7);

  const __m256i voffset = _mm256_set1_epi8(16);
  for (; n >= 64 * sizeof(uint8_t); n -= 64 * sizeof(uint8_t)) {
    __m256i vx0 = _mm256_loadu_si256((const __m256i*) x);
    __m256i vx1 = _mm256_loadu_si256((const __m256i*) (x + 32));
    x += 64;

    __m256i vy0 = _mm256_shuffle_epi8(vtable0, vx0);
    __m256i vy1 = _mm256_shuffle_epi8(vtable0, vx1);

    vx0 = _mm256_sub_epi8(vx0, voffset);
    vx1 = _mm256_sub_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtable1, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtable1, vx1));
    vx0 = _mm256_sub_epi8(vx0, voffset);
    vx1 = _mm256_sub_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtable2, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtable2, vx1));
    vx0 = _mm256_sub_epi8(vx0, voffset);
    vx1 = _mm256_sub_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtable3, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtable3, vx1));
    vx0 = _mm256_sub_epi8(vx0, voffset);
    vx1 = _mm256_sub_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtable4, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtable4, vx1));
    vx0 = _mm256_sub_epi8(vx0, voffset);
    vx1 = _mm256_sub_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtable5, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtable5, vx1));
    vx0 = _mm256_sub_epi8(vx0, voffset);
    vx1 = _mm256_sub_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtable6, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtable6, vx1));
    vx0 = _mm256_sub_epi8(vx0, voffset);
    vx1 = _mm256_sub_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtable7, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtable7, vx1));
    vx0 = _mm256_sub_epi8(vx0, voffset);
    vx1 = _mm256_sub_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtable8, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtable8, vx1));

    vx0 = _mm256_subs_epi8(vx0, voffset);
    vx1 = _mm256_subs_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtable9, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtable9, vx1));
    vx0 = _mm256_subs_epi8(vx0, voffset);
    vx1 = _mm256_subs_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtableA, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtableA, vx1));
    vx0 = _mm256_subs_epi8(vx0, voffset);
    vx1 = _mm256_subs_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtableB, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtableB, vx1));
    vx0 = _mm256_subs_epi8(vx0, voffset);
    vx1 = _mm256_subs_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtableC, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtableC, vx1));
    vx0 = _mm256_subs_epi8(vx0, voffset);
    vx1 = _mm256_subs_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtableD, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtableD, vx1));
    vx0 = _mm256_subs_epi8(vx0, voffset);
    vx1 = _mm256_subs_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtableE, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtableE, vx1));
    vx0 = _mm256_subs_epi8(vx0, voffset);
    vx1 = _mm256_subs_epi8(vx1, voffset);
    vy0 = _mm256_xor_si256(vy0, _mm256_shuffle_epi8(vtableF, vx0));
    vy1 = _mm256_xor_si256(vy1, _mm256_shuffle_epi8(vtableF, vx1));

    _mm256_storeu_si256((__m256i*) y, vy0);
    _mm256_storeu_si256((__m256i*) (y + 32), vy1);
    y += 64;
  }
  for (; n >= 16 * sizeof(uint8_t); n -= 16 * sizeof(uint8_t)) {
    __m128i vx = _mm_loadu_si128((const __m128i*) x);
    x += 16;

    __m128i vy = _mm_shuffle_epi8(_mm256_castsi256_si128(vtable0), vx);

    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable1), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable2), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable3), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable4), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable5), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable6), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable7), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable8), vx));

    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable9), vx));
    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtableA), vx));
    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtableB), vx));
    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtableC), vx));
    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtableD), vx));
    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtableE), vx));
    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtableF), vx));

    _mm_storeu_si128((__m128i*) y, vy);
    y += 16;
  }
  if XNN_UNLIKELY(n != 0) {
    __m128i vx = _mm_loadu_si128((const __m128i*) x);

    __m128i vy = _mm_shuffle_epi8(_mm256_castsi256_si128(vtable0), vx);

    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable1), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable2), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable3), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable4), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable5), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable6), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable7), vx));
    vx = _mm_sub_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable8), vx));

    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtable9), vx));
    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtableA), vx));
    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtableB), vx));
    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtableC), vx));
    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtableD), vx));
    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtableE), vx));
    vx = _mm_subs_epi8(vx, _mm256_castsi256_si128(voffset));
    vy = _mm_xor_si128(vy, _mm_shuffle_epi8(_mm256_castsi256_si128(vtableF), vx));

    if (n & (8 * sizeof(uint8_t))) {
      _mm_storel_epi64((__m128i*) y, vy);
      vy = _mm_unpackhi_epi64(vy, vy);
      y += 8;
    }
    if (n & (4 * sizeof(uint8_t))) {
      _mm_storeu_si32(y, vy);
      vy = _mm_srli_epi64(vy, 32);
      y += 4;
    }
    if (n & (2 * sizeof(uint8_t))) {
      *((uint16_t*) y) = (uint16_t) _mm_extract_epi16(vy, 0);
      vy = _mm_srli_epi32(vy, 16);
      y += 2;
    }
    if (n & (1 * sizeof(uint8_t))) {
      *y = (uint8_t) _mm_extract_epi8(vy, 0);
    }
  }
}
