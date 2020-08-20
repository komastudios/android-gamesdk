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

package View.Fidelity;

import com.intellij.openapi.ui.ComboBox;
import java.awt.Component;
import java.util.List;
import javax.swing.AbstractCellEditor;
import javax.swing.JComboBox;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;

public class FidelityTableDecorators {

  public static final class ComboBoxEditor extends AbstractCellEditor implements TableCellEditor {

    JComboBox<FieldType> comboBox;

    public ComboBoxEditor(FieldType[] fieldTypes) {
      comboBox = new ComboBox<>();
      for (FieldType fieldType : fieldTypes) {
        comboBox.addItem(fieldType);
      }
      // Used to update the UI.
      comboBox.addItemListener(itemEvent -> fireEditingStopped());
    }

    @Override
    public Component getTableCellEditorComponent(
        JTable table, Object value, boolean isSelected, int row, int column) {
      FidelityTableData feed = (FidelityTableData) value;
      comboBox.setSelectedItem(feed.getFieldType());
      return comboBox;
    }

    @Override
    public Object getCellEditorValue() {
      return comboBox.getSelectedItem();
    }
  }

  public static final class ComboBoxRenderer extends JComboBox<String>
      implements TableCellRenderer {

    public ComboBoxRenderer() {

    }

    @Override
    public Component getTableCellRendererComponent(
        JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
      removeAllItems();
      this.addItem(((FidelityTableData) value).getFieldType().getName());
      return this;
    }
  }

  public static class JPanelDecorator extends AbstractCellEditor
      implements TableCellEditor, TableCellRenderer {

    FidelityTablePanel fidelityTablePanel;
    List<String> enums;

    public JPanelDecorator(List<String> enumsTemp) {
      this.enums = enumsTemp;
      fidelityTablePanel = new FidelityTablePanel();
    }

    @Override
    public Component getTableCellEditorComponent(
        JTable table, Object value, boolean isSelected, int row, int column) {
      FidelityTableData fidelityTableData = (FidelityTableData) value;
      fidelityTablePanel.updateData(fidelityTableData, isSelected, table);
      fidelityTablePanel.setComboBoxChoices(enums);
      fidelityTablePanel.getEnumTypes().setSelectedItem(fidelityTableData.getFieldEnumName());
      return fidelityTablePanel;
    }

    @Override
    public Object getCellEditorValue() {
      return fidelityTablePanel.getFidelityData();
    }

    public Component getTableCellRendererComponent(
        JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
      FidelityTableData feed = (FidelityTableData) value;
      fidelityTablePanel.updateData(feed, isSelected, table);
      fidelityTablePanel.getEnumTypes().addItem(feed.getFieldEnumName());
      return fidelityTablePanel;
    }
  }

  public static final class TextBoxRenderer implements TableCellRenderer {

    JTextField textField;

    public TextBoxRenderer() {
      textField = new JTextField();
    }

    public Component getTableCellRendererComponent(
        JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
      FidelityTableData feed = (FidelityTableData) value;
      textField.setText(feed.getFieldParamName());
      return textField;
    }
  }

  public static final class TextBoxEditor extends AbstractCellEditor implements TableCellEditor {

    JTextField textField;

    public TextBoxEditor() {
      textField = new JTextField();
    }

    @Override
    public Component getTableCellEditorComponent(
        JTable table, Object value, boolean isSelected, int row, int column) {
      FidelityTableData feed = (FidelityTableData) value;
      textField.setText(feed.getFieldParamName());
      return textField;
    }

    @Override
    public Object getCellEditorValue() {
      return textField.getText();
    }
  }
}
