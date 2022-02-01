#!/bin/sh
# Copyright 2021 Google LLC
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

#################################### Scalar ###################################
tools/xngen src/s8-ibilinear/scalar.c.in -D CHANNEL_TILE=1 -D PIXEL_TILE=1 -D DATATYPE=S8 -o src/s8-ibilinear/gen/scalar-c1.c &
tools/xngen src/s8-ibilinear/scalar.c.in -D CHANNEL_TILE=2 -D PIXEL_TILE=1 -D DATATYPE=S8 -o src/s8-ibilinear/gen/scalar-c2.c &
tools/xngen src/s8-ibilinear/scalar.c.in -D CHANNEL_TILE=4 -D PIXEL_TILE=1 -D DATATYPE=S8 -o src/s8-ibilinear/gen/scalar-c4.c &

tools/xngen src/s8-ibilinear/scalar.c.in -D CHANNEL_TILE=1 -D PIXEL_TILE=1 -D DATATYPE=U8 -o src/u8-ibilinear/gen/scalar-c1.c &
tools/xngen src/s8-ibilinear/scalar.c.in -D CHANNEL_TILE=2 -D PIXEL_TILE=1 -D DATATYPE=U8 -o src/u8-ibilinear/gen/scalar-c2.c &
tools/xngen src/s8-ibilinear/scalar.c.in -D CHANNEL_TILE=4 -D PIXEL_TILE=1 -D DATATYPE=U8 -o src/u8-ibilinear/gen/scalar-c4.c &

################################## WAsm SIMD ##################################
tools/xngen src/s8-ibilinear/wasmsimd-dot16x2.c.in -D CHANNEL_TILE=8  -D PIXEL_TILE=1 -D DATATYPE=S8 -o src/s8-ibilinear/gen/wasmsimd-dot16x2-c8.c &
tools/xngen src/s8-ibilinear/wasmsimd-dot16x2.c.in -D CHANNEL_TILE=16 -D PIXEL_TILE=1 -D DATATYPE=S8 -o src/s8-ibilinear/gen/wasmsimd-dot16x2-c16.c &

tools/xngen src/s8-ibilinear/wasmsimd-mul32.c.in -D CHANNEL_TILE=8  -D PIXEL_TILE=1 -D DATATYPE=S8 -o src/s8-ibilinear/gen/wasmsimd-mul32-c8.c &
tools/xngen src/s8-ibilinear/wasmsimd-mul32.c.in -D CHANNEL_TILE=16 -D PIXEL_TILE=1 -D DATATYPE=S8 -o src/s8-ibilinear/gen/wasmsimd-mul32-c16.c &

tools/xngen src/s8-ibilinear/wasmsimd-dot16x2.c.in -D CHANNEL_TILE=8  -D PIXEL_TILE=1 -D DATATYPE=U8 -o src/u8-ibilinear/gen/wasmsimd-dot16x2-c8.c &
tools/xngen src/s8-ibilinear/wasmsimd-dot16x2.c.in -D CHANNEL_TILE=16 -D PIXEL_TILE=1 -D DATATYPE=U8 -o src/u8-ibilinear/gen/wasmsimd-dot16x2-c16.c &

tools/xngen src/s8-ibilinear/wasmsimd-mul32.c.in -D CHANNEL_TILE=8  -D PIXEL_TILE=1 -D DATATYPE=U8 -o src/u8-ibilinear/gen/wasmsimd-mul32-c8.c &
tools/xngen src/s8-ibilinear/wasmsimd-mul32.c.in -D CHANNEL_TILE=16 -D PIXEL_TILE=1 -D DATATYPE=U8 -o src/u8-ibilinear/gen/wasmsimd-mul32-c16.c &

################################### ARM NEON ##################################
tools/xngen src/s8-ibilinear/neon.c.in -D CHANNEL_TILE=8  -D PIXEL_TILE=1 -D DATATYPE=S8 -o src/s8-ibilinear/gen/neon-c8.c &
tools/xngen src/s8-ibilinear/neon.c.in -D CHANNEL_TILE=16 -D PIXEL_TILE=1 -D DATATYPE=S8 -o src/s8-ibilinear/gen/neon-c16.c &

tools/xngen src/s8-ibilinear/neon.c.in -D CHANNEL_TILE=8  -D PIXEL_TILE=1 -D DATATYPE=U8 -o src/u8-ibilinear/gen/neon-c8.c &
tools/xngen src/s8-ibilinear/neon.c.in -D CHANNEL_TILE=16 -D PIXEL_TILE=1 -D DATATYPE=U8 -o src/u8-ibilinear/gen/neon-c16.c &

################################### x86 SSE ###################################
tools/xngen src/s8-ibilinear/sse.c.in -D CHANNEL_TILE=8  -D PIXEL_TILE=1 -D DATATYPE=S8 -D SSE=2 -D AVX=0 -D XOP=0 -o src/s8-ibilinear/gen/sse2-c8.c &
tools/xngen src/s8-ibilinear/sse.c.in -D CHANNEL_TILE=16 -D PIXEL_TILE=1 -D DATATYPE=S8 -D SSE=2 -D AVX=0 -D XOP=0 -o src/s8-ibilinear/gen/sse2-c16.c &

tools/xngen src/s8-ibilinear/sse.c.in -D CHANNEL_TILE=8  -D PIXEL_TILE=1 -D DATATYPE=S8 -D SSE=4 -D AVX=0 -D XOP=0 -o src/s8-ibilinear/gen/sse41-c8.c &
tools/xngen src/s8-ibilinear/sse.c.in -D CHANNEL_TILE=16 -D PIXEL_TILE=1 -D DATATYPE=S8 -D SSE=4 -D AVX=0 -D XOP=0 -o src/s8-ibilinear/gen/sse41-c16.c &

tools/xngen src/s8-ibilinear/sse.c.in -D CHANNEL_TILE=8  -D PIXEL_TILE=1 -D DATATYPE=U8 -D SSE=2 -D AVX=0 -D XOP=0 -o src/u8-ibilinear/gen/sse2-c8.c &
tools/xngen src/s8-ibilinear/sse.c.in -D CHANNEL_TILE=16 -D PIXEL_TILE=1 -D DATATYPE=U8 -D SSE=2 -D AVX=0 -D XOP=0 -o src/u8-ibilinear/gen/sse2-c16.c &

tools/xngen src/s8-ibilinear/sse.c.in -D CHANNEL_TILE=8  -D PIXEL_TILE=1 -D DATATYPE=U8 -D SSE=4 -D AVX=0 -D XOP=0 -o src/u8-ibilinear/gen/sse41-c8.c &
tools/xngen src/s8-ibilinear/sse.c.in -D CHANNEL_TILE=16 -D PIXEL_TILE=1 -D DATATYPE=U8 -D SSE=4 -D AVX=0 -D XOP=0 -o src/u8-ibilinear/gen/sse41-c16.c &

################################## Unit tests #################################
tools/generate-ibilinear-test.py --spec test/s8-ibilinear.yaml --output test/s8-ibilinear.cc &
tools/generate-ibilinear-test.py --spec test/u8-ibilinear.yaml --output test/u8-ibilinear.cc &

wait
