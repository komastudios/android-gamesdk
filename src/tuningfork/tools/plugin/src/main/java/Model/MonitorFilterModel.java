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

import Controller.Monitoring.HistogramTree.Node;
import View.Monitoring.MonitoringTab.JTreeNode;
import java.util.List;

public class MonitorFilterModel {

  private final String phoneModel;
  private final List<JTreeNode> annotations;
  private final Node fidelity;

  public MonitorFilterModel(String phoneModel, List<JTreeNode> annotations,
      Node fidelity) {
    this.phoneModel = phoneModel;
    this.annotations = annotations;
    this.fidelity = fidelity;
  }

  public String getPhoneModel() {
    return phoneModel;
  }

  public List<JTreeNode> getAnnotations() {
    return annotations;
  }

  public Node getFidelity() {
    return fidelity;
  }

  @Override
  public String toString() {
    return "MonitorFilterModel{" +
        "phoneModel='" + phoneModel + '\'' +
        ", annotations=" + annotations +
        ", fidelity=" + fidelity +
        '}';
  }
}
