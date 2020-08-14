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
import com.intellij.ui.table.JBTable;
import javax.swing.table.DefaultTableModel;
import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JScrollPane;
import java.awt.Dimension;
import java.awt.BorderLayout;
import java.util.Random;
import org.jdesktop.swingx.VerticalLayout;

public class FidelityTab extends TabLayout {
    private JPanel subPanelButtons;
    private JPanel subPanelTable;
    private JButton buttonAdd;
    private JButton buttonDelete;
    private JPanel decoratorPanel;
    private JBTable fidelityTable;
    private JScrollPane scrollPane;
    private DefaultTableModel tableModel;
    private final JLabel title = new JLabel("Fidelity settings");

    FidelityTab() {
        this.setLayout(new VerticalLayout());
        setSize();
        initComponents();
        addComponents();
    }

    private void addComponents() {
        subPanelButtons.add(buttonAdd);
        subPanelButtons.add(buttonDelete);

        this.add(title, BorderLayout.NORTH);
        this.add(decoratorPanel, BorderLayout.CENTER);
        this.add(subPanelButtons, BorderLayout.SOUTH);
    }

    private void initComponents() {
        subPanelButtons = new JPanel();
        subPanelTable = new JPanel();

        buttonAdd = new JButton("+");
        buttonAdd.setPreferredSize(new Dimension(20, 20));
        buttonDelete = new JButton("-");
        buttonDelete.setPreferredSize(new Dimension(20, 20));

        tableModel = new DefaultTableModel();
        tableModel.addColumn("quality_settings (int or enum)");
        tableModel.addColumn("lod_level (int32)");
        tableModel.addColumn("distance (float)");

        fidelityTable = new JBTable(tableModel);
        scrollPane = new JScrollPane(fidelityTable);
        subPanelTable.add(scrollPane);

        title.setFont(getMainFont());

        buttonAdd.addActionListener(actionListener -> {
            DefaultTableModel model = (DefaultTableModel) fidelityTable.getModel();
            // TODO(volobushek): change to actual insert data
            Random rand = new Random();
            int first = rand.nextInt();
            int second = rand.nextInt();
            float third = rand.nextFloat();
            model.addRow(new Object[]{first, second, third});
        });

        buttonDelete.addActionListener(actionListener -> {
            DefaultTableModel model = (DefaultTableModel) fidelityTable.getModel();
            int[] rows = fidelityTable.getSelectedRows();
            for (int i = 0; i < rows.length; i++) {
                model.removeRow(rows[i] - i);
            }
        });

        decoratorPanel = ToolbarDecorator.createDecorator(fidelityTable).createPanel();
        setDecoratorPanelSize(decoratorPanel);
        setTableSettings(scrollPane, decoratorPanel, fidelityTable);
    }
}
