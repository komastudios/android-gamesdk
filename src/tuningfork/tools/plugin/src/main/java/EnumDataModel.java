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

import com.google.protobuf.Descriptors.EnumValueDescriptor;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

public class EnumDataModel {

  /**
   * setId holds the index number that is chosen for a specific enum in a quality setting
   */
  private Integer setId;
  private String name;
  private List<String> options;

  EnumDataModel(String name, ArrayList<String> options) {
    this.name = name;
    this.options = options;
  }

  EnumDataModel(String name, List<EnumValueDescriptor> options) {
    this.name = name;
    this.options = new ArrayList<>();

    for (EnumValueDescriptor enumValueDescriptor : options) {
      this.options.add(enumValueDescriptor.getName());
    }
  }

  EnumDataModel(String name, List<EnumValueDescriptor> options, int setId) {
    this(name, options);
    this.setId = setId;
  }

  public List<String> getOptions() {
    return options;
  }

  public String getName() {
    return name;
  }

  public Optional<Integer> getId() {
    return Optional.of(setId);
  }

  public void updateName(String name) {
    this.name = name;
  }

  public void addOption(String option) {
    options.add(option);
  }

  public void removeOption(String option) {
    options.remove(options);
  }
}