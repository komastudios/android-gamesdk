/*
 * Copyright (C) 2019 The Android Open Source Project
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
import java.awt.Dimension;
import java.awt.Font;
import java.awt.LayoutManager;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.DefaultCellEditor;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.table.TableColumn;
import org.jdesktop.swingx.VerticalLayout;

class AnnotationTab extends JPanel {
    private JBScrollPane scrollPane;
    private JBTable annotationTable;
    private JBLabel annotationLabel, informationLabel;
    private Font mainFont, secondaryLabel;
    private LayoutManager boxLayout;
    private JPanel decoratorPanel;
    private static final int PANEL_MIN_WIDTH = 600;
    private static final int PANEL_MIN_HEIGHT = 500;
    private static final int TABLE_MIN_WIDTH = 500;
    private static final int TABLE_MIN_HEIGHT = 230;
    private static final int FONT_BIG = 18;
    private static final int FONT_SMALL = 12;

    public AnnotationTab() {
        initVariables();
        initComponents();
    }

    private void initVariables() {
        scrollPane = new JBScrollPane();
        annotationTable = new JBTable();
        annotationLabel = new JBLabel("Annotation Settings");
        informationLabel =
            new JBLabel("Annotation is used by tuning fork to mark the histograms being sent.");
        mainFont = new Font(Font.SANS_SERIF, Font.BOLD, FONT_BIG);
        secondaryLabel = new Font(Font.SANS_SERIF, Font.PLAIN, FONT_SMALL);
        boxLayout = new BoxLayout(this, BoxLayout.Y_AXIS);
        decoratorPanel =
            ToolbarDecorator.createDecorator(annotationTable)
                .setAddAction(it -> AnnotationTabController.addRowAction(annotationTable))
                .setRemoveActionUpdater(e -> annotationTable.getSelectedRowCount() != 0)
                .setRemoveAction(it -> AnnotationTabController.removeRowAction(annotationTable))
                .createPanel();
    }

    private void initComponents() {
        // initialize layout
        this.setLayout(new VerticalLayout());
        this.setMinimumSize(new Dimension(PANEL_MIN_WIDTH, PANEL_MIN_HEIGHT));
        this.setPreferredSize(new Dimension(PANEL_MIN_WIDTH, PANEL_MIN_HEIGHT));

        // Add labels
        annotationLabel.setFont(mainFont);
        informationLabel.setFont(secondaryLabel);
        this.add(annotationLabel);
        this.add(Box.createVerticalStrut(10));
        this.add(informationLabel);

        // initialize toolbar and table
        AnnotationTableModel model = new AnnotationTableModel();
        annotationTable.setModel(model);
        decoratorPanel.setPreferredSize(new Dimension(TABLE_MIN_WIDTH, TABLE_MIN_HEIGHT));
        decoratorPanel.setMinimumSize(new Dimension(TABLE_MIN_WIDTH, TABLE_MIN_HEIGHT));
        annotationTable.setFillsViewportHeight(true);
        scrollPane.setViewportView(decoratorPanel);
        annotationTable.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        annotationTable.getTableHeader().setReorderingAllowed(false);
        annotationTable.setRowSelectionAllowed(true);
        annotationTable.setSelectionBackground(null);

        // initialize field type column
        TableColumn typeColumn = annotationTable.getColumnModel().getColumn(0);
        typeColumn.setCellEditor(getComboBoxModel());
        typeColumn.setCellRenderer(new TableRenderer.ComboBoxRenderer());
        typeColumn.setPreferredWidth(150);

        // initialize field name column
        TableColumn nameColumn = annotationTable.getColumnModel().getColumn(1);
        nameColumn.setCellEditor(getTextFieldModel());
        nameColumn.setCellRenderer(new TableRenderer.RoundedCornerRenderer());
        nameColumn.setPreferredWidth(350);

        // table extra feature such as dynamic size and no grid lines
        annotationTable.getModel().addTableModelListener(tableModelEvent -> resizePanelToFit());
        annotationTable.setIntercellSpacing(new Dimension(0, 0));

        this.add(scrollPane);
    }

    private void resizePanelToFit() {
        int oldWidth = scrollPane.getWidth();
        int newHeight = Math.max(decoratorPanel.getMinimumSize().height + 2,
            Math.min(AnnotationTab.this.getHeight() - 100,
                annotationTable.getRowHeight() * annotationTable.getRowCount() + 30));
        scrollPane.setSize(new Dimension(oldWidth, newHeight));
        scrollPane.revalidate();
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
        // This will be replaced by enum values
        comboBoxModel.addItem("loadingState");
        comboBoxModel.addItem("Scene");
        comboBoxModel.addItem("bigBoss");
        DefaultCellEditor boxEditor = new DefaultCellEditor(comboBoxModel);
        boxEditor.setClickCountToStart(1);
        return boxEditor;
    }
}
