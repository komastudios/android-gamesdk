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

import static Utils.Validation.UIValidator.ILLEGAL_TEXT_PATTERN;

import Controller.Annotation.AnnotationTabController;
import Controller.Annotation.AnnotationTableModel;
import Model.EnumDataModel;
import Utils.Validation.UIValidator;
import View.Annotation.AnnotationDecorator.EnumComboBoxDecorator;
import View.Decorator.TableRenderer;
import View.Decorator.TableRenderer.RoundedCornerRenderer;
import View.EnumTable;
import View.TabLayout;
import com.intellij.openapi.Disposable;
import com.intellij.openapi.ui.ValidationInfo;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.Collections;
import java.util.List;
import java.util.regex.Pattern;
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
  private final Disposable disposable;

  public AnnotationTab(AnnotationTabController annotationController, Disposable disposable) {
    this.annotationController = annotationController;
    this.disposable = disposable;
    initVariables();
    initComponents();
    initValidators();
  }

  public AnnotationTabController getAnnotationController() {
    return annotationController;
  }

  private void initVariables() {
    scrollPane = new JBScrollPane();
    annotationTable = new JBTable();
    decoratorPanel =
        ToolbarDecorator.createDecorator(annotationTable)
            .setAddAction(it -> annotationController.addRowAction(annotationTable))
            .setRemoveAction(it -> annotationController.removeRowAction(annotationTable))
            .createPanel();
    model = new AnnotationTableModel(annotationController);
    annotationController.addPropertyChangeListener(this);
  }

  private void initComponents() {
    // Initialize layout.
    this.setLayout(new VerticalLayout());
    setSize();

    // Add labels.
    annotationLabel.setFont(getMainFont());
    informationLabel.setFont(getSecondaryFont());
    this.add(annotationLabel);
    this.add(Box.createVerticalStrut(10));
    this.add(informationLabel);
    // Initialize toolbar and table.
    annotationTable.setModel(model);
    TableColumn enumColumn = annotationTable.getColumnModel().getColumn(0);
    enumColumn.setCellEditor(new EnumComboBoxDecorator(
        annotationController.getEnums()));
    //noinspection UnstableApiUsage
    enumColumn
        .setCellRenderer(TableRenderer.getRendererComboBoxWithValidation(new EnumComboBoxDecorator(
                annotationController.getEnums()),
            (value, row, column) -> {
              String strVal = value.toString();
              if (strVal.isEmpty()) {
                return new ValidationInfo("Field Can Not Be Empty");
              }
              return null;
            })
        );
    annotationController.addInitialAnnotation(annotationTable);
    setDecoratorPanelSize(decoratorPanel);
    setTableSettings(scrollPane, decoratorPanel, annotationTable);
    TableColumn nameColumn = annotationTable.getColumnModel().getColumn(1);
    //noinspection UnstableApiUsage
    nameColumn.setCellRenderer(
        TableRenderer.getRendererTextBoxWithValidation(new RoundedCornerRenderer(),
            (value, row, column) -> {
              String strVal = value.toString();
              if (strVal.isEmpty()) {
                return new ValidationInfo("Field Can Not Be Empty");
              }
              if (Pattern.compile(ILLEGAL_TEXT_PATTERN).matcher(strVal).find()) {
                return new ValidationInfo(strVal + " contains illegal characters");
              }
              return null;
            }));
    nameColumn.setCellEditor(TableRenderer.getEditorTextBoxWithValidation(disposable));
    this.add(scrollPane);
    this.add(Box.createVerticalStrut(10));
    this.add(new EnumTable(annotationController));
  }

  public boolean isViewValid() {
    return UIValidator.isComponentValid(annotationTable) && UIValidator
        .isTableCellsValid(annotationTable);
  }

  private void initValidators() {
    TableRenderer.addCellToolTipManager(annotationTable, disposable);
    UIValidator.createTableValidator(disposable, annotationTable, () -> {
      List<String> fieldNames = annotationController.getAnnotationData().getFieldNames();
      List<String> fieldTypes = annotationController.getAnnotationData().getFieldTypes();
      boolean isNamesDuplicate =
          fieldNames.stream().anyMatch(option -> Collections.frequency(fieldNames, option) > 1);
      if (isNamesDuplicate) {
        return new ValidationInfo("Repeated Fields Names Are Not Allowed.", annotationTable);
      }
      return null;
    });

  }

  public void saveSettings() {
    annotationTable.clearSelection();
  }

  @Override
  public void propertyChange(PropertyChangeEvent propertyChangeEvent) {
    switch (propertyChangeEvent.getPropertyName()) {
      case "addEnum":
        annotationTable.repaint();
        break;
      case "editEnum":
        String oldName = propertyChangeEvent.getOldValue().toString();
        EnumDataModel newEnum = (EnumDataModel) propertyChangeEvent.getNewValue();
        String newName = newEnum.getName();
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