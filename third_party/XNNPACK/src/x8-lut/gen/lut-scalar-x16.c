// Auto-generated file. Do not edit!
//   Template: src/x8-lut/scalar.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <xnnpack/lut.h>
#include <xnnpack/common.h>


void xnn_x8_lut_ukernel__scalar_x16(
    size_t n,
    const uint8_t* x,
    uint8_t* y,
    const uint8_t t[restrict XNN_MIN_ELEMENTS(256)])
{
  assert(n != 0);
  assert(x != NULL);
  assert(y != NULL);

  for (; n >= 16 * sizeof(uint8_t); n -= 16 * sizeof(uint8_t)) {
    const size_t vx0 = (size_t) x[0];
    const size_t vx1 = (size_t) x[1];
    const size_t vx2 = (size_t) x[2];
    const size_t vx3 = (size_t) x[3];
    const size_t vx4 = (size_t) x[4];
    const size_t vx5 = (size_t) x[5];
    const size_t vx6 = (size_t) x[6];
    const size_t vx7 = (size_t) x[7];
    const size_t vx8 = (size_t) x[8];
    const size_t vx9 = (size_t) x[9];
    const size_t vx10 = (size_t) x[10];
    const size_t vx11 = (size_t) x[11];
    const size_t vx12 = (size_t) x[12];
    const size_t vx13 = (size_t) x[13];
    const size_t vx14 = (size_t) x[14];
    const size_t vx15 = (size_t) x[15];
    x += 16;

    const uint32_t vt0 = (uint32_t) t[vx0];
    const uint32_t vt1 = (uint32_t) t[vx1];
    const uint32_t vt2 = (uint32_t) t[vx2];
    const uint32_t vt3 = (uint32_t) t[vx3];
    const uint32_t vt4 = (uint32_t) t[vx4];
    const uint32_t vt5 = (uint32_t) t[vx5];
    const uint32_t vt6 = (uint32_t) t[vx6];
    const uint32_t vt7 = (uint32_t) t[vx7];
    const uint32_t vt8 = (uint32_t) t[vx8];
    const uint32_t vt9 = (uint32_t) t[vx9];
    const uint32_t vt10 = (uint32_t) t[vx10];
    const uint32_t vt11 = (uint32_t) t[vx11];
    const uint32_t vt12 = (uint32_t) t[vx12];
    const uint32_t vt13 = (uint32_t) t[vx13];
    const uint32_t vt14 = (uint32_t) t[vx14];
    const uint32_t vt15 = (uint32_t) t[vx15];

    y[0] = (uint8_t) vt0;
    y[1] = (uint8_t) vt1;
    y[2] = (uint8_t) vt2;
    y[3] = (uint8_t) vt3;
    y[4] = (uint8_t) vt4;
    y[5] = (uint8_t) vt5;
    y[6] = (uint8_t) vt6;
    y[7] = (uint8_t) vt7;
    y[8] = (uint8_t) vt8;
    y[9] = (uint8_t) vt9;
    y[10] = (uint8_t) vt10;
    y[11] = (uint8_t) vt11;
    y[12] = (uint8_t) vt12;
    y[13] = (uint8_t) vt13;
    y[14] = (uint8_t) vt14;
    y[15] = (uint8_t) vt15;
    y += 16;
  }
  if XNN_UNLIKELY(n != 0) {
    do {
      const size_t vx = (size_t) *x++;
      const uint32_t vt = (uint32_t) t[vx];
      *y++ = (uint8_t) vt;
      n -= sizeof(uint8_t);
    } while (n != 0);
  }
}
