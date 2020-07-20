/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

package com.google.tuningfork.validation;

import com.google.common.collect.ImmutableList;
import com.google.protobuf.ByteString;
import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.EnumValueDescriptor;
import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.DynamicMessage;
import com.google.protobuf.EnumValue;
import com.google.protobuf.Field;
import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.TextFormat;
import com.google.protobuf.TextFormat.ParseException;
import com.google.tuningfork.Tuningfork.Settings;
import com.google.tuningfork.Tuningfork.Settings.AggregationStrategy;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Optional;
import java.util.Set;

/**
 * Utility methods for validating Tuning Fork protos and settings.
 */
final class ValidationUtil {

  private ValidationUtil() {
  }

  private static final Integer MAX_INTSTRUMENTATION_KEYS = 256;

  private static final ImmutableList<FieldDescriptor.Type> ALLOWED_FIDELITYPARAMS_TYPES =
      ImmutableList.of(
          FieldDescriptor.Type.ENUM, FieldDescriptor.Type.FLOAT, FieldDescriptor.Type.INT32);

  /* Validate settings */
  public static Optional<Settings> validateSettings(
      List<Integer> enumSizes, String settingsTextProto, ErrorCollector errors) {
    try {
      Settings.Builder builder = Settings.newBuilder();
      TextFormat.merge(settingsTextProto, builder);
      Settings settings = builder.build();
      validateSettings(settings, enumSizes, errors);
      return Optional.of(settings);
    } catch (ParseException e) {
      errors.addError(ErrorType.SETTINGS_PARSING, "Parsing tuningfork_settings.txt", e);
      return Optional.empty();
    }
  }

  /* Validate settings */
  public static final void validateSettings(
      List<Integer> enumSizes, ByteString settingsContent, ErrorCollector errors) {
    try {
      Settings settings = Settings.parseFrom(settingsContent);
      validateSettings(settings, enumSizes, errors);
    } catch (InvalidProtocolBufferException e) {
      errors.addError(ErrorType.SETTINGS_PARSING, "Parsing tuningfork_settings.bin", e);
    }
  }

  /* Validate settings */
  public static final void validateSettings(
      Settings settings, List<Integer> enumSizes, ErrorCollector errors) {
    validateSettingsAggregation(settings, enumSizes, errors);
    validateSettingsHistograms(settings, errors);
    validateSettingsBaseUri(settings, errors);
    validateSettingsApiKey(settings, errors);
    // We don't validate default_fidelity_parameters_filename
    validateSettingsRequestTimeouts(settings, errors);
    validateSettingsLoadingAnnotationIndex(settings, errors);
  }

  /*
   * Validate Histograms
   * No restrictions since Tuning Fork Scaled allows empty settings.
   * */
  public static void validateSettingsHistograms(Settings settings, ErrorCollector errors) {
  }

  /*
   * Validate Aggregation
   * */
  public static void validateSettingsAggregation(
      Settings settings, List<Integer> enumSizes, ErrorCollector errors) {

    if (!settings.hasAggregationStrategy()) {
      errors.addError(ErrorType.AGGREGATION_EMPTY, "Aggregation message is null");
      return;
    }

    AggregationStrategy aggregation = settings.getAggregationStrategy();

    if (!aggregation.hasMaxInstrumentationKeys()) {
      errors.addError(
          ErrorType.AGGREGATION_INSTRUMENTATION_KEY,
          "Aggregation strategy doesn't have max_instrumentation_key field");
    }
    int maxKey = aggregation.getMaxInstrumentationKeys();
    if (maxKey < 1 || maxKey > MAX_INTSTRUMENTATION_KEYS) {
      errors.addError(
          ErrorType.AGGREGATION_INSTRUMENTATION_KEY,
          String.format(
              "max_instrumentation_keys should be between 1 and %d, current value is %d",
              MAX_INTSTRUMENTATION_KEYS, maxKey));
    }

    int annotationCount = aggregation.getAnnotationEnumSizeCount();
    if (annotationCount != enumSizes.size()) {
      errors.addError(
          ErrorType.AGGREGATION_ANNOTATIONS,
          "\"tuningfork_settings.bin\" should contains same number of annotations"
              + " as \"dev_tuningfork.proto\"");
      return;
    }

    for (int i = 0; i < annotationCount; ++i) {
      Integer annSize = (Integer) aggregation.getAnnotationEnumSize(i);
      if (!annSize.equals(enumSizes.get(i))) {
        errors.addError(
            ErrorType.AGGREGATION_ANNOTATIONS,
            String.format(
                "\"tuningfork_settings.bin\" should contains same annotations as "
                    + "\"dev_tuningfork.proto\", expected %d, but was %d",
                enumSizes.get(i), annSize));
      }
    }
  }

