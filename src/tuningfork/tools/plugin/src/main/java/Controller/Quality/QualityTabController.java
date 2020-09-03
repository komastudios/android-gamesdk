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

import Controller.Enum.EnumController;
import Model.EnumDataModel;
import Model.MessageDataModel;
import Model.QualityDataModel;
import Utils.Assets.AssetsFinder;
import com.intellij.openapi.project.Project;
import java.io.File;
import java.util.HashSet;
import javax.swing.JTable;

import java.util.ArrayList;
import java.util.List;
import javax.swing.JLabel;

public class QualityTabController {

  private static int filesCount = 0;
  private static int filesToDelete = 0;
  private static int fieldsCount;
  private List<QualityDataModel> qualityDataModels;


  public QualityTabController() {
    qualityDataModels = new ArrayList<>();
  }

  public QualityTabController(List<QualityDataModel> qualityDataModels, int fieldsCount) {
    this.qualityDataModels = qualityDataModels;
    this.fieldsCount = fieldsCount;
  }

  public boolean containsField(String name) {
    if (filesCount > 0) {
      return qualityDataModels.get(0).getFieldNames().contains(name);
    }
    return true;
  }

  public void addEmptyFields(String name) {
    for (QualityDataModel qualityDataModel : qualityDataModels) {
      qualityDataModel.addField(name, "");
    }
  }

  public static void updateQualityColumnNames(JTable table, MessageDataModel fidelityModel) {
    QualityTableModel tableModel = (QualityTableModel) table.getModel();
    tableModel.setColumnNames(fidelityModel);
  }

  public void addInitialQuality(JTable table) {
    QualityTableModel tableModel = (QualityTableModel) table.getModel();
    filesCount = qualityDataModels.size();

    for (int file = 0; file < qualityDataModels.size(); file++) {
      List<String> rowToAdd = qualityDataModels.get(file).getFieldValues();
      rowToAdd.add(0, Integer.toString(file + 1));
      String rowArray[] = new String[rowToAdd.size()];
      rowArray = rowToAdd.toArray(rowArray);
      tableModel.addRow(rowArray, table);
    }
  }

  public static void addRow(JTable table) {
    QualityTableModel tableModel = (QualityTableModel) table.getModel();
    String[] emptyRow = new String[fieldsCount + 1];
    emptyRow[0] = Integer.toString(++filesCount);
    for (int i = 1; i <= fieldsCount; i++) {
      emptyRow[i] = " ";
    }
    tableModel.addRow(emptyRow, table);
  }

  public static void removeRow(JTable table, Project project) {
    QualityTableModel tableModel = (QualityTableModel) table.getModel();
    int row = table.getSelectedRow();

    filesToDelete++;
    filesCount--;

    if (table.getCellEditor() != null) {
      table.getCellEditor().stopCellEditing();
    }
    tableModel.removeRow(row);
  }

  public static int getFilesCount() {
    return filesCount;
  }

  public List<QualityDataModel> getQualityDataModels() {
    return qualityDataModels;
  }

  public static void increaseFilesCount() {
    filesCount++;
  }

  public static void increaseFieldsCount() {
    fieldsCount++;
  }

  public static void decreaseFieldsCount() {
    fieldsCount--;
  }

  private String getEnumOption(EnumController enumController, String enumName) {
    List<EnumDataModel> enums = enumController.getEnums();
    for (EnumDataModel enumDataModel : enums) {
      if (enumDataModel.getName().equals(enumName)) {
        return enumDataModel.getOptions().get(1);
      }
    }
    return "";
  }

  public int getFilesToDelete() {
    return filesToDelete;
  }

  public boolean saveSettings(JTable jTable, MessageDataModel fidelityDataModel,
      EnumController enumController) {
    ArrayList<ArrayList<String>> qualitySettings = ((QualityTableModel) jTable.getModel())
        .getQualitySettings();
    List<String> fieldNames = fidelityDataModel.getFieldNames();

    qualityDataModels = new ArrayList<>();

    List<String> emptyFields = new ArrayList<>();
    // Values added just to 'mark' the new field in the Quality Settings Table.
    // Will have to be manually modified by the user, or else will probably generate warnings
    // on saving.
    for (int i = fieldsCount; i < fieldNames.size(); i++) {
      String fieldType = fidelityDataModel.getType(fieldNames.get(i));
      if (fieldType.equals("int32")) {
        emptyFields.add("1");
      } else if (fieldType.equals("float")) {
        emptyFields.add("1.0");
      } else {
        emptyFields.add(getEnumOption(enumController, fieldNames.get(i)));
      }
    }

    for (List<String> qualitySetting : qualitySettings) {
      qualitySetting.addAll(emptyFields);
      qualityDataModels.add(new QualityDataModel(fieldNames, qualitySetting));
    }

    fieldsCount = fieldNames.size();

    return true;
  }
}
