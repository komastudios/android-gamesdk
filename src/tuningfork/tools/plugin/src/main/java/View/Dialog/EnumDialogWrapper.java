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
package View.Dialog;

import static Utils.Validation.UIValidator.ILLEGAL_TEXT_PATTERN;
import static View.Decorator.TableRenderer.getEditorTextBoxWithValidation;

import Controller.Enum.EnumController;
import Model.EnumDataModel;
import Utils.Validation.UIValidator;
import View.Decorator.TableRenderer;
import View.Decorator.TableRenderer.RoundedCornerRenderer;
import View.TabLayout;
import com.intellij.openapi.Disposable;
import com.intellij.openapi.ui.ComponentValidator;
import com.intellij.openapi.ui.DialogWrapper;
import com.intellij.openapi.ui.Messages;
import com.intellij.openapi.ui.ValidationInfo;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.table.JBTable;
import java.awt.Dimension;
import java.util.ArrayList;
import java.util.Collections;
import java.util.regex.Pattern;
import javax.swing.Box;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.table.DefaultTableModel;
import org.jdesktop.swingx.HorizontalLayout;
import org.jdesktop.swingx.VerticalLayout;
import org.jetbrains.annotations.Nullable;

public class EnumDialogWrapper extends DialogWrapper {


  private EnumLayout enumLayout;
  private final EnumController controller;
  private boolean isEdit;
  private int editingRow;
  private EnumDataModel enumDataModel;

  public EnumDialogWrapper(EnumController controller) {
    super(true);
    setTitle("Add Enum");
    this.controller = controller;
    init();
  }

  public EnumDialogWrapper(EnumController controller, int row, EnumDataModel enumDataModel) {
    super(true);
    setTitle("Edit Enum");
    this.controller = controller;
    this.isEdit = true;
    this.editingRow = row;
    this.enumDataModel = enumDataModel;
    init();
  }

  private void stopCellEditingMomentarily() {
    if (!enumLayout.optionsTable.isEditing()) {
      return;
    }
    enumLayout.optionsTable.getCellEditor().stopCellEditing();
  }


  @Override
  protected void doOKAction() {
    stopCellEditingMomentarily();
    if (!enumLayout.isViewValid()) {
      Messages.showErrorDialog("Please Fix the errors first", "Unable To Close");
      return;
    }
    if (isEdit) {
      if (controller.editEnum(editingRow, enumLayout.getName(), enumLayout.getOptions())) {
        super.doOKAction();
      } else {
        Messages.showInfoMessage(
            "Conflicting enums names. Please change the enum name", "Unable to Edit Enum!");
      }
    } else {
      if (controller.addEnum(enumLayout.getName(), enumLayout.getOptions())) {
        super.doOKAction();
      } else {
        Messages.showInfoMessage(
            "Enum name already exists. Please change the enum name", "Unable to Add Enum!");
      }
    }
  }

  @Override
  protected @Nullable
  JComponent createCenterPanel() {
    enumLayout = new EnumLayout(getDisposable());
    if (isEdit) {
      enumLayout.setData(enumDataModel);
    }
    return enumLayout;
  }

  private static final class EnumLayout extends TabLayout {

    private JTable optionsTable;
    private JPanel decoratorPanel;
    private JTextField nameTextField;
    private DefaultTableModel model;
    private final JLabel nameLabel = new JBLabel("Name");
    private final Disposable disposable;

    EnumLayout(Disposable disposable) {
      this.disposable = disposable;
      initComponents();
      addComponents();
      initValidators();
    }

    private void addComponents() {
      JPanel namePanel = new JPanel(new HorizontalLayout());
      namePanel.add(nameLabel);
      namePanel.add(nameTextField);
      this.add(namePanel);
      this.add(Box.createVerticalStrut(30));
      this.add(decoratorPanel);
    }

    public void setData(EnumDataModel enumDataModel) {
      nameTextField.setText(enumDataModel.getName());
      enumDataModel.getOptions().forEach(optionName -> model.addRow(new String[]{optionName}));
    }

    public String getName() {
      return nameTextField.getText();
    }

