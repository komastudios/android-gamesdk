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

package Utils.Validation;

import static com.intellij.openapi.ui.cellvalidators.ValidatingTableCellRendererWrapper.CELL_VALIDATION_PROPERTY;

import View.Decorator.RoundedCornerBorder;
import View.Decorator.RoundedCornerBorder.BorderType;
import com.intellij.openapi.ui.ComponentValidator;
import com.intellij.openapi.ui.ValidationInfo;
import java.awt.Component;
import javax.swing.AbstractCellEditor;
import javax.swing.JComponent;
import javax.swing.JTable;
import javax.swing.table.TableCellEditor;

public class CellCustomDataEditorValidatorWrapper extends AbstractCellEditor implements
    TableCellEditor {

  private final TableCellEditor tableCellEditor;

  public CellCustomDataEditorValidatorWrapper(TableCellEditor cellEditor) {
    this.tableCellEditor = cellEditor;
  }

  @Override
  public Object getCellEditorValue() {
    return tableCellEditor.getCellEditorValue();
  }

  @Override
  public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected,
      int row, int column) {
    JComponent editor = (JComponent) tableCellEditor
        .getTableCellEditorComponent(table, value, isSelected, row, column);
    JComponent renderer = (JComponent) table.getCellRenderer(row, column)
        .getTableCellRendererComponent(table, value, isSelected, true, row, column);
    ValidationInfo cellInfo =
        renderer != null ? (ValidationInfo) renderer.getClientProperty(CELL_VALIDATION_PROPERTY)
            : null;
    if (cellInfo != null) {
      editor.setBorder(new RoundedCornerBorder(BorderType.ERROR));
      editor.putClientProperty(CELL_VALIDATION_PROPERTY, cellInfo.forComponent(editor));
      ComponentValidator.getInstance(editor).ifPresent(ComponentValidator::revalidate);
    } else {
      editor.setBorder(new RoundedCornerBorder(BorderType.NORMAL));
    }
    return editor;
  }

}
