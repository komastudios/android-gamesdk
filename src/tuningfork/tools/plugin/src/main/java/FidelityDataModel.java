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

import java.util.ArrayList;
import java.util.List;

public class FidelityDataModel {

  private List<String> fieldNames;
  private List<Object> fieldValues;

  FidelityDataModel() {
    fieldNames = new ArrayList<>();
    fieldValues = new ArrayList<>();
  }

  /**
   * Allows adding a new Fidelity Setting, when 'type' is an empty Integer/Float/EnumDataModel obj.
   * Allows adding a new Quality Setting, when 'type' has a value.
   */
  public boolean addField(String paramName, Object type) {
    if (fieldNames.contains(paramName)) {
      return false;
    }
    fieldNames.add(paramName);
    fieldValues.add(type);
    return true;
  }

  public List<String> getFieldNames() {
    return fieldNames;
  }

  public List<Object> getFieldValues() {
    return fieldValues;
  }
}