    public ArrayList<String> getOptions() {
      ArrayList<String> options = new ArrayList<>();
      int rows = optionsTable.getModel().getRowCount();

      for (int i = 0; i < rows; i++) {
        options.add((String) optionsTable.getModel().getValueAt(i, 0));
      }

      return options;
    }

    private void initComponents() {
      this.setLayout(new VerticalLayout());
      nameTextField = new JTextField("", 30);
      model =
          new DefaultTableModel() {
            @Override
            public void removeRow(int row) {
              dataVector.remove(row);
              fireTableDataChanged();
            }
          };
      optionsTable = new JBTable();
      decoratorPanel =
          ToolbarDecorator.createDecorator(optionsTable)
              .setAddAction(it -> model.addRow(new String[]{""}))
              .setRemoveAction(
                  it -> {
                    int currentRow = optionsTable.getSelectedRow();
                    if (optionsTable.isEditing()) {
                      optionsTable.getCellEditor().stopCellEditing();
                    }
                    model.removeRow(currentRow);
                  })
              .createPanel();
      setDecoratorPanelSize(decoratorPanel);
      optionsTable.setFillsViewportHeight(true);
      optionsTable.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
      optionsTable.getTableHeader().setReorderingAllowed(false);
      optionsTable.setRowSelectionAllowed(true);
      optionsTable.setSelectionForeground(null);
      optionsTable.setSelectionBackground(null);
      optionsTable.setIntercellSpacing(new Dimension(0, 0));
      model.setColumnIdentifiers(new String[]{"Options"});
      optionsTable.setModel(model);
    }

    private void initValidators() {
      new ComponentValidator(disposable).withValidator(() -> {
        String name = nameTextField.getText();
        if (name.isEmpty()) {
          return new ValidationInfo("Field Can Not Be Empty", nameTextField);
        }
        if (Pattern.compile(ILLEGAL_TEXT_PATTERN).matcher(name).find()) {
          return new ValidationInfo(name + " contains illegal characters", nameTextField);
        }
        return null;
      }).andRegisterOnDocumentListener(nameTextField).installOn(nameTextField);

      UIValidator.createTableValidator(disposable, optionsTable,
          () -> {
            if (optionsTable.getRowCount() == 0) {
              return new ValidationInfo("Options Table Can Not Be Empty", optionsTable);
            }
            ArrayList<String> options = getOptions();
            boolean isOptionDuplicate =
                options.stream().anyMatch(option -> Collections.frequency(options, option) > 1);
            if (isOptionDuplicate) {
              return new ValidationInfo("Repeated Fields Are Not Allowed", optionsTable);
            }
            return null;
          });
      model.addTableModelListener(
          e -> ComponentValidator.getInstance(optionsTable)
              .ifPresent(ComponentValidator::revalidate));
      optionsTable.getColumnModel().getColumn(0)
          .setCellEditor(getEditorTextBoxWithValidation(disposable));

      //noinspection UnstableApiUsage
      optionsTable.getColumnModel().getColumn(0)
          .setCellRenderer(
              TableRenderer.getRendererTextBoxWithValidation(new RoundedCornerRenderer(),
                  (value, row, column) -> {
                    String strVal = value.toString();
                    if (strVal.isEmpty()) {
                      return new ValidationInfo("Field Can Not Be Empty!");
                    }
                    if (Pattern.compile(ILLEGAL_TEXT_PATTERN).matcher(strVal).find()) {
                      return new ValidationInfo(strVal + " contains illegal characters");
                    }
                    return null;
                  }));

      TableRenderer.addCellToolTipManager(optionsTable, disposable);
    }

    public boolean isViewValid() {
      return isTableCellsValidate() & isNameTextFieldValid() & isTableValid();
    }

    private boolean isTableCellsValidate() {
      return UIValidator.isTableCellsValid(optionsTable);
    }

    private boolean isNameTextFieldValid() {
      return UIValidator.isComponentValid(nameTextField);
    }

    private boolean isTableValid() {
      return UIValidator.isComponentValid(optionsTable);
    }
  }
}