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
package View.Monitoring;

import Controller.Monitoring.HistogramTree.Node;
import Controller.Monitoring.MonitoringController;
import Model.MonitorFilterModel;
import Utils.Monitoring.RequestServer;
import Utils.Resources.ResourceLoader;
import Utils.UI.UIUtils;
import View.Decorator.TreeSelections.FirstNodeSelection;
import View.Dialog.MonitoringFilterDialogWrapper;
import View.TabLayout;
import com.google.android.performanceparameters.v1.PerformanceParameters.UploadTelemetryRequest;
import com.google.gson.annotations.Expose;
import com.intellij.icons.AllIcons.Actions;
import com.intellij.openapi.ui.ComboBox;
import com.intellij.ui.ToolbarDecorator;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.treeStructure.Tree;
import com.intellij.util.ui.UIUtil;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.Vector;
import java.util.concurrent.TimeUnit;
import java.util.function.Consumer;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTree;
import javax.swing.SwingUtilities;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;
import org.jdesktop.swingx.HorizontalLayout;
import org.jdesktop.swingx.VerticalLayout;
import org.jfree.chart.ChartPanel;

/*
 * The Logic of the monitoring goes as follow
 * 1-A new UploadTelemetry report is sent to the plugin
 * 2-Loading thread is interrupted
 * 3-The telemetry report is checked for the fidelity byte string. If it exists then isNewQuality = false
 * else isNewQuality = true and addFidelity property is fired
 * 4-Device specification is set
 * 5-Histograms are merged together. For each instrument key and the same fidelity parameters. they get merged
 * 6-Combobox is renewed with the unique instrument keys that exists so far.
 * 7-UI is refreshed by deleting all the old charts and making new charts
 * 8-Each chart represent a single instrument key. each chart will have data for all different quality
 * at the beginning. but the "Not selected quality" is deleted later on. But XYSeries originally saves
 * all quality settings data and deletes the one it doesn't need on plotting.
 */
public class MonitoringTab extends TabLayout implements PropertyChangeListener {

  private final static ResourceLoader RESOURCE_LOADER = ResourceLoader.getInstance();
  private final static String LOADING_TEXT = RESOURCE_LOADER.get("retrieving_histograms");
  private final JLabel warningLabel = new JLabel(RESOURCE_LOADER.get("monitoring_info"));
  private final JButton startMonitoring = new JButton(RESOURCE_LOADER.get("start_monitoring"));
  private final JButton loadReport = new JButton(RESOURCE_LOADER.get("load_report"));
  private final JButton saveReport = new JButton(RESOURCE_LOADER.get("save_report"));

  private static final Dimension SCREEN_SIZE = Toolkit.getDefaultToolkit().getScreenSize();
  private final Dimension chartSize = new Dimension(SCREEN_SIZE.width / 4,
      SCREEN_SIZE.height / 5);
  private final JButton stopMonitoring = new JButton(RESOURCE_LOADER.get("stop_monitoring"));
  private JPanel retrievedInformationPanel;
  private JPanel loadingPanel;
  private JPanel graphPanel;

  private JComboBox<String> instrumentIDComboBox;
  private ArrayList<ChartPanel> histogramsGraphPanels = new ArrayList<>();
  private MonitoringController controller;
  private Thread loadingAnimationThread;
  private JLabel loadingLabel;
  private JPanel beforeLoadingButtonsPanel, afterLoadingButtonsPanel;
  private Tree dataTree, filterTree;
  private JPanel dataPanel;
  private JPanel filterPanel;
  private final Consumer<UploadTelemetryRequest> requestConsumer = uploadTelemetryRequest ->
      SwingUtilities.invokeLater(() -> setMonitoringTabData(uploadTelemetryRequest));

  public MonitoringTab() {
    this.setLayout(new VerticalLayout());
    setSize();
    initComponents();
    addComponents();
  }

  private void addComboBoxData(Set<String> renderTimeHistogramsKeys) {
    List<String> uniqueIDs = new ArrayList<>(renderTimeHistogramsKeys);
    Vector<String> uniqueIdVector = new Vector<>(uniqueIDs);
    uniqueIdVector.add(0, RESOURCE_LOADER.get("instrument_id"));

    // Preserve the selected item on combo box update
    String selectedObject = null;
    if (instrumentIDComboBox.getSelectedItem() != null && uniqueIDs
        .contains(instrumentIDComboBox.getSelectedItem().toString())) {
      selectedObject = (String) instrumentIDComboBox.getSelectedItem();
    }
    instrumentIDComboBox.setModel(new DefaultComboBoxModel<>(uniqueIdVector));
    if (selectedObject != null) {
      instrumentIDComboBox.setSelectedItem(selectedObject);
    }
  }

