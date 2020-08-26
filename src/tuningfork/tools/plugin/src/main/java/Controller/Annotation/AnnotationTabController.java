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
import Model.EnumDataModel;
import Model.MessageDataModel;
import com.intellij.openapi.ui.ValidationInfo;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import javax.swing.JTable;

public class AnnotationTabController extends EnumController {

  private final MessageDataModel annotationDataModel;
  private final PropertyChangeSupport propertyChangeSupport;
  private static final String FIELD_NAME_PATTERN = "[a-zA-Z_]+$";

  public AnnotationTabController(MessageDataModel annotationDataModel, List<EnumDataModel> enums) {
    super(enums);
    this.annotationDataModel = annotationDataModel;
    propertyChangeSupport = new PropertyChangeSupport(this);
  }

  public MessageDataModel getAnnotationData() {
    return annotationDataModel;
  }

  public void addInitialAnnotation(JTable table) {
    List<String> enumNames = annotationDataModel.getFieldNames();
    List<String> enumValues = annotationDataModel.getFieldTypes();
    AnnotationTableModel model = (AnnotationTableModel) table.getModel();
    ArrayList<String[]> data = new ArrayList<>();
    for (int i = 0; i < enumNames.size(); i++) {
      data.add(new String[]{enumValues.get(i), enumNames.get(i)});
    }
    model.setData(data);
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

  public List<ValidationInfo> validate() {
    List<ValidationInfo> validationInfos = new ArrayList<>();
    List<String> types = annotationDataModel.getFieldTypes();
    boolean emptyType = types.stream().anyMatch(String::isEmpty);
    if (emptyType) {
      validationInfos.add(new ValidationInfo("Empty Field Type Is Not Allowed"));
    }
    List<String> names = annotationDataModel.getFieldNames();
    boolean duplicateField =
        names.stream().anyMatch(name -> Collections.frequency(names, name) > 1);
    if (duplicateField) {
      validationInfos.add(new ValidationInfo("Duplicate Fields Are Not Allowed"));
    }
    names.stream()
        .filter(s -> !s.matches(FIELD_NAME_PATTERN) && !s.isEmpty())
        .forEach(s ->
            validationInfos.add(
                new ValidationInfo(s + " Does Not Match The Pattern [a-zA-Z_].")));
    if (names.stream().anyMatch(String::isEmpty)) {
      validationInfos.add(new ValidationInfo("Empty Field Name Is Not Allowed."));
    }
    return validationInfos;
  }
}
