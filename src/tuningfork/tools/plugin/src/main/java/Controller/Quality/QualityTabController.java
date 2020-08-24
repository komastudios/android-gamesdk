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

import Model.QualityDataModel;
import javax.swing.JTable;

import java.util.ArrayList;
import java.util.List;
import javax.swing.JLabel;

public class QualityTabController {
  private static int filesCount = 0;
  private List<QualityDataModel> qualityDataModels;

  public QualityTabController() {
    qualityDataModels = new ArrayList<>();
  }

  public static void addRow(JTable table) {
    QualityTableModel tableModel = (QualityTableModel) table.getModel();
    tableModel.addRow(new String[]{Integer.toString(++filesCount), "", "", ""}, table);
  }

  public static void removeRow(JTable table) {
    QualityTableModel tableModel = (QualityTableModel) table.getModel();
    tableModel.removeRow(table.getSelectedRow());
  }

  public boolean saveSettings(JTable table, JLabel savedSettingsLabel) {
    List<String> fieldNames = ((QualityTableModel) table.getModel()).getColumnNames();
    List<List<String>> qualitySettings = ((QualityTableModel) table.getModel()).getQualitySettings();

    for (List<String> qualitySetting : qualitySettings) {
      QualityDataModel qualityDataModel = new QualityDataModel(fieldNames, qualitySetting);

      //TODO (targintaru) integrate validation

      qualityDataModels.add(qualityDataModel);
    }

    savedSettingsLabel.setVisible(true);
    return true;
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
}

