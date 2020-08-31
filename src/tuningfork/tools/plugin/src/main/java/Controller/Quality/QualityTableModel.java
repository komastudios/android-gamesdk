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

package Controller.Quality;

import Model.MessageDataModel;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.stream.Collectors;
import javax.swing.JTable;
import javax.swing.table.AbstractTableModel;

public class QualityTableModel extends AbstractTableModel {

  private String[] columnNames;
  private List<String[]> data;
  private HashMap<Integer, String> enumNames;

  public QualityTableModel(MessageDataModel fidelityDataModel) {
    data = new ArrayList<>();
    enumNames = new HashMap<>();
    setColumnNames(fidelityDataModel);
  }

  public void setColumnNames(MessageDataModel fidelityDataModel) {
    List<String> names = fidelityDataModel.getFieldNames();
    List<String> values = fidelityDataModel.getFieldValues();
    int fieldCount = names.size();
    columnNames = new String[fieldCount + 1];
    columnNames[0] = "File no.";

    for (int i = 0; i < fieldCount; i++) {
      columnNames[i + 1] = names.get(i) + " - " + values.get(i);
      if (!values.get(i).equals("int32") && !values.get(i).equals("float")) {
        enumNames.put(i + 1, values.get(i));
      }
    }
  }


  public HashMap<Integer, String> getEnumNames() {
    return enumNames;
  }

  public void addRow(String[] row, JTable table) {
    data.add(row);
    fireTableRowsInserted(getRowCount() - 1, getRowCount());
  }

  public void removeRow(int row) {
    data.remove(row);
    fireTableDataChanged();
  }

  @Override
  public String getColumnName(int i) {
    return columnNames[i];
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
  public boolean isCellEditable(int row, int column) {
    return column >= 1;
  }

  @Override
  public Object getValueAt(int row, int column) {
    return data.get(row)[column];
  }

  @Override
  public void setValueAt(Object o, int row, int column) {
    data.get(row)[column] = o.toString();
    fireTableCellUpdated(row, column);
  }

  public List<String> getColumnNames() {
    return Arrays.stream(columnNames)
        .map(columnName -> columnName.split(" ")[0])
        .collect(Collectors.toList());
  }

  public List<List<String>> getQualitySettings() {
    List<List<String>> qualitySettings = new ArrayList<>();

    for (String[] row : data) {
      qualitySettings.add(Arrays.asList(row).subList(1, getColumnCount()));
    }

    return qualitySettings;
  }
}
