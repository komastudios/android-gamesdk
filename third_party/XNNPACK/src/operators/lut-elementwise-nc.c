// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <xnnpack.h>
#include <xnnpack/allocator.h>
#include <xnnpack/operator.h>
#include <xnnpack/log.h>


typedef float (*xnn_lut_init_fn)(float, const void*);

static enum xnn_status create_lut_elementwise_nc(
    size_t channels,
    size_t input_stride,
    size_t output_stride,
    int32_t input_zero_point,
    float input_scale,
    int32_t input_min,
    long output_zero_point,
    float output_scale,
    long output_min,
    long output_max,
    uint32_t flags,
    xnn_lut_init_fn init_fn,
    const void* init_params,
    enum xnn_operator_type operator_type,
    xnn_operator_t* lut_elementwise_op_out)
{
  xnn_operator_t lut_elementwise_op = NULL;
  enum xnn_status status = xnn_status_uninitialized;

  if ((xnn_params.init_flags & XNN_INIT_FLAG_XNNPACK) == 0) {
    xnn_log_error("failed to create %s operator: XNNPACK is not initialized",
      xnn_operator_type_to_string(operator_type));
    goto error;
  }

  status = xnn_status_invalid_parameter;

  if (channels == 0) {
    xnn_log_error(
      "failed to create %s operator with %zu channels: number of channels must be non-zero",
      xnn_operator_type_to_string(operator_type), channels);
    goto error;
  }

  if (input_stride < channels) {
    xnn_log_error(
      "failed to create %s operator with input element stride of %zu: "
      "stride must be at least as large as the number of channels (%zu)",
      xnn_operator_type_to_string(operator_type), input_stride, channels);
    goto error;
  }

  if (output_stride < channels) {
    xnn_log_error(
      "failed to create %s operator with output element stride of %zu: "
      "stride must be at least as large as the number of channels (%zu)",
      xnn_operator_type_to_string(operator_type), output_stride, channels);
    goto error;
  }

  if (input_scale <= 0.0f || !isnormal(input_scale)) {
    xnn_log_error(
      "failed to create %s operator with %.7g input scale: scale must be finite, normalized, and positive",
      xnn_operator_type_to_string(operator_type), input_scale);
    goto error;
  }

  if (output_scale <= 0.0f || !isnormal(output_scale)) {
    xnn_log_error(
      "failed to create %s operator with %.7g output scale: scale must be finite, normalized, and positive",
      xnn_operator_type_to_string(operator_type), output_scale);
    goto error;
  }

  if (output_min >= output_max) {
    xnn_log_error(
      "failed to create %s operator with [%ld, %ld] output range: range min must be below range max",
      xnn_operator_type_to_string(operator_type), output_min, output_max);
    goto error;
  }

  status = xnn_status_out_of_memory;

  lut_elementwise_op = xnn_allocate_zero_simd_memory(sizeof(struct xnn_operator));
  if (lut_elementwise_op == NULL) {
    xnn_log_error(
      "failed to allocate %zu bytes for %s operator descriptor",
      sizeof(struct xnn_operator), xnn_operator_type_to_string(operator_type));
    goto error;
  }

  lut_elementwise_op->lookup_table = xnn_allocate_simd_memory(256 * sizeof(uint8_t));
  if (lut_elementwise_op->lookup_table == NULL) {
    xnn_log_error(
      "failed to allocate 256 bytes for %s operator lookup table",
      xnn_operator_type_to_string(operator_type));
    goto error;
  }

  uint8_t* lookup_table = lut_elementwise_op->lookup_table;
  const float inv_output_scale = 1.0f / output_scale;
  for (int32_t i = input_min; i < input_min + 256; i++) {
    const float dequantized_input = (i - input_zero_point) * input_scale;
    const float dequantized_output = init_fn(dequantized_input, init_params);
    long quantized_output = lrintf(dequantized_output * inv_output_scale) + output_zero_point;
    quantized_output = XNN_UNPREDICTABLE(quantized_output < output_min) ? output_min : quantized_output;
    quantized_output = XNN_UNPREDICTABLE(quantized_output > output_max) ? output_max : quantized_output;
    lookup_table[(uint8_t) i] = (uint8_t) quantized_output;
  }

  lut_elementwise_op->channels = channels;
  lut_elementwise_op->input_pixel_stride = input_stride;
  lut_elementwise_op->output_pixel_stride = output_stride;

  lut_elementwise_op->type = operator_type;
  lut_elementwise_op->flags = flags;

  lut_elementwise_op->state = xnn_run_state_invalid;

  *lut_elementwise_op_out = lut_elementwise_op;
  return xnn_status_success;

error:
  xnn_delete_operator(lut_elementwise_op);
  return status;
}