  // Deleting all the old ChartPanels used to render histograms
  private void deleteExistingGraphs() {
    for (ChartPanel panel : histogramsGraphPanels) {
      graphPanel.remove(panel);
    }
    histogramsGraphPanels = new ArrayList<>();
  }

  private void plotData() {
    deleteExistingGraphs();
    addComboBoxData(controller.prepareFiltering());
    UIUtils.reloadTreeAndKeepState(dataTree, controller.getTree());
    UIUtils.reloadTreeAndKeepState(filterTree, controller.getFilterTree());
    graphPanel.revalidate();
  }

  // Hide all other panels except the one at index
  private void changePanelVisibility(int index) {
    for (int i = 0; i < histogramsGraphPanels.size(); i++) {
      histogramsGraphPanels.get(i).setVisible(i == index);
    }
  }

  private void refreshUI() {
    plotData();
    SwingUtilities.updateComponentTreeUI(retrievedInformationPanel);
    filterTree.setBackground(UIUtil.getWindowColor());
    dataTree.setBackground(UIUtil.getWindowColor());
  }

  public void setMonitoringTabData(UploadTelemetryRequest telemetryRequest) {
    if (!loadingAnimationThread.isInterrupted()) {
      loadingAnimationThread.interrupt();
    }
    onDataReceived();
    controller.makeTree(telemetryRequest);
    refreshUI();
  }


  private void addComponents() {
    this.add(Box.createVerticalStrut(10));
    this.add(warningLabel);
    beforeLoadingButtonsPanel = new JPanel();
    beforeLoadingButtonsPanel.add(startMonitoring);
    beforeLoadingButtonsPanel.add(loadReport);
    this.add(beforeLoadingButtonsPanel);
    afterLoadingButtonsPanel = new JPanel();
    afterLoadingButtonsPanel.add(stopMonitoring);
    afterLoadingButtonsPanel.add(saveReport);
    this.add(afterLoadingButtonsPanel);

    loadingPanel.add(Box.createVerticalStrut(30));
    loadingPanel.add(loadingLabel);
    this.add(loadingPanel);

    JPanel horizontalPanel = new JPanel(new GridLayout(1, 2));
    horizontalPanel.add(dataPanel);
    horizontalPanel.add(filterPanel);
    dataPanel.setPreferredSize(new Dimension(dataPanel.getPreferredSize().width, 190));
    filterPanel.setPreferredSize(new Dimension(filterPanel.getPreferredSize().width, 180));
    retrievedInformationPanel.add(horizontalPanel);
    retrievedInformationPanel.add(instrumentIDComboBox);
    retrievedInformationPanel.add(graphPanel);
    this.add(retrievedInformationPanel);
    retrievedInformationPanel.setVisible(false);
    afterLoadingButtonsPanel.setVisible(false);
    filterTree.setOpaque(true);
    dataTree.setOpaque(true);
    System.out.println(filterPanel.getBackground().toString());
    filterTree.setBackground(filterPanel.getBackground());
    System.out.println(filterTree.getBackground().toString());
    dataTree.setBackground(dataPanel.getBackground());
  }

  private void sleepUI() throws InterruptedException {
    TimeUnit.SECONDS.sleep(1);
  }

  /*
   * Show loading panel and the new buttons
   */
  private void onStartMonitoring() {
    startMonitoring();
    controller.clearData();
    initLoadingAnimationThread();
    loadingPanel.setVisible(true);
    beforeLoadingButtonsPanel.setVisible(false);
    afterLoadingButtonsPanel.setVisible(true);
  }

  /*
   * Hide the loading panel and show the beforeLoading buttons and hide histograms
   */
  private void onStopMonitoring() {
    stopMonitoring();
    if (loadingAnimationThread != null && !loadingAnimationThread.isInterrupted()) {
      loadingAnimationThread.interrupt();
    }
    loadingPanel.setVisible(false);
    beforeLoadingButtonsPanel.setVisible(true);
    afterLoadingButtonsPanel.setVisible(false);
    retrievedInformationPanel.setVisible(false);
  }

