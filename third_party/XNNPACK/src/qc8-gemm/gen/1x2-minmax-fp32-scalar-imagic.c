// Auto-generated file. Do not edit!
//   Template: src/qs8-gemm/scalar.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <fp16.h>

#include <xnnpack/math.h>
#include <xnnpack/gemm.h>


void xnn_qc8_gemm_minmax_fp32_ukernel_1x2__scalar_imagic(
    size_t mr,
    size_t nc,
    size_t kc,
    const int8_t* restrict a,
    size_t a_stride,
    const void* restrict w,
    int8_t* restrict c,
    size_t cm_stride,
    size_t cn_stride,
    const union xnn_qs8_minmax_params params[restrict XNN_MIN_ELEMENTS(1)])
{
  assert(mr != 0);
  assert(mr <= 1);
  assert(nc != 0);
  assert(kc != 0);

  const int8_t* a0 = a;
  int8_t* c0 = c;

  do {
    int32_t vacc0x0 = ((const int32_t*) w)[0];
    int32_t vacc0x1 = ((const int32_t*) w)[1];
    w = (const void*) ((const int32_t*) w + 2);

    size_t k = kc;
    do {
      const int32_t va0 = (int32_t) *a0++;

      const int32_t vb0 = (int32_t) ((const int8_t*) w)[0];
      const int32_t vb1 = (int32_t) ((const int8_t*) w)[1];
      w = (const void*) ((const int8_t*) w + 2);

      vacc0x0 += va0 * vb0;
      vacc0x1 += va0 * vb1;

      k -= sizeof(int8_t);
    } while (k != 0);

    float vfpacc0x0 = (float) vacc0x0;
    float vfpacc0x1 = (float) vacc0x1;

    typedef XNN_UNALIGNED float unaligned_float;
    const float vscale0 = ((const unaligned_float*) w)[0];
    vfpacc0x0 *= vscale0;
    const float vscale1 = ((const unaligned_float*) w)[1];
    vfpacc0x1 *= vscale1;
    w = (const void*) ((const float*) w + 2);

    const float vmagic_bias = params->scalar_imagic.magic_bias;
    vfpacc0x0 += vmagic_bias;
    vfpacc0x1 += vmagic_bias;

    int32_t vout0x0 = (int32_t) fp32_to_bits(vfpacc0x0);
    int32_t vout0x1 = (int32_t) fp32_to_bits(vfpacc0x1);

    const int32_t vmagic_min = params->scalar_imagic.magic_min;
    vout0x0 = math_max_s32(vout0x0, vmagic_min);
    vout0x1 = math_max_s32(vout0x1, vmagic_min);

    const int32_t vmagic_max = params->scalar_imagic.magic_max;
    vout0x0 = math_min_s32(vout0x0, vmagic_max);
    vout0x1 = math_min_s32(vout0x1, vmagic_max);

    const int32_t vmagic_bias_less_zero_point = params->scalar_imagic.magic_bias_less_zero_point;
    vout0x0 -= vmagic_bias_less_zero_point;
    vout0x1 -= vmagic_bias_less_zero_point;

    if XNN_LIKELY(nc >= 2) {
      c0[0] = (int8_t) vout0x0;
      c0[1] = (int8_t) vout0x1;

      a0 = (const int8_t*) ((uintptr_t) a0 - kc);

      c0 = (int8_t*) ((uintptr_t) c0 + cn_stride);

      nc -= 2;
    } else {
      if (nc & 1) {
        c0[0] = (int8_t) vout0x0;
      }

      nc = 0;
    }
  } while (nc != 0);
}
