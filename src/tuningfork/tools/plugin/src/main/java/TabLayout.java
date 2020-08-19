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

import java.awt.Dimension;
import java.awt.Font;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;

public class TabLayout extends JPanel {

  private static final int PANEL_MIN_WIDTH = 600;
  private static final int PANEL_MIN_HEIGHT = 500;
  private static final int TABLE_MIN_WIDTH = 500;
  private static final int TABLE_MIN_HEIGHT = 230;
  private static final int FONT_BIG = 18;
  private static final int FONT_SMALL = 12;
  private static final Font MAIN_FONT = new Font(Font.SANS_SERIF, Font.BOLD, FONT_BIG);
  private static final Font SECONDARY_FONT = new Font(Font.SANS_SERIF, Font.PLAIN, FONT_SMALL);

  public static Font getMainFont() {
    return MAIN_FONT;
  }

  public static Font getSecondaryLabel() {
    return SECONDARY_FONT;
  }

  public void setDecoratorPanelSize(JPanel decoratorPanel) {
    decoratorPanel.setPreferredSize(new Dimension(TABLE_MIN_WIDTH, TABLE_MIN_HEIGHT));
    decoratorPanel.setMinimumSize(new Dimension(TABLE_MIN_WIDTH, TABLE_MIN_HEIGHT));
  }

  public void setSize() {
    this.setMinimumSize(new Dimension(PANEL_MIN_WIDTH, PANEL_MIN_HEIGHT));
    this.setPreferredSize(new Dimension(PANEL_MIN_WIDTH, PANEL_MIN_HEIGHT));
  }

  /*
   * Dynamically resizes when current row number is greater than initial height.
   */
  private void resizePanelToFit(JScrollPane scrollPane, JPanel decoratorPanel, JTable table) {
    int oldWidth = scrollPane.getWidth();
    int newHeight = Math.max(decoratorPanel.getMinimumSize().height + 2,
        Math.min(this.getHeight() - 100,
            table.getRowHeight() * table.getRowCount() + 30));
    scrollPane.setSize(new Dimension(oldWidth, newHeight));
    scrollPane.revalidate();
  }

  public void setTableSettings(JScrollPane scrollPane, JPanel decoratorPanel, JTable table) {
    scrollPane.setViewportView(decoratorPanel);
    table.setFillsViewportHeight(true);
    table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    table.getTableHeader().setReorderingAllowed(false);
    table.setRowSelectionAllowed(true);
    table.setSelectionBackground(null);
    table.getModel().addTableModelListener(
        tableModelEvent -> resizePanelToFit(scrollPane, decoratorPanel, table));
    table.setIntercellSpacing(new Dimension(0, 0));
  }
}