  private void onDataReceived() {
    loadingPanel.setVisible(false);
    beforeLoadingButtonsPanel.setVisible(false);
    afterLoadingButtonsPanel.setVisible(true);
    retrievedInformationPanel.setVisible(true);

  }

  private void startMonitoring() {
    RequestServer.getInstance().setMonitoringAction(requestConsumer);
  }

  private void stopMonitoring() {
    RequestServer.getInstance().setMonitoringAction(null);
  }

  private void onLoadReport() {
    stopMonitoring();
    if (controller.loadReport()) {
      refreshUI();
      beforeLoadingButtonsPanel.setVisible(false);
      afterLoadingButtonsPanel.setVisible(true);
      retrievedInformationPanel.setVisible(true);
    } // Silently ignore invalid report or user not choosing anything.
  }

  private void addFilter() {
    MonitoringFilterDialogWrapper filterDialog = new MonitoringFilterDialogWrapper(
        controller.getHistogramTree());
    filterDialog.addPropertyChangeListener(this);
    if (filterDialog.showAndGet()) {
      UIUtils.reloadTreeAndKeepState(filterTree, controller.getFilterTree());
      refreshUI();
    }
  }

  private void removeFilter() {
    TreePath path = filterTree.getSelectionPath();
    if (path == null) {
      return;
    }
    JTreeNode leafNode = (JTreeNode) path.getLastPathComponent();
    JTreeNode leafParent = (JTreeNode) leafNode.getParent();
    int filterIndex = leafParent.getIndex(leafNode);
    controller.removeFilter(filterIndex);
    UIUtils.reloadTreeAndKeepState(filterTree, controller.getFilterTree());
    refreshUI();
  }

  private void initLoadingAnimationThread() {
    loadingAnimationThread = new Thread(() -> {
      try {
        while (!Thread.interrupted()) {
          SwingUtilities.invokeLater(() -> loadingLabel.setText(LOADING_TEXT));
          sleepUI();
          SwingUtilities.invokeLater(() -> loadingLabel.setText(LOADING_TEXT + " ."));
          sleepUI();
          SwingUtilities.invokeLater(() -> loadingLabel.setText(LOADING_TEXT + " . ."));
          sleepUI();
          SwingUtilities.invokeLater(() -> loadingLabel.setText(LOADING_TEXT + " . . ."));
          sleepUI();
        }
      } catch (InterruptedException e) {
        // Silently Ignore Interruption exception and let thread exit
      }
    });
    loadingAnimationThread.start();
  }

  private void initComponents() {
    controller = new MonitoringController();
    controller.addMonitoringPropertyChangeListener(this);
    warningLabel.setFont(new Font(Font.SANS_SERIF, Font.ITALIC, 12));

    instrumentIDComboBox = new ComboBox<>();
    DefaultComboBoxModel<String> comboBoxModel = new DefaultComboBoxModel<>();
    comboBoxModel.addElement(RESOURCE_LOADER.get("instrument_id"));
    instrumentIDComboBox.setModel(comboBoxModel);
    instrumentIDComboBox.addActionListener(actionEvent -> {
      if (instrumentIDComboBox.getSelectedIndex() == 0) {
        return;
      }
      changePanelVisibility(instrumentIDComboBox.getSelectedIndex() - 1);
    });

    startMonitoring.addActionListener(actionEvent -> onStartMonitoring());
    loadReport.addActionListener(actionEvent -> onLoadReport());
    stopMonitoring.addActionListener(actionEvent -> onStopMonitoring());

    filterTree = new Tree(new DefaultMutableTreeNode());
    filterTree.setSelectionModel(new FirstNodeSelection());
    filterTree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
    filterTree.setRootVisible(false);
    filterTree.setCellRenderer(new TreeToolTipRenderer());
    filterPanel = ToolbarDecorator.createDecorator(filterTree)
        .setAddAction(anActionButton -> addFilter())
        .setRemoveAction(anActionButton -> removeFilter())
        .createPanel();
    filterPanel.setBorder(BorderFactory.createTitledBorder(RESOURCE_LOADER.get("filters")));
    dataTree = new Tree(new DefaultMutableTreeNode());
    dataTree.setRootVisible(false);
    dataTree.setCellRenderer(new TreeToolTipRenderer());
    dataPanel = ToolbarDecorator.createDecorator(dataTree).createPanel();
    dataPanel.setBorder(BorderFactory.createTitledBorder(RESOURCE_LOADER.get("data_collected")));
    loadingPanel = new JPanel();
    loadingLabel = new JBLabel();

    retrievedInformationPanel = new JPanel(new VerticalLayout());
    graphPanel = new JPanel(new VerticalLayout());
    graphPanel.setMaximumSize(new Dimension(500, 200));
    graphPanel.revalidate();
    saveReport.addActionListener(actionEvent -> controller.saveReport());
  }