static float calculate_elu(float x, const float* alpha_ptr) {
  const float alpha = *alpha_ptr;
  return signbit(x) ? alpha * expm1f(x) : x;
}

enum xnn_status xnn_create_elu_nc_qs8(
    size_t channels,
    size_t input_stride,
    size_t output_stride,
    float alpha,
    int8_t input_zero_point,
    float input_scale,
    int8_t output_zero_point,
    float output_scale,
    int8_t output_min,
    int8_t output_max,
    uint32_t flags,
    xnn_operator_t* elu_op_out)
{
  if (alpha <= 0.0f || !isnormal(alpha)) {
    xnn_log_error(
      "failed to create %s operator with %.7g alpha parameter: alpha must be finite, normalized, and positive",
      xnn_operator_type_to_string(xnn_operator_type_elu_nc_qs8), alpha);
    return xnn_status_invalid_parameter;
  }

  return create_lut_elementwise_nc(
    channels, input_stride, output_stride,
    (int32_t) input_zero_point, input_scale, INT8_MIN,
    (long) output_zero_point, output_scale,
    (long) output_min, (long) output_max,
    flags,
    (xnn_lut_init_fn) &calculate_elu, &alpha,
    xnn_operator_type_elu_nc_qs8, elu_op_out);
}

static float calculate_leaky_relu(float x, const float* negative_slope_ptr) {
  const float negative_slope = *negative_slope_ptr;
  return signbit(x) ? x * negative_slope : x;
}

enum xnn_status xnn_create_leaky_relu_nc_qu8(
    size_t channels,
    size_t input_stride,
    size_t output_stride,
    float negative_slope,
    uint8_t input_zero_point,
    float input_scale,
    uint8_t output_zero_point,
    float output_scale,
    uint8_t output_min,
    uint8_t output_max,
    uint32_t flags,
    xnn_operator_t* leaky_relu_op_out)
{
  if (negative_slope <= 0.0f || !isnormal(negative_slope)) {
    xnn_log_error(
      "failed to create %s operator with %.7g negative slope: slope must be finite, normalized, and positive",
      xnn_operator_type_to_string(xnn_operator_type_leaky_relu_nc_qu8), negative_slope);
    return xnn_status_invalid_parameter;
  }

  if (negative_slope > 1.0f) {
    xnn_log_error(
      "failed to create %s operator with %.7g negative slope: slope must not exceed 1.0",
      xnn_operator_type_to_string(xnn_operator_type_leaky_relu_nc_qu8), negative_slope);
    return xnn_status_invalid_parameter;
  }

  const float input_output_scale = input_scale / output_scale;
  if (input_output_scale < 0x1.0p-8f || input_output_scale >= 0x1.0p+8f) {
    xnn_log_error(
      "failed to create %s operator with %.7g input-to-output scale ratio: "
      "scale ratio must be in [2**-8, 2**8) range",
      xnn_operator_type_to_string(xnn_operator_type_leaky_relu_nc_qu8), input_output_scale);
    return xnn_status_unsupported_parameter;
  }

  return create_lut_elementwise_nc(
    channels, input_stride, output_stride,
    (int32_t) (uint32_t) input_zero_point, input_scale, 0 /* input min */,
    (long) (unsigned long) output_zero_point, output_scale,
    (long) (unsigned long) output_min, (long) (unsigned long) output_max,
    flags,
    (xnn_lut_init_fn) &calculate_leaky_relu, &negative_slope,
    xnn_operator_type_leaky_relu_nc_qu8, leaky_relu_op_out);
}

