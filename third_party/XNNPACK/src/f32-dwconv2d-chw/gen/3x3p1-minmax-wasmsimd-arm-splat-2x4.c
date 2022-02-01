// Auto-generated file. Do not edit!
//   Template: src/f32-dwconv2d-chw/3x3p1-wasmsimd-splat.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <wasm_simd128.h>

#include <xnnpack/dwconv.h>
#include <xnnpack/math.h>



void xnn_f32_dwconv2d_chw_ukernel_3x3p1__wasmsimd_arm_splat_2x4(
    size_t input_height,
    size_t input_width,
    const float* input,
    const float* weights,
    const float* zero,
    float* output,
    uint32_t padding_top,
    const union xnn_f32_chw_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(input_height != 0);
  assert(input_width != 0);
  assert(input_width % sizeof(float) == 0);
  assert(padding_top == 1);

  const v128_t vmask = wasm_v128_load(params->scalar.mask);
  const v128_t vmax = wasm_v128_load32_splat(&params->scalar.max);
  const v128_t vmin = wasm_v128_load32_splat(&params->scalar.min);

  const v128_t vw0123 = wasm_v128_load(weights);
  const v128_t vw4567 = wasm_v128_load(weights + 4);
  const v128_t vw89 = wasm_v128_load64_splat(weights + 8);

  const size_t input_decrement = round_up_po2(input_width, 4 * sizeof(float));

  const float* i0 = zero;
  const float* i1 = input;
  const float* i2 = (const float*) ((uintptr_t) i1 + input_width);
  const float* i3 = (const float*) ((uintptr_t) i2 + input_width);

  float* o0 = output;
  float* o1 = (float*) ((uintptr_t) o0 + input_width);

  size_t output_height = input_height;
  do {
    if XNN_UNPREDICTABLE(output_height < 2) {
      i2 = zero;
      o1 = o0;
    }
    if XNN_UNPREDICTABLE(output_height < 3) {
      i3 = zero;
    }

    v128_t vi0x0123 = wasm_f32x4_const_splat(0.0f);
    v128_t vi1x0123 = wasm_f32x4_const_splat(0.0f);
    v128_t vi2x0123 = wasm_f32x4_const_splat(0.0f);
    v128_t vi3x0123 = wasm_f32x4_const_splat(0.0f);

    v128_t vi0x4567 = wasm_v128_load(i0); i0 += 4;
    v128_t vi1x4567 = wasm_v128_load(i1); i1 += 4;
    v128_t vi2x4567 = wasm_v128_load(i2); i2 += 4;
    v128_t vi3x4567 = wasm_v128_load(i3); i3 += 4;

    size_t w = input_width;
    for (; w > 4 * sizeof(float); w -= 4 * sizeof(float)) {
      v128_t vo0p0 = wasm_v32x4_shuffle(vw0123, vw0123, 0, 0, 0, 0);
      v128_t vo1p0 = wasm_v32x4_shuffle(vw0123, vw0123, 0, 0, 0, 0);

      const v128_t vi0x89AB = wasm_v128_load(i0); i0 += 4;
      const v128_t vi1x89AB = wasm_v128_load(i1); i1 += 4;
      const v128_t vi2x89AB = wasm_v128_load(i2); i2 += 4;
      const v128_t vi3x89AB = wasm_v128_load(i3); i3 += 4;

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi0x4567, wasm_v32x4_shuffle(vw0123, vw0123, 2, 2, 2, 2)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi1x4567, wasm_v32x4_shuffle(vw0123, vw0123, 2, 2, 2, 2)));

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi1x4567, wasm_v32x4_shuffle(vw4567, vw4567, 1, 1, 1, 1)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi2x4567, wasm_v32x4_shuffle(vw4567, vw4567, 1, 1, 1, 1)));

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi2x4567, wasm_v32x4_shuffle(vw89, vw89, 0, 0, 0, 0)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi3x4567, wasm_v32x4_shuffle(vw89, vw89, 0, 0, 0, 0)));

      const v128_t vi0x3456 = wasm_v32x4_shuffle(vi0x0123, vi0x4567, 3, 4, 5, 6);
      const v128_t vi1x3456 = wasm_v32x4_shuffle(vi1x0123, vi1x4567, 3, 4, 5, 6);
      const v128_t vi2x3456 = wasm_v32x4_shuffle(vi2x0123, vi2x4567, 3, 4, 5, 6);
      const v128_t vi3x3456 = wasm_v32x4_shuffle(vi3x0123, vi3x4567, 3, 4, 5, 6);

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi0x3456, wasm_v32x4_shuffle(vw0123, vw0123, 1, 1, 1, 1)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi1x3456, wasm_v32x4_shuffle(vw0123, vw0123, 1, 1, 1, 1)));

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi1x3456, wasm_v32x4_shuffle(vw4567, vw4567, 0, 0, 0, 0)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi2x3456, wasm_v32x4_shuffle(vw4567, vw4567, 0, 0, 0, 0)));

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi2x3456, wasm_v32x4_shuffle(vw4567, vw4567, 3, 3, 3, 3)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi3x3456, wasm_v32x4_shuffle(vw4567, vw4567, 3, 3, 3, 3)));

      vi0x0123 = vi0x4567;
      vi1x0123 = vi1x4567;
      vi2x0123 = vi2x4567;
      vi3x0123 = vi3x4567;

      const v128_t vi0x5678 = wasm_v32x4_shuffle(vi0x4567, vi0x89AB, 1, 2, 3, 4);
      const v128_t vi1x5678 = wasm_v32x4_shuffle(vi1x4567, vi1x89AB, 1, 2, 3, 4);
      const v128_t vi2x5678 = wasm_v32x4_shuffle(vi2x4567, vi2x89AB, 1, 2, 3, 4);
      const v128_t vi3x5678 = wasm_v32x4_shuffle(vi3x4567, vi3x89AB, 1, 2, 3, 4);

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi0x5678, wasm_v32x4_shuffle(vw0123, vw0123, 3, 3, 3, 3)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi1x5678, wasm_v32x4_shuffle(vw0123, vw0123, 3, 3, 3, 3)));

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi1x5678, wasm_v32x4_shuffle(vw4567, vw4567, 2, 2, 2, 2)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi2x5678, wasm_v32x4_shuffle(vw4567, vw4567, 2, 2, 2, 2)));

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi2x5678, wasm_v32x4_shuffle(vw89, vw89, 1, 1, 1, 1)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi3x5678, wasm_v32x4_shuffle(vw89, vw89, 1, 1, 1, 1)));

      vi0x4567 = vi0x89AB;
      vi1x4567 = vi1x89AB;
      vi2x4567 = vi2x89AB;
      vi3x4567 = vi3x89AB;


      v128_t vo0 = wasm_f32x4_max(vo0p0, vmin);
      v128_t vo1 = wasm_f32x4_max(vo1p0, vmin);
      vo0 = wasm_f32x4_min(vo0, vmax);
      vo1 = wasm_f32x4_min(vo1, vmax);

      wasm_v128_store(o1, vo1); o1 += 4;
      wasm_v128_store(o0, vo0); o0 += 4;
    }
    // Always process the last block of 1..4 pixels.
    assert(w >= 1 * sizeof(float));
    assert(w <= 4 * sizeof(float));
    {
      v128_t vo0p0 = wasm_v32x4_shuffle(vw0123, vw0123, 0, 0, 0, 0);
      v128_t vo1p0 = wasm_v32x4_shuffle(vw0123, vw0123, 0, 0, 0, 0);

      vi0x4567 = wasm_v128_and(vmask, vi0x4567);
      vi1x4567 = wasm_v128_and(vmask, vi1x4567);
      vi2x4567 = wasm_v128_and(vmask, vi2x4567);
      vi3x4567 = wasm_v128_and(vmask, vi3x4567);

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi0x4567, wasm_v32x4_shuffle(vw0123, vw0123, 2, 2, 2, 2)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi1x4567, wasm_v32x4_shuffle(vw0123, vw0123, 2, 2, 2, 2)));

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi1x4567, wasm_v32x4_shuffle(vw4567, vw4567, 1, 1, 1, 1)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi2x4567, wasm_v32x4_shuffle(vw4567, vw4567, 1, 1, 1, 1)));

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi2x4567, wasm_v32x4_shuffle(vw89, vw89, 0, 0, 0, 0)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi3x4567, wasm_v32x4_shuffle(vw89, vw89, 0, 0, 0, 0)));

      const v128_t vi0x3456 = wasm_v32x4_shuffle(vi0x0123, vi0x4567, 3, 4, 5, 6);
      const v128_t vi1x3456 = wasm_v32x4_shuffle(vi1x0123, vi1x4567, 3, 4, 5, 6);
      const v128_t vi2x3456 = wasm_v32x4_shuffle(vi2x0123, vi2x4567, 3, 4, 5, 6);
      const v128_t vi3x3456 = wasm_v32x4_shuffle(vi3x0123, vi3x4567, 3, 4, 5, 6);

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi0x3456, wasm_v32x4_shuffle(vw0123, vw0123, 1, 1, 1, 1)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi1x3456, wasm_v32x4_shuffle(vw0123, vw0123, 1, 1, 1, 1)));

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi1x3456, wasm_v32x4_shuffle(vw4567, vw4567, 0, 0, 0, 0)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi2x3456, wasm_v32x4_shuffle(vw4567, vw4567, 0, 0, 0, 0)));

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi2x3456, wasm_v32x4_shuffle(vw4567, vw4567, 3, 3, 3, 3)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi3x3456, wasm_v32x4_shuffle(vw4567, vw4567, 3, 3, 3, 3)));

      const v128_t vzero = wasm_f32x4_const_splat(0.0f);
      const v128_t vi0x5678 = wasm_v32x4_shuffle(vi0x4567, vzero, 1, 2, 3, 4);
      const v128_t vi1x5678 = wasm_v32x4_shuffle(vi1x4567, vzero, 1, 2, 3, 4);
      const v128_t vi2x5678 = wasm_v32x4_shuffle(vi2x4567, vzero, 1, 2, 3, 4);
      const v128_t vi3x5678 = wasm_v32x4_shuffle(vi3x4567, vzero, 1, 2, 3, 4);

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi0x5678, wasm_v32x4_shuffle(vw0123, vw0123, 3, 3, 3, 3)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi1x5678, wasm_v32x4_shuffle(vw0123, vw0123, 3, 3, 3, 3)));

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi1x5678, wasm_v32x4_shuffle(vw4567, vw4567, 2, 2, 2, 2)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi2x5678, wasm_v32x4_shuffle(vw4567, vw4567, 2, 2, 2, 2)));

      vo0p0 = wasm_f32x4_add(vo0p0, wasm_f32x4_mul(vi2x5678, wasm_v32x4_shuffle(vw89, vw89, 1, 1, 1, 1)));
      vo1p0 = wasm_f32x4_add(vo1p0, wasm_f32x4_mul(vi3x5678, wasm_v32x4_shuffle(vw89, vw89, 1, 1, 1, 1)));


      v128_t vo0 = wasm_f32x4_max(vo0p0, vmin);
      v128_t vo1 = wasm_f32x4_max(vo1p0, vmin);
      vo0 = wasm_f32x4_min(vo0, vmax);
      vo1 = wasm_f32x4_min(vo1, vmax);

      if XNN_LIKELY(w == 4 * sizeof(float)) {
        wasm_v128_store(o1, vo1); o1 += 4;
        wasm_v128_store(o0, vo0); o0 += 4;
      } else {
        if (w & (2 * sizeof(float))) {
          *((double*) o1) = wasm_f64x2_extract_lane(vo1, 0); o1 += 2;
          *((double*) o0) = wasm_f64x2_extract_lane(vo0, 0); o0 += 2;

          vo0 = wasm_v32x4_shuffle(vo0, vo0, 2, 3, 0, 1);
          vo1 = wasm_v32x4_shuffle(vo1, vo1, 2, 3, 0, 1);
        }
        if (w & (1 * sizeof(float))) {
          *o1 = wasm_f32x4_extract_lane(vo1, 0); o1 += 1;
          *o0 = wasm_f32x4_extract_lane(vo0, 0); o0 += 1;
        }
      }
    }

    i0 = (const float*) ((uintptr_t) i2 - input_decrement);
    i1 = (const float*) ((uintptr_t) i3 - input_decrement);
    i2 = (const float*) ((uintptr_t) i1 + input_width);
    i3 = (const float*) ((uintptr_t) i2 + input_width);

    o0 = o1;
    o1 = (float*) ((uintptr_t) o0 + input_width);

    output_height = doz(output_height, 2);
  } while (output_height != 0);
}
