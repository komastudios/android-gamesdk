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

package View;

import Controller.EnumController;
import View.Dialog.EnumDialogWrapper;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import java.awt.Dimension;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListSelectionModel;
import javax.swing.table.DefaultTableModel;
import org.jdesktop.swingx.HorizontalLayout;

public class EnumTable extends JPanel {

  private final JBTable enumTable;
  private final JPanel enumDecoratePanel;
  private final EnumController controller;
  private JScrollPane jScrollPane;
  private final static int TABLE_WIDTH = 300;
  private final static int TABLE_HEIGHT = 200;

  public EnumTable(EnumController controller) {
    this.setLayout(new HorizontalLayout());
    this.enumTable = new JBTable(new DefaultTableModel(new Object[][]{}, new Object[]{"Enums"}) {
      @Override
      public void removeRow(int row) {
        dataVector.remove(row);
        fireTableDataChanged();
      }
    });
    this.controller = controller;
    enumDecoratePanel = ToolbarDecorator.createDecorator(enumTable)
        .setAddAction(it -> addEnum())
        .setRemoveAction(it -> removeEnum())
        .setEditAction(it -> editEnum())
        .createPanel();
    initComponent();
    enumDecoratePanel.setPreferredSize(new Dimension(TABLE_WIDTH, TABLE_HEIGHT));
    enumDecoratePanel.setMinimumSize(new Dimension(TABLE_WIDTH, TABLE_HEIGHT));
    this.add(jScrollPane);
  }

  private void initComponent() {
    jScrollPane = new JBScrollPane();
    jScrollPane.setViewportView(enumDecoratePanel);
    enumTable.setFillsViewportHeight(true);
    enumTable.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    enumTable.getTableHeader().setReorderingAllowed(false);
    enumTable.setRowSelectionAllowed(true);
    enumTable.setIntercellSpacing(new Dimension(0, 0));
  }

  private void addEnum() {
    EnumDialogWrapper enumDialogWrapper = new EnumDialogWrapper(controller);
    if (enumDialogWrapper.showAndGet()) {
      controller.addEnumToTable(enumTable);
    }
  }

  private void removeEnum() {
    int row = enumTable.getSelectedRow();
    controller.removeEnum(row);
    controller.removeEnumFromTable(enumTable, row);
  }

  private void editEnum() {
    int selectedRow = enumTable.getSelectedRow();
    EnumDialogWrapper enumDialogWrapper = new EnumDialogWrapper(
        controller, selectedRow, controller.getEnums().get(selectedRow));
    if (enumDialogWrapper.showAndGet()) {
      controller.editEnumInTable(enumTable, selectedRow);
    }
  }
}
