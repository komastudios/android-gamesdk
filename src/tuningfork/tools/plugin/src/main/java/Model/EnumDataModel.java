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
package Model;

import com.google.protobuf.Descriptors.EnumValueDescriptor;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class EnumDataModel {

  private String name;
  private List<String> options;

  public EnumDataModel(String name, ArrayList<String> options) {
    this.name = name;
    this.options = options;
  }

  public EnumDataModel(String name, List<EnumValueDescriptor> options) {
    this.name = name;
    this.options = new ArrayList<>();

    for (EnumValueDescriptor enumValueDescriptor : options) {
      this.options.add(enumValueDescriptor.getName());
    }
  }

  public List<String> getOptions() {
    return options;
  }

  public String getName() {
    return name;
  }

  public void updateName(String name) {
    this.name = name;
  }

  public boolean addOption(String option) {
    if (options.contains(option)) {
      return false;
    }
    options.add(option);
    return true;
  }

  public void removeOption(String option) {
    options.remove(options);
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }
    EnumDataModel enumDataModel = (EnumDataModel) o;
    return name.equals(enumDataModel.getName());
  }

  @Override
  public int hashCode() {
    return Objects.hash(name);
  }
}