  public static final void validateDevFidelityParams(
      Collection<ByteString> devFidelityList,
      Descriptor fidelityParamsDesc,
      ErrorCollector errors,
      List<DynamicMessage> fidelityMessages) {
    if (devFidelityList.isEmpty()) {
      errors.addError(ErrorType.DEV_FIDELITY_PARAMETERS_EMPTY, "Fidelity parameters list is empty");
      return;
    }
    devFidelityList.forEach(
        entry -> validateDevFidelityParams(entry, fidelityParamsDesc, errors, fidelityMessages));
  }

  /*
   * Helper method to check if [...,valueA, valueB, valueC,...] are in order.
   * They are in order if (valueA - valueB) * (valueC - valueB) >= 0.
   */
  private static final boolean hasComparisonErrorsWithPrevValue(float[] firstComparison,
      Object fieldValue,
      HashMap<FieldDescriptor, Float> prevValueForDescriptor, FieldDescriptor currentField,
      int fieldsIndex,
      ErrorCollector errors) {
    float currentComparison = prevValueForDescriptor.containsKey(currentField) ?
        Float.parseFloat(fieldValue.toString()) - prevValueForDescriptor.get(currentField) : 0;

    if (currentComparison * firstComparison[fieldsIndex] < 0) {
      errors
          .addWarning(ErrorType.DEV_FIDELITY_PARAMETERS_ORDER, "Fidelity parameters should be " +
              "in either increasing or decreasing order.");
      return true;
    }

    if (firstComparison[fieldsIndex] == 0 && currentComparison != 0) {
      firstComparison[fieldsIndex] = currentComparison;
    }
    prevValueForDescriptor.put(currentField, Float.valueOf(fieldValue.toString()));
    return false;
  }

  /*
   * Validate content of fidelity parameters from all dev_tuningfork_fidelityparams_*.bin files.
   * These should be in either increasing/decreasing order, and at least one value must be
   * different than zero.
   * Only works for int32, float, enums.
   */
  public static final void validateDevFidelityParamsOrder(
      Descriptor fidelityParamsDesc,
      List<DynamicMessage> fidelityMessages,
      ErrorCollector errors) {
    float[] firstComparison = new float[fidelityParamsDesc.getFields().size()];
    HashMap<FieldDescriptor, Float> prevValueForDescriptor = new HashMap<>();
    List<FieldDescriptor> fields = fidelityParamsDesc.getFields();

    validationLoop:
    for (int fidelityMsgIndex = 0; fidelityMsgIndex < fidelityMessages.size(); fidelityMsgIndex++) {
      for (int fieldsIndex = 0; fieldsIndex < fields.size(); fieldsIndex++) {
        FieldDescriptor currentField = fields.get(fieldsIndex);
        Object fieldValue = fidelityMessages.get(fidelityMsgIndex).getField(currentField);

        if (fieldValue instanceof EnumValueDescriptor) {
          if (((EnumValueDescriptor) fieldValue).getNumber() == 0) {
            errors.addWarning(ErrorType.DEV_FIDELITY_PARAMETERS_ENUMS_ZERO, "Enums usually "
                + "don't have 0 values.");
            continue;
          } else {
            fieldValue = ((EnumValueDescriptor) fieldValue).getNumber();
          }
        }

        if (hasComparisonErrorsWithPrevValue(firstComparison, fieldValue, prevValueForDescriptor,
            currentField, fieldsIndex, errors) == true) {
          break validationLoop;
        }
      }
    }
  }

  /*
   * Validate content of dev_tuningfork_fidelityparams_*.bin files
   * Each file should be parsed to Fidelity proto
   * */
  public static final void validateDevFidelityParams(
      ByteString devFidelityContent, Descriptor fidelityParamsDesc, ErrorCollector errors,
      List<DynamicMessage> fidelityMessages) {
    try {
      DynamicMessage fidelityMessage =
          DynamicMessage.parseFrom(fidelityParamsDesc, devFidelityContent);
      if (fidelityMessage == null) {
        errors.addError(ErrorType.DEV_FIDELITY_PARAMETERS_EMPTY, "Fidelity parameters is empty");
      }
      fidelityMessages.add(fidelityMessage);
    } catch (InvalidProtocolBufferException e) {
      errors.addError(
          ErrorType.DEV_FIDELITY_PARAMETERS_PARSING, "Fidelity parameters not parsed properly", e);
    }
  }

