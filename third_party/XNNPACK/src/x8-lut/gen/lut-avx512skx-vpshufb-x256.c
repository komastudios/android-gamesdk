// Auto-generated file. Do not edit!
//   Template: src/x8-lut/avx512skx-vpshufb.c.in
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


void xnn_x8_lut_ukernel__avx512skx_vpshufb_x256(
    size_t n,
    const uint8_t* x,
    uint8_t* y,
    const uint8_t t[restrict XNN_MIN_ELEMENTS(256)])
{
  assert(n != 0);
  assert(x != NULL);
  assert(y != NULL);

  const __m512i vt0 = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) t));
  const __m512i vt1 = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 16)));
  const __m512i vt2 = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 32)));
  const __m512i vt3 = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 48)));
  const __m512i vt4 = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 64)));
  const __m512i vt5 = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 80)));
  const __m512i vt6 = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 96)));
  const __m512i vt7 = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 112)));
  const __m512i vt8 = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 128)));
  const __m512i vt9 = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 144)));
  const __m512i vtA = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 160)));
  const __m512i vtB = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 176)));
  const __m512i vtC = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 192)));
  const __m512i vtD = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 208)));
  const __m512i vtE = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 224)));
  const __m512i vtF = _mm512_broadcast_i32x4(_mm_load_si128((const __m128i*) (t + 240)));

  const __m512i vtable0 = vt0;
  const __m512i vtable1 = _mm512_xor_si512(vt0, vt1);
  const __m512i vtable2 = _mm512_xor_si512(vt1, vt2);
  const __m512i vtable3 = _mm512_xor_si512(vt2, vt3);
  const __m512i vtable4 = _mm512_xor_si512(vt3, vt4);
  const __m512i vtable5 = _mm512_xor_si512(vt4, vt5);
  const __m512i vtable6 = _mm512_xor_si512(vt5, vt6);
  const __m512i vtable7 = _mm512_xor_si512(vt6, vt7);
  const __m512i vtable8 = _mm512_xor_si512(_mm512_xor_si512(vt7, vt8), vtable0);
  const __m512i vtable9 = _mm512_xor_si512(_mm512_xor_si512(vt8, vt9), vtable1);
  const __m512i vtableA = _mm512_xor_si512(_mm512_xor_si512(vt9, vtA), vtable2);
  const __m512i vtableB = _mm512_xor_si512(_mm512_xor_si512(vtA, vtB), vtable3);
  const __m512i vtableC = _mm512_xor_si512(_mm512_xor_si512(vtB, vtC), vtable4);
  const __m512i vtableD = _mm512_xor_si512(_mm512_xor_si512(vtC, vtD), vtable5);
  const __m512i vtableE = _mm512_xor_si512(_mm512_xor_si512(vtD, vtE), vtable6);
  const __m512i vtableF = _mm512_xor_si512(_mm512_xor_si512(vtE, vtF), vtable7);

  const __m512i voffset = _mm512_set1_epi8(16);
  for (; n >= 256 * sizeof(uint8_t); n -= 256 * sizeof(uint8_t)) {
    __m512i vx0 = _mm512_loadu_si512(x);
    __m512i vx1 = _mm512_loadu_si512(x + 64);
    __m512i vx2 = _mm512_loadu_si512(x + 128);
    __m512i vx3 = _mm512_loadu_si512(x + 192);
    x += 256;

    __m512i vy0 = _mm512_shuffle_epi8(vtable0, vx0);
    __m512i vy1 = _mm512_shuffle_epi8(vtable0, vx1);
    __m512i vy2 = _mm512_shuffle_epi8(vtable0, vx2);
    __m512i vy3 = _mm512_shuffle_epi8(vtable0, vx3);

    vx0 = _mm512_sub_epi8(vx0, voffset);
    vx1 = _mm512_sub_epi8(vx1, voffset);
    vx2 = _mm512_sub_epi8(vx2, voffset);
    vx3 = _mm512_sub_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtable1, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtable1, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtable1, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtable1, vx3));
    vx0 = _mm512_sub_epi8(vx0, voffset);
    vx1 = _mm512_sub_epi8(vx1, voffset);
    vx2 = _mm512_sub_epi8(vx2, voffset);
    vx3 = _mm512_sub_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtable2, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtable2, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtable2, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtable2, vx3));
    vx0 = _mm512_sub_epi8(vx0, voffset);
    vx1 = _mm512_sub_epi8(vx1, voffset);
    vx2 = _mm512_sub_epi8(vx2, voffset);
    vx3 = _mm512_sub_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtable3, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtable3, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtable3, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtable3, vx3));
    vx0 = _mm512_sub_epi8(vx0, voffset);
    vx1 = _mm512_sub_epi8(vx1, voffset);
    vx2 = _mm512_sub_epi8(vx2, voffset);
    vx3 = _mm512_sub_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtable4, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtable4, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtable4, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtable4, vx3));
    vx0 = _mm512_sub_epi8(vx0, voffset);
    vx1 = _mm512_sub_epi8(vx1, voffset);
    vx2 = _mm512_sub_epi8(vx2, voffset);
    vx3 = _mm512_sub_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtable5, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtable5, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtable5, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtable5, vx3));
    vx0 = _mm512_sub_epi8(vx0, voffset);
    vx1 = _mm512_sub_epi8(vx1, voffset);
    vx2 = _mm512_sub_epi8(vx2, voffset);
    vx3 = _mm512_sub_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtable6, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtable6, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtable6, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtable6, vx3));
    vx0 = _mm512_sub_epi8(vx0, voffset);
    vx1 = _mm512_sub_epi8(vx1, voffset);
    vx2 = _mm512_sub_epi8(vx2, voffset);
    vx3 = _mm512_sub_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtable7, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtable7, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtable7, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtable7, vx3));
    vx0 = _mm512_sub_epi8(vx0, voffset);
    vx1 = _mm512_sub_epi8(vx1, voffset);
    vx2 = _mm512_sub_epi8(vx2, voffset);
    vx3 = _mm512_sub_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtable8, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtable8, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtable8, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtable8, vx3));

    vx0 = _mm512_subs_epi8(vx0, voffset);
    vx1 = _mm512_subs_epi8(vx1, voffset);
    vx2 = _mm512_subs_epi8(vx2, voffset);
    vx3 = _mm512_subs_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtable9, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtable9, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtable9, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtable9, vx3));
    vx0 = _mm512_subs_epi8(vx0, voffset);
    vx1 = _mm512_subs_epi8(vx1, voffset);
    vx2 = _mm512_subs_epi8(vx2, voffset);
    vx3 = _mm512_subs_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtableA, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtableA, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtableA, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtableA, vx3));
    vx0 = _mm512_subs_epi8(vx0, voffset);
    vx1 = _mm512_subs_epi8(vx1, voffset);
    vx2 = _mm512_subs_epi8(vx2, voffset);
    vx3 = _mm512_subs_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtableB, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtableB, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtableB, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtableB, vx3));
    vx0 = _mm512_subs_epi8(vx0, voffset);
    vx1 = _mm512_subs_epi8(vx1, voffset);
    vx2 = _mm512_subs_epi8(vx2, voffset);
    vx3 = _mm512_subs_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtableC, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtableC, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtableC, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtableC, vx3));
    vx0 = _mm512_subs_epi8(vx0, voffset);
    vx1 = _mm512_subs_epi8(vx1, voffset);
    vx2 = _mm512_subs_epi8(vx2, voffset);
    vx3 = _mm512_subs_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtableD, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtableD, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtableD, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtableD, vx3));
    vx0 = _mm512_subs_epi8(vx0, voffset);
    vx1 = _mm512_subs_epi8(vx1, voffset);
    vx2 = _mm512_subs_epi8(vx2, voffset);
    vx3 = _mm512_subs_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtableE, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtableE, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtableE, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtableE, vx3));
    vx0 = _mm512_subs_epi8(vx0, voffset);
    vx1 = _mm512_subs_epi8(vx1, voffset);
    vx2 = _mm512_subs_epi8(vx2, voffset);
    vx3 = _mm512_subs_epi8(vx3, voffset);
    vy0 = _mm512_xor_si512(vy0, _mm512_shuffle_epi8(vtableF, vx0));
    vy1 = _mm512_xor_si512(vy1, _mm512_shuffle_epi8(vtableF, vx1));
    vy2 = _mm512_xor_si512(vy2, _mm512_shuffle_epi8(vtableF, vx2));
    vy3 = _mm512_xor_si512(vy3, _mm512_shuffle_epi8(vtableF, vx3));

    _mm512_storeu_si512(y, vy0);
    _mm512_storeu_si512(y + 64, vy1);
    _mm512_storeu_si512(y + 128, vy2);
    _mm512_storeu_si512(y + 192, vy3);
    y += 256;
  }
  for (; n >= 64 * sizeof(uint8_t); n -= 64 * sizeof(uint8_t)) {
    __m512i vx = _mm512_loadu_si512(x);
    x += 64;

    __m512i vy = _mm512_shuffle_epi8(vtable0, vx);

    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable1, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable2, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable3, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable4, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable5, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable6, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable7, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable8, vx));

    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable9, vx));
    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtableA, vx));
    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtableB, vx));
    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtableC, vx));
    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtableD, vx));
    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtableE, vx));
    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtableF, vx));

    _mm512_storeu_si512(y, vy);
    y += 64;
  }
  if XNN_UNLIKELY(n != 0) {
    assert(n < 64);
    const __mmask64 vmask = _cvtu64_mask64((uint64_t) ((UINT64_C(1) << n) - UINT64_C(1)));

    __m512i vx = _mm512_maskz_loadu_epi8(vmask, x);

    __m512i vy = _mm512_shuffle_epi8(vtable0, vx);

    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable1, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable2, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable3, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable4, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable5, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable6, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable7, vx));
    vx = _mm512_sub_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable8, vx));

    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtable9, vx));
    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtableA, vx));
    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtableB, vx));
    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtableC, vx));
    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtableD, vx));
    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtableE, vx));
    vx = _mm512_subs_epi8(vx, voffset);
    vy = _mm512_xor_si512(vy, _mm512_shuffle_epi8(vtableF, vx));

    _mm512_mask_storeu_epi8(y, vmask, vy);
  }
}
