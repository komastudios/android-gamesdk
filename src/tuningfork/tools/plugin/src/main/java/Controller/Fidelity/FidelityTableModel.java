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

import View.Fidelity.FidelityTableData;
import View.Fidelity.FieldType;
import com.intellij.openapi.ui.Messages;

import javax.swing.table.AbstractTableModel;
import java.util.ArrayList;
import java.util.List;

public class FidelityTableModel extends AbstractTableModel {

  private final String[] columnNames = {"Type", "Parameter name"};
  private List<FidelityTableData> data;
  private FidelityTabController controller;

  public FidelityTableModel(FidelityTabController controller) {
    this.controller = controller;
    data = new ArrayList<>();
  }

  @Override
  public int getRowCount() {
    return data.size();
  }

  @Override
  public int getColumnCount() {
    return columnNames.length;
  }

  @Override
  public Object getValueAt(int rowIndex, int columnIndex) {
    return data.get(rowIndex);
  }

  @Override
  public String getColumnName(int column) {
    return columnNames[column];
  }

  public void addRow(FidelityTableData row) {
    data.add(row);
    controller.addFidelityField("", row.getFieldType().getName());
    fireTableRowsInserted(getRowCount() - 1, getRowCount());
  }

  @Override
  public boolean isCellEditable(int row, int column) {
    return true;
  }

  @Override
  public void setValueAt(Object value, int row, int column) {
    if (column == 0) {
      FieldType fieldType = (FieldType) value;
      if (fieldType.equals(FieldType.ENUM) && !controller.hasEnums()) {
        Messages.showErrorDialog("You Need to Add An Enum First.", "Unable to Choose Enum");
        return;
      }
      if (fieldType.equals(FieldType.INT32) || fieldType.equals(FieldType.FLOAT)) {
        controller.updateType(row, fieldType.getName());
      }
      data.get(row).setFieldType((FieldType) value);
    } else if (column == 1) {
      if (data.get(row).getFieldType().equals(FieldType.ENUM)) {
        FidelityTableData fidelityTableData = (FidelityTableData) value;
        data.get(row).setFieldEnumName(fidelityTableData.getFieldEnumName());
        data.get(row).setFieldParamName(fidelityTableData.getFieldParamName());
        controller.updateName(row, fidelityTableData.getFieldParamName());
        controller.updateType(row, fidelityTableData.getFieldEnumName());
      } else {
        data.get(row).setFieldParamName(value.toString());
        controller.updateName(row, value.toString());
      }
    }
    fireTableCellUpdated(row, column);
  }

  public void removeRow(int row) {
    data.remove(row);
    controller.removeFidelityField(row);
    fireTableDataChanged();
  }
}
