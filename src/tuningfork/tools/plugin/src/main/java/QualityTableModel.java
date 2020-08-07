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

import java.util.HashSet;
import javax.swing.*;
import javax.swing.table.*;
import java.awt.*;
import java.util.Vector;

public class QualityTableModel extends DefaultTableModel {

  private static QualityTableData qualityTableData;
  private static int columnCount;
  private static HashSet<Integer> enumIndexes;

  QualityTableModel() {
    super();
    qualityTableData = new QualityTableData();
    columnCount = qualityTableData.getColNames().size();
    enumIndexes = qualityTableData.getEnums();
    updateData();
  }

  private void setEnums(JTable table) {
    for (Integer enumIndexEntry : enumIndexes) {
      System.out.println(enumIndexEntry);
      TableColumn enumCol = table.getColumnModel().getColumn(enumIndexEntry);
      // TODO - probably elsewhere - retrieve data from fidelity settings enum....
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
    public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected,
        boolean hasFocus, int row, int column) {
      setSelectedItem(value);
      return this;
    }
  }

  public void addRow(JTable table) {
    // probably this method will only update the data, and the QualityTableData class will
    // retrieve the data
    Vector<Object> data = new Vector<>();
    for (int i = 0; i < columnCount - 2; i++) {
      data.add("");
    }
    qualityTableData.addData(data);
    updateData();
    setEnums(table);
  }

  private void updateData() {
    this.setDataVector(qualityTableData.getData(), qualityTableData.getColNames());
  }

  public void removeRow(int index) {
    qualityTableData.removeData(index);
    updateData();
  }

  @Override
  public boolean isCellEditable(int row, int column) {
    return column >= 1;
  }
}
