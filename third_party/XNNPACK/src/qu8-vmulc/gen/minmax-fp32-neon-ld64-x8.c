// Auto-generated file. Do not edit!
//   Template: src/qs8-vmulc/neon.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <arm_neon.h>

#include <xnnpack/vmul.h>


void xnn_qu8_vmulc_minmax_fp32_ukernel__neon_ld64_x8(
    size_t n,
    const uint8_t* input_a,
    const uint8_t* input_b,
    uint8_t* output,
    const union xnn_qu8_mul_minmax_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  const uint8x8_t va_zero_point = vld1_dup_u8(params->fp32_neon.a_zero_point);
  const float32x4_t vscale = vld1q_dup_f32(&params->fp32_neon.scale);
  const float32x4_t vmagic_bias = vld1q_dup_f32(&params->fp32_neon.magic_bias);
  const int32x4_t vmagic_bias_less_output_zero_point = vld1q_dup_s32(&params->fp32_neon.magic_bias_less_output_zero_point);
  const uint8x8_t voutput_min = vld1_dup_u8(&params->fp32_neon.output_min);
  const uint8x8_t voutput_max = vld1_dup_u8(&params->fp32_neon.output_max);

  const uint8x8_t vb = vld1_dup_u8(input_b);
  const uint8x8_t vb_zero_point = vld1_dup_u8(params->fp32_neon.b_zero_point);
  const int16x8_t vxb = vreinterpretq_s16_u16(vsubl_u8(vb, vb_zero_point));
  for (; n >= 8 * sizeof(uint8_t); n -= 8 * sizeof(uint8_t)) {
    const uint8x8_t va01234567 = vld1_u8(input_a); input_a += 8;

    const int16x8_t vxa01234567 = vreinterpretq_s16_u16(vsubl_u8(va01234567, va_zero_point));

    int32x4_t vacc0123 = vmull_s16(vget_low_s16(vxa01234567), vget_low_s16(vxb));
    int32x4_t vacc4567 = vmull_s16(vget_high_s16(vxa01234567), vget_high_s16(vxb));

    float32x4_t vfpacc0123 = vcvtq_f32_s32(vacc0123);
    float32x4_t vfpacc4567 = vcvtq_f32_s32(vacc4567);

    vfpacc0123 = vmulq_f32(vfpacc0123, vscale);
    vfpacc4567 = vmulq_f32(vfpacc4567, vscale);

    vacc0123 = vreinterpretq_s32_f32(vaddq_f32(vfpacc0123, vmagic_bias));
    vacc4567 = vreinterpretq_s32_f32(vaddq_f32(vfpacc4567, vmagic_bias));

    vacc0123 = vqsubq_s32(vacc0123, vmagic_bias_less_output_zero_point);
    vacc4567 = vqsubq_s32(vacc4567, vmagic_bias_less_output_zero_point);

    #if XNN_ARCH_ARM64
      int16x8_t vacc01234567 = vqmovn_high_s32(vqmovn_s32(vacc0123), vacc4567);
    #else
      int16x8_t vacc01234567 = vcombine_s16(vqmovn_s32(vacc0123), vqmovn_s32(vacc4567));
    #endif


    #if XNN_ARCH_ARM64
      uint8x8_t vout01234567 = vqmovun_s16(vacc01234567);
    #else
      uint8x8_t vout01234567 = vqmovun_s16(vacc01234567);
    #endif

    vout01234567 = vmax_u8(vout01234567, voutput_min);

    vout01234567 = vmin_u8(vout01234567, voutput_max);

    vst1_u8(output, vout01234567); output += 8;
  }
  if XNN_UNLIKELY(n != 0) {
    {
      const uint8x8_t va01234567 = vld1_u8(input_a);

      const int16x8_t vxa01234567 = vreinterpretq_s16_u16(vsubl_u8(va01234567, va_zero_point));

      int32x4_t vacc0123 = vmull_s16(vget_low_s16(vxa01234567), vget_low_s16(vxb));
      int32x4_t vacc4567 = vmull_s16(vget_high_s16(vxa01234567), vget_high_s16(vxb));

      float32x4_t vfpacc0123 = vcvtq_f32_s32(vacc0123);
      float32x4_t vfpacc4567 = vcvtq_f32_s32(vacc4567);

      vfpacc0123 = vmulq_f32(vfpacc0123, vscale);
      vfpacc4567 = vmulq_f32(vfpacc4567, vscale);

      vacc0123 = vreinterpretq_s32_f32(vaddq_f32(vfpacc0123, vmagic_bias));
      vacc4567 = vreinterpretq_s32_f32(vaddq_f32(vfpacc4567, vmagic_bias));

      vacc0123 = vqsubq_s32(vacc0123, vmagic_bias_less_output_zero_point);
      vacc4567 = vqsubq_s32(vacc4567, vmagic_bias_less_output_zero_point);

      #if XNN_ARCH_ARM64
        int16x8_t vacc01234567 = vqmovn_high_s32(vqmovn_s32(vacc0123), vacc4567);
      #else
        int16x8_t vacc01234567 = vcombine_s16(vqmovn_s32(vacc0123), vqmovn_s32(vacc4567));
      #endif


      uint8x8_t vout01234567 = vqmovun_s16(vacc01234567);

      vout01234567 = vmax_u8(vout01234567, voutput_min);
      vout01234567 = vmin_u8(vout01234567, voutput_max);
      if (n & (4 * sizeof(uint8_t))) {
        vst1_lane_u32((void*) output, vreinterpret_u32_u8(vout01234567), 0); output += 4;
        vout01234567 = vext_u8(vout01234567, vout01234567, 4);
      }
      if (n & (2 * sizeof(uint8_t))) {
        vst1_lane_u16((void*) output, vreinterpret_u16_u8(vout01234567), 0); output += 2;
        vout01234567 = vext_u8(vout01234567, vout01234567, 2);
      }
      if (n & (1 * sizeof(uint8_t))) {
        vst1_lane_u8(output, vout01234567, 0);
      }
    }
  }
}
