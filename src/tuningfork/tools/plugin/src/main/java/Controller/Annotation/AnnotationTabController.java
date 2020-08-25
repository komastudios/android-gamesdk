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
import java.beans.PropertyChangeSupport;
import java.util.ArrayList;
import java.util.List;
import javax.swing.JTable;

public class AnnotationTabController extends EnumController {

  private MessageDataModel annotationDataModel;
  private final PropertyChangeSupport propertyChangeSupport;

  public AnnotationTabController(MessageDataModel annotationDataModel) {
    super();
    this.annotationDataModel = annotationDataModel;
    propertyChangeSupport = new PropertyChangeSupport(this);
  }

  public void addInitialAnnotation(JTable table) {
    List<String> enumNames = annotationDataModel.getFieldNames();
    List<String> enumValues = annotationDataModel.getFieldTypes();
    AnnotationTableModel model = (AnnotationTableModel) table.getModel();
    List<String[]> data = new ArrayList<>();
    for (int i = 0; i < enumNames.size(); i++) {
      data.add(new String[]{enumValues.get(i), enumNames.get(i)});
    }
    model.setData(data);
  }



  @Override
  public void onEnumTableChanged(ChangeType changeType, Object[] changeList) {
    if (changeType.equals(ChangeType.EDIT)) {
      propertyChangeSupport.firePropertyChange("editEnum", changeList[0], changeList[1]);
    } else if (changeType.equals(ChangeType.REMOVE)) {
      propertyChangeSupport.firePropertyChange("deleteEnum", changeList[0], "");
    }
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

  public boolean saveSettings(JTable jTable) {
    List<String> annotationEnumNames = ((AnnotationTableModel) jTable.getModel())
        .getAnnotationEnumNames();
    List<String> annotationFieldNames = ((AnnotationTableModel) jTable.getModel())
        .getAnnotationFieldNames();
    annotationDataModel = new MessageDataModel();
    annotationDataModel.setMessageType(Type.ANNOTATION);

    return annotationDataModel.addMultipleFields(annotationFieldNames, annotationEnumNames);
  }

  public MessageDataModel getAnnotationData() {
    return annotationDataModel;
  }
}