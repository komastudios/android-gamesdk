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

package Controller.Quality;

import Model.EnumDataModel;
import Model.MessageDataModel;
import Model.QualityDataModel;
import View.Fidelity.FieldType;
import java.util.ArrayList;
import java.util.List;
import java.util.OptionalInt;
import java.util.stream.IntStream;
import javax.swing.JTable;

public class QualityTabController {

  private static int filesCount = 1;
  private final List<QualityDataModel> qualityDataModels;
  private final MessageDataModel fidelityData;
  private final List<EnumDataModel> enums;

  public QualityTabController(List<QualityDataModel> qualityDataModels,
      MessageDataModel fidelityData,
      List<EnumDataModel> enums) {
    this.qualityDataModels = qualityDataModels;
    this.fidelityData = fidelityData;
    this.enums = enums;
  }

  public void addColumn(JTable table) {
    QualityTableModel tableModel = (QualityTableModel) table.getModel();
    tableModel.addColumn(filesCount++);
  }

  public void removeColumn(JTable table) {
    QualityTableModel tableModel = (QualityTableModel) table.getModel();
    int column = table.getSelectedColumn();
    if (table.isEditing()) {
      table.getCellEditor().stopCellEditing();
    }
    tableModel.removeColumn(column);
  }

  public List<EnumDataModel> getEnums() {
    return enums;
  }

  public void addNewQualityFile() {
    QualityDataModel qualityDataModel = new QualityDataModel();
    fidelityData.getFieldNames().forEach(fieldName -> qualityDataModel.addField(fieldName, ""));
    qualityDataModels.add(qualityDataModel);
  }

  public void addNewFieldToAllFiles() {
    qualityDataModels.forEach(qualityDataModel -> qualityDataModel.addField("", ""));
  }

  public void updateFieldName(final int row, final String newName) {
    qualityDataModels.forEach(qualityDataModel -> qualityDataModel.updateName(row, newName));
  }

  public void updateFieldValue(final int fileID, final int row, final String value) {
    qualityDataModels.get(fileID).updateValue(row, value);
  }

  public void addRow(JTable table) {
    QualityTableModel tableModel = (QualityTableModel) table.getModel();
    tableModel.addRow();
  }

  public void removeRow(JTable table, int row) {
    QualityTableModel tableModel = (QualityTableModel) table.getModel();
    tableModel.removeRow(row);
  }

  public boolean isEnum(String name) {
    OptionalInt index = IntStream.range(0, fidelityData.getFieldNames().size())
        .filter(i -> fidelityData.getFieldNames().get(i).equals(name))
        .findFirst();

    if (index.isPresent()) {
      String fieldType = fidelityData.getFieldTypes().get(index.getAsInt());
      return !fieldType.equals(FieldType.INT32.getName())
          && !fieldType.equals(FieldType.FLOAT.getName());
    }
    return false;
  }

  public List<String> getEnumOptionsByName(String name) {
    if (isEnum(name)) {
      int index = getFieldIndexByName(name);
      for (EnumDataModel anEnum : enums) {
        if (anEnum.getName().equals(fidelityData.getFieldTypes().get(index))) {
          return anEnum.getOptions();
        }
      }
    }
    return new ArrayList<>();
  }

  public Integer getFieldIndexByName(String name) {
    return IntStream.range(0, fidelityData.getFieldNames().size())
        .filter(i -> fidelityData.getFieldNames().get(i).equals(name))
        .findFirst().getAsInt();
  }
}

