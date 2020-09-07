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

package View.Annotation;

import Model.EnumDataModel;
import java.awt.Component;
import java.util.List;
import javax.swing.AbstractCellEditor;
import javax.swing.JComboBox;
import javax.swing.JTable;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;

public class AnnotationDecorator {

  public static class EnumComboBoxDecorator extends AbstractCellEditor
      implements TableCellEditor, TableCellRenderer {

    final List<EnumDataModel> enums;
    final JComboBox<String> comboBox;

    public EnumComboBoxDecorator(List<EnumDataModel> enumsTemp) {
      this.enums = enumsTemp;
      this.comboBox = new JComboBox<>();
    }

    @Override
    public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected,
        int row, int column) {
      String strValue = value.toString();
      setComboBoxChoices(enums);
      comboBox.setSelectedItem(strValue);
      return comboBox;
    }

    private void setComboBoxChoices(List<EnumDataModel> choices) {
      comboBox.removeAllItems();
      for (EnumDataModel enumDataModel : choices) {
        comboBox.addItem(enumDataModel.getName());
      }
    }

    @Override
    public Object getCellEditorValue() {
      return comboBox.getSelectedIndex() == -1 ? "" : comboBox.getSelectedItem().toString();
    }

    public Component getTableCellRendererComponent(
        JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
      String strValue = value.toString();
      setComboBoxChoices(enums);
      if (enums.stream()
          .anyMatch(enumDataModel -> enumDataModel.getName().equals(strValue))) {
        comboBox.setSelectedItem(strValue);
      } else {
        comboBox.setSelectedIndex(-1);
      }
      return comboBox;
    }
  }
}
