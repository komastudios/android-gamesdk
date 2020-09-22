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

import com.intellij.openapi.Disposable;
import com.intellij.openapi.ui.ValidationInfo;
import com.intellij.openapi.util.Disposer;
import com.intellij.ui.DocumentAdapter;
import com.intellij.ui.JBColor;
import java.awt.Component;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.util.function.Supplier;
import javax.swing.BorderFactory;
import javax.swing.JComponent;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.event.DocumentEvent;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.TableModel;
import org.jetbrains.annotations.NotNull;

public class UIValidator {

  public static final String ILLEGAL_TEXT_PATTERN = "[{|}]";
  public static final String VALIDATION_METHOD = "ComponentValidation.Method";

  public static void createTableValidator(Disposable disposable, JTable table,
      Supplier<ValidationInfo> validationMethod) {
    TableModelListener tableModelListener = new ComponentTableValidationUpdater(table,
        validationMethod);
    table.getModel().addTableModelListener(tableModelListener);
    FocusListener focusListener = new ComponentFocusValidationUpdater(table, validationMethod);
    table.addFocusListener(focusListener);
    Disposer.register(disposable, () -> {
      table.removeFocusListener(focusListener);
      table.getModel().removeTableModelListener(tableModelListener);
    });
    table.putClientProperty(VALIDATION_METHOD, validationMethod);
  }

  public static void createTextValidator(Disposable disposable, JTextField jTextField,
      Supplier<ValidationInfo> validationMethod) {
    DocumentAdapter documentAdapter = new ComponentTextValidationUpdater(jTextField,
        validationMethod);
    jTextField.getDocument().addDocumentListener(documentAdapter);
    Disposer.register(disposable,
        () -> jTextField.getDocument().removeDocumentListener(documentAdapter));
    jTextField.putClientProperty(VALIDATION_METHOD, validationMethod);
  }

  private static ValidationInfo hasValidationInfo(JComponent component) {
    return component != null ? (ValidationInfo) component
        .getClientProperty(CELL_VALIDATION_PROPERTY) : null;
  }

  private static Supplier<ValidationInfo> getValidationMethod(JComponent component) {
    return component != null ? (Supplier<ValidationInfo>) component
        .getClientProperty(VALIDATION_METHOD) : null;
  }

  public static boolean isComponentValid(JComponent jComponent) {
    if (getValidationMethod(jComponent) != null) {
      updateValidation(getValidationMethod(jComponent), jComponent);
    }
    ValidationInfo cellInfo = UIValidator.hasValidationInfo(jComponent);
    return cellInfo == null || cellInfo.warning;
  }

  public static boolean isTableCellsValid(JTable jTable) {
    TableModel tableModel = jTable.getModel();
    for (int i = 0; i < jTable.getRowCount(); i++) {
      for (int j = 0; j < jTable.getColumnCount(); j++) {
        JComponent component = (JComponent) jTable
            .getCellRenderer(i, j).getTableCellRendererComponent(jTable,
                tableModel.getValueAt(i, j), false, false, i, 0);
        if (!isComponentValid(component)) {
          return false;
        }
      }
    }
    return true;
  }

  public static boolean isTableCellsValidDeep(JTable jTable) {
    TableModel tableModel = jTable.getModel();
    for (int i = 0; i < jTable.getRowCount(); i++) {
      for (int j = 0; j < jTable.getColumnCount(); j++) {
        JComponent component = (JComponent) jTable
            .getCellRenderer(i, j).getTableCellRendererComponent(jTable,
                tableModel.getValueAt(i, j), false, false, i, 0);
        if (!isComponentValid(component)) {
          return false;
        }
        for (Component childComp : component.getComponents()) {
          if (childComp instanceof JComponent && !isComponentValid((JComponent) childComp)) {
            return false;
          }
        }
      }
    }
    return true;
  }

  private static void updateValidation(
      Supplier<ValidationInfo> validationMethod,
      JComponent jComponent) {
    ValidationInfo cellInfo = validationMethod.get();
    jComponent.putClientProperty(CELL_VALIDATION_PROPERTY, cellInfo);
    if (cellInfo != null) {
      jComponent.setToolTipText(cellInfo.message);
      if (cellInfo.warning) {
        jComponent.setBorder(BorderFactory.createLineBorder(JBColor.YELLOW));
      } else {
        jComponent.setBorder(BorderFactory.createLineBorder(JBColor.RED));
      }
      validationMethod.get();
      return;
    }
    jComponent.setToolTipText("");
    jComponent.setBorder(null);
  }

  private final static class ComponentFocusValidationUpdater implements FocusListener {

    private final JComponent jComponent;
    private final Supplier<ValidationInfo> validationMethod;

    public ComponentFocusValidationUpdater(JComponent jComponent,
        Supplier<ValidationInfo> supplier) {
      this.jComponent = jComponent;
      this.validationMethod = supplier;
    }

    @Override
    public void focusGained(FocusEvent e) {
      updateValidation(validationMethod, jComponent);
    }

    @Override
    public void focusLost(FocusEvent e) {
      updateValidation(validationMethod, jComponent);
    }
  }

  private final static class ComponentTextValidationUpdater extends DocumentAdapter {

    private final JComponent jComponent;
    private final Supplier<ValidationInfo> validationMethod;
    private boolean textChanged = false;

    public ComponentTextValidationUpdater(JComponent jComponent,
        Supplier<ValidationInfo> supplier) {
      this.jComponent = jComponent;
      this.validationMethod = supplier;
    }

    @Override
    protected void textChanged(@NotNull DocumentEvent documentEvent) {
      updateValidation(validationMethod, jComponent);
    }
  }

  private final static class ComponentTableValidationUpdater implements TableModelListener {

    private final JComponent jComponent;
    private final Supplier<ValidationInfo> validationMethod;

    public ComponentTableValidationUpdater(JComponent jComponent,
        Supplier<ValidationInfo> supplier) {
      this.jComponent = jComponent;
      this.validationMethod = supplier;
    }

    @Override
    public void tableChanged(TableModelEvent e) {
      updateValidation(validationMethod, jComponent);
    }
  }
}
