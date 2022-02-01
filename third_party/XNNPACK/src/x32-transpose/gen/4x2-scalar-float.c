// Auto-generated file. Do not edit!
//   Template: src/x32-transpose/scalar.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.


#include <assert.h>

#include <xnnpack/common.h>
#include <xnnpack/math.h>
#include <xnnpack/transpose.h>

void xnn_x32_transpose_ukernel__4x2_scalar_float(
    const uint32_t *input,
    uint32_t * output,
    size_t input_stride,
    size_t output_stride,
    size_t block_width,
    size_t block_height)
{
  assert(output_stride >= block_height * sizeof(float));
  assert(input_stride >= block_width * sizeof(float));

  const size_t tile_height = 4;
  const size_t tile_width = 2;
  const size_t tile_wbytes = tile_width * sizeof(float);
  const size_t input_reset = tile_wbytes - round_down_po2(block_height, tile_height) * input_stride;
  const size_t output_reset = tile_width * output_stride - round_down_po2(block_height, 2) * sizeof(float);
  const size_t input_offset = tile_height * input_stride;

  const float* i0 = (const float*) input;
  const float* i1 = (const float*) ((uintptr_t) i0 + input_stride);
  const float* i2 = (const float*) ((uintptr_t) i1 + input_stride);
  const float* i3 = (const float*) ((uintptr_t) i2 + input_stride);

  float* o0 = (float*) output;
  float* o1 = (float*) ((uintptr_t) o0 + output_stride);

  do {
    if XNN_UNPREDICTABLE(block_width < 2) {
      o1 = o0;
    }
    size_t bh = block_height;
    for (; bh >= 4; bh -= 4) {
      *o1++ = i0[1];
      *o1++ = i1[1];
      *o1++ = i2[1];
      *o1++ = i3[1];
      *o0++ = i0[0];
      *o0++ = i1[0];
      *o0++ = i2[0];
      *o0++ = i3[0];
      i0 = (const float*) ((uintptr_t) i0 + input_offset);
      i1 = (const float*) ((uintptr_t) i1 + input_offset);
      i2 = (const float*) ((uintptr_t) i2 + input_offset);
      i3 = (const float*) ((uintptr_t) i3 + input_offset);
    }
    const float* i = i0;
    if (bh & 2) {
      o1[0] = i0[1];
      o1[1] = i1[1];
      o1 += 2;
      o0[0] = i0[0];
      o0[1] = i1[0];
      o0 += 2;
      i = i2;
    }
    if (bh & 1) {
      o1[0] = i[1];
      o0[0] = i[0];
    }

    i0 = (const float*) ((uintptr_t) i0 + input_reset);
    i1 = (const float*) ((uintptr_t) i0 + input_stride);
    i2 = (const float*) ((uintptr_t) i1 + input_stride);
    i3 = (const float*) ((uintptr_t) i2 + input_stride);
    o0 = (float*) ((uintptr_t) o0 + output_reset);
    o1 = (float*) ((uintptr_t) o1 + output_reset);
    block_width = doz(block_width, tile_width);
  } while (block_width != 0);
}
