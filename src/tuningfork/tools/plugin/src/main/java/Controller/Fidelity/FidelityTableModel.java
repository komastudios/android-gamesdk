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

import Controller.Enum.EnumController;
import View.Fidelity.FidelityTableData;
import View.Fidelity.FieldType;
import com.intellij.openapi.ui.Messages;
import java.util.ArrayList;
import java.util.List;
import javax.swing.table.AbstractTableModel;

public class FidelityTableModel extends AbstractTableModel {

  private final String[] columnNames = {"Type", "Parameter name"};
  private List<FidelityTableData> data;
  private EnumController controller;

  public FidelityTableModel(EnumController controller) {
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
        Messages.showErrorDialog("You Need to Add An Enum First.",
            "Unable to Choose Enum");
        return;
      }
      data.get(row).setFieldType((FieldType) value);
    } else if (column == 1) {
      if (data.get(row).getFieldType().equals(FieldType.ENUM)) {
        FidelityTableData fidelityTableData = (FidelityTableData) value;
        data.get(row).setFieldEnumName(fidelityTableData.getFieldEnumName());
        data.get(row).setFieldParamName(fidelityTableData.getFieldParamName());
      } else {
        data.get(row).setFieldParamName(value.toString());
      }
    }
    fireTableCellUpdated(row, column);
  }

  public void removeRow(int row) {
    data.remove(row);
    fireTableDataChanged();
  }
}
