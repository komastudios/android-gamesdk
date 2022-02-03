// Auto-generated file. Do not edit!
//   Template: src/f32-qs8-vcvt/scalar-lrintf.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>
#include <math.h>

#include <xnnpack/common.h>
#include <xnnpack/math.h>
#include <xnnpack/vcvt.h>


void xnn_f32_qu8_vcvt_ukernel__scalar_lrintf_x1(
    size_t n,
    const float* x,
    uint8_t* y,
    const union xnn_f32_qu8_cvt_params params[restrict XNN_MIN_ELEMENTS(1)])
{
  assert(n != 0);
  assert(n % sizeof(float) == 0);
  assert(x != NULL);
  assert(y != NULL);

  const float vscale = params->scalar_lrintf.scale;
  const float voutput_min_less_zero_point = params->scalar_lrintf.output_min_less_zero_point;
  const float voutput_max_less_zero_point = params->scalar_lrintf.output_max_less_zero_point;
  const int32_t voutput_zero_point = params->scalar_lrintf.output_zero_point;

  do {
    float vx = *x++;
    vx *= vscale;
    vx = math_max_f32(vx, voutput_min_less_zero_point);
    vx = math_min_f32(vx, voutput_max_less_zero_point);

    int32_t vy = (int32_t) lrintf(vx);
    vy += voutput_zero_point;

    *y++ = (uint8_t) vy;

    n -= sizeof(float);
  } while (n != 0);
}
