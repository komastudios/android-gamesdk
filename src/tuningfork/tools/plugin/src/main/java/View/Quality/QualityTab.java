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

import Controller.Quality.QualityTabController;
import Controller.Quality.QualityTableModel;
import View.TabLayout;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import java.util.Arrays;
import java.util.HashSet;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
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
  private final static JButton saveSettingsButton = new JButton("Save settings");
  private final JBLabel savedSettingsLabel = new JBLabel(
      "Annotation Settings are successfully saved!");

  private JBTable qualityParametersTable;
  private QualityTableModel qualityTableModel;
  private QualityTabController qualityTabController;
  private JPanel decoratorPanel;
  private JPanel saveSettingsPanel;
  private JPanel centerLabelPanel;

  public QualityTab() {
    this.setLayout(new VerticalLayout());
    setSize();
    initComponents();
    addComponents();
  }

  private void addComponents() {
    this.add(title);
    this.add(aboutQualitySettings);
    this.add(scrollPane);
    this.add(saveSettingsPanel);
    this.add(centerLabelPanel);
  }

  private void setColumnsAndRenderers() {
    HashSet<Integer> enumColumns = qualityTableModel.getEnumIndexes();
    int columnCount = qualityTableModel.getColumnCount();
    for (int i = 0; i < columnCount; i++) {
      if (enumColumns.contains(i)) {
        // TODO (targintaru) add enums from fidelity controller
        initComboBoxColumns(qualityParametersTable, i,
            Arrays.asList("option1", "option2", "option3"));
      } else {
        initTextFieldColumns(qualityParametersTable, i);
      }
    }
  }

  private void initComponents() {
    title.setFont(getMainFont());
    aboutQualitySettings.setFont(getSecondaryLabel());
    qualityTabController = new QualityTabController();

    qualityTableModel = new QualityTableModel();
    qualityParametersTable = new JBTable(qualityTableModel);
    decoratorPanel = ToolbarDecorator.createDecorator(qualityParametersTable)
        .setAddAction(anActionButton -> QualityTabController.addRow(qualityParametersTable))
        .setRemoveAction(
            anActionButton -> QualityTabController.removeRow(qualityParametersTable))
        .createPanel();
    scrollPane = new JBScrollPane();
    setDecoratorPanelSize(decoratorPanel);
    setTableSettings(scrollPane, decoratorPanel, qualityParametersTable);
    setColumnsAndRenderers();

    savedSettingsLabel.setVisible(false);
    saveSettingsPanel = new JPanel();
    saveSettingsPanel.add(saveSettingsButton);
    saveSettingsButton.addActionListener(actionEvent ->
        qualityTabController.saveSettings(qualityParametersTable, savedSettingsLabel));

    centerLabelPanel = new JPanel();
    centerLabelPanel.add(savedSettingsLabel);
  }
}