  @Override
  public void propertyChange(PropertyChangeEvent propertyChangeEvent) {
    switch (propertyChangeEvent.getPropertyName()) {
      case "addChart":
        ChartPanel chartPanel = (ChartPanel) propertyChangeEvent.getNewValue();
        chartPanel.setMaximumSize(chartSize);
        chartPanel.setMinimumSize(chartSize);
        chartPanel.setPreferredSize(chartSize);

        String instrumentID = (String) propertyChangeEvent.getOldValue();
        chartPanel.setVisible(instrumentIDComboBox.getSelectedItem().equals(instrumentID));

        graphPanel.add(chartPanel);
        histogramsGraphPanels.add(chartPanel);
        break;
      case "addFilter":
        MonitorFilterModel model = (MonitorFilterModel) propertyChangeEvent.getNewValue();
        controller.addFilter(model);
        refreshUI();
    }
  }

  public static class TreeToolTipRenderer extends DefaultTreeCellRenderer {

    JPanel panel;
    JLabel nameLabel, iconLabel;

    public TreeToolTipRenderer() {
      panel = new JPanel(new HorizontalLayout());
      nameLabel = new JLabel();
      iconLabel = new JLabel();
      panel.add(nameLabel);
      panel.add(iconLabel);
      setOpenIcon(null);
      setClosedIcon(null);
      setBackgroundSelectionColor(null);
      setLeafIcon(null);
    }

    @Override
    public Component getTreeCellRendererComponent(JTree tree, Object value, boolean sel,
        boolean expanded, boolean leaf, int row, boolean hasFocus) {
      if (value instanceof JTreeNode) {
        JTreeNode jTreeNode = (JTreeNode) value;
        panel.setToolTipText(jTreeNode.getToolTipDesc());
        nameLabel.setText(jTreeNode.getNodeName());
        iconLabel.setIcon((jTreeNode.getSelectedState() ? Actions.SetDefault : null));
        return panel;
      }
      return super.getTreeCellRendererComponent(tree, value, sel, expanded, leaf, row, hasFocus);
    }
  }

  public static final class JTreeNode extends DefaultMutableTreeNode {

    @Expose
    private final String nodeName, toolTipDesc;
    @Expose
    private boolean selectedState;

    public JTreeNode() {
      super();
      this.nodeName = "";
      this.toolTipDesc = "";
      this.selectedState = false;
    }

    public JTreeNode(String nodeName) {
      super();
      this.nodeName = nodeName;
      this.toolTipDesc = "";
      this.selectedState = false;
    }

    public JTreeNode(Node node) {
      super();
      this.nodeName = node.getNodeName();
      this.toolTipDesc = node.getNodeToolTip();
      this.selectedState = node.getNodeState();
    }

    public String getNodeName() {
      return nodeName;
    }

    public String getToolTipDesc() {
      return toolTipDesc;
    }

    public boolean getSelectedState() {
      return selectedState;
    }

    public void setNodeState(boolean state) {
      this.selectedState = state;
    }

    public Node asNode() {
      return new Node(getNodeName(), getToolTipDesc());
    }

    @Override
    public String toString() {
      return nodeName;
    }

    @Override
    public boolean equals(Object o) {
      if (this == o) {
        return true;
      }
      if (o == null || getClass() != o.getClass()) {
        return false;
      }
      JTreeNode jTreeNode = (JTreeNode) o;
      return this.nodeName.equals(jTreeNode.getNodeName());
    }

    @Override
    public int hashCode() {
      return Objects.hashCode(nodeName);
    }
  }
}
