/*
 * Copyright (C) 2020 The Android Open Source Project
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

package Utils.Validation;

import com.google.common.collect.LinkedListMultimap;
import com.google.common.collect.ListMultimap;
import com.google.common.collect.Multimap;

/**
 * Collecting validation errors
 */
public class ErrorCollector {

  private final ListMultimap<ErrorType, String> errors = LinkedListMultimap.create();
  private final ListMultimap<ErrorType, String> warnings = LinkedListMultimap.create();

  public Multimap<ErrorType, String> getErrors() {
    return errors;
  }

  public void addError(ErrorType errorType, String message) {
    errors.put(errorType, message);
  }

  public void addError(ErrorType errorType, String message, Exception e) {
    errors.put(errorType, message);
  }

  public Integer getErrorCount() {
    return errors.size();
  }

  public Integer getErrorCount(ErrorType errorType) {
    return errors.get(errorType).size();
  }

  public Boolean hasErrors(ErrorType.ErrorGroup group) {
    for (ErrorType errorType : ErrorType.values()) {
      if (errorType.getGroup() == group && errors.containsKey(errorType)) {
        return true;
      }
    }
    return false;
  }

  public Boolean hasAnnotationErrors() {
    return hasErrors(ErrorType.ErrorGroup.ANNOTATION);
  }

  public Boolean hasFidelityParamsErrors() {
    return hasErrors(ErrorType.ErrorGroup.FIDELITY);
  }

  public Boolean hasSettingsErrors() {
    return hasErrors(ErrorType.ErrorGroup.SETTINGS);
  }

  public Integer getWarningCount() {
    return warnings.size();
  }

  public Integer getWarningCount(ErrorType errorType) {
    return warnings.get(errorType).size();
  }

  public void addWarning(ErrorType errorType, String message) {
    warnings.put(errorType, message);
  }

  public Multimap<ErrorType, String> getWarnings() {
    return warnings;
  }
}