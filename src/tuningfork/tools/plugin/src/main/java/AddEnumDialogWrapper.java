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

import com.intellij.openapi.project.Project;
import com.intellij.openapi.ui.DialogWrapper;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import java.util.ArrayList;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.table.DefaultTableModel;
import org.jdesktop.swingx.VerticalLayout;
import org.jetbrains.annotations.Nullable;

public class AddEnumDialogWrapper extends DialogWrapper {

  private AnnotationTabController controller;
  private EnumLayout enumLayout;

  protected AddEnumDialogWrapper(
      @Nullable Project project, AnnotationTabController controller) {
    super(project);
    init();
    setTitle("Add enum");
    this.controller = controller;
  }

  @Override
  protected void doOKAction() {
    controller.addAnnotationEnum(new EnumDataModel(enumLayout.getName(), enumLayout.getOptions()));
    super.doOKAction();
  }

  @Override
  protected @Nullable JComponent createCenterPanel() {
    enumLayout = new EnumLayout();
    return enumLayout;
  }

  private static class EnumLayout extends TabLayout {

    private JBScrollPane scrollPane;
    private JBTable optionsTable;
    private JPanel decoratorPanel;
    private JTextField nameTextField;

    private final JLabel nameLabel = new JBLabel("Name");

    EnumLayout() {
      initComponents();
      addComponents();
    }

    private void addComponents() {
      this.add(nameLabel);
      this.add(nameTextField);
      this.add(scrollPane);
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

      nameTextField = new JTextField();

      DefaultTableModel model = new DefaultTableModel();
      model.setColumnIdentifiers(new String[]{"Options"});
      optionsTable = new JBTable();
      optionsTable.setModel(model);
      initTextFieldColumns(optionsTable, 0);

      scrollPane = new JBScrollPane();
      decoratorPanel =
          ToolbarDecorator.createDecorator(optionsTable)
              .setAddAction(it -> model.addRow(new String[]{}))
              .setRemoveAction(it -> model.removeRow(optionsTable.getSelectedRow()))
              .createPanel();
      setDecoratorPanelSize(decoratorPanel);
      setTableSettings(scrollPane, decoratorPanel, optionsTable);
    }
  }
}
