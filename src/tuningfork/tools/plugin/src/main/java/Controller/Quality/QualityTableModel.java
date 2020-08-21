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

import java.awt.Component;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import javax.swing.DefaultCellEditor;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JComboBox;
import javax.swing.JTable;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableColumn;

public class QualityTableModel extends AbstractTableModel {

  private final String[] columnNames = {
      "Quality level number", "Field1" + " (int32)", "Field2" + " (float)", "Field3" + " (ENUM)"
  };
  private List<String[]> data;
  private HashSet<Integer> enumIndexes;

  public QualityTableModel() {
    super();
    data = new ArrayList<>();
    enumIndexes = new HashSet<>();
    enumIndexes.add(3);
  }

  public void setEnums(JTable table) {
    for (Integer enumIndexEntry : enumIndexes) {
      TableColumn enumCol = table.getColumnModel().getColumn(enumIndexEntry);
      // TODO(targintaru) - probably elsewhere - retrieve data from fidelity settings enum....
      JComboBox<String> enumValues = new JComboBox();
      DefaultComboBoxModel comboBoxModel = new DefaultComboBoxModel();
      comboBoxModel.addElement("Low");
      comboBoxModel.addElement("Medium");
      comboBoxModel.addElement("High");
      enumValues.setModel(comboBoxModel);
      enumCol.setCellEditor(new DefaultCellEditor(enumValues));
      ComboBoxTableCellRenderer renderer = new ComboBoxTableCellRenderer();
      renderer.setModel(comboBoxModel);
      enumCol.setCellRenderer(renderer);
    }
  }

  static class ComboBoxTableCellRenderer extends JComboBox implements TableCellRenderer {

    @Override
    public Component getTableCellRendererComponent(
        JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
      setSelectedItem(value);
      return this;
    }
  }

  public void addRow(String[] row, JTable table) {
    data.add(row);
    fireTableRowsInserted(getRowCount() - 1, getRowCount());
    setEnums(table);
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
}
