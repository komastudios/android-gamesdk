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

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Random;

public class MainPanel {
    static final int BORDER = 20;


    protected JPanel getJPanel() {
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        JPanel dialogPanel = new JPanel(new BorderLayout());
        JPanel subPanelButtons = new JPanel();
        JPanel subPanelTable = new JPanel();

        JButton buttonAdd = new JButton("+");
        JButton buttonDelete = new JButton("-");
        DefaultTableModel model = new DefaultTableModel();
        model.addColumn("quality_settings (int or enum)");
        model.addColumn("lod_level (int32)");
        model.addColumn("distance (float)");
        JTable table = new JTable(model);
        JScrollPane sp = new JScrollPane(table);
        table.setPreferredScrollableViewportSize(new Dimension(screenSize.width / 2 - BORDER, screenSize.height / 3));
        subPanelTable.add(sp);

        buttonAdd.setPreferredSize(new Dimension(20, 20));
        buttonDelete.setPreferredSize(new Dimension(20, 20));
        JLabel label = new JLabel("FidelityParams");

        buttonAdd.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                DefaultTableModel model = (DefaultTableModel) table.getModel();
                // TODO(ldap): change to actual insert data
                Random rand = new Random();
                int first = rand.nextInt();
                int second = rand.nextInt();
                float third = rand.nextFloat();
                model.addRow(new Object[]{first, second, third});
            }
        });

        buttonDelete.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                DefaultTableModel model = (DefaultTableModel) table.getModel();
                int[] rows = table.getSelectedRows();
                for (int i = 0; i < rows.length; i++) {
                    model.removeRow(rows[i] - i);
                }
            }
        });

        subPanelButtons.add(buttonAdd);
        subPanelButtons.add(buttonDelete);
        dialogPanel.add(subPanelButtons, BorderLayout.SOUTH);
        dialogPanel.add(label, BorderLayout.NORTH);
        JPanel decoratorPanel = ToolbarDecorator.createDecorator(table).createPanel();
        dialogPanel.add(decoratorPanel, BorderLayout.CENTER);
        dialogPanel.setPreferredSize(new Dimension(screenSize.width / 2, screenSize.height / 2));
        return dialogPanel;
    }
}
