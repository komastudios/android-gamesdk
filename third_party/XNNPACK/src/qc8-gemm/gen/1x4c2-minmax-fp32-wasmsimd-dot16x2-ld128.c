// Auto-generated file. Do not edit!
//   Template: src/qs8-gemm/MRx4c2-wasmsimd-dot16x2.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <wasm_simd128.h>

#include <xnnpack/gemm.h>
#include <xnnpack/math.h>



void xnn_qc8_gemm_minmax_fp32_ukernel_1x4c2__wasmsimd_dot16x2_ld128(
    size_t mr,
    size_t nc,
    size_t kc,
    const int8_t* restrict a,
    size_t a_stride,
    const void* restrict w,
    int8_t* restrict c,
    size_t cm_stride,
    size_t cn_stride,
    const union xnn_qs8_minmax_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(mr != 0);
  assert(mr <= 1);
  assert(nc != 0);
  assert(kc != 0);
  assert(kc % sizeof(int8_t) == 0);
  assert(a != NULL);
  assert(w != NULL);
  assert(c != NULL);

  kc = round_up_po2(kc, 2);
  const int8_t* a0 = a;
  int8_t* c0 = c;

  do {
    v128_t vacc0x0123 = wasm_v128_load(w);
    w = (const void*) ((const int32_t*) w + 4);

    size_t k = kc;
    while (k >= 8 * sizeof(int8_t)) {
      const v128_t vxa0 = wasm_i16x8_load8x8((const v128_t*) a0);
      a0 += 8;

      const v128_t vb01 = wasm_v128_load(w);
      const v128_t vxb0 = wasm_i16x8_extend_low_i8x16(vb01);
      const v128_t vxb1 = wasm_i16x8_extend_high_i8x16(vb01);

      vacc0x0123 = wasm_i32x4_add(vacc0x0123,
        wasm_i32x4_dot_i16x8(wasm_v32x4_shuffle(vxa0, vxa0, 0, 0, 0, 0), vxb0));

      vacc0x0123 = wasm_i32x4_add(vacc0x0123,
        wasm_i32x4_dot_i16x8(wasm_v32x4_shuffle(vxa0, vxa0, 1, 1, 1, 1), vxb1));
      const v128_t vb23 = wasm_v128_load((const int8_t*) w + 16);
      const v128_t vxb2 = wasm_i16x8_extend_low_i8x16(vb23);
      const v128_t vxb3 = wasm_i16x8_extend_high_i8x16(vb23);

      vacc0x0123 = wasm_i32x4_add(vacc0x0123,
        wasm_i32x4_dot_i16x8(wasm_v32x4_shuffle(vxa0, vxa0, 2, 2, 2, 2), vxb2));

      vacc0x0123 = wasm_i32x4_add(vacc0x0123,
        wasm_i32x4_dot_i16x8(wasm_v32x4_shuffle(vxa0, vxa0, 3, 3, 3, 3), vxb3));

      w = (const void*) ((const int8_t*) w + 32);
      k -= 8 * sizeof(int8_t);
    }
    if (k != 0) {
      const v128_t vxa0 = wasm_i16x8_load8x8(a0);
      a0 = (const int8_t*) ((uintptr_t) a0 + k);

      const v128_t vxb0 = wasm_i16x8_load8x8(w);
      w = (const void*) ((const int8_t*) w + 8);

      vacc0x0123 = wasm_i32x4_add(vacc0x0123,
        wasm_i32x4_dot_i16x8(wasm_v32x4_shuffle(vxa0, vxa0, 0, 0, 0, 0), vxb0));

      if (k > 2 * sizeof(int8_t)) {
        const v128_t vxb1 = wasm_i16x8_load8x8(w);
        w = (const void*) ((const int8_t*) w + 8);

        vacc0x0123 = wasm_i32x4_add(vacc0x0123,
          wasm_i32x4_dot_i16x8(wasm_v32x4_shuffle(vxa0, vxa0, 1, 1, 1, 1), vxb1));

        if (k > 4 * sizeof(int8_t)) {
          const v128_t vxb2 = wasm_i16x8_load8x8(w);
          w = (const void*) ((const int8_t*) w + 8);

          vacc0x0123 = wasm_i32x4_add(vacc0x0123,
            wasm_i32x4_dot_i16x8(wasm_v32x4_shuffle(vxa0, vxa0, 2, 2, 2, 2), vxb2));
        }
      }
    }

    vacc0x0123 = wasm_f32x4_convert_i32x4(vacc0x0123);

    const v128_t vscale0123 = wasm_v128_load(w);
    w = (const void*) ((const float*) w + 4);
    vacc0x0123 = wasm_f32x4_mul(vacc0x0123, vscale0123);

    const v128_t vmagic_bias = wasm_v128_load64_splat(params->wasmsimd.magic_bias);
    vacc0x0123 = wasm_f32x4_add(vacc0x0123, vmagic_bias);

    const v128_t vmagic_min = wasm_v128_load64_splat(params->wasmsimd.magic_min);
    vacc0x0123 = wasm_i32x4_max(vacc0x0123, vmagic_min);

    const v128_t vmagic_bias_less_output_zero_point = wasm_v128_load64_splat(params->wasmsimd.magic_bias_less_output_zero_point);
    vacc0x0123 = wasm_i32x4_sub(vacc0x0123, vmagic_bias_less_output_zero_point);

    v128_t vacc00x0123 = wasm_i16x8_narrow_i32x4(vacc0x0123, vacc0x0123);

    v128_t vout = wasm_i8x16_narrow_i16x8(vacc00x0123, vacc00x0123);

    const v128_t voutput_max = wasm_v128_load64_splat(params->wasmsimd.output_max);
    vout = wasm_i8x16_min(vout, voutput_max);

    if (nc >= 4) {
      *((float*) c0) = (float) wasm_f32x4_extract_lane(vout, 0);

      c0 = (int8_t*) ((uintptr_t) c0 + cn_stride);

      a0 = (const int8_t*) ((uintptr_t) a0 - kc);

      nc -= 4;
    } else {
      uint32_t vout0 = wasm_i32x4_extract_lane(vout, 0);
      if (nc & 2) {
        *((uint16_t*) c0) = (uint16_t) vout0;
        vout0 >>= 16;
        c0 += 2;
      }
      if (nc & 1) {
        *c0 = (int8_t) vout0;
      }

      nc = 0;
    }
  } while (nc != 0);
}
