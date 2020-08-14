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

import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListSelectionModel;
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
  private QualityTableData qualityTableData;
  private JPanel decoratorPanel;

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
  }

  private void initComponents() {
    title.setFont(getMainFont());
    aboutQualitySettings.setFont(getSecondaryLabel());

    qualityTableModel = new QualityTableModel();
    qualityParametersTable = new JBTable(qualityTableModel);
    qualityParametersTable.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    qualityTableData = new QualityTableData();
    decoratorPanel = ToolbarDecorator.createDecorator(qualityParametersTable)
        .setAddAction(anActionButton -> qualityTableData.addRow(qualityParametersTable))
        .setRemoveAction(
            anActionButton -> qualityTableData.removeRow(qualityParametersTable))
        .createPanel();
    scrollPane = new JBScrollPane();

    setDecoratorPanelSize(decoratorPanel);
    setTableSettings(scrollPane, decoratorPanel, qualityParametersTable);
  }
}