  /*
   * Validate FidelityParams in proto file.
   * FidelityParams message can only contains ENUM, FLOAT and INT32.
   * FidelityParams should not be complex - no oneofs, nested types or extensions
   * Enums should not start with 0 index.
   * */
  public static final void validateFidelityParams(
      Descriptor fidelityParamsDesc, ErrorCollector errors) {
    if (fidelityParamsDesc == null) {
      errors.addError(ErrorType.FIDELITY_PARAMS_EMPTY, "FidelityParams descriptor is not exist");
      return;
    }

    if (!fidelityParamsDesc.getOneofs().isEmpty()) {
      errors.addError(
          ErrorType.FIDELITY_PARAMS_COMPLEX, "FidelityParams too complex - has oneof field");
      return;
    }

    if (!fidelityParamsDesc.getNestedTypes().isEmpty()) {
      errors.addError(
          ErrorType.FIDELITY_PARAMS_COMPLEX, "FidelityParams too complex - has nested types");
      return;
    }

    if (!fidelityParamsDesc.getExtensions().isEmpty()) {
      errors.addError(
          ErrorType.FIDELITY_PARAMS_COMPLEX, "FidelityParams too complex - has extensions");
      return;
    }

    for (FieldDescriptor field : fidelityParamsDesc.getFields()) {
      FieldDescriptor.Type type = field.getType();

      if (!ALLOWED_FIDELITYPARAMS_TYPES.contains(type)) {
        errors.addError(
            ErrorType.FIDELITY_PARAMS_TYPE,
            String.format(
                "FidelityParams can only be of type FLOAT, INT32 or ENUM, %s field has type %s",
                field.getName(), type));
      }
    }
  }

  /**
   * Validate Annotation field in proto file. Annotation can only contains enums, each enum should
   * not start from 0 index. Annotation message should not be complex - no oneof, nestedTypes ot
   * extensions Check for all _fields_ in proto Return list of enum size for annotation's field
   */
  public static final ImmutableList<Integer> validateAnnotationAndGetEnumSizes(
      Descriptor annotationDesc, ErrorCollector errors) {
    if (annotationDesc == null) {
      errors.addError(ErrorType.ANNOTATION_EMPTY, "Annotation descriptor is not exist");
      return null;
    }

    if (!annotationDesc.getOneofs().isEmpty()) {
      errors.addError(ErrorType.ANNOTATION_COMPLEX, "Annotation too complex - has oneofs");
      return null;
    }

    if (!annotationDesc.getNestedTypes().isEmpty()) {
      errors.addError(ErrorType.ANNOTATION_COMPLEX, "Annotation too complex - has nested types");
      return null;
    }

    if (!annotationDesc.getExtensions().isEmpty()) {
      errors.addError(ErrorType.ANNOTATION_COMPLEX, "Annotation too complex - hes extension");
      return null;
    }

    List<Integer> sizes = new ArrayList<>();

    for (FieldDescriptor field : annotationDesc.getFields()) {
      if (field.getType() != FieldDescriptor.Type.ENUM) {
        errors.addError(
            ErrorType.ANNOTATION_TYPE,
            String.format(
                "Annotation can only contains enums, but contains %s field with type %s",
                field.getName(), field.getType()));
        return null;
      }
      sizes.add(field.getEnumType().getValues().size());
    }
    return ImmutableList.copyOf(sizes);
  }

  private static final void validateSettingsBaseUri(Settings settings, ErrorCollector errors) {
    if (settings.hasBaseUri()) {
      // Check it's a valid URL
      final URL url;
      try {
        url = new URL(settings.getBaseUri());
        url.getHost();
      } catch (Exception e) {
        errors.addError(ErrorType.BASE_URI_NOT_URL, "base_uri is not a valid URL");
      }
    }
    // Missing is OK
  }

  private static final void validateSettingsApiKey(Settings settings, ErrorCollector errors) {
    if (settings.hasApiKey()) {
      // This checks that someone has changed from the default value in the samples.
      if (settings.getApiKey().equals("enter-your-api-key-here")) {
        errors.addError(ErrorType.API_KEY_INVALID, "api_key not set to a valid value");
      }
    } else {
      errors.addError(ErrorType.API_KEY_MISSING, "api_key is missing");
    }
  }

  private static final void validateSettingsLoadingAnnotationIndex(
      Settings settings, ErrorCollector errors) {
    if (!settings.hasLoadingAnnotationIndex()) {
      errors.addWarning(ErrorType.LOADING_ANNOTATION_INDEX_MISSING,
          "loading_annotation_index not set");
    }
  }

  private static final void validateSettingsRequestTimeouts(
      Settings settings, ErrorCollector errors) {
    if (settings.hasInitialRequestTimeoutMs()) {
      int initialRequestTimeoutMs = settings.getInitialRequestTimeoutMs();
      if (initialRequestTimeoutMs < 0) {
        errors.addError(ErrorType.INITIAL_REQUEST_TIMEOUT_INVALID,
            "initial_request_timeout_ms is invalid");
      }
    }
    if (settings.hasUltimateRequestTimeoutMs()) {
      int ultimateRequestTimeoutMs = settings.getUltimateRequestTimeoutMs();
      if (ultimateRequestTimeoutMs < 0) {
        errors.addError(ErrorType.ULTIMATE_REQUEST_TIMEOUT_INVALID,
            "ultimate_request_timeout_ms is invalid");
      }
    }
  }
}
