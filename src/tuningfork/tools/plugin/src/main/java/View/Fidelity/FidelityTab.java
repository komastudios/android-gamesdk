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

import Controller.Fidelity.FidelityTabController;
import Controller.Fidelity.FidelityTableModel;
import View.EnumTable;
import View.Fidelity.FidelityTableDecorators.ComboBoxEditor;
import View.Fidelity.FidelityTableDecorators.ComboBoxRenderer;
import View.TabLayout;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import javax.swing.Box;
import javax.swing.JPanel;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableColumn;
import org.jdesktop.swingx.VerticalLayout;

public class FidelityTab extends TabLayout implements PropertyChangeListener {

  private final JBLabel fidelityLabel = new JBLabel("Fidelity Settings");
  private final JBLabel informationLabel = new JBLabel("Fidelity parameters settings info.");
  FidelityTabController fidelityTabController;
  private JBScrollPane scrollPane;
  private JBTable fidelityTable;
  private JPanel fidelityDecoratorPanel;

  public FidelityTab(FidelityTabController fidelityTabController) {
    this.fidelityTabController = fidelityTabController;
    initVariables();
    initComponents();
    fidelityTabController.addPropertyChangeListener(this);
  }

  private void initVariables() {
    scrollPane = new JBScrollPane();
    fidelityTable =
        new JBTable() {
          @Override
          public TableCellRenderer getCellRenderer(int row, int column) {
            if (column == 1) {
              FidelityTableData currentData =
                  (FidelityTableData) this.getModel().getValueAt(row, column);
              return getCellRendererByValue(currentData);
            } else {
              return super.getCellRenderer(row, column);
            }
          }

          @Override
          public TableCellEditor getCellEditor(int row, int column) {
            if (column == 1) {
              FidelityTableData currentData =
                  (FidelityTableData) this.getModel().getValueAt(row, column);
              return getCellEditorByValue(currentData);
            } else {
              return super.getCellEditor(row, column);
            }
          }
        };
    fidelityDecoratorPanel =
        ToolbarDecorator.createDecorator(fidelityTable)
            .setAddAction(it -> fidelityTabController.addRowAction(fidelityTable))
            .setRemoveAction(it -> fidelityTabController.removeRowAction(fidelityTable))
            .createPanel();

    fidelityLabel.setFont(TabLayout.getMainFont());
    informationLabel.setFont(TabLayout.getSecondaryLabel());
  }

  private void initComponents() {
    this.setLayout(new VerticalLayout());
    setSize();
    fidelityLabel.setFont(TabLayout.getMainFont());
    informationLabel.setFont(TabLayout.getSecondaryLabel());
    this.add(fidelityLabel);
    this.add(Box.createVerticalStrut(10));
    this.add(informationLabel);
    FidelityTableModel model = new FidelityTableModel(fidelityTabController);
    fidelityTable.setModel(model);
    TableColumn enumColumn = fidelityTable.getColumnModel().getColumn(1);
    enumColumn.setCellEditor(new FidelityTableDecorators.TextBoxEditor());
    enumColumn.setCellRenderer(new FidelityTableDecorators.TextBoxRenderer());
    TableColumn typeColumn = fidelityTable.getColumnModel().getColumn(0);
    typeColumn.setMinWidth(150);
    typeColumn.setMaxWidth(300);
    typeColumn.setCellEditor(
        new ComboBoxEditor(new FieldType[]{FieldType.INT32, FieldType.FLOAT, FieldType.ENUM}));
    typeColumn.setCellRenderer(new ComboBoxRenderer());
    setDecoratorPanelSize(fidelityDecoratorPanel);
    setTableSettings(scrollPane, fidelityDecoratorPanel, fidelityTable);
    this.add(scrollPane);
    this.add(Box.createVerticalStrut(10));
    this.add(new EnumTable(fidelityTabController));
  }

  private TableCellRenderer getCellRendererByValue(FidelityTableData data) {
    if (data.getFieldType().equals(FieldType.ENUM)) {
      return new FidelityTableDecorators.JPanelDecorator(fidelityTabController.getEnumsNames());
    }
    return new FidelityTableDecorators.TextBoxRenderer();
  }

  private TableCellEditor getCellEditorByValue(FidelityTableData data) {
    if (data.getFieldType().equals(FieldType.ENUM)) {
      return new FidelityTableDecorators.JPanelDecorator(fidelityTabController.getEnumsNames());
    }
    return new FidelityTableDecorators.TextBoxEditor();
  }

  @Override
  public void propertyChange(PropertyChangeEvent evt) {
    if (evt.getPropertyName().equals("editEnum")) {
      String oldName = evt.getOldValue().toString();
      String newName = evt.getNewValue().toString();
      FidelityTableModel fidelityTableModel = (FidelityTableModel) fidelityTable.getModel();
      for (int i = 0; i < fidelityTableModel.getRowCount(); i++) {
        FidelityTableData rowData = (FidelityTableData) fidelityTableModel.getValueAt(i, 0);
        if (rowData.getFieldEnumName().equals(oldName)) {
          rowData.setFieldEnumName(newName);
        }
      }
      repaint();
    } else if (evt.getPropertyName().equals("deleteEnum")) {
      String name = evt.getOldValue().toString();
      FidelityTableModel fidelityTableModel = (FidelityTableModel) fidelityTable.getModel();
      for (int i = 0; i < fidelityTableModel.getRowCount(); i++) {
        FidelityTableData rowData = (FidelityTableData) fidelityTableModel.getValueAt(i, 0);
        if (rowData.getFieldEnumName().equals(name)) {
          rowData.setFieldEnumName("");
        }
      }
      repaint();
    }
  }
}
