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

package View.ValidationSettings;

import Controller.Annotation.AnnotationTabController;
import Controller.ValidationSettings.ValidationSettingsTabController;
import Controller.ValidationSettings.ValidationSettingsTableModel;
import Model.EnumDataModel;
import Model.MessageDataModel;
import View.Annotation.AnnotationTab;
import View.TabLayout;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;

import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.JProgressBar;
import javax.swing.Box;

import org.jdesktop.swingx.VerticalLayout;

import java.util.List;

public class ValidationSettingsTab extends TabLayout {
    private JBScrollPane scrollPane;
    private JBTable validationSettingsTable;
    private JPanel decoratorPanel;

    private JRadioButton radioButtonTimeBased;
    private JRadioButton radioButtonIntervalBased;
    private ButtonGroup radioButtonGroup;
    private ValidationSettingsTabController validationSettingsTabController;
    private List<EnumDataModel> enumDataModelList;

    private JButton saveSettingsButton;
    private JPanel saveSettingsPanel;

    private JTextField apiKey;
    private JBLabel intervals = new JBLabel("intervalms_or_count");
    private JTextField intervalms_or_count;

    private JProgressBar barUpload;

    private final JBLabel validationSettingsLabel = new JBLabel("Settings parameters");
    private final JBLabel informationLabel = new JBLabel(
            "<html> Choose your preferred settings for game analysis. <br>" +
            "Histograms:</html>");

    public ValidationSettingsTab(List<EnumDataModel> enumDataModelList) {
        initVariables(enumDataModelList);
        initComponents();
    }

    private void initVariables(List<EnumDataModel> enumDataModelList) {
        scrollPane = new JBScrollPane();
        validationSettingsTable = new JBTable();
        radioButtonGroup = new ButtonGroup();
        radioButtonTimeBased = new JRadioButton("TIME BASED");
        radioButtonIntervalBased = new JRadioButton("INTERVAL BASED");
        barUpload = new JProgressBar(0, 200);
        barUpload.setBounds(40,40,160,30);
        barUpload.setValue(0);
        barUpload.setStringPainted(true);
        this.enumDataModelList = enumDataModelList;
        barUpload.setSize(250, 150);
        apiKey = new JTextField("api key");
        intervalms_or_count = new JTextField("1000");
        saveSettingsButton = new JButton("Save settings");
        validationSettingsTabController = new ValidationSettingsTabController(enumDataModelList);
        decoratorPanel =
                ToolbarDecorator.createDecorator(validationSettingsTable)
                        .setAddAction(it ->
                                ValidationSettingsTabController.addRowAction(validationSettingsTable))
                        .setRemoveAction(it ->
                                ValidationSettingsTabController.removeRowAction(validationSettingsTable))
                        .createPanel();
    }

    private void initComponents() {
        this.setLayout(new VerticalLayout());
        setSize();

        validationSettingsLabel.setFont(getMainFont());
        informationLabel.setFont(getSecondaryLabel());
        this.add(validationSettingsLabel);
        this.add(Box.createVerticalStrut(10));
        this.add(informationLabel);
        ValidationSettingsTableModel model = new ValidationSettingsTableModel();
        validationSettingsTable.setModel(model);
        setDecoratorPanelSize(decoratorPanel);
        setTableSettings(scrollPane, decoratorPanel, validationSettingsTable);
        this.add(scrollPane);
        radioButtonGroup.add(radioButtonIntervalBased);
        radioButtonGroup.add(radioButtonTimeBased);
        this.add(radioButtonTimeBased);
        this.add(radioButtonIntervalBased);
        this.add(apiKey);
        this.add(intervals);
        this.add(intervalms_or_count);
        this.add(barUpload);
        saveSettingsPanel = new JPanel();
        saveSettingsPanel.add(saveSettingsButton);
        saveSettingsButton.addActionListener(actionEvent ->
                validationSettingsTabController.saveSettings(validationSettingsTable, apiKey.getText(),
                        radioButtonTimeBased.isSelected() ?
                                radioButtonTimeBased.getText(): radioButtonIntervalBased.getText(),
                        intervalms_or_count.getText()));
        this.add(saveSettingsPanel);

        // this function will change
        tickExample(barUpload);
    }

    public void tickExample(JProgressBar barUpload) {
        int i = 0;
        while (i <= 200){
            barUpload.setValue(i);
            i += 20;
            try {
                Thread.sleep(10);
            } catch(Exception e){}
        }
    }

}