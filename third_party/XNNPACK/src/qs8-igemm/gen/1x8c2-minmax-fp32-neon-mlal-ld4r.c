// Auto-generated file. Do not edit!
//   Template: src/qs8-igemm/c2-neon-mull-dup.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <arm_neon.h>

#include <xnnpack/gemm.h>
#include <xnnpack/math.h>

void xnn_qs8_igemm_minmax_fp32_ukernel_1x8c2__neon_mlal_ld4r(
    size_t mr,
    size_t nc,
    size_t kc,
    size_t ks,
    const int8_t** restrict a,
    const void* restrict w,
    int8_t* restrict c,
    size_t cm_stride,
    size_t cn_stride,
    size_t a_offset,
    const int8_t* zero,
    const union xnn_qs8_conv_minmax_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(mr != 0);
  assert(mr <= 1);
  assert(nc != 0);
  assert(kc != 0);
  assert(ks != 0);
  assert(ks % (1 * sizeof(void*)) == 0);
  assert(a_offset % sizeof(int8_t) == 0);
  assert(a != NULL);
  assert(w != NULL);
  assert(c != NULL);

  kc = round_up_po2(kc, 2 * sizeof(int8_t));
  int8_t* c0 = c;

  do {
    int32x4_t vacc0x0123 = vld1q_s32(w); w = (const void*) ((uintptr_t) w + 4 * sizeof(int32_t));
    int32x4_t vacc0x4567 = vld1q_s32(w); w = (const void*) ((uintptr_t) w + 4 * sizeof(int32_t));

    size_t p = ks;
    do {
      const int8_t* restrict a0 = a[0];
      if XNN_UNPREDICTABLE(a0 != zero) {
        a0 = (const int8_t*) ((uintptr_t) a0 + a_offset);
      }
      a += 1;

      size_t k = kc;

      while (k >= 16 * sizeof(int8_t)) {
        const int16x4x4_t va0x0 = vld4_dup_s16((const void*)a0); a0 += 8;
        const int16x4x4_t va0x1 = vld4_dup_s16((const void*)a0); a0 += 8;

        const int8x8_t vb0123c0x0 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb4567c0x0 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb0123c1x0 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb4567c1x0 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb0123c2x0 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb4567c2x0 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb0123c3x0 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb4567c3x0 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));

        const int8x8_t va0c0x0 = vreinterpret_s8_s16(va0x0.val[0]);
        const int8x8_t va0c0x1 = vreinterpret_s8_s16(va0x1.val[0]);

        int16x8_t vprod0x0123c0 = vmull_s8(vb0123c0x0, va0c0x0);
        const int8x8_t vb0123c0x1 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        vprod0x0123c0 = vmlal_s8(vprod0x0123c0, vb0123c0x1, va0c0x1);
        vacc0x0123 = vpadalq_s16(vacc0x0123, vprod0x0123c0);
        int16x8_t vprod0x4567c0 = vmull_s8(vb4567c0x0, va0c0x0);
        const int8x8_t vb4567c0x1 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        vprod0x4567c0 = vmlal_s8(vprod0x4567c0, vb4567c0x1, va0c0x1);
        vacc0x4567 = vpadalq_s16(vacc0x4567, vprod0x4567c0);
        const int8x8_t va0c1x0 = vreinterpret_s8_s16(va0x0.val[1]);
        const int8x8_t va0c1x1 = vreinterpret_s8_s16(va0x1.val[1]);

        int16x8_t vprod0x0123c1 = vmull_s8(vb0123c1x0, va0c1x0);
        const int8x8_t vb0123c1x1 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        vprod0x0123c1 = vmlal_s8(vprod0x0123c1, vb0123c1x1, va0c1x1);
        vacc0x0123 = vpadalq_s16(vacc0x0123, vprod0x0123c1);
        int16x8_t vprod0x4567c1 = vmull_s8(vb4567c1x0, va0c1x0);
        const int8x8_t vb4567c1x1 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        vprod0x4567c1 = vmlal_s8(vprod0x4567c1, vb4567c1x1, va0c1x1);
        vacc0x4567 = vpadalq_s16(vacc0x4567, vprod0x4567c1);
        const int8x8_t va0c2x0 = vreinterpret_s8_s16(va0x0.val[2]);
        const int8x8_t va0c2x1 = vreinterpret_s8_s16(va0x1.val[2]);

        int16x8_t vprod0x0123c2 = vmull_s8(vb0123c2x0, va0c2x0);
        const int8x8_t vb0123c2x1 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        vprod0x0123c2 = vmlal_s8(vprod0x0123c2, vb0123c2x1, va0c2x1);
        vacc0x0123 = vpadalq_s16(vacc0x0123, vprod0x0123c2);
        int16x8_t vprod0x4567c2 = vmull_s8(vb4567c2x0, va0c2x0);
        const int8x8_t vb4567c2x1 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        vprod0x4567c2 = vmlal_s8(vprod0x4567c2, vb4567c2x1, va0c2x1);
        vacc0x4567 = vpadalq_s16(vacc0x4567, vprod0x4567c2);
        const int8x8_t va0c3x0 = vreinterpret_s8_s16(va0x0.val[3]);
        const int8x8_t va0c3x1 = vreinterpret_s8_s16(va0x1.val[3]);

        int16x8_t vprod0x0123c3 = vmull_s8(vb0123c3x0, va0c3x0);
        const int8x8_t vb0123c3x1 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        vprod0x0123c3 = vmlal_s8(vprod0x0123c3, vb0123c3x1, va0c3x1);
        vacc0x0123 = vpadalq_s16(vacc0x0123, vprod0x0123c3);
        int16x8_t vprod0x4567c3 = vmull_s8(vb4567c3x0, va0c3x0);
        const int8x8_t vb4567c3x1 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        vprod0x4567c3 = vmlal_s8(vprod0x4567c3, vb4567c3x1, va0c3x1);
        vacc0x4567 = vpadalq_s16(vacc0x4567, vprod0x4567c3);

        k -= 16 * sizeof(int8_t);
      }

      if (k >= 8 * sizeof(int8_t)) {
        const int16x4x4_t va0 = vld4_dup_s16((const void*)a0); a0 += 8;

        const int8x8_t vb0123c0 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb4567c0 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb0123c1 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb4567c1 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb0123c2 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb4567c2 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb0123c3 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb4567c3 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));

        const int8x8_t va0c0 = vreinterpret_s8_s16(va0.val[0]);

        const int16x8_t vprod0x0123c0 = vmull_s8(vb0123c0, va0c0);
        vacc0x0123 = vpadalq_s16(vacc0x0123, vprod0x0123c0);
        const int16x8_t vprod0x4567c0 = vmull_s8(vb4567c0, va0c0);
        vacc0x4567 = vpadalq_s16(vacc0x4567, vprod0x4567c0);
        const int8x8_t va0c1 = vreinterpret_s8_s16(va0.val[1]);

        const int16x8_t vprod0x0123c1 = vmull_s8(vb0123c1, va0c1);
        vacc0x0123 = vpadalq_s16(vacc0x0123, vprod0x0123c1);
        const int16x8_t vprod0x4567c1 = vmull_s8(vb4567c1, va0c1);
        vacc0x4567 = vpadalq_s16(vacc0x4567, vprod0x4567c1);
        const int8x8_t va0c2 = vreinterpret_s8_s16(va0.val[2]);

        const int16x8_t vprod0x0123c2 = vmull_s8(vb0123c2, va0c2);
        vacc0x0123 = vpadalq_s16(vacc0x0123, vprod0x0123c2);
        const int16x8_t vprod0x4567c2 = vmull_s8(vb4567c2, va0c2);
        vacc0x4567 = vpadalq_s16(vacc0x4567, vprod0x4567c2);
        const int8x8_t va0c3 = vreinterpret_s8_s16(va0.val[3]);

        const int16x8_t vprod0x0123c3 = vmull_s8(vb0123c3, va0c3);
        vacc0x0123 = vpadalq_s16(vacc0x0123, vprod0x0123c3);
        const int16x8_t vprod0x4567c3 = vmull_s8(vb4567c3, va0c3);
        vacc0x4567 = vpadalq_s16(vacc0x4567, vprod0x4567c3);

        k -= 8 * sizeof(int8_t);
      }

      if XNN_UNLIKELY(k != 0) {
        const int8x8_t va0 = vld1_s8(a0); a0 = (const int8_t*) ((uintptr_t) a0 + k);

        const int8x8_t vb0123c0 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
        const int8x8_t vb4567c0 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));

        const int8x8_t va0c0 = vreinterpret_s8_s16(vdup_lane_s16(vreinterpret_s16_s8(va0), 0));
        const int16x8_t vprod0x0123c0 = vmull_s8(vb0123c0, va0c0);
        vacc0x0123 = vpadalq_s16(vacc0x0123, vprod0x0123c0);
        const int16x8_t vprod0x4567c0 = vmull_s8(vb4567c0, va0c0);
        vacc0x4567 = vpadalq_s16(vacc0x4567, vprod0x4567c0);

        if (k > 2 * sizeof(int8_t)) {
          const int8x8_t vb0123c1 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
          const int8x8_t vb4567c1 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));

          const int8x8_t va0c1 = vreinterpret_s8_s16(vdup_lane_s16(vreinterpret_s16_s8(va0), 1));
          const int16x8_t vprod0x0123c1 = vmull_s8(vb0123c1, va0c1);
          vacc0x0123 = vpadalq_s16(vacc0x0123, vprod0x0123c1);
          const int16x8_t vprod0x4567c1 = vmull_s8(vb4567c1, va0c1);
          vacc0x4567 = vpadalq_s16(vacc0x4567, vprod0x4567c1);

          if (k > 4 * sizeof(int8_t)) {
            const int8x8_t vb0123c2 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));
            const int8x8_t vb4567c2 = vld1_s8(w); w = (const void*) ((uintptr_t) w + 8 * sizeof(int8_t));

            const int8x8_t va0c2 = vreinterpret_s8_s16(vdup_lane_s16(vreinterpret_s16_s8(va0), 2));
            const int16x8_t vprod0x0123c2 = vmull_s8(vb0123c2, va0c2);
            vacc0x0123 = vpadalq_s16(vacc0x0123, vprod0x0123c2);
            const int16x8_t vprod0x4567c2 = vmull_s8(vb4567c2, va0c2);
            vacc0x4567 = vpadalq_s16(vacc0x4567, vprod0x4567c2);
          }
        }
      }
      p -= 1 * sizeof(void*);
    } while (p != 0);

    float32x4_t vfpacc0x0123 = vcvtq_f32_s32(vacc0x0123);
    float32x4_t vfpacc0x4567 = vcvtq_f32_s32(vacc0x4567);

    const float32x4_t vscale = vld1q_dup_f32(&params->fp32_neon.scale);
    vfpacc0x0123 = vmulq_f32(vfpacc0x0123, vscale);
    vfpacc0x4567 = vmulq_f32(vfpacc0x4567, vscale);

    const float32x4_t vmagic_bias = vld1q_dup_f32(&params->fp32_neon.magic_bias);
    vacc0x0123 = vreinterpretq_s32_f32(vaddq_f32(vfpacc0x0123, vmagic_bias));
    vacc0x4567 = vreinterpretq_s32_f32(vaddq_f32(vfpacc0x4567, vmagic_bias));

    const int32x4_t vmagic_bias_less_output_zero_point = vld1q_dup_s32(&params->fp32_neon.magic_bias_less_output_zero_point);
    vacc0x0123 = vqsubq_s32(vacc0x0123, vmagic_bias_less_output_zero_point);
    vacc0x4567 = vqsubq_s32(vacc0x4567, vmagic_bias_less_output_zero_point);

