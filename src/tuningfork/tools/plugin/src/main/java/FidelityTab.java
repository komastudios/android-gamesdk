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
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBScrollPane;
import com.intellij.ui.table.JBTable;
import javax.swing.JPanel;
import javax.swing.Box;
import org.jdesktop.swingx.VerticalLayout;

public class FidelityTab extends TabLayout {
    private JBScrollPane scrollPane;
    private JBTable fidelityTable;
    private JPanel decoratorPanel;

    private final JBLabel fidelityLabel = new JBLabel("Fidelity Settings");
    private final JBLabel informationLabel = new JBLabel("Fidelity parameters settings info.");

    public FidelityTab() {
        initVariables();
        initComponents();
    }

    private void initVariables() {
        scrollPane = new JBScrollPane();
        fidelityTable = new JBTable();
        decoratorPanel =
                ToolbarDecorator.createDecorator(fidelityTable)
                        .setAddAction(it -> FidelityTabController.addRowAction(fidelityTable))
                        .setRemoveAction(it -> FidelityTabController.removeRowAction(fidelityTable))
                        .createPanel();
    }

    private void initComponents() {
        this.setLayout(new VerticalLayout());
        setSize();
        fidelityLabel.setFont(getMainFont());
        informationLabel.setFont(getSecondaryLabel());
        this.add(fidelityLabel);
        this.add(Box.createVerticalStrut(10));
        this.add(informationLabel);
        FidelityTableModel model = new FidelityTableModel();
        fidelityTable.setModel(model);
        setDecoratorPanelSize(decoratorPanel);
        setTableSettings(scrollPane, decoratorPanel, fidelityTable);
        this.add(scrollPane);
    }
}