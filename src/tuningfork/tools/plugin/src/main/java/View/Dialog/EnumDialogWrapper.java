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

import Controller.EnumController;
import Model.EnumDataModel;
import View.TabLayout;
import com.intellij.openapi.ui.DialogWrapper;
import com.intellij.openapi.ui.ValidationInfo;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import java.util.ArrayList;
import javax.swing.Box;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.table.DefaultTableModel;
import org.jdesktop.swingx.HorizontalLayout;
import org.jdesktop.swingx.VerticalLayout;
import org.jetbrains.annotations.Nullable;

public class EnumDialogWrapper extends DialogWrapper {

  private EnumLayout enumLayout;
  private EnumController controller;
  private boolean isEdit;
  private int editingRow;
  private EnumDataModel enumDataModel;

  public EnumDialogWrapper(EnumController controller) {
    super(true);
    setTitle("Add Enum");
    this.controller = controller;
    this.enumLayout = new EnumLayout();
    init();
  }

  public EnumDialogWrapper(EnumController controller, int row,
      EnumDataModel enumDataModel) {
    super(true);
    this.controller = controller;
    this.isEdit = true;
    this.editingRow = row;
    this.enumDataModel = enumDataModel;
    setTitle("Edit Enum");
    init();
  }


  @Nullable
  @Override
  protected ValidationInfo doValidate() {
    if (enumLayout.optionsTable.isEditing()) {
      enumLayout.optionsTable.getCellEditor().stopCellEditing();
    }
    if (enumLayout.getName().isEmpty()) {
      return new ValidationInfo("Name can not be empty", enumLayout.nameTextField);
    }
    if (enumLayout.getOptions().isEmpty()) {
      return new ValidationInfo("Options can not be empty", enumLayout.scrollPane);
    }
    return super.doValidate();
  }

  @Override
  protected void doOKAction() {
    if (isEdit) {
      controller.editEnum(editingRow, enumLayout.getName(), enumLayout.getOptions());
    } else {
      controller.addEnum(enumLayout.getName(), enumLayout.getOptions());
    }
    super.doOKAction();
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
      this.add(Box.createVerticalStrut(10));
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
      model = new DefaultTableModel() {
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
              .setAddAction(it -> model.addRow(new String[]{}))
              .setRemoveAction(it -> {
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