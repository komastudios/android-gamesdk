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

import Controller.Monitoring.MonitoringController.HistogramTree;
import Controller.Monitoring.MonitoringController.HistogramTree.Node;
import Utils.Resources.ResourceLoader;
import View.Monitoring.MonitoringTab.JTreeNode;
import View.Monitoring.MonitoringTab.TreeToolTipRenderer;
import com.intellij.icons.AllIcons.Actions;
import com.intellij.openapi.actionSystem.AnActionEvent;
import com.intellij.openapi.ui.DialogWrapper;
import com.intellij.ui.AnActionButton;
import com.intellij.ui.ToolbarDecorator;
import java.awt.Dimension;
import java.awt.event.ItemEvent;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.Vector;
import java.util.function.Consumer;
import java.util.function.Supplier;
import java.util.stream.Collectors;
import javax.swing.Box;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import org.jdesktop.swingx.VerticalLayout;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

public class MonitoringFilterDialogWrapper extends DialogWrapper {

  private final HistogramTree histogramTree;

  public MonitoringFilterDialogWrapper(HistogramTree tree) {
    super(true);
    histogramTree = tree;
    setTitle("Histograms Filter");
    init();
  }

  @Nullable
  @Override
  protected JComponent createCenterPanel() {
    return new FilterPanel();
  }

  @Override
  protected void doOKAction() {
    super.doOKAction();
  }

  private final class FilterPanel extends JPanel {

    private final ResourceLoader resourceLoader = ResourceLoader.getInstance();
    private JTree annotationsTree;
    private JTree fidelityTree;
    private JPanel annotationDecorator, fidelityDecorator;
    private JComboBox<Node> phoneModelComboBox;
    private List<JTreeNode> selectedAnnotationNodes, selectedFidelityNodes;

    public FilterPanel() {
      super(new VerticalLayout());
      selectedAnnotationNodes = new ArrayList<>();
      selectedFidelityNodes = new ArrayList<>();
      initComponent();
      addComponents();
    }

    private void addComponents() {
      add(new JLabel(resourceLoader.get("filter_phone_model")));
      add(phoneModelComboBox);
      add(new JLabel(resourceLoader.get("filter_annotation")));
      add(annotationDecorator);
      add(Box.createVerticalStrut(10));
      add(new JLabel(resourceLoader.get("filter_fidelity")));
      add(fidelityDecorator);
      annotationDecorator.setPreferredSize(new Dimension(150, 200));
      fidelityDecorator.setPreferredSize(new Dimension(150, 200));
    }

    private void initComponent() {
      annotationsTree = new JTree(new DefaultMutableTreeNode());
      annotationsTree.setRootVisible(false);
      annotationsTree.setCellRenderer(new TreeToolTipRenderer());
      annotationDecorator = ToolbarDecorator.createDecorator(annotationsTree)
          .addExtraAction(getAnnotationTreeButton()).createPanel();
      fidelityTree = new JTree(new DefaultMutableTreeNode());
      fidelityTree.setRootVisible(false);
      fidelityTree.setCellRenderer(new TreeToolTipRenderer());
      fidelityDecorator = ToolbarDecorator.createDecorator(fidelityTree)
          .addExtraAction(getFidelityTreeButton()).createPanel();
      phoneModelComboBox = new JComboBox<>(new Vector<>(histogramTree.getAllPhoneModels()));
      Node selectedItem = (Node) phoneModelComboBox.getSelectedItem();
      if (selectedItem != null) {
        setAnnotations(selectedItem);
      }
      phoneModelComboBox.addItemListener(event -> {
        if (event.getStateChange() == ItemEvent.SELECTED) {
          Node chosenNode = (Node) phoneModelComboBox.getSelectedItem();
          setAnnotations(chosenNode);
        }
      });
    }

    private void setAnnotations(Node chosenModel) {
      List<Node> annotations = histogramTree.getAllAnnotations(chosenModel);
      JTreeNode rootNode = new JTreeNode();
      for (Node annotationNode : annotations) {
        JTreeNode jTreeNode = new JTreeNode(annotationNode);
        if (selectedAnnotationNodes.contains(jTreeNode)) {
          jTreeNode.setNodeState(true);
        }
        rootNode.add(jTreeNode);
      }
      ((DefaultTreeModel) annotationsTree.getModel()).setRoot(rootNode);
      selectedFidelityNodes.clear();
      updateFidelityTree();
    }

    private void updateFidelityTree() {
      List<Node> chosenAnnotationNodes = selectedAnnotationNodes.stream()
          .map(jTreeNode -> histogramTree.findNodeByName(jTreeNode.getNodeName())).collect(
              Collectors.toList());
      Set<Node> fidelityNodes = histogramTree.getAllFidelity(chosenAnnotationNodes);
      JTreeNode rootNode = new JTreeNode();
      for (Node fidelityNode : fidelityNodes) {
        JTreeNode jTreeNode = new JTreeNode(fidelityNode);
        if (selectedFidelityNodes.contains(jTreeNode)) {
          jTreeNode.setNodeState(true);
        }
        rootNode.add(jTreeNode);
      }
      ((DefaultTreeModel) fidelityTree.getModel()).setRoot(rootNode);
    }

    private SelectAction getAnnotationTreeButton() {
      return new SelectAction(annotationsTree, () -> annotationsTree.getSelectionPath() != null,
          lastNode -> {
            Optional<JTreeNode> nodeExist = selectedAnnotationNodes.stream()
                .filter(node -> node.getNodeName().equals(lastNode.getNodeName())).findFirst();
            if (nodeExist.isPresent()) {
              selectedAnnotationNodes.remove(nodeExist.get());
            } else {
              selectedAnnotationNodes.add(lastNode);
            }
            setAnnotations((Node) phoneModelComboBox.getSelectedItem());
          });
    }

    private SelectAction getFidelityTreeButton() {
      return new SelectAction(fidelityTree, () -> fidelityTree.getSelectionPath() != null,
          lastNode -> {
            Optional<JTreeNode> nodeExist = selectedFidelityNodes.stream()
                .filter(node -> node.getNodeName().equals(lastNode.getNodeName())).findFirst();
            if (nodeExist.isPresent()) {
              selectedFidelityNodes.remove(nodeExist.get());
            } else {
              selectedFidelityNodes.add(lastNode);
            }
            updateFidelityTree();
          });
    }

    private class SelectAction extends AnActionButton {

      private final JTree jTree;
      private final Consumer<JTreeNode> consumer;
      private final Supplier<Boolean> supplier;

      public SelectAction(JTree jTree, Supplier<Boolean> update, Consumer<JTreeNode> consumer) {
        super(resourceLoader.get("toggle"), Actions.SetDefault);
        this.jTree = jTree;
        this.supplier = update;
        this.consumer = consumer;
      }

      @Override
      public void updateButton(@NotNull AnActionEvent e) {
        e.getPresentation().setEnabled(supplier.get());
      }

      @Override
      public void actionPerformed(@NotNull AnActionEvent anActionEvent) {
        TreePath selectedPath = jTree.getSelectionPath();
        if (selectedPath == null) {
          return;
        }
        JTreeNode lastNode = (JTreeNode) selectedPath.getLastPathComponent();
        consumer.accept(lastNode);
      }
    }

  }
}
