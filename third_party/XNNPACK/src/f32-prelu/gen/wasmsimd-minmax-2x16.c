// Auto-generated file. Do not edit!
//   Template: src/f32-prelu/wasmsimd-minmax.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <wasm_simd128.h>

#include <xnnpack/math.h>
#include <xnnpack/prelu.h>


void xnn_f32_prelu_ukernel__wasmsimd_minmax_2x16(
    size_t rows,
    size_t channels,
    const float*restrict input,
    size_t input_stride,
    const float*restrict weights,
    float*restrict output,
    size_t output_stride) XNN_OOB_READS
{
  assert(rows != 0);
  assert(channels != 0);
  assert(channels % sizeof(float) == 0);

  const float* i0 = input;
  float* o0 = output;
  const float* i1 = (const float*) ((uintptr_t) i0 + input_stride);
  float* o1 = (float*) ((uintptr_t) o0 + output_stride);

  const size_t input_increment = input_stride * 2 - channels;
  const size_t output_increment = output_stride * 2 - channels;

  const v128_t vzero = wasm_i32x4_const_splat(0);
  do {
    if XNN_UNPREDICTABLE(rows < 2) {
      i1 = i0;
      o1 = o0;
    }

    const float* w = weights;
    size_t c = channels;
    for (; c >= 16 * sizeof(float); c -= 16 * sizeof(float)) {
      const v128_t vw0123 = wasm_v128_load(w);
      const v128_t vw4567 = wasm_v128_load(w + 4);
      const v128_t vw89AB = wasm_v128_load(w + 8);
      const v128_t vwCDEF = wasm_v128_load(w + 12);
      w += 16;

      v128_t vi0x0123 = wasm_v128_load(i0);
      v128_t vi0x4567 = wasm_v128_load(i0 + 4);
      v128_t vi0x89AB = wasm_v128_load(i0 + 8);
      v128_t vi0xCDEF = wasm_v128_load(i0 + 12);
      i0 += 16;
      v128_t vi1x0123 = wasm_v128_load(i1);
      v128_t vi1x4567 = wasm_v128_load(i1 + 4);
      v128_t vi1x89AB = wasm_v128_load(i1 + 8);
      v128_t vi1xCDEF = wasm_v128_load(i1 + 12);
      i1 += 16;

      v128_t vacc0x0123 = wasm_i32x4_max(vi0x0123, vzero);
      vi0x0123 = wasm_i32x4_min(vi0x0123, vzero);
      v128_t vacc0x4567 = wasm_i32x4_max(vi0x4567, vzero);
      vi0x4567 = wasm_i32x4_min(vi0x4567, vzero);
      v128_t vacc0x89AB = wasm_i32x4_max(vi0x89AB, vzero);
      vi0x89AB = wasm_i32x4_min(vi0x89AB, vzero);
      v128_t vacc0xCDEF = wasm_i32x4_max(vi0xCDEF, vzero);
      vi0xCDEF = wasm_i32x4_min(vi0xCDEF, vzero);
      v128_t vacc1x0123 = wasm_i32x4_max(vi1x0123, vzero);
      vi1x0123 = wasm_i32x4_min(vi1x0123, vzero);
      v128_t vacc1x4567 = wasm_i32x4_max(vi1x4567, vzero);
      vi1x4567 = wasm_i32x4_min(vi1x4567, vzero);
      v128_t vacc1x89AB = wasm_i32x4_max(vi1x89AB, vzero);
      vi1x89AB = wasm_i32x4_min(vi1x89AB, vzero);
      v128_t vacc1xCDEF = wasm_i32x4_max(vi1xCDEF, vzero);
      vi1xCDEF = wasm_i32x4_min(vi1xCDEF, vzero);

      vacc0x0123 = wasm_f32x4_add(vacc0x0123, wasm_f32x4_mul(vi0x0123, vw0123));
      vacc0x4567 = wasm_f32x4_add(vacc0x4567, wasm_f32x4_mul(vi0x4567, vw4567));
      vacc0x89AB = wasm_f32x4_add(vacc0x89AB, wasm_f32x4_mul(vi0x89AB, vw89AB));
      vacc0xCDEF = wasm_f32x4_add(vacc0xCDEF, wasm_f32x4_mul(vi0xCDEF, vwCDEF));
      vacc1x0123 = wasm_f32x4_add(vacc1x0123, wasm_f32x4_mul(vi1x0123, vw0123));
      vacc1x4567 = wasm_f32x4_add(vacc1x4567, wasm_f32x4_mul(vi1x4567, vw4567));
      vacc1x89AB = wasm_f32x4_add(vacc1x89AB, wasm_f32x4_mul(vi1x89AB, vw89AB));
      vacc1xCDEF = wasm_f32x4_add(vacc1xCDEF, wasm_f32x4_mul(vi1xCDEF, vwCDEF));

      wasm_v128_store(o0, vacc0x0123);
      wasm_v128_store(o0 + 4, vacc0x4567);
      wasm_v128_store(o0 + 8, vacc0x89AB);
      wasm_v128_store(o0 + 12, vacc0xCDEF);
      o0 += 16;
      wasm_v128_store(o1, vacc1x0123);
      wasm_v128_store(o1 + 4, vacc1x4567);
      wasm_v128_store(o1 + 8, vacc1x89AB);
      wasm_v128_store(o1 + 12, vacc1xCDEF);
      o1 += 16;
    }
    for (; c >= 4 * sizeof(float); c -= 4 * sizeof(float)) {
      const v128_t vw0123 = wasm_v128_load(w);
      w += 4;

      v128_t vi0x0123 = wasm_v128_load(i0);
      i0 += 4;
      v128_t vi1x0123 = wasm_v128_load(i1);
      i1 += 4;

      v128_t vacc0x0123 = wasm_i32x4_max(vi0x0123, vzero);
      vi0x0123 = wasm_i32x4_min(vi0x0123, vzero);
      v128_t vacc1x0123 = wasm_i32x4_max(vi1x0123, vzero);
      vi1x0123 = wasm_i32x4_min(vi1x0123, vzero);

      vacc0x0123 = wasm_f32x4_add(vacc0x0123, wasm_f32x4_mul(vi0x0123, vw0123));
      vacc1x0123 = wasm_f32x4_add(vacc1x0123, wasm_f32x4_mul(vi1x0123, vw0123));

      wasm_v128_store(o0, vacc0x0123);
      o0 += 4;
      wasm_v128_store(o1, vacc1x0123);
      o1 += 4;
    }
    if XNN_UNLIKELY(c != 0) {
      const v128_t vw0123 = wasm_v128_load(w);
      w = (const float*) ((uintptr_t) w + c);

      v128_t vi0x0123 = wasm_v128_load(i0);
      i0 = (const float*) ((uintptr_t) i0 + c);
      v128_t vi1x0123 = wasm_v128_load(i1);
      i1 = (const float*) ((uintptr_t) i1 + c);

      v128_t vacc0x0123 = wasm_i32x4_max(vi0x0123, vzero);
      vi0x0123 = wasm_i32x4_min(vi0x0123, vzero);
      v128_t vacc1x0123 = wasm_i32x4_max(vi1x0123, vzero);
      vi1x0123 = wasm_i32x4_min(vi1x0123, vzero);

      vacc0x0123 = wasm_f32x4_add(vacc0x0123, wasm_f32x4_mul(vi0x0123, vw0123));
      vacc1x0123 = wasm_f32x4_add(vacc1x0123, wasm_f32x4_mul(vi1x0123, vw0123));

      if (c & (2 * sizeof(float))) {
        *((double*) o0) = wasm_f64x2_extract_lane(vacc0x0123, 0);
        *((double*) o1) = wasm_f64x2_extract_lane(vacc1x0123, 0);

        vacc0x0123 = wasm_v32x4_shuffle(vacc0x0123, vacc0x0123, 2, 3, 2, 3);
        vacc1x0123 = wasm_v32x4_shuffle(vacc1x0123, vacc1x0123, 2, 3, 2, 3);

        o0 += 2;
        o1 += 2;
      }
      if (c & (1 * sizeof(float))) {
        *o0 = wasm_f32x4_extract_lane(vacc0x0123, 0);
        *o1 = wasm_f32x4_extract_lane(vacc1x0123, 0);

        o0 += 1;
        o1 += 1;
      }
    }
    i0 = (const float*) ((uintptr_t) i0 + input_increment);
    o0 = (float*) ((uintptr_t) o0 + output_increment);
    i1 = (const float*) ((uintptr_t) i1 + input_increment);
    o1 = (float*) ((uintptr_t) o1 + output_increment);
    rows = doz(rows, 2);
  } while (rows != 0);
}
