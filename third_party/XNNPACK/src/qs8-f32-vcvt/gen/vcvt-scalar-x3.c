// Auto-generated file. Do not edit!
//   Template: src/qs8-f32-vcvt/scalar.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <xnnpack/common.h>
#include <xnnpack/math.h>
#include <xnnpack/vcvt.h>


void xnn_qs8_f32_vcvt_ukernel__scalar_x3(
    size_t n,
    const int8_t* x,
    float* y,
    const union xnn_qs8_f32_cvt_params params[restrict XNN_MIN_ELEMENTS(1)])
{
  assert(n != 0);
  assert(n % sizeof(int8_t) == 0);
  assert(x != NULL);
  assert(y != NULL);

  const int32_t vzero_point = params->scalar.zero_point;
  const float vscale = params->scalar.scale;

  for (; n >= 3 * sizeof(int8_t); n -= 3 * sizeof(int8_t)) {
    int32_t vx0 = (int32_t) x[0];
    int32_t vx1 = (int32_t) x[1];
    int32_t vx2 = (int32_t) x[2];
    x += 3;

    vx0 -= vzero_point;
    vx1 -= vzero_point;
    vx2 -= vzero_point;

    float vy0 = (float) vx0;
    float vy1 = (float) vx1;
    float vy2 = (float) vx2;

    vy0 *= vscale;
    vy1 *= vscale;
    vy2 *= vscale;

    y[0] = vy0;
    y[1] = vy1;
    y[2] = vy2;
    y += 3;
  }
  if XNN_UNLIKELY(n != 0) {
    do {
      int32_t vx = *x++;
      vx -= vzero_point;

      float vy = (float) vx;
      vy *= vscale;
      *y++ = vy;

      n -= sizeof(int8_t);
    } while (n != 0);
  }
}