static float calculate_sigmoid(float x, const void* params) {
  return signbit(x) ? 1.0f / (1.0f + expf(-x)) : 1.0f - 1.0f / (1.0f + expf(x));
}

enum xnn_status xnn_create_sigmoid_nc_qs8(
    size_t channels,
    size_t input_stride,
    size_t output_stride,
    int8_t input_zero_point,
    float input_scale,
    int8_t output_zero_point,
    float output_scale,
    int8_t output_min,
    int8_t output_max,
    uint32_t flags,
    xnn_operator_t* sigmoid_op_out)
{
  if (output_scale != 0x1.0p-8f) {
    xnn_log_error(
      "failed to create %s operator with %.7g output scale: only output scale of 1/256 is supported",
      xnn_operator_type_to_string(xnn_operator_type_sigmoid_nc_qs8), output_scale);
    return xnn_status_unsupported_parameter;
  }

  if (output_zero_point != -128) {
    xnn_log_error(
      "failed to create %s operator with %" PRIu8 " output zero point: only output zero point of -128 is supported",
      xnn_operator_type_to_string(xnn_operator_type_sigmoid_nc_qs8), output_zero_point);
    return xnn_status_unsupported_parameter;
  }

  return create_lut_elementwise_nc(
    channels, input_stride, output_stride,
    (int32_t) input_zero_point, input_scale, INT8_MIN,
    (long) output_zero_point, output_scale,
    (long) output_min, (long) output_max,
    flags,
    (xnn_lut_init_fn) &calculate_sigmoid, NULL,
    xnn_operator_type_sigmoid_nc_qs8, sigmoid_op_out);
}

enum xnn_status xnn_create_sigmoid_nc_qu8(
    size_t channels,
    size_t input_stride,
    size_t output_stride,
    uint8_t input_zero_point,
    float input_scale,
    uint8_t output_zero_point,
    float output_scale,
    uint8_t output_min,
    uint8_t output_max,
    uint32_t flags,
    xnn_operator_t* sigmoid_op_out)
{
  if (output_scale != 0x1.0p-8f) {
    xnn_log_error(
      "failed to create %s operator with %.7g output scale: only output scale of 1/256 is supported",
      xnn_operator_type_to_string(xnn_operator_type_sigmoid_nc_qu8), output_scale);
    return xnn_status_unsupported_parameter;
  }

  if (output_zero_point != 0) {
    xnn_log_error(
      "failed to create %s operator with %" PRIu8 " output zero point: only output zero point of 0 is supported",
      xnn_operator_type_to_string(xnn_operator_type_sigmoid_nc_qu8), output_zero_point);
    return xnn_status_unsupported_parameter;
  }

  return create_lut_elementwise_nc(
    channels, input_stride, output_stride,
    (int32_t) (uint32_t) input_zero_point, input_scale, 0 /* input min */,
    (long) (unsigned long) output_zero_point, output_scale,
    (long) (unsigned long) output_min, (long) (unsigned long) output_max,
    flags,
    (xnn_lut_init_fn) &calculate_sigmoid, NULL,
    xnn_operator_type_sigmoid_nc_qu8, sigmoid_op_out);
}

static float calculate_tanh(float x, const void* params) {
  return tanhf(x);
}

