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

import com.intellij.openapi.ui.ComboBox;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import javax.swing.Box;
import javax.swing.DefaultCellEditor;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.table.TableColumn;
import org.jdesktop.swingx.VerticalLayout;

class AnnotationTab extends TabLayout {
    private JBScrollPane scrollPane;
    private JBTable annotationTable;
    private JPanel decoratorPanel;

    private final JBLabel annotationLabel = new JBLabel("Annotation Settings");
    private final JBLabel informationLabel =
        new JBLabel("Annotation is used by tuning fork to mark the histograms being sent.");

    public AnnotationTab() {
        initVariables();
        initComponents();
    }

    private void initVariables() {
        scrollPane = new JBScrollPane();
        annotationTable = new JBTable();
        decoratorPanel =
            ToolbarDecorator.createDecorator(annotationTable)
                .setAddAction(it -> AnnotationTabController.addRowAction(annotationTable))
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

        // Initialize toolbar and table.
        AnnotationTableModel model = new AnnotationTableModel();
        annotationTable.setModel(model);
        setDecoratorPanelSize(decoratorPanel);
        setTableSettings(scrollPane, decoratorPanel, annotationTable);

        // Initialize field type column.
        TableColumn typeColumn = annotationTable.getColumnModel().getColumn(0);
        typeColumn.setCellEditor(getComboBoxModel());
        typeColumn.setCellRenderer(new TableRenderer.ComboBoxRenderer());

        // Initialize field name column.
        TableColumn nameColumn = annotationTable.getColumnModel().getColumn(1);
        nameColumn.setCellEditor(getTextFieldModel());
        nameColumn.setCellRenderer(new TableRenderer.RoundedCornerRenderer());

        this.add(scrollPane);
    }

    private DefaultCellEditor getTextFieldModel() {
        JTextField textFieldModel = new JTextField();
        textFieldModel.setBorder(new RoundedCornerBorder());
        DefaultCellEditor textEditor = new DefaultCellEditor(textFieldModel);
        textEditor.setClickCountToStart(1);
        return textEditor;
    }

    private DefaultCellEditor getComboBoxModel() {
        ComboBox<String> comboBoxModel = new ComboBox<>();
        // TODO(mohanad): This will be replaced by enum values.
        comboBoxModel.addItem("loadingState");
        comboBoxModel.addItem("Scene");
        comboBoxModel.addItem("bigBoss");
        DefaultCellEditor boxEditor = new DefaultCellEditor(comboBoxModel);
        boxEditor.setClickCountToStart(1);
        return boxEditor;
    }
}