#if XNN_ARCH_ARM64
    int16x8_t vacc0x01234567 = vqmovn_high_s32(vqmovn_s32(vacc0x0123), vacc0x4567);


    int8x8_t vout0x01234567 = vqmovn_s16(vacc0x01234567);
#else
    int16x8_t vacc0x01234567 = vcombine_s16(vqmovn_s32(vacc0x0123), vqmovn_s32(vacc0x4567));


    int8x8_t vout0x01234567 = vqmovn_s16(vacc0x01234567);
#endif

    const int8x8_t voutput_min = vld1_dup_s8(&params->fp32_neon.output_min);
    vout0x01234567 = vmax_s8(vout0x01234567, voutput_min);

    const int8x8_t voutput_max = vld1_dup_s8(&params->fp32_neon.output_max);
    vout0x01234567 = vmin_s8(vout0x01234567, voutput_max);

    if (nc >= 8) {
      vst1_s8(c0 + 0, vout0x01234567);

      c0 = (int8_t*) ((uintptr_t) c0 + cn_stride);

      a = (const int8_t**restrict) ((uintptr_t) a - ks);

      nc -= 8;
    } else {
      if (nc & 4) {
        vst1_lane_u32((void*) c0, vreinterpret_u32_s8(vout0x01234567), 0); c0 += 4;
        vout0x01234567 = vext_s8(vout0x01234567, vout0x01234567, 4);
      }
      if (nc & 2) {
        vst1_lane_u16((void*) c0, vreinterpret_u16_s8(vout0x01234567), 0); c0 += 2;
        vout0x01234567 = vext_s8(vout0x01234567, vout0x01234567, 2);
      }
      if (nc & 1) {
        vst1_lane_s8(c0, vout0x01234567, 0);
      }

      nc = 0;
    }
  } while (nc != 0);
}
