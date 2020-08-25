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

import Controller.Enum.EnumController;
import Model.EnumDataModel;
import View.TabLayout;
import com.intellij.openapi.ui.DialogWrapper;
import com.intellij.openapi.ui.Messages;
import com.intellij.openapi.ui.ValidationInfo;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import javax.swing.Box;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.table.DefaultTableModel;
import org.jdesktop.swingx.HorizontalLayout;
import org.jdesktop.swingx.VerticalLayout;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

public class EnumDialogWrapper extends DialogWrapper {

  private static final String ENUM_PATTERN = "[a-zA-Z_]+$";
  private EnumLayout enumLayout;
  private EnumController controller;
  private boolean isEdit;
  private int editingRow;
  private EnumDataModel enumDataModel;

  public EnumDialogWrapper(EnumController controller) {
    super(true);
    setTitle("Add Enum");
    this.controller = controller;
    setValidationDelay(100);
    init();
  }

  public EnumDialogWrapper(EnumController controller, int row, EnumDataModel enumDataModel) {
    super(true);
    setTitle("Edit Enum");
    this.controller = controller;
    this.isEdit = true;
    this.editingRow = row;
    this.enumDataModel = enumDataModel;
    setValidationDelay(100);
    init();
  }

  private void stopCellEditingMomentarily() {
    if (!enumLayout.optionsTable.isEditing()) {
      return;
    }
    int row = enumLayout.optionsTable.getSelectedRow();
    enumLayout.optionsTable.getCellEditor().stopCellEditing();
    enumLayout.optionsTable.editCellAt(row, 0);
  }

  @NotNull
  @Override
  protected List<ValidationInfo> doValidateAll() {
    List<ValidationInfo> validationInfo = new ArrayList<>();
    if (enumLayout.getName().isEmpty()) {
      validationInfo.add(new ValidationInfo("Name can not be empty", enumLayout.nameTextField));
    }
    validateOptions(validationInfo);
    validatePattern(validationInfo);
    if (validationInfo.isEmpty()) {
      return validationInfo;
    }
    // Show at most 3 errors at once.
    return validationInfo.subList(0, Math.min(validationInfo.size(), 3));
  }

  private void validatePattern(List<ValidationInfo> validationInfo) {
    if (!enumLayout.getName().matches(ENUM_PATTERN)) {
      validationInfo.add(
          new ValidationInfo("Name Must Match Pattern [a-zA-Z_]", enumLayout.nameTextField));
    }
    enumLayout.getOptions().stream()
        .filter(option -> !option.matches(ENUM_PATTERN) && !option.isEmpty())
        .forEach(s ->
            validationInfo.add(
                new ValidationInfo(s + " Does Not Match The Pattern [a-zA-Z_]")));
  }

  private void validateOptions(List<ValidationInfo> validationInfo) {
    List<String> options = enumLayout.getOptions();
    if (options.isEmpty()) {
      validationInfo.add(new ValidationInfo("Options Table Can Not Be Empty"));
      return;
    }

    boolean isOptionTextEmpty = options.stream().anyMatch(String::isEmpty);
    if (isOptionTextEmpty) {
      validationInfo.add(new ValidationInfo("Empty Fields Are Not Allowed"));
    }
    boolean isOptionDuplicate =
        options.stream().anyMatch(option -> Collections.frequency(options, option) > 1);
    if (isOptionDuplicate) {
      validationInfo.add(new ValidationInfo("Repeated Fields Are Not Allowed"));
    }
  }

  @Override
  protected void doOKAction() {
    stopCellEditingMomentarily();
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
    enumLayout = new EnumLayout();
    if (isEdit) {
      enumLayout.setData(enumDataModel);
    }
    return enumLayout;
  }

  private static final class EnumLayout extends TabLayout {

    private JBScrollPane scrollPane;
    private JTable optionsTable;
    private JPanel decoratorPanel;
    private JTextField nameTextField;
    private DefaultTableModel model;
    private final JLabel nameLabel = new JBLabel("Name");

    EnumLayout() {
      initComponents();
      addComponents();
    }

    private void addComponents() {
      JPanel namePanel = new JPanel(new HorizontalLayout());
      namePanel.add(nameLabel);
      namePanel.add(nameTextField);
      this.add(namePanel);
      this.add(Box.createVerticalStrut(30));
      this.add(scrollPane);
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
      scrollPane = new JBScrollPane();
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
      setTableSettings(scrollPane, decoratorPanel, optionsTable);
      model.setColumnIdentifiers(new String[]{"Options"});
      optionsTable.setModel(model);
      initTextFieldColumns(optionsTable, 0);
    }
  }
}
