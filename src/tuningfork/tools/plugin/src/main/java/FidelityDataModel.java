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

import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.DynamicMessage;
import java.util.HashMap;

public class FidelityDataModel {

  private HashMap<FieldDescriptor, Object> fields;

  FidelityDataModel() {
    fields = new HashMap<>();
  }

  FidelityDataModel(Descriptor desc, DynamicMessage message) {

  }

  /**
   * Allows adding a new Fidelity Setting, when 'type' is an empty Integer/Float/EnumDataModel obj.
   * Allows adding a new Quality Setting, when 'type' has a value.
   */
  public boolean addField(FieldDescriptor paramName, Object type) {
    if (fields.containsKey(paramName)) {
      return false;
    }
    fields.put(paramName, type);
    return true;
  }

  public Object getValue(FieldDescriptor paramName) {
    return fields.get(paramName);
  }
}
