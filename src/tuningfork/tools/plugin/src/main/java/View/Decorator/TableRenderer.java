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

package View.Decorator;

import static com.intellij.openapi.ui.cellvalidators.ValidatingTableCellRendererWrapper.CELL_VALIDATION_PROPERTY;

import Utils.Validation.ValidatingComboBoxTableCellWrapper;
import View.Decorator.RoundedCornerBorder.BorderType;
import com.intellij.openapi.Disposable;
import com.intellij.openapi.ui.ValidationInfo;
import com.intellij.openapi.ui.cellvalidators.CellComponentProvider;
import com.intellij.openapi.ui.cellvalidators.CellTooltipManager;
import com.intellij.openapi.ui.cellvalidators.StatefulValidatingCellEditor;
import com.intellij.openapi.ui.cellvalidators.TableCellValidator;
import com.intellij.openapi.ui.cellvalidators.ValidatingTableCellRendererWrapper;
import com.intellij.ui.components.fields.ExtendableTextField;
import java.awt.Component;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JTable;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;

public class TableRenderer {

  public static final class RoundedCornerRenderer extends DefaultTableCellRenderer {

    @Override
    public Component getTableCellRendererComponent(JTable jTable, Object value,
        boolean isSelected, boolean hasFocus, int row, int column) {
      JComponent component = (JComponent) super.getTableCellRendererComponent(
          jTable, value, isSelected, hasFocus, row, column);
      component.setBorder(new RoundedCornerBorder());
      setForeground(isSelected ? jTable.getSelectionForeground() : jTable.getForeground());
      setBackground(isSelected ? jTable.getSelectionBackground() : jTable.getBackground());
      return component;
    }
  }

  public static final class ComboBoxRenderer
      extends JComboBox<String> implements TableCellRenderer {

    @Override
    public Component getTableCellRendererComponent(
        JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
      removeAllItems();
      addItem(value.toString());
      return this;
    }
  }

  // Unstable API is used for table validation, Use with Cautious.
  @SuppressWarnings({"UnstableApiUsage"})
  public static void addCellToolTipManager(JTable table, Disposable disposable) {
    new CellTooltipManager(disposable).
        withCellComponentProvider(CellComponentProvider.forTable(table)).
        installOn(table);
  }

  public static TableCellEditor getEditorTextBoxWithValidation(Disposable disposable) {
    ExtendableTextField cellEditor = new ExtendableTextField();
    StatefulValidatingCellEditor validatingCellEditor = new StatefulValidatingCellEditor(cellEditor,
        disposable) {
      @Override
      public Component getTableCellEditorComponent(JTable table, Object value,
          boolean isSelected, int row, int column) {
        JComponent component = (JComponent) super
            .getTableCellEditorComponent(table, value, isSelected, row, column);
        ValidationInfo cellInfo = (ValidationInfo) component
            .getClientProperty(CELL_VALIDATION_PROPERTY);
        if (cellInfo == null) {
          component.setBorder(new RoundedCornerBorder(BorderType.NORMAL));
        } else {
          component.setBorder(new RoundedCornerBorder(BorderType.ERROR));
        }
        return component;
      }
    };
    validatingCellEditor.setClickCountToStart(1);
    return validatingCellEditor;
  }

  // Unstable API is used for validation. Use with cautious.
  @SuppressWarnings("UnstableApiUsage")
  public static TableCellRenderer getRendererTextBoxWithValidation(
      TableCellRenderer tableCellRenderer,
      TableCellValidator tableCellValidator) {
    return new ValidatingTableCellRendererWrapper(tableCellRenderer)
        .withCellValidator(tableCellValidator);
  }

  public static TableCellRenderer getRendererComboBoxWithValidation(
      TableCellRenderer tableCellRenderer,
      TableCellValidator tableCellValidator) {
    return new ValidatingComboBoxTableCellWrapper(tableCellRenderer)
        .withCellValidator(tableCellValidator);
  }
}
