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

package View.Quality;

import Controller.Enum.EnumController;
import Controller.Fidelity.FidelityTabController;
import Controller.Quality.QualityTabController;
import Controller.Quality.QualityTableModel;
import Model.EnumDataModel;
import View.TabLayout;
import com.intellij.openapi.project.Project;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.stream.Collectors;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import org.bouncycastle.jcajce.provider.digest.MD2.HashMac;
import org.jdesktop.swingx.VerticalLayout;


public class QualityTab extends TabLayout {

  private final static JLabel title = new JLabel("Quality levels");
  private JScrollPane scrollPane;

  private final static JLabel aboutQualitySettings = new JLabel(
      "<html> All quality settings are saved into " +
          "app/src/main/assets/tuningfork/dev_tuningfork_fidelityparams_*.txt files. <br>" +
          "You should have at least one quality level. <br>" +
          "Once you add a new level, you can edit/add data it by" +
          "modifying the text in the table below.</html> ");

  private JBTable qualityParametersTable;
  private QualityTableModel qualityTableModel;
  private QualityTabController qualityTabController;
  private FidelityTabController fidelityTabController;
  private EnumController enumController;
  private JPanel decoratorPanel;
  private Project project;

  public QualityTab(FidelityTabController fidelityTabController,
      QualityTabController qualityTabController,
      EnumController enumController,
      Project project) {
    this.setLayout(new VerticalLayout());
    this.qualityTabController = qualityTabController;
    this.fidelityTabController = fidelityTabController;
    this.enumController = enumController;
    this.project = project;
    setSize();
    initComponents();
    addComponents();
  }

  public QualityTabController getQualityTabController() {
    return qualityTabController;
  }

  private void addComponents() {
    this.add(title);
    this.add(aboutQualitySettings);
    this.add(scrollPane);
  }

  private List<String> getMatchingEnumOptions(String enumName) {
    for (EnumDataModel enumDataModel : enumController.getEnums()) {
      System.out.println(enumDataModel.getName());
    }
    EnumDataModel matchingEnum = enumController.getEnums().stream()
        .filter(enumModel -> enumModel.getName().equals(enumName))
        .collect(Collectors.toList())
        .get(0);
    return matchingEnum.getOptions();
  }

  private void setColumnsAndRenderers() {
    HashMap<Integer, String> enumColumns = qualityTableModel.getEnumNames();
    int columnCount = qualityTableModel.getColumnCount();
    for (int i = 1; i < columnCount; i++) {
      if (enumColumns.containsKey(i)) {
        initComboBoxColumns(qualityParametersTable, i, getMatchingEnumOptions(enumColumns.get(i)));
      } else {
        initTextFieldColumns(qualityParametersTable, i);
      }
    }
  }

  public boolean saveSettings() {
    return qualityTabController.saveSettings(qualityParametersTable,
        fidelityTabController.getFidelityDataModel());
  }

  private void initComponents() {
    title.setFont(getMainFont());
    aboutQualitySettings.setFont(getSecondaryLabel());

    qualityTableModel = new QualityTableModel(fidelityTabController.getFidelityDataModel());
    qualityParametersTable = new JBTable(qualityTableModel);
    qualityTabController.addInitialQuality(qualityParametersTable);
    decoratorPanel = ToolbarDecorator.createDecorator(qualityParametersTable)
        .setAddAction(anActionButton -> QualityTabController.addRow(qualityParametersTable))
        .setRemoveAction(
            anActionButton -> QualityTabController.removeRow(qualityParametersTable, project))
        .createPanel();
    scrollPane = new JBScrollPane();
    setDecoratorPanelSize(decoratorPanel);
    setTableSettings(scrollPane, decoratorPanel, qualityParametersTable);
    setColumnsAndRenderers();
  }
}
