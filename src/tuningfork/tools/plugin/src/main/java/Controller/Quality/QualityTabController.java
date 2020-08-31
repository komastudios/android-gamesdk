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

import Model.MessageDataModel;
import Model.QualityDataModel;
import Utils.Assets.AssetsFinder;
import com.intellij.openapi.project.Project;
import java.io.File;
import javax.swing.JTable;

import java.util.ArrayList;
import java.util.List;

public class QualityTabController {

  private static int filesCount = 0;
  private List<QualityDataModel> qualityDataModels;

  public QualityTabController() {
    qualityDataModels = new ArrayList<>();
  }

  public QualityTabController(List<QualityDataModel> qualityDataModels) {
    this.qualityDataModels = qualityDataModels;
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
    tableModel.addRow(new String[]{Integer.toString(++filesCount), "", "", ""}, table);
  }

  public static void removeRow(JTable table, Project project) {
    QualityTableModel tableModel = (QualityTableModel) table.getModel();
    int fileIndex = (int) table.getValueAt(table.getSelectedRow(), 0);
    tableModel.removeRow(table.getSelectedRow());
    String assetsDir =
        AssetsFinder.findAssets(project.getProjectFilePath().split(".idea")[0]).getAbsolutePath();
    File fileToRemove = new File(assetsDir, "/dev_tuningfork_fidelityparams_" + fileIndex + ".bin");
    fileToRemove.delete();
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

  public boolean saveSettings(JTable jTable, MessageDataModel fidelityDataModel) {
    List<List<String>> qualitySettings = ((QualityTableModel) jTable.getModel())
        .getQualitySettings();
    List<String> fieldNames = fidelityDataModel.getFieldNames();
    qualityDataModels = new ArrayList<>();

    for (List<String> qualitySetting : qualitySettings) {
      qualityDataModels.add(new QualityDataModel(fieldNames, qualitySetting));
    }

    return true;
  }
}

