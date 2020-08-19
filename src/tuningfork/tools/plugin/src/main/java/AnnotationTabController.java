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

import com.intellij.ui.components.JBLabel;
import java.util.ArrayList;
import java.util.List;
import javax.swing.JTable;

public class AnnotationTabController {
    private List<EnumDataModel> annotationEnums;
    private MessageDataModel annotationDataModel;

    AnnotationTabController() {
        annotationEnums = new ArrayList<>();
        annotationDataModel = new MessageDataModel();
        annotationDataModel.setMessageType(MessageDataModel.Type.ANNOTATION);
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
        model.addRow(new String[] {
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
        List<String[]> annotationData = ((AnnotationTableModel) jTable.getModel()).getData();

        for (String[] row : annotationData) {
            String enumName = row[0];
            String fieldName = row[1];
            if (!annotationDataModel.addField(fieldName, enumName)) {
                return false;
            }
        }

        //TODO (aymanm, targintaru, volobushenk) integrate validation
        savedSettingsLabel.setVisible(true);
        return true;
    }

    public MessageDataModel getAnnotationDataModel() {
        return annotationDataModel;
    }
}