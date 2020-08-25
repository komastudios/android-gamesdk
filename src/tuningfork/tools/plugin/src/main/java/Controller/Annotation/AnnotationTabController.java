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
package Controller.Annotation;

import Controller.Enum.EnumController;
import Model.MessageDataModel;
import Model.MessageDataModel.Type;
import View.Annotation.AnnotationTab;
import com.intellij.ui.components.JBLabel;
import java.util.List;
import javax.swing.JTable;

public class AnnotationTabController extends EnumController {

  private MessageDataModel annotationDataModel;
  private AnnotationTab annotationTab;

  public AnnotationTabController() {
    super();
    annotationDataModel = new MessageDataModel();
    annotationDataModel.setMessageType(Type.ANNOTATION);
  }

  public void setAnnotationTab(AnnotationTab annotationTab) {
    this.annotationTab = annotationTab;
  }

  @Override
  public void onEnumTableChanged() {
    annotationTab.initComboBoxColumns(annotationTab.getAnnotationTable(), 0, getEnumsNames());
  }

  public static void addRowAction(JTable jtable) {
    AnnotationTableModel model = (AnnotationTableModel) jtable.getModel();
    // TODO(mohanad): placeholder values used. Will be replaced later with default enum value.
    model.addRow(new String[]{
        "",
        "",
    });
  }

  public static void removeRowAction(JTable jtable) {
    AnnotationTableModel model = (AnnotationTableModel) jtable.getModel();
    int row = jtable.getSelectedRow();
    if (jtable.getCellEditor() != null) {
      jtable.getCellEditor().stopCellEditing();
    }
    model.removeRow(row);
  }

  public boolean saveSettings(JTable jTable, JBLabel savedSettingsLabel) {
    List<String> annotationEnumNames = ((AnnotationTableModel) jTable.getModel())
        .getAnnotationEnumNames();
    List<String> annotationFieldNames = ((AnnotationTableModel) jTable.getModel())
        .getAnnotationFieldNames();
    annotationDataModel = new MessageDataModel();
    annotationDataModel.setMessageType(Type.ANNOTATION);

    savedSettingsLabel.setVisible(true);

    if (!annotationDataModel.addMultipleFields(annotationFieldNames, annotationEnumNames)) {
      savedSettingsLabel.setText("ERROR: multiple fields with the same name.");
      return false;
    }

    //TODO (aymanm, targintaru, volobushenk) integrate validation; return false if errors
    savedSettingsLabel.setText("Settings saved successfully!");
    return true;
  }

  public MessageDataModel getAnnotationDataModel() {
    return annotationDataModel;
  }

}