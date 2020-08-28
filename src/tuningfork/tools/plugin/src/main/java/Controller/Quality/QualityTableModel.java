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
import java.util.List;
import javax.swing.table.AbstractTableModel;

public class QualityTableModel extends AbstractTableModel {

  private final List<String> columnNames;
  private final List<List<String>> data;
  private final QualityTabController qualityTabController;

  public QualityTableModel(QualityTabController qualityTabController) {
    data = new ArrayList<>();
    columnNames = new ArrayList<>(Arrays.asList("Parameter name", "Trend"));
    this.qualityTabController = qualityTabController;
  }

  public void addRow() {
    List<String> row = new ArrayList<>();
    row.add("");
    row.add("increase");
    for (int i = 2; i < getColumnCount(); i++) {
      row.add(qualityTabController.getDefaultValueByIndex(getRowCount()));
    }
    qualityTabController.addNewFieldToAllFiles(getRowCount());
    data.add(row);
    fireTableRowsInserted(getRowCount() - 1, getRowCount());
  }

  public void addColumn() {
    columnNames.add(String.valueOf(getColumnCount() - 1));
    qualityTabController.addNewQualityFile();
    for (int i = 0; i < getRowCount(); i++) {
      data.get(i).add(qualityTabController.getDefaultValueByIndex(i));
      setValueAt(qualityTabController.getNewTrendState(i), i, 1);
    }

    fireTableStructureChanged();
  }

  public void setRowValue(int row, String value) {
    for (int i = 2; i < getColumnCount(); i++) {
      setValueAt(value, row, i);
    }
  }

  public void removeColumn(int column) {
    columnNames.remove(column);
    qualityTabController.removeQualityFile(column - 2);
    for (int i = 0; i < getRowCount(); i++) {
      data.get(i).remove(column);
      if (getColumnCount() > 2) {
        setValueAt(qualityTabController.getNewTrendState(i), i, 1);
      } else {
        setValueAt("increase", i, 1);
      }
    }
    for (int i = 2; i < getColumnCount(); i++) {
      columnNames.set(i, String.valueOf(i - 1));
    }
    fireTableStructureChanged();
  }

  public void removeRow(int row) {
    data.remove(row);
    fireTableDataChanged();
  }

  @Override
  public String getColumnName(int i) {
    return columnNames.get(i);
  }

  @Override
  public int getRowCount() {
    return data.size();
  }

  @Override
  public int getColumnCount() {
    return columnNames.size();
  }

  @Override
  public boolean isCellEditable(int row, int column) {
    return column >= 2;
  }

  @Override
  public Object getValueAt(int row, int column) {
    return data.get(row).get(column);
  }

  @Override
  public void setValueAt(Object object, int row, int column) {
    data.get(row).set(column, object.toString());
    if (column == 0) {
      qualityTabController.updateFieldName(row, object.toString());
    } else if (column > 1) {
      qualityTabController.updateFieldValue(column - 2, row, object.toString());
      setValueAt(qualityTabController.getNewTrendState(row), row, 1);
    }
    fireTableCellUpdated(row, column);
  }

  public List<String> getColumnNames() {
    return columnNames;
  }

}
