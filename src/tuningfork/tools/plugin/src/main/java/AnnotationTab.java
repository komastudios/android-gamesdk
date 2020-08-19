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

import com.intellij.openapi.project.Project;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import java.util.List;
import java.util.stream.Collectors;
import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JPanel;
import org.jdesktop.swingx.VerticalLayout;

class AnnotationTab extends TabLayout {
    private JBScrollPane scrollPane;
    private JBTable annotationTable;
    private JPanel decoratorPanel;
    private JPanel addEnumPanel;
    private JPanel saveSettingsPanel;
    private Project project;

    private AnnotationTabController annotationController;

    private final JBLabel annotationLabel = new JBLabel("Annotation Settings");
    private final JBLabel informationLabel =
        new JBLabel("Annotation is used by tuning fork to mark the histograms being sent.");
    private final JBLabel savedSettingsLabel = new JBLabel("Annotation Settings are successfully saved!");
    private final JButton addEnumButton = new JButton("Add enum");
    private final JButton saveSettingsButton = new JButton("Save settings");

    public AnnotationTab(Project project) {
        this.project = project;
        initVariables();
        initComponents();
    }

    private void setColumnsEditorsAndRenderers() {
        List<String> annotationNames = annotationController
            .getAnnotationEnums()
            .stream()
            .map(EnumDataModel::getName)
            .collect(Collectors.toList());
        initComboBoxColumns(annotationTable, 0, annotationNames);
        initTextFieldColumns(annotationTable, 1);
    }

    private void initVariables() {
        scrollPane = new JBScrollPane();
        annotationTable = new JBTable();
        decoratorPanel =
            ToolbarDecorator.createDecorator(annotationTable)
                .setAddAction(it -> {
                    AnnotationTabController.addRowAction(annotationTable);
                    setColumnsEditorsAndRenderers();
                })
                .setRemoveAction(it -> AnnotationTabController.removeRowAction(annotationTable))
                .createPanel();
    }

    private void initComponents() {
        // Initialize layout.
        this.setLayout(new VerticalLayout());
        setSize();

        // Add labels.
        annotationLabel.setFont(getMainFont());
        informationLabel.setFont(getSecondaryLabel());
        this.add(annotationLabel);
        this.add(Box.createVerticalStrut(10));
        this.add(informationLabel);

        annotationController = new AnnotationTabController();
        addEnumPanel = new JPanel();
        addEnumPanel.add(addEnumButton);
        this.add(addEnumPanel);
        addEnumButton.addActionListener(actionEvent -> {
            AddEnumDialogWrapper enumDialogWrapper = new AddEnumDialogWrapper(project,
                annotationController);
            enumDialogWrapper.show();
        });

        // Initialize toolbar and table.
        AnnotationTableModel model = new AnnotationTableModel();
        annotationTable.setModel(model);
        setDecoratorPanelSize(decoratorPanel);
        setTableSettings(scrollPane, decoratorPanel, annotationTable);
        setColumnsEditorsAndRenderers();

        this.add(scrollPane);

        saveSettingsPanel = new JPanel();
        saveSettingsPanel.add(saveSettingsButton);
        this.add(saveSettingsPanel);
        saveSettingsButton.addActionListener(actionEvent -> {
            annotationController.saveSettings(annotationTable, savedSettingsLabel);
        });

        JPanel settingsLabelPanel = new JPanel();
        settingsLabelPanel.add(savedSettingsLabel);
        savedSettingsLabel.setVisible(false);
        this.add(settingsLabelPanel);
    }
}