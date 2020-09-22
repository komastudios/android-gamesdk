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
package View.Dialog;

import com.intellij.openapi.ui.DialogWrapper;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.util.ArrayList;
import java.util.stream.Collectors;
import java.util.stream.IntStream;
import javax.swing.BorderFactory;
import javax.swing.JCheckBox;
import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ScrollPaneConstants;
import org.jdesktop.swingx.VerticalLayout;
import org.jetbrains.annotations.Nullable;

public class MonitoringQualityDialogWrapper extends DialogWrapper {

  private ArrayList<JCheckBox> checkBoxList;
  private JPanel checkBoxPanel;
  private JScrollPane scrollPane;
  private static final Dimension SCREEN_SIZE = Toolkit.getDefaultToolkit().getScreenSize();
  private final Dimension scrollPaneSize = new Dimension(SCREEN_SIZE.width / 4,
      SCREEN_SIZE.height / 5);
  private PropertyChangeSupport saveQualitySupport;
  private int qualityDatasetsSize;
  private ArrayList<Integer> indexesNotToPlot;

  private void initCheckBoxListData(int qualityDatasetsSize, ArrayList<Integer> indexesNotToPlot) {
    for (int i = 1; i <= qualityDatasetsSize; i++) {
      JCheckBox checkBox = new JCheckBox("Quality settings " + i);
      if (indexesNotToPlot.contains(i - 1)) {
        checkBox.setSelected(false);
      } else {
        checkBox.setSelected(true);
      }
      checkBoxList.add(checkBox);
      checkBoxPanel.add(checkBox);
    }
  }

  public MonitoringQualityDialogWrapper(int qualityDatasetsSize, ArrayList<Integer> indexesNotToPlot) {
    super(true);
    setTitle("Plotted quality");
    saveQualitySupport = new PropertyChangeSupport(this);
    checkBoxList = new ArrayList<>();
    this.qualityDatasetsSize = qualityDatasetsSize;
    this.indexesNotToPlot = indexesNotToPlot;
    init();
  }

  public void addPropertyChangeListener(PropertyChangeListener listener) {
    saveQualitySupport.addPropertyChangeListener(listener);
  }

  @Override
  protected void doOKAction() {
    ArrayList<Integer> notSelectedIndexes = (ArrayList<Integer>) IntStream.range(0, checkBoxList.size())
        .filter(i -> !checkBoxList.get(i).isSelected()).boxed().collect(
            Collectors.toList());
    saveQualitySupport.firePropertyChange("plotSelectedQuality", null, notSelectedIndexes);
    super.doOKAction();
  }

  @Nullable
  @Override
  protected JComponent createCenterPanel() {
    JPanel mainPanel = new JPanel(new VerticalLayout());
    checkBoxPanel = new JPanel(new VerticalLayout());
    scrollPane = new JScrollPane();
    scrollPane.setViewportView(checkBoxPanel);
    scrollPane.setPreferredSize(scrollPaneSize);
    scrollPane.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
    scrollPane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
    scrollPane.setBorder(BorderFactory.createEmptyBorder());
    initCheckBoxListData(qualityDatasetsSize, indexesNotToPlot);
    mainPanel.add(scrollPane);
    return mainPanel;
  }
}
