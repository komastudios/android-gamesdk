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

package View.InstrumentationSettings;

import Controller.InstrumentationSettings.InstrumentationSettingsTabController;
import Controller.InstrumentationSettings.InstrumentationSettingsTableModel;
import View.Decorator.RoundedCornerBorder;
import View.Decorator.RoundedCornerBorder.BorderType;
import View.TabLayout;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.table.JBTable;
import java.awt.Dimension;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.Hashtable;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.ButtonGroup;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JSlider;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import org.jdesktop.swingx.HorizontalLayout;
import org.jdesktop.swingx.VerticalLayout;
import org.jdesktop.swingx.prompt.PromptSupport;
import org.jdesktop.swingx.prompt.PromptSupport.FocusBehavior;

public class InstrumentationSettingsTab extends TabLayout {

  private JBTable validationSettingsTable;
  private JPanel decoratorPanel;

  private JRadioButton radioButtonTimeBased;
  private JRadioButton radioButtonIntervalBased;
  private ButtonGroup radioButtonGroup;
  private final InstrumentationSettingsTabController instrumentationSettingsTabController;
  private JTextField apiKey;
  private JSlider intervalSlider;
  private final JBLabel validationSettingsLabel = new JBLabel("Settings parameters");
  private final JBLabel informationLabel = new JBLabel(
      "<html> Choose your preferred settings for game analysis. <br>" +
          "Histograms:</html>");

  public InstrumentationSettingsTab(InstrumentationSettingsTabController controller) {
    this.instrumentationSettingsTabController = controller;
    initVariables();
    initComponents();
    addComponents();
  }

  private void addComponents() {
    this.add(validationSettingsLabel);
    this.add(Box.createVerticalStrut(10));
    this.add(informationLabel);
    this.add(decoratorPanel);
    this.add(Box.createVerticalStrut(10));
    this.add(apiKey);
    this.add(Box.createVerticalStrut(14));
    JPanel aggregationPanel = new JPanel(new VerticalLayout());
    aggregationPanel.setBorder(BorderFactory.createTitledBorder("Aggregation Strategy"));
    JPanel radioButtonsPanel = new JPanel(new HorizontalLayout());
    radioButtonsPanel.add(radioButtonTimeBased);
    radioButtonsPanel.add(Box.createHorizontalStrut(20));
    radioButtonsPanel.add(radioButtonIntervalBased);
    aggregationPanel.add(radioButtonsPanel);
    aggregationPanel.add(intervalSlider);
    this.add(aggregationPanel);
  }

  private void initVariables() {
    validationSettingsTable = new JBTable();
    radioButtonGroup = new ButtonGroup();
    radioButtonTimeBased = new JRadioButton("Time Based");
    radioButtonIntervalBased = new JRadioButton("Tick Based");
    intervalSlider = new JSlider(JSlider.HORIZONTAL);
    apiKey = new JTextField();
    apiKey.setBorder(new RoundedCornerBorder(BorderType.NORMAL));
    PromptSupport.setPrompt("Api Key", apiKey);
    PromptSupport.setFocusBehavior(FocusBehavior.HIDE_PROMPT, apiKey);
    decoratorPanel =
        ToolbarDecorator.createDecorator(validationSettingsTable)
            .setAddAction(it ->
                InstrumentationSettingsTabController.addRowAction(validationSettingsTable))
            .setRemoveAction(it ->
                InstrumentationSettingsTabController.removeRowAction(validationSettingsTable))
            .createPanel();
  }

  private void initComponents() {
    this.setLayout(new VerticalLayout());
    setSize();
    validationSettingsLabel.setFont(getMainFont());
    informationLabel.setFont(getSecondaryFont());
    InstrumentationSettingsTableModel model = new InstrumentationSettingsTableModel(
        instrumentationSettingsTabController);
    validationSettingsTable.setModel(model);
    setDecoratorPanelSize(decoratorPanel);
    setTableSettings(validationSettingsTable);
    radioButtonGroup.add(radioButtonIntervalBased);
    radioButtonGroup.add(radioButtonTimeBased);
    radioButtonTimeBased.addItemListener(new TimeRadioButtonsChange());
    radioButtonIntervalBased.addItemListener(new TickRadioButtonsChange());
    for (int i = 0; i < validationSettingsTable.getModel().getColumnCount(); i++) {
      initTextFieldColumns(validationSettingsTable, i);
    }
  }

  private void setTableSettings(JTable table) {
    table.setFillsViewportHeight(true);
    table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    table.getTableHeader().setReorderingAllowed(false);
    table.setRowSelectionAllowed(true);
    table.setSelectionBackground(null);
    table.setSelectionForeground(null);
    table.setIntercellSpacing(new Dimension(0, 0));
  }

  public InstrumentationSettingsTabController getInstrumentationSettingsTabController() {
    return instrumentationSettingsTabController;
  }

  private final class TimeRadioButtonsChange implements ItemListener {

    @Override
    public void itemStateChanged(ItemEvent e) {
      if (e.getStateChange() == ItemEvent.SELECTED) {
        intervalSlider.setMajorTickSpacing(6);
        intervalSlider.setMinorTickSpacing(1);
        intervalSlider.setMinimum(0);
        intervalSlider.setMaximum(60);
        intervalSlider.setPaintTicks(true);
        intervalSlider.setPaintLabels(true);
        intervalSlider.setSnapToTicks(true);
        intervalSlider.setValue(30);
        Hashtable<Integer, JLabel> labelTable = new Hashtable<>();
        labelTable.put(0, new JLabel("0"));
        labelTable.put(30, new JLabel("5 Minutes"));
        labelTable.put(60, new JLabel("10 Minutes"));
        intervalSlider.setLabelTable(labelTable);

      }
    }
  }

  private final class TickRadioButtonsChange implements ItemListener {

    @Override
    public void itemStateChanged(ItemEvent e) {
      if (e.getStateChange() == ItemEvent.SELECTED) {
        intervalSlider.setMajorTickSpacing(1000);
        intervalSlider.setMinorTickSpacing(100);
        intervalSlider.setMinimum(0);
        intervalSlider.setMaximum(6000);
        intervalSlider.setPaintTicks(true);
        intervalSlider.setPaintLabels(true);
        intervalSlider.setSnapToTicks(true);
        intervalSlider.setValue(3600);
        Hashtable<Integer, JLabel> labelTable = new Hashtable<>();
        labelTable.put(0, new JLabel("0"));
        labelTable.put(3000, new JLabel("3000 Tick"));
        labelTable.put(6000, new JLabel("6000 Tick"));
        intervalSlider.setLabelTable(labelTable);
      }
    }
  }

  public void saveSettings() {
    instrumentationSettingsTabController.setApiKey(apiKey.getText());
    if (radioButtonTimeBased.isSelected()) {
      int timeInMillisecond = intervalSlider.getValue() * 10 * 1000;
      instrumentationSettingsTabController.setUploadInterval(timeInMillisecond);
      instrumentationSettingsTabController
          .setAggregationMethod(radioButtonTimeBased.getText());
    } else if (radioButtonIntervalBased.isSelected()) {
      instrumentationSettingsTabController.setUploadInterval(intervalSlider.getValue());
      instrumentationSettingsTabController
          .setAggregationMethod(radioButtonIntervalBased.getText());
    }
    validationSettingsTable.clearSelection();
  }
}