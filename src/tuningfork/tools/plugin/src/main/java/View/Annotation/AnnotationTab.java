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

package View.Annotation;

import Controller.Annotation.AnnotationTabController;
import Controller.Annotation.AnnotationTableModel;
import View.Annotation.AnnotationDecorator.EnumComboBoxDecorator;
import View.EnumTable;
import View.TabLayout;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import javax.swing.Box;
import javax.swing.JPanel;
import javax.swing.table.TableColumn;
import org.jdesktop.swingx.VerticalLayout;

public class AnnotationTab extends TabLayout implements PropertyChangeListener {

  private JBScrollPane scrollPane;
  private JBTable annotationTable;
  private JPanel decoratorPanel;
  private AnnotationTableModel model;
  private final AnnotationTabController annotationController;

  private final JBLabel annotationLabel = new JBLabel("Annotation Settings");
  private final JBLabel informationLabel =
      new JBLabel("Annotation is used by tuning fork to mark the histograms being sent.");

  public AnnotationTab(AnnotationTabController annotationController) {
    this.annotationController = annotationController;
    initVariables();
    initComponents();
  }


  private void initVariables() {
    scrollPane = new JBScrollPane();
    annotationTable = new JBTable();
    decoratorPanel =
        ToolbarDecorator.createDecorator(annotationTable)
            .setAddAction(it -> annotationController.addRowAction(annotationTable))
            .setRemoveAction(it -> annotationController.removeRowAction(annotationTable))
            .createPanel();
    annotationController.addPropertyChangeListener(this);
  }

  private void initComponents() {
    // Initialize layout.
    this.setLayout(new VerticalLayout());
    setSize();

    // Add labels.
    annotationLabel.setFont(getMainFont());
    informationLabel.setFont(getSecondaryLabel());
    this.add(annotationLabel);
    this.add(Box.createVerticalStrut(10));
    this.add(informationLabel);
    // Initialize toolbar and table.
    model = new AnnotationTableModel(annotationController);
    annotationTable.setModel(model);
    TableColumn enumColumn = annotationTable.getColumnModel().getColumn(0);
    enumColumn.setCellRenderer(new EnumComboBoxDecorator(
        annotationController.getEnums()));
    enumColumn.setCellEditor(new EnumComboBoxDecorator(
        annotationController.getEnums()));
    setDecoratorPanelSize(decoratorPanel);
    setTableSettings(scrollPane, decoratorPanel, annotationTable);
    initTextFieldColumns(annotationTable, 1);

    this.add(scrollPane);
    this.add(Box.createVerticalStrut(10));
    this.add(new EnumTable(annotationController));
  }


  @Override
  public void propertyChange(PropertyChangeEvent propertyChangeEvent) {
    switch (propertyChangeEvent.getPropertyName()) {
      case "addEnum":
        annotationTable.repaint();
        break;
      case "editEnum":
        String oldName = propertyChangeEvent.getOldValue().toString();
        String newName = propertyChangeEvent.getNewValue().toString();
        for (int i = 0; i < model.getRowCount(); i++) {
          String enumType = model.getValueAt(i, 0).toString();
          if (enumType.equals(oldName)) {
            model.setValueAt(newName, i, 0);
          }
        }
        annotationTable.repaint();
        break;
      case "deleteEnum":
        String name = propertyChangeEvent.getOldValue().toString();
        for (int i = 0; i < model.getRowCount(); i++) {
          String enumType = model.getValueAt(i, 0).toString();
          if (enumType.equals(name)) {
            model.setValueAt("", i, 0);
          }
        }
        annotationTable.repaint();
        break;
    }
  }
}