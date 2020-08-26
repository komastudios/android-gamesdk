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
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import javax.swing.JTable;

public class AnnotationTabController extends EnumController {

  private MessageDataModel annotationDataModel;
  private final PropertyChangeSupport propertyChangeSupport;
  public AnnotationTabController() {
    super();
    annotationDataModel = new MessageDataModel();
    annotationDataModel.setMessageType(Type.ANNOTATION);
    this.propertyChangeSupport = new PropertyChangeSupport(this);
  }

  @Override
  public void onEnumTableChanged(ChangeType changeType,
      Object[] changeList) {
    if (changeType.equals(ChangeType.ADD)) {
      propertyChangeSupport
          .firePropertyChange("addEnum", changeList[0], "");
    } else if (changeType.equals(ChangeType.EDIT)) {
      propertyChangeSupport
          .firePropertyChange("editEnum", changeList[0], changeList[1]);
    } else if (changeType.equals(ChangeType.REMOVE)) {
      propertyChangeSupport
          .firePropertyChange("deleteEnum", changeList[0], "");
    }
  }

  public void addRowAction(JTable jtable) {
    AnnotationTableModel model = (AnnotationTableModel) jtable.getModel();
    model.addRow(
        new String[]{
            "", "",
        });
  }

  public void removeRowAction(JTable jtable) {
    AnnotationTableModel model = (AnnotationTableModel) jtable.getModel();
    int row = jtable.getSelectedRow();
    if (jtable.getCellEditor() != null) {
      jtable.getCellEditor().stopCellEditing();
    }
    model.removeRow(row);
  }

  public void setEnumFieldType(int row, String enumType) {
    annotationDataModel.updateType(row, enumType);
  }

  public void setEnumFieldName(int row, String enumType) {
    annotationDataModel.updateName(row, enumType);
  }

  public void addEnumField() {
    annotationDataModel.addField("", "");
  }

  public void removeEnumField(int row) {
    annotationDataModel.removeSetting(row);
  }

  public void addPropertyChangeListener(
      PropertyChangeListener propertyChangeListener) {
    propertyChangeSupport
        .addPropertyChangeListener(propertyChangeListener);
  }
}