enum xnn_status xnn_create_tanh_nc_qs8(
    size_t channels,
    size_t input_stride,
    size_t output_stride,
    int8_t input_zero_point,
    float input_scale,
    int8_t output_zero_point,
    float output_scale,
    int8_t output_min,
    int8_t output_max,
    uint32_t flags,
    xnn_operator_t* tanh_op_out)
{
  if (output_scale != 0x1.0p-7f) {
    xnn_log_error(
      "failed to create %s operator with %.7g output scale: only output scale of 1/128 is supported",
      xnn_operator_type_to_string(xnn_operator_type_tanh_nc_qs8), output_scale);
    return xnn_status_unsupported_parameter;
  }

  if (output_zero_point != 0) {
    xnn_log_error(
      "failed to create %s operator with %" PRIu8 " output zero point: only output zero point of 0 is supported",
      xnn_operator_type_to_string(xnn_operator_type_tanh_nc_qs8), output_zero_point);
    return xnn_status_unsupported_parameter;
  }

  return create_lut_elementwise_nc(
    channels, input_stride, output_stride,
    (int32_t) input_zero_point, input_scale, INT8_MIN,
    (long) output_zero_point, output_scale,
    (long) output_min, (long) output_max,
    flags,
    (xnn_lut_init_fn) &calculate_tanh, NULL,
    xnn_operator_type_tanh_nc_qs8, tanh_op_out);
}

enum xnn_status xnn_create_tanh_nc_qu8(
    size_t channels,
    size_t input_stride,
    size_t output_stride,
    uint8_t input_zero_point,
    float input_scale,
    uint8_t output_zero_point,
    float output_scale,
    uint8_t output_min,
    uint8_t output_max,
    uint32_t flags,
    xnn_operator_t* tanh_op_out)
{
  if (output_scale != 0x1.0p-7f) {
    xnn_log_error(
      "failed to create %s operator with %.7g output scale: only output scale of 1/128 is supported",
      xnn_operator_type_to_string(xnn_operator_type_tanh_nc_qu8), output_scale);
    return xnn_status_unsupported_parameter;
  }

  if (output_zero_point != 128) {
    xnn_log_error(
      "failed to create %s operator with %" PRIu8 " output zero point: only output zero point of 128 is supported",
      xnn_operator_type_to_string(xnn_operator_type_tanh_nc_qu8), output_zero_point);
    return xnn_status_unsupported_parameter;
  }

  return create_lut_elementwise_nc(
    channels, input_stride, output_stride,
    (int32_t) (uint32_t) input_zero_point, input_scale, 0 /* input min */,
    (long) (unsigned long) output_zero_point, output_scale,
    (long) (unsigned long) output_min, (long) (unsigned long) output_max,
    flags,
    (xnn_lut_init_fn) &calculate_tanh, NULL,
    xnn_operator_type_tanh_nc_qu8, tanh_op_out);
}

static enum xnn_status setup_lut_elementwise_nc(
    xnn_operator_t lut_elementwise_op,
    enum xnn_operator_type expected_operator_type,
    size_t batch_size,
    const void* input,
    void* output)
{
  if (lut_elementwise_op->type != expected_operator_type) {
    xnn_log_error("failed to setup operator: operator type mismatch (expected %s, got %s)",
      xnn_operator_type_to_string(expected_operator_type),
      xnn_operator_type_to_string(lut_elementwise_op->type));
    return xnn_status_invalid_parameter;
  }
  lut_elementwise_op->state = xnn_run_state_invalid;

  if ((xnn_params.init_flags & XNN_INIT_FLAG_XNNPACK) == 0) {
    xnn_log_error(
      "failed to setup %s operator: XNNPACK is not initialized",
      xnn_operator_type_to_string(expected_operator_type));
    return xnn_status_uninitialized;
  }

  if (batch_size == 0) {
    lut_elementwise_op->state = xnn_run_state_skip;
    return xnn_status_success;
  }

  const size_t channels = lut_elementwise_op->channels;
  const size_t input_stride = lut_elementwise_op->input_pixel_stride;
  const size_t output_stride = lut_elementwise_op->output_pixel_stride;
  if ((((input_stride ^ channels) | (output_stride ^ channels)) == 0) || batch_size == 1) {
    const size_t block_size = 1024;
    lut_elementwise_op->context.lut_contiguous = (struct lut_contiguous_context) {
      .x = input,
      .x_stride = input_stride * sizeof(uint8_t),
      .t = lut_elementwise_op->lookup_table,
      .y = output,
      .y_stride = output_stride * sizeof(uint8_t),
      .ukernel = xnn_params.x8.lut,
    };
    lut_elementwise_op->compute.type = xnn_parallelization_type_1d_tile_1d;
    lut_elementwise_op->compute.task_1d_tile_1d = (pthreadpool_task_1d_tile_1d_t) xnn_compute_lut_contiguous;
    lut_elementwise_op->compute.range[0] = batch_size * channels * sizeof(uint8_t);
    lut_elementwise_op->compute.tile[0] = block_size;
  } else {
    lut_elementwise_op->context.lut_strided = (struct lut_strided_context) {
      .n = channels,
      .x = input,
      .x_stride = input_stride * sizeof(uint8_t),
      .t = lut_elementwise_op->lookup_table,
      .y = output,
      .y_stride = output_stride * sizeof(uint8_t),
      .ukernel = xnn_params.x8.lut,
    };
    lut_elementwise_op->compute.type = xnn_parallelization_type_1d;
    lut_elementwise_op->compute.task_1d = (pthreadpool_task_1d_t) xnn_compute_lut_strided;
    lut_elementwise_op->compute.range[0] = batch_size;
    lut_elementwise_op->compute.tile[0] = 0;
  }
  lut_elementwise_op->state = xnn_run_state_ready;

  return xnn_status_success;
}

enum xnn_status xnn_setup_elu_nc_qs8(
    xnn_operator_t sigmoid_op,
    size_t batch_size,
    const int8_t* input,
    int8_t* output,
    pthreadpool_t threadpool)
{
  return setup_lut_elementwise_nc(
    sigmoid_op, xnn_operator_type_elu_nc_qs8,
    batch_size, input, output);
}

enum xnn_status xnn_setup_leaky_relu_nc_qu8(
    xnn_operator_t leaky_relu_op,
    size_t batch_size,
    const uint8_t* input,
    uint8_t* output,
    pthreadpool_t threadpool)
{
  return setup_lut_elementwise_nc(
    leaky_relu_op, xnn_operator_type_leaky_relu_nc_qu8,
    batch_size, input, output);
}

enum xnn_status xnn_setup_sigmoid_nc_qs8(
    xnn_operator_t sigmoid_op,
    size_t batch_size,
    const int8_t* input,
    int8_t* output,
    pthreadpool_t threadpool)
{
  return setup_lut_elementwise_nc(
    sigmoid_op, xnn_operator_type_sigmoid_nc_qs8,
    batch_size, input, output);
}

enum xnn_status xnn_setup_sigmoid_nc_qu8(
    xnn_operator_t sigmoid_op,
    size_t batch_size,
    const uint8_t* input,
    uint8_t* output,
    pthreadpool_t threadpool)
{
  return setup_lut_elementwise_nc(
    sigmoid_op, xnn_operator_type_sigmoid_nc_qu8,
    batch_size, input, output);
}

enum xnn_status xnn_setup_tanh_nc_qs8(
    xnn_operator_t tanh_op,
    size_t batch_size,
    const int8_t* input,
    int8_t* output,
    pthreadpool_t threadpool)
{
  return setup_lut_elementwise_nc(
    tanh_op, xnn_operator_type_tanh_nc_qs8,
    batch_size, input, output);
}

enum xnn_status xnn_setup_tanh_nc_qu8(
    xnn_operator_t tanh_op,
    size_t batch_size,
    const uint8_t* input,
    uint8_t* output,
    pthreadpool_t threadpool)
{
  return setup_lut_elementwise_nc(
    tanh_op, xnn_operator_type_tanh_nc_qu8,
    batch_size, input, output);
}
