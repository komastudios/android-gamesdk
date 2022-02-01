// Auto-generated file. Do not edit!
//   Template: src/f32-dwconv2d-chw/3x3s2p1-neon.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <arm_neon.h>

#include <xnnpack/dwconv.h>
#include <xnnpack/math.h>


void xnn_f32_dwconv2d_chw_ukernel_3x3s2p1__neon_2x4_acc2(
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
  assert(padding_top >= 0);
  assert(padding_top <= 1);

  const uint32x4_t vmask_even = vld1q_u32(params->neon.mask_even);
  const uint32x4_t vmask_odd  = vld1q_u32(params->neon.mask_odd);
  const float32x4_t vmax = vld1q_dup_f32(&params->neon.max);
  const float32x4_t vmin = vld1q_dup_f32(&params->neon.min);

  const float32x4_t vw0123 = vld1q_f32(weights);
  const float32x4_t vw4567 = vld1q_f32(weights + 4);
  const float32x2_t vw89 = vld1_f32(weights + 8);

  const size_t input_decrement = round_down_po2(input_width, 4 /* SIMD output width */ * 2 /* subsampling */ * sizeof(float));
  const size_t output_width = round_down_po2((input_width + (2 /* padding */ - 3 /* kernel size */ + 2 /* subsampling */) * sizeof(float)) / 2, sizeof(float));

  const float* i0 = (const float*) ((uintptr_t) input - ((-padding_top) & input_width));
  const float* i1 = (const float*) ((uintptr_t) i0 + input_width);
  if XNN_UNPREDICTABLE(padding_top != 0) {
    i0 = zero;
  }
  const float* i2 = (const float*) ((uintptr_t) i1 + input_width);
  const float* i3 = (const float*) ((uintptr_t) i2 + input_width);
  const float* i4 = (const float*) ((uintptr_t) i3 + input_width);

  float* o0 = output;
  float* o1 = (float*) ((uintptr_t) o0 + output_width);

  size_t padded_input_height = input_height + padding_top + 1 /* padding bottom */;
  size_t output_height = (padded_input_height - 3 /* kernel size */ + 2 /* subsampling */) / 2;
  do {
    if XNN_UNPREDICTABLE(padded_input_height < 4) {
      i2 = zero;
    }
    if XNN_UNPREDICTABLE(padded_input_height < 5) {
      i3 = zero;
      o1 = o0;
    }
    if XNN_UNPREDICTABLE(padded_input_height < 6) {
      i4 = zero;
    }

    float32x4_t vi0x1357 = vmovq_n_f32(0.0f);
    float32x4_t vi1x1357 = vmovq_n_f32(0.0f);
    float32x4_t vi2x1357 = vmovq_n_f32(0.0f);
    float32x4_t vi3x1357 = vmovq_n_f32(0.0f);
    float32x4_t vi4x1357 = vmovq_n_f32(0.0f);

    size_t w = input_width;
    for (; w >= 8 * sizeof(float); w -= 8 * sizeof(float)) {
      float32x4_t vo0p0 = vdupq_lane_f32(vget_low_f32(vw0123), 0);
      float32x4_t vo1p0 = vdupq_lane_f32(vget_low_f32(vw0123), 0);

      const float32x4x2_t vi0x8ACE9BDF = vld2q_f32(i0); i0 += 8;
      const float32x4x2_t vi1x8ACE9BDF = vld2q_f32(i1); i1 += 8;
      const float32x4x2_t vi2x8ACE9BDF = vld2q_f32(i2); i2 += 8;
      const float32x4x2_t vi3x8ACE9BDF = vld2q_f32(i3); i3 += 8;
      const float32x4x2_t vi4x8ACE9BDF = vld2q_f32(i4); i4 += 8;

      float32x4_t vo0p1 = vmulq_lane_f32(vi0x8ACE9BDF.val[0], vget_high_f32(vw0123), 0);
      float32x4_t vo1p1 = vmulq_lane_f32(vi2x8ACE9BDF.val[0], vget_high_f32(vw0123), 0);

      vo0p0 = vmlaq_lane_f32(vo0p0, vi1x8ACE9BDF.val[0], vget_low_f32(vw4567), 1);
      vo1p0 = vmlaq_lane_f32(vo1p0, vi3x8ACE9BDF.val[0], vget_low_f32(vw4567), 1);

      vo0p0 = vmlaq_lane_f32(vo0p0, vi2x8ACE9BDF.val[0], vw89, 0);
      vo1p0 = vmlaq_lane_f32(vo1p0, vi4x8ACE9BDF.val[0], vw89, 0);

      const float32x4_t vi0x7BDF = vextq_f32(vi0x1357, vi0x8ACE9BDF.val[1], 3);
      vi0x1357 = vi0x8ACE9BDF.val[1];
      const float32x4_t vi1x7BDF = vextq_f32(vi1x1357, vi1x8ACE9BDF.val[1], 3);
      vi1x1357 = vi1x8ACE9BDF.val[1];
      const float32x4_t vi2x7BDF = vextq_f32(vi2x1357, vi2x8ACE9BDF.val[1], 3);
      vi2x1357 = vi2x8ACE9BDF.val[1];
      const float32x4_t vi3x7BDF = vextq_f32(vi3x1357, vi3x8ACE9BDF.val[1], 3);
      vi3x1357 = vi3x8ACE9BDF.val[1];
      const float32x4_t vi4x7BDF = vextq_f32(vi4x1357, vi4x8ACE9BDF.val[1], 3);
      vi4x1357 = vi4x8ACE9BDF.val[1];

      vo0p1 = vmlaq_lane_f32(vo0p1, vi0x7BDF, vget_low_f32(vw0123), 1);
      vo1p1 = vmlaq_lane_f32(vo1p1, vi2x7BDF, vget_low_f32(vw0123), 1);

      vo0p0 = vmlaq_lane_f32(vo0p0, vi1x7BDF, vget_low_f32(vw4567), 0);
      vo1p0 = vmlaq_lane_f32(vo1p0, vi3x7BDF, vget_low_f32(vw4567), 0);

      vo0p1 = vmlaq_lane_f32(vo0p1, vi2x7BDF, vget_high_f32(vw4567), 1);
      vo1p1 = vmlaq_lane_f32(vo1p1, vi4x7BDF, vget_high_f32(vw4567), 1);

      vo0p0 = vmlaq_lane_f32(vo0p0, vi0x8ACE9BDF.val[1], vget_high_f32(vw0123), 1);
      vo1p0 = vmlaq_lane_f32(vo1p0, vi2x8ACE9BDF.val[1], vget_high_f32(vw0123), 1);

      vo0p1 = vmlaq_lane_f32(vo0p1, vi1x8ACE9BDF.val[1], vget_high_f32(vw4567), 0);
      vo1p1 = vmlaq_lane_f32(vo1p1, vi3x8ACE9BDF.val[1], vget_high_f32(vw4567), 0);

      vo0p0 = vmlaq_lane_f32(vo0p0, vi2x8ACE9BDF.val[1], vw89, 1);
      vo1p0 = vmlaq_lane_f32(vo1p0, vi4x8ACE9BDF.val[1], vw89, 1);

      vo0p0 = vaddq_f32(vo0p0, vo0p1);
      vo1p0 = vaddq_f32(vo1p0, vo1p1);

      float32x4_t vo0 = vmaxq_f32(vo0p0, vmin);
      float32x4_t vo1 = vmaxq_f32(vo1p0, vmin);

      vo0 = vminq_f32(vo0, vmax);
      vo1 = vminq_f32(vo1, vmax);

      vst1q_f32(o1, vo1); o1 += 4;
      vst1q_f32(o0, vo0); o0 += 4;
    }
    // Last block has 0-7 pixels to process.
    assert(w < 8 * sizeof(float));
    if XNN_LIKELY(w != 0) {
      float32x4_t vo0p0 = vdupq_lane_f32(vget_low_f32(vw0123), 0);
      float32x4_t vo1p0 = vdupq_lane_f32(vget_low_f32(vw0123), 0);

      const float32x4x2_t vi0x8ACE9BDF = vld2q_f32(i0);
      const float32x4x2_t vi1x8ACE9BDF = vld2q_f32(i1);
      const float32x4x2_t vi2x8ACE9BDF = vld2q_f32(i2);
      const float32x4x2_t vi3x8ACE9BDF = vld2q_f32(i3);
      const float32x4x2_t vi4x8ACE9BDF = vld2q_f32(i4);

      const float32x4_t vi0x8ACE = vreinterpretq_f32_u32(vandq_u32(vmask_even, vreinterpretq_u32_f32(vi0x8ACE9BDF.val[0])));
      const float32x4_t vi0x9BDF = vreinterpretq_f32_u32(vandq_u32(vmask_odd,  vreinterpretq_u32_f32(vi0x8ACE9BDF.val[1])));
      const float32x4_t vi1x8ACE = vreinterpretq_f32_u32(vandq_u32(vmask_even, vreinterpretq_u32_f32(vi1x8ACE9BDF.val[0])));
      const float32x4_t vi1x9BDF = vreinterpretq_f32_u32(vandq_u32(vmask_odd,  vreinterpretq_u32_f32(vi1x8ACE9BDF.val[1])));
      const float32x4_t vi2x8ACE = vreinterpretq_f32_u32(vandq_u32(vmask_even, vreinterpretq_u32_f32(vi2x8ACE9BDF.val[0])));
      const float32x4_t vi2x9BDF = vreinterpretq_f32_u32(vandq_u32(vmask_odd,  vreinterpretq_u32_f32(vi2x8ACE9BDF.val[1])));
      const float32x4_t vi3x8ACE = vreinterpretq_f32_u32(vandq_u32(vmask_even, vreinterpretq_u32_f32(vi3x8ACE9BDF.val[0])));
      const float32x4_t vi3x9BDF = vreinterpretq_f32_u32(vandq_u32(vmask_odd,  vreinterpretq_u32_f32(vi3x8ACE9BDF.val[1])));
      const float32x4_t vi4x8ACE = vreinterpretq_f32_u32(vandq_u32(vmask_even, vreinterpretq_u32_f32(vi4x8ACE9BDF.val[0])));
      const float32x4_t vi4x9BDF = vreinterpretq_f32_u32(vandq_u32(vmask_odd,  vreinterpretq_u32_f32(vi4x8ACE9BDF.val[1])));

      float32x4_t vo0p1 = vmulq_lane_f32(vi0x8ACE, vget_high_f32(vw0123), 0);
      float32x4_t vo1p1 = vmulq_lane_f32(vi2x8ACE, vget_high_f32(vw0123), 0);

      vo0p0 = vmlaq_lane_f32(vo0p0, vi1x8ACE, vget_low_f32(vw4567), 1);
      vo1p0 = vmlaq_lane_f32(vo1p0, vi3x8ACE, vget_low_f32(vw4567), 1);

      vo0p0 = vmlaq_lane_f32(vo0p0, vi2x8ACE, vw89, 0);
      vo1p0 = vmlaq_lane_f32(vo1p0, vi4x8ACE, vw89, 0);

      const float32x4_t vi0x7BDF = vextq_f32(vi0x1357, vi0x9BDF, 3);
      const float32x4_t vi1x7BDF = vextq_f32(vi1x1357, vi1x9BDF, 3);
      const float32x4_t vi2x7BDF = vextq_f32(vi2x1357, vi2x9BDF, 3);
      const float32x4_t vi3x7BDF = vextq_f32(vi3x1357, vi3x9BDF, 3);
      const float32x4_t vi4x7BDF = vextq_f32(vi4x1357, vi4x9BDF, 3);

      vo0p1 = vmlaq_lane_f32(vo0p1, vi0x7BDF, vget_low_f32(vw0123), 1);
      vo1p1 = vmlaq_lane_f32(vo1p1, vi2x7BDF, vget_low_f32(vw0123), 1);

      vo0p0 = vmlaq_lane_f32(vo0p0, vi1x7BDF, vget_low_f32(vw4567), 0);
      vo1p0 = vmlaq_lane_f32(vo1p0, vi3x7BDF, vget_low_f32(vw4567), 0);

      vo0p1 = vmlaq_lane_f32(vo0p1, vi2x7BDF, vget_high_f32(vw4567), 1);
      vo1p1 = vmlaq_lane_f32(vo1p1, vi4x7BDF, vget_high_f32(vw4567), 1);

      vo0p0 = vmlaq_lane_f32(vo0p0, vi0x9BDF, vget_high_f32(vw0123), 1);
      vo1p0 = vmlaq_lane_f32(vo1p0, vi2x9BDF, vget_high_f32(vw0123), 1);

      vo0p1 = vmlaq_lane_f32(vo0p1, vi1x9BDF, vget_high_f32(vw4567), 0);
      vo1p1 = vmlaq_lane_f32(vo1p1, vi3x9BDF, vget_high_f32(vw4567), 0);

      vo0p0 = vmlaq_lane_f32(vo0p0, vi2x9BDF, vw89, 1);
      vo1p0 = vmlaq_lane_f32(vo1p0, vi4x9BDF, vw89, 1);

      vo0p0 = vaddq_f32(vo0p0, vo0p1);
      vo1p0 = vaddq_f32(vo1p0, vo1p1);

      float32x4_t vo0 = vmaxq_f32(vo0p0, vmin);
      float32x4_t vo1 = vmaxq_f32(vo1p0, vmin);

      vo0 = vminq_f32(vo0, vmax);
      vo1 = vminq_f32(vo1, vmax);

      w += 1 * sizeof(float);
      if (w & (8 * sizeof(float))) {
        vst1q_f32(o1, vo1); o1 += 4;
        vst1q_f32(o0, vo0); o0 += 4;
      } else {
        float32x2_t vo0_lo = vget_low_f32(vo0);
        float32x2_t vo1_lo = vget_low_f32(vo1);
        if (w & (4 * sizeof(float))) {
          vst1_f32(o1, vo1_lo); o1 += 2;
          vst1_f32(o0, vo0_lo); o0 += 2;

          vo0_lo = vget_high_f32(vo0);
          vo1_lo = vget_high_f32(vo1);
        }
        if (w & (2 * sizeof(float))) {
          vst1_lane_f32(o1, vo1_lo, 0); o1 += 1;
          vst1_lane_f32(o0, vo0_lo, 0); o0 += 1;
        }
      }
    }

    i0 = (const float*) ((uintptr_t) i4 - input_decrement);
    i1 = (const float*) ((uintptr_t) i0 + input_width);
    i2 = (const float*) ((uintptr_t) i1 + input_width);
    i3 = (const float*) ((uintptr_t) i2 + input_width);
    i4 = (const float*) ((uintptr_t) i3 + input_width);

    o0 = o1;
    o1 = (float*) ((uintptr_t) o0 + output_width);

    output_height = doz(output_height, 2);
    padded_input_height = doz(padded_input_height, 4);
  } while (output_height != 0);
}
