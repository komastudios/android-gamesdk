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

package View;

import Controller.Annotation.AnnotationTabController;
import Controller.Fidelity.FidelityTabController;
import Controller.FidelityChanger.FidelityChangerController;
import Controller.InstrumentationSettings.InstrumentationSettingsTabController;
import Controller.Quality.QualityTabController;
import Model.EnumDataModel;
import Model.MessageDataModel;
import Model.QualityDataModel;
import Utils.Resources.ResourceLoader;
import View.Annotation.AnnotationTab;
import View.DebugInfo.debugInfoTab;
import View.Fidelity.FidelityTab;
import View.FidelityChanger.FidelityChanger;
import View.InstrumentationSettings.InstrumentationSettingsTab;
import View.Monitoring.MonitoringTab;
import View.Quality.QualityTab;
import com.google.tuningfork.Tuningfork.Settings;
import com.intellij.openapi.Disposable;
import com.intellij.openapi.ui.OnePixelDivider;
import com.intellij.ui.treeStructure.Tree;
import com.intellij.util.ui.UIUtil;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Toolkit;
import java.util.ArrayList;
import java.util.List;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.JPanel;
import javax.swing.JTree;
import javax.swing.UIManager;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeSelectionModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;
import org.jdesktop.swingx.HorizontalLayout;

public class PluginLayout extends JPanel {

  private QualityTab qualityLayout;
  private AnnotationTab annotationsLayout;
  private FidelityTab fidelitySettingsLayout;
  private InstrumentationSettingsTab instrumentationSettingsTab;
  private MonitoringTab monitoringTab;
  private FidelityChanger fidelityChanger;
  private debugInfoTab debugInfoTab;
  private InstrumentationSettingsTabController instrumentationSettingsTabController;
  private JPanel menuPanel;
  private JTree menu;
  private ArrayList<JPanel> panels;

  private final ResourceLoader resourceLoader = ResourceLoader.getInstance();
  private static final Dimension SCREEN_SIZE = Toolkit.getDefaultToolkit().getScreenSize();
  private final MessageDataModel annotationData;
  private final MessageDataModel fidelityData;
  private final List<EnumDataModel> enumData;
  private final List<QualityDataModel> qualityData;
  private final Dimension panelSize = new Dimension(5 * SCREEN_SIZE.width / 6,
      SCREEN_SIZE.height / 2);
  private final Dimension menuSize = new Dimension(SCREEN_SIZE.width / 6, SCREEN_SIZE.height / 2);

  private final Disposable disposable;

  public PluginLayout(MessageDataModel annotationData, MessageDataModel fidelityData,
      List<EnumDataModel> enumData, List<QualityDataModel> qualityData, Disposable disposable) {
    this.annotationData = annotationData;
    this.qualityData = qualityData;
    this.enumData = enumData;
    this.fidelityData = fidelityData;
    this.disposable = disposable;
    panels = new ArrayList<>();
    fidelityData.setEnumData(enumData);
    this.setPreferredSize(new Dimension(SCREEN_SIZE.width / 2, SCREEN_SIZE.height / 2));
    this.setLayout(new HorizontalLayout());
    initComponents();
    addComponents();
  }

  public boolean isViewValid() {
    return annotationsLayout.isViewValid() && fidelitySettingsLayout.isViewValid() && qualityLayout
        .isViewValid();
  }

  private void addComponents() {
    this.add(menuPanel);
    this.add(Box.createHorizontalStrut(10));
    this.add(fidelitySettingsLayout);
    this.add(annotationsLayout);
    this.add(qualityLayout);
    this.add(instrumentationSettingsTab);
    this.add(monitoringTab);
    this.add(fidelityChanger);
    this.add(debugInfoTab);
  }

  private void changeLayoutVisibility(JPanel toSetVisible) {
    for (JPanel panel : panels) {
      panel.setVisible(panel.equals(toSetVisible));
    }
  }

