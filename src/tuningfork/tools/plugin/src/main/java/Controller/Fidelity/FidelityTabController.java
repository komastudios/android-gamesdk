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

package Controller.Fidelity;

import Model.EnumDataModel;
import Model.MessageDataModel;
import View.Fidelity.FidelityTableData;
import View.Fidelity.FieldType;
import com.intellij.openapi.ui.ValidationInfo;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;
import javax.swing.JTable;

public class FidelityTabController {

  private final MessageDataModel fidelityMessage;
  private final List<EnumDataModel> enums;
  private static final String FIELD_NAME_PATTERN = "[a-zA-Z_]+$";

  public FidelityTabController(MessageDataModel fidelityMessage, List<EnumDataModel> enums) {
    super();
    this.fidelityMessage = fidelityMessage;
    this.enums = enums;
  }

  public void addFidelityField(String name, String type) {
    fidelityMessage.addField(name, type);
  }

  public void removeFidelityField(int index) {
    fidelityMessage.removeSetting(index);
  }

  public void updateName(int index, String newName) {
    fidelityMessage.updateName(index, newName);
  }

  public void updateType(int index, String type) {
    fidelityMessage.updateType(index, type);
  }

  public void addRowAction(JTable jtable) {
    FidelityTableModel model = (FidelityTableModel) jtable.getModel();
    model.addRow(new FidelityTableData(FieldType.INT32, "", ""));
  }

  public void removeRowAction(JTable jtable) {
    FidelityTableModel model = (FidelityTableModel) jtable.getModel();
    int row = jtable.getSelectedRow();
    if (jtable.getCellEditor() != null) {
      jtable.getCellEditor().stopCellEditing();
    }
    model.removeRow(row);
  }

  public List<String> getEnumNames() {
    return enums.stream().map(EnumDataModel::getName)
        .collect(Collectors.toList());
  }

  public List<ValidationInfo> validate() {
    List<ValidationInfo> validationInfos = new ArrayList<>();
    List<String> names = fidelityMessage.getFieldNames();
    boolean duplicateField =
        names.stream().anyMatch(name -> Collections.frequency(names, name) > 1);
    if (duplicateField) {
      validationInfos.add(new ValidationInfo("Duplicate Fields Are Not Allowed"));
    }
    names.stream()
        .filter(s -> !s.matches(FIELD_NAME_PATTERN) && !s.isEmpty())
        .forEach(s ->
            validationInfos.add(
                new ValidationInfo(s + " Does Not Match The Pattern [a-zA-Z_].")));
    if (names.stream().anyMatch(String::isEmpty)) {
      validationInfos.add(new ValidationInfo("Empty Fields Are Not Allowed."));
    }
    return validationInfos;
  }
}
