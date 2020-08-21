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

import Controller.Quality.QualityTabController;
import Controller.Quality.QualityTableModel;
import Model.QualityDataModel;
import com.intellij.ui.table.JBTable;
import java.util.Arrays;
import java.util.List;
import javax.swing.JLabel;
import org.junit.Assert;
import org.junit.Test;

public class QualityTabControllerTest {

  @Test
  public void addQualityTest() {
    QualityTabController controller = new QualityTabController();
    QualityTableModel model = new QualityTableModel(
        new String[]{"Shadows (ENUM)", "Velocity (float)", "Quality (ENUM)"});
    JBTable table = new JBTable(model);

    model.addRow(new String[]{"DARK", "1.23", "GOOD"}, table);
    model.addRow(new String[]{"DARK", "1.28", "POOR"}, table);
    model.addRow(new String[]{"BRIGHT", "1.45", "BAD"}, table);

    controller.saveSettings(table, new JLabel());
    List<QualityDataModel> qualityDataModels = controller.getQualityDataModels();

    Assert.assertEquals(QualityTabController.getFilesCount(), 3);
    Assert.assertEquals(qualityDataModels.get(0).getFieldNames(),
        Arrays.asList("Shadows", "Velocity", "Quality"));
    Assert.assertEquals(qualityDataModels.get(0).getFieldValues(),
        Arrays.asList("DARK", "1.23", "GOOD"));
    Assert.assertEquals(qualityDataModels.get(1).getFieldValues(),
        Arrays.asList("DARK", "1.28", "POOR"));
    Assert.assertEquals(qualityDataModels.get(2).getFieldValues(),
        Arrays.asList("BRIGHT", "1.45", "BAD"));
  }
}