  private void initMenuTree() {
    DefaultMutableTreeNode settingsRoot = new DefaultMutableTreeNode(
        resourceLoader.get("settings"));
    DefaultMutableTreeNode annotationsNode = new DefaultMutableTreeNode(
        resourceLoader.get("annotation_settings"));
    DefaultMutableTreeNode fidelityNode = new DefaultMutableTreeNode(
        resourceLoader.get("fidelity_settings"));
    DefaultMutableTreeNode qualitySettingsNode = new DefaultMutableTreeNode(
        resourceLoader.get("quality_settings"));

    DefaultMutableTreeNode validationSettings = new DefaultMutableTreeNode(
        resourceLoader.get("instrumentation_settings"));

    settingsRoot.add(annotationsNode);
    settingsRoot.add(fidelityNode);
    settingsRoot.add(qualitySettingsNode);
    settingsRoot.add(validationSettings);

    DefaultMutableTreeNode root = new DefaultMutableTreeNode(resourceLoader.get("preference"));
    DefaultMutableTreeNode monitoringRoot = new DefaultMutableTreeNode(
        resourceLoader.get("monitoring"));
    DefaultMutableTreeNode telemetryNode =
        new DefaultMutableTreeNode(resourceLoader.get("telemetry_report"));
    DefaultMutableTreeNode debugInfo =
        new DefaultMutableTreeNode(resourceLoader.get("debug_info"));
    DefaultMutableTreeNode experimentalNode = new DefaultMutableTreeNode(
        resourceLoader.get("fidelity_changer"));
    monitoringRoot.add(telemetryNode);
    monitoringRoot.add(debugInfo);
    monitoringRoot.add(experimentalNode);
    root.add(settingsRoot);
    root.add(monitoringRoot);

    UIManager.put("Tree.rendererFillBackground", false);
    menu = new Tree(root);
    menu.setCellRenderer(new CustomCellRenderer());
    menu.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
    menu.setRootVisible(false);
    menu.addTreeSelectionListener(treeSelectionEvent -> {
      if (treeSelectionEvent.getNewLeadSelectionPath() != null) {
        DefaultMutableTreeNode node = (DefaultMutableTreeNode)
            treeSelectionEvent.getNewLeadSelectionPath().getLastPathComponent();
        if (node.equals(annotationsNode)) {
          changeLayoutVisibility(annotationsLayout);
        } else if (node.equals(fidelityNode)) {
          changeLayoutVisibility(fidelitySettingsLayout);
        } else if (node.equals(qualitySettingsNode)) {
          changeLayoutVisibility(qualityLayout);
        } else if (node.equals(validationSettings)) {
          changeLayoutVisibility(instrumentationSettingsTab);
        } else if (node.equals(telemetryNode)) {
          changeLayoutVisibility(monitoringTab);
        } else if (node.equals(experimentalNode)) {
          changeLayoutVisibility(fidelityChanger);
        } else if (node.equals(debugInfo)) {
          changeLayoutVisibility(debugInfoTab);
        } else if (!node.isLeaf()) {
          menu.setMinimumSize(menuSize);
        }
      }
    });
    menu.setMinimumSize(menuSize);
    menu.setPreferredSize(new Dimension(180, this.getPreferredSize().height));
    menu.setSelectionModel(new LeafOnlySelectionModel());
    menuPanel.setSize(menuSize);
    menuPanel.add(menu);
    menuPanel.setBackground(UIUtil.SIDE_PANEL_BACKGROUND);
    menuPanel.setBorder(BorderFactory.createLineBorder(OnePixelDivider.BACKGROUND));
  }

