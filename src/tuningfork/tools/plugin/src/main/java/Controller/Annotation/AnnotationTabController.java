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

import Controller.BaseController;
import Model.EnumDataModel;
import Model.MessageDataModel;
import Model.MessageDataModel.Type;
import com.intellij.ui.components.JBLabel;
import java.util.ArrayList;
import java.util.List;
import javax.swing.JTable;

public class AnnotationTabController extends BaseController {

    private List<EnumDataModel> annotationEnums;
    private MessageDataModel annotationDataModel;

    public AnnotationTabController() {
        annotationEnums = new ArrayList<>();
        annotationDataModel = new MessageDataModel();
        annotationDataModel.setMessageType(Type.ANNOTATION);
    }

    public void addAnnotationEnum(EnumDataModel enumDataModel) {
        annotationEnums.add(enumDataModel);
    }

    public List<EnumDataModel> getAnnotationEnums() {
        return annotationEnums;
    }

    public void addEnumToAnnotation(String enumName, String fieldName) {
        annotationDataModel.addField(fieldName, enumName);
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
        annotationDataModel.setMessageType(MessageDataModel.Type.FIDELITY);

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


    @Override
    public void onEnumAdd(String name, ArrayList<String> options) {
        annotationEnums.add(new EnumDataModel(name, options));
    }
}