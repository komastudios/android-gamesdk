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

package View.Decorator;

import java.awt.Dimension;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ScrollPaneConstants;
import org.jdesktop.swingx.VerticalLayout;

public class LabelScrollPane {

  private JPanel jPanel;
  private JScrollPane scrollPane;

  public LabelScrollPane(int width, int height) {
    jPanel = new JPanel(new VerticalLayout());
    scrollPane = new JScrollPane();
    Dimension dimension = new Dimension(width, height);
    scrollPane.setMaximumSize(dimension);
    scrollPane.setMinimumSize(dimension);
    scrollPane.setPreferredSize(dimension);
    scrollPane.setViewportView(jPanel);
    scrollPane
        .setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
    scrollPane
        .setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
    scrollPane.setViewportBorder(null);
    addText("Nothing To Be Shown Currently");
  }

  public void addText(String text) {
    jPanel.add(new JLabel(text));
  }

  public void removeAll() {
    jPanel.removeAll();
  }

  public void removeText(int index) {
    jPanel.remove(index);
  }

  public JScrollPane getPanel() {
    return scrollPane;
  }
}