  private void initComponents() {
    // Annotation Initialization.
    AnnotationTabController annotationTabController = new AnnotationTabController(annotationData,
        enumData);
    annotationsLayout = new AnnotationTab(annotationTabController, disposable);
    annotationsLayout.setSize(new Dimension(5 * SCREEN_SIZE.width / 6, SCREEN_SIZE.height / 2));
    annotationsLayout.setVisible(true);

    // Fidelity initialization.
    FidelityTabController fidelityTabController = new FidelityTabController(fidelityData,
        annotationTabController.getEnums());
    fidelitySettingsLayout = new FidelityTab(fidelityTabController, disposable);
    annotationTabController.addPropertyChangeListener(
        fidelitySettingsLayout);
    fidelitySettingsLayout.setVisible(false);
    fidelitySettingsLayout.setSize(panelSize);

    // Quality Setting Initialization.
    QualityTabController qualityTabController = new QualityTabController(qualityData,
        fidelityData, enumData);
    qualityLayout = new QualityTab(qualityTabController, disposable);
    fidelityTabController.addPropertyChangeListener(qualityLayout);
    annotationTabController.addPropertyChangeListener(qualityLayout);
    qualityLayout.setSize(panelSize);
    qualityLayout.setVisible(false);

    // Validation settings initialization.
    instrumentationSettingsTabController = new InstrumentationSettingsTabController(
        Settings.newBuilder().build());
    instrumentationSettingsTab = new InstrumentationSettingsTab(
        instrumentationSettingsTabController);
    instrumentationSettingsTab.setSize(panelSize);
    instrumentationSettingsTab.setVisible(false);

    // Monitoring initialization.
    monitoringTab = new MonitoringTab();
    monitoringTab.setSize(panelSize);
    monitoringTab.setVisible(false);

    //Fidelity Changer initialization
    fidelityChanger = new FidelityChanger(new FidelityChangerController(qualityData));
    fidelityChanger.setVisible(false);

    //Debug Info initialization.
    debugInfoTab = new debugInfoTab();
    debugInfoTab.setVisible(false);
    menuPanel = new JPanel();

    panels.add(qualityLayout);
    panels.add(annotationsLayout);
    panels.add(fidelitySettingsLayout);
    panels.add(instrumentationSettingsTab);
    panels.add(monitoringTab);
    panels.add(fidelityChanger);
    panels.add(debugInfoTab);
    initMenuTree();
  }

  static class CustomCellRenderer extends DefaultTreeCellRenderer {

    CustomCellRenderer() {
      setLeafIcon(null);
      setClosedIcon(null);
      setOpenIcon(null);
    }

    @Override
    public Component getTreeCellRendererComponent(
        JTree jTree,
        Object value,
        boolean selected,
        boolean expanded,
        boolean leaf,
        int row,
        boolean hasFocus) {
      Component cell =
          super.getTreeCellRendererComponent(jTree, value, selected, expanded, leaf, row, hasFocus);

      if (!leaf) {
        cell.setFont(new Font("Roboto", Font.BOLD, 13));
      } else {
        cell.setFont(new Font("Roboto", Font.PLAIN, 12));
      }

      return cell;
    }
  }

  static class LeafOnlySelectionModel extends DefaultTreeSelectionModel {

    private TreePath[] getLeafs(TreePath[] fullPaths) {
      ArrayList<TreePath> paths = new ArrayList<>();

      for (TreePath fullPath : fullPaths) {
        if (((DefaultMutableTreeNode) fullPath.getLastPathComponent()).isLeaf()) {
          paths.add(fullPath);
        }
      }

      return paths.toArray(fullPaths);
    }

    @Override
    public void setSelectionPaths(TreePath[] treePaths) {
      super.setSelectionPaths(getLeafs(treePaths));
    }

    @Override
    public void addSelectionPaths(TreePath[] treePaths) {
      super.addSelectionPaths(getLeafs(treePaths));
    }
  }

  public FidelityTabController getFidelityTabController() {
    return fidelitySettingsLayout.getFidelityTabController();
  }

  public AnnotationTabController getAnnotationTabController() {
    return annotationsLayout.getAnnotationController();
  }

  public InstrumentationSettingsTabController getInstrumentationSettingsTabController() {
    return instrumentationSettingsTab.getInstrumentationSettingsTabController();
  }

  public void saveSettings() {
    fidelitySettingsLayout.saveSettings();
    annotationsLayout.saveSettings();
    instrumentationSettingsTab.saveSettings();
    qualityLayout.saveSettings();
  }

  public QualityTabController getQualityTabController() {
    return qualityLayout.getQualityTabController();
  }
}
