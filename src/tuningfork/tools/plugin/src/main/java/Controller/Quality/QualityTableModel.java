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

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;
import java.util.stream.IntStream;
import javax.swing.JTable;
import javax.swing.table.AbstractTableModel;

public class QualityTableModel extends AbstractTableModel {

  private String[] columnNames = {"Quality level number",
      "Field1 (int32)",
      "Field2 (float)",
      "Field3 (ENUM)"};
  private List<String[]> data;
  private Set<Integer> enumIndexes;

  public QualityTableModel() {
    data = new ArrayList<>();
    enumIndexes = new HashSet<>();
    addEnumIndexes();
  }

  public QualityTableModel(String[] columnNames) {
    data = new ArrayList<>();
    enumIndexes = new HashSet<>();
    this.columnNames = columnNames;
    addEnumIndexes();
  }

  private void addEnumIndexes() {
    ArrayList<String> fidelitySettingsTypes = (ArrayList<String>) Arrays.stream(columnNames)
        .map(columnName -> columnName.split(" ")[1])
        .collect(Collectors.toList());
    enumIndexes = IntStream.range(0, fidelitySettingsTypes.size())
        .filter(i -> fidelitySettingsTypes.get(i).equals("(ENUM)")).boxed()
        .collect(Collectors.toSet());
  }

  public HashSet<Integer> getEnumIndexes() {
    return (HashSet<Integer>) enumIndexes;
  }

  public void addRow(String[] row, JTable table) {
    data.add(row);
    QualityTabController.increaseFilesCount();
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
      qualitySettings.add(Arrays.asList(row));
    }

    return qualitySettings;
  }
}
