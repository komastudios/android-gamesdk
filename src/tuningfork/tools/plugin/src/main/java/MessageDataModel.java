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

public class MessageDataModel {
  enum Type {FIDELITY, ANNOTATION};
  private List<String> fieldNames;
  private List<String> fieldTypes;
  private Type messageType;

  MessageDataModel() {
    fieldNames = new ArrayList<>();
    fieldTypes = new ArrayList<>();
  }

  MessageDataModel(List<String> fieldNames, List<String> fieldTypes, Type messageType) {
    this.fieldTypes = fieldTypes;
    this.fieldNames = fieldNames;
    this.messageType = messageType;
  }

  public boolean addField(String paramName, String type) {
    if (fieldNames.contains(paramName)) {
      return false;
    }
    fieldNames.add(paramName);
    fieldTypes.add(type);
    return true;
  }

  public List<String> getFieldNames() {
    return fieldNames;
  }

  public List<String> getFieldValues() {
    return fieldTypes;
  }

  public void removeSetting(int index) {
    fieldNames.remove(index);
    fieldTypes.remove(index);
  }

  public boolean updateName(int index, String name) {
    if (fieldNames.contains(name)) {
      return false;
    }
    fieldNames.set(index, name);
    return true;
  }

  public void updateType(int index, String type) {
    fieldTypes.set(index, type);
  }
  
  public void setMessageType(Type messageType) {
    this.messageType = messageType;
  }
}