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
package Controller.Monitoring;

import Controller.Monitoring.MonitoringController.HistogramTree.JTreeNode;
import Controller.Monitoring.MonitoringController.HistogramTree.Node;
import Model.QualityDataModel;
import Utils.DataModelTransformer;
import Utils.Monitoring.TelemetryProcessing;
import com.google.android.performanceparameters.v1.PerformanceParameters.Telemetry;
import com.google.android.performanceparameters.v1.PerformanceParameters.UploadTelemetryRequest;
import com.google.protobuf.ByteString;
import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.FileDescriptor;
import com.google.protobuf.DynamicMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.util.ArrayList;
import java.util.Base64;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;
import javax.swing.tree.DefaultMutableTreeNode;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

public class MonitoringController {

  /*
   * Key = instrument id, Value = XYSeriesCollection.
   * Holds all the XYSeries for quality settings received so far for an instrument ID.
   * If the user wants not to plot a certain quality setting, then the collection is cloned and the specific
   * settings are not plotted.
   */
  private final HashMap<ByteString, LinkedHashMap<String, XYSeriesCollection>> seriesCollectionMap;
  /*
   * Each fidelity setting received so far has a LinkedHashMap with merged count values
   * received so far, where Key = instrument id, Value = list of merged bucket counts.
   */
  private final HashMap<ByteString, HashMap<ByteString, LinkedHashMap<String, List<Integer>>>>
      renderTimeHistograms;
  private ArrayList<Integer> indexesNotToPlot;
  private final LinkedHashSet<QualityDataModel> qualitySettingsList;
  private final PropertyChangeSupport monitoringPropertyChange;
  private int currentQualityIndex;
  private boolean isNewQuality;
  private ByteString currentFidelitySettings;
  private ByteString currentAnnotationSettings;
  private HistogramTree<List<Integer>> histogramTree;

  public MonitoringController() {
    renderTimeHistograms = new HashMap<>();
    qualitySettingsList = new LinkedHashSet<>();
    monitoringPropertyChange = new PropertyChangeSupport(this);
    seriesCollectionMap = new LinkedHashMap<>();
    indexesNotToPlot = new ArrayList<>();
    histogramTree = new HistogramTree<>();
    currentQualityIndex = 0;
    isNewQuality = false;
  }

  public int getQualityNumber() {
    return qualitySettingsList.size();
  }

  public void setIndexesNotToPlot(ArrayList<Integer> indexesNotToPlot) {
    this.indexesNotToPlot = indexesNotToPlot;
  }

  public ArrayList<Integer> getIndexesNotToPlot() {
    return indexesNotToPlot;
  }

  private int getIndexOf(QualityDataModel currentQuality) {
    int index = 0;
    for (QualityDataModel qualityDataModel : qualitySettingsList) {
      if (qualityDataModel.equals(currentQuality)) {
        return index;
      }
      index++;
    }
    return index;
  }

  private Optional<Integer> transformQualitySettings()
      throws InvalidProtocolBufferException {
    FileDescriptor devTuningForkDesc = DataModelTransformer.getDevTuningforkDesc();

    if (devTuningForkDesc == null) {
      return Optional.empty();
    }

    Descriptor fidelityParamsDesc = devTuningForkDesc.findMessageTypeByName("FidelityParams");
    DynamicMessage qualityMessage = DynamicMessage
        .parseFrom(fidelityParamsDesc, currentFidelitySettings);

    QualityDataModel newQuality = DataModelTransformer
        .transformToQuality(qualityMessage, fidelityParamsDesc.getFields()).get();

    if (!renderTimeHistograms.containsKey(currentFidelitySettings)) {
      renderTimeHistograms.put(currentFidelitySettings, new LinkedHashMap<>());
      qualitySettingsList.add(newQuality);
      monitoringPropertyChange
          .firePropertyChange("addFidelity", qualitySettingsList.size(), newQuality);
      isNewQuality = true;
    } else {
      isNewQuality = false;
      return Optional.of(getIndexOf(newQuality));
    }

    return Optional.of(qualitySettingsList.size() - 1);
  }

  public void checkAnnotationParams(UploadTelemetryRequest telemetryRequest) {
    if (telemetryRequest.getTelemetry(0) == null) {
      return;
    }
    currentAnnotationSettings = telemetryRequest.getTelemetry(0).getContext()
        .getAnnotations();
    if (!seriesCollectionMap.containsKey(currentAnnotationSettings)) {
      seriesCollectionMap.put(currentAnnotationSettings, new LinkedHashMap<>());
    }
    try {
      DynamicMessage dynamicMessage = DynamicMessage
          .parseFrom(
              DataModelTransformer.getDevTuningforkDesc().findMessageTypeByName("Annotation"),
              currentAnnotationSettings);
      monitoringPropertyChange.firePropertyChange("addAnnotation", null, dynamicMessage);
    } catch (InvalidProtocolBufferException e) {
      e.printStackTrace();
    }
    for (int i = 0; i < telemetryRequest.getTelemetryCount(); i++) {
      ByteString byteString = telemetryRequest.getTelemetry(i).getContext().getAnnotations();
      try {
        System.out.println(DynamicMessage
            .parseFrom(
                DataModelTransformer.getDevTuningforkDesc().findMessageTypeByName("Annotation"),
                byteString).toString());
      } catch (InvalidProtocolBufferException e) {
        e.printStackTrace();
      }
    }
  }

  private void makeTree(UploadTelemetryRequest telemetryRequest) {
    String deviceName = telemetryRequest.getSessionContext().getDevice().getBrand() + "/" +
        telemetryRequest.getSessionContext().getDevice().getModel();
    Node<List<Integer>> deviceNode = new Node<>(deviceName);
    for (Telemetry telemetry : telemetryRequest.getTelemetryList()) {
      ByteString annotationByteString = telemetry.getContext().getAnnotations();
      ByteString fidelityByteString = telemetry.getContext().getTuningParameters()
          .getSerializedFidelityParameters();
      Node<List<Integer>> annotation = new Node<>(
          Base64.getEncoder().encodeToString(annotationByteString.toByteArray()), "HEY");
      Node<List<Integer>> fidelity = new Node<>(
          Base64.getEncoder().encodeToString(fidelityByteString.toByteArray()), "BLEP");
      annotation.addChild(fidelity);
      deviceNode.addChild(annotation);
    }
    histogramTree.addPath(deviceNode);
  }

  public JTreeNode getTree() {
    return histogramTree.getAsTreeNode();
  }

  public void checkFidelityParams(UploadTelemetryRequest telemetryRequest)
      throws InvalidProtocolBufferException {
    if (telemetryRequest.getTelemetry(0) == null) {
      return;
    }
    makeTree(telemetryRequest);
    currentFidelitySettings = telemetryRequest.getTelemetry(0).getContext()
        .getTuningParameters().getSerializedFidelityParameters();

    transformQualitySettings().ifPresent(index -> currentQualityIndex = index);
  }

  public void setRenderTimeHistograms(UploadTelemetryRequest telemetryRequest) {
    if (!renderTimeHistograms.containsKey(currentAnnotationSettings)) {
      renderTimeHistograms.put(currentAnnotationSettings, new HashMap<>());
    }
    renderTimeHistograms.get(currentAnnotationSettings).put(currentFidelitySettings,
        TelemetryProcessing.processTelemetryData(telemetryRequest));
  }

  public Set<String> getRenderTimeHistogramsKeys() {
    return renderTimeHistograms.get(currentAnnotationSettings).get(currentFidelitySettings)
        .keySet();
  }

  public void addMonitoringPropertyChangeListener(PropertyChangeListener propertyChangeListener) {
    monitoringPropertyChange.addPropertyChangeListener(propertyChangeListener);
  }

  /*
   * Replots only a certain series from the XYseriescollection, and maintains the order of the
   * other XY series.
   */
  private void replotCertainSeries(String instrumentID, int currentQualityIndex,
      XYSeries histogramDataset) {
    XYSeriesCollection seriesToModify = new XYSeriesCollection();
    XYSeriesCollection originalSeries = seriesCollectionMap.get(currentAnnotationSettings)
        .get(instrumentID);

    for (int i = 0; i < currentQualityIndex; i++) {
      seriesToModify.addSeries(originalSeries.getSeries(i));
    }
    seriesToModify.addSeries(histogramDataset);
    for (int i = currentQualityIndex + 1; i < qualitySettingsList.size(); i++) {
      seriesToModify.addSeries(originalSeries.getSeries(i));
    }

    seriesCollectionMap.get(currentAnnotationSettings).put(instrumentID, seriesToModify);
  }

  public void createChartPanels() {
    for (Map.Entry<String, List<Integer>> entry : renderTimeHistograms
        .get(currentAnnotationSettings).get(currentFidelitySettings)
        .entrySet()) {
      String instrumentId = entry.getKey();
      XYSeries histogramDataset = new XYSeries("Quality settings " + (currentQualityIndex + 1));
      List<Integer> fpsList = entry.getValue();
      for (int i = 0; i < fpsList.size(); i++) {
        histogramDataset.add(i, fpsList.get(i));
      }

      if (!seriesCollectionMap.get(currentAnnotationSettings).containsKey(instrumentId)) {
        seriesCollectionMap.get(currentAnnotationSettings)
            .put(instrumentId, new XYSeriesCollection());
      }

      if (!isNewQuality) {
        replotCertainSeries(instrumentId, currentQualityIndex, histogramDataset);
      } else {
        seriesCollectionMap.get(currentAnnotationSettings).get(instrumentId)
            .addSeries(histogramDataset);
      }
    }
    removeQualitySettingsNotToPlot();
  }

  public void removeQualitySettingsNotToPlot() {
    for (Map.Entry<String, XYSeriesCollection> entry : seriesCollectionMap
        .get(currentAnnotationSettings).entrySet()) {
      try {
        XYSeriesCollection datasets = (XYSeriesCollection) entry.getValue().clone();

        for (int pos = 0; pos < indexesNotToPlot.size(); pos++) {
          datasets.removeSeries(indexesNotToPlot.get(pos) - pos);
        }

        JFreeChart histogram = ChartFactory.createXYBarChart("Render Time Histogram",
            "Frame Time", false, "Count", datasets,
            PlotOrientation.VERTICAL, true, false, false);

        ChartPanel chartPanel = new ChartPanel(histogram);
        monitoringPropertyChange.firePropertyChange("addChart", entry.getKey(), chartPanel);
      } catch (CloneNotSupportedException e) {
        e.printStackTrace();
      }
    }
  }

  static final class HistogramTree<T> {

    private final Node<T> root = new Node<>("");

    public HistogramTree() {
    }

    public void addPath(Node<T> node) {
      root.findAndAddChild(node);
      for (Node<T> childNode : node.childrenNodes) {
        addPath(childNode, node);
      }
    }

    public void addPath(Node<T> node, Node<T> parentNode) {
      parentNode.findAndAddChild(node);
      for (Node<T> childNode : node.childrenNodes) {
        addPath(childNode, node);
      }
    }

    public JTreeNode getAsTreeNode() {
      return getAsTreeNode(root, new JTreeNode("", ""));
    }

    public JTreeNode getAsTreeNode(Node<T> currentNode, JTreeNode treeNode) {
      for (Node<T> childNode : currentNode.childrenNodes) {
        JTreeNode childTreeNode = new JTreeNode(childNode.nodeName, currentNode.nodeToolTip);
        treeNode.add(childTreeNode);
        getAsTreeNode(childNode, childTreeNode);
      }
      return treeNode;
    }

    public static final class JTreeNode extends DefaultMutableTreeNode {

      private final String shortDesc, longDesc;

      public JTreeNode(String shortDesc, String longDesc) {
        super();
        this.shortDesc = shortDesc;
        this.longDesc = longDesc;
      }

      public String getToolTip() {
        return longDesc;
      }

      @Override
      public String toString() {
        return shortDesc;
      }
    }

    public static final class Node<V> {

      private final List<Node<V>> childrenNodes;
      private final String nodeName;
      private String nodeToolTip;
      private Optional<V> value;

      public Node(String nodeName) {
        this.childrenNodes = new ArrayList<>();
        this.nodeName = nodeName;
        this.nodeToolTip = "";
      }

      public Node(String nodeName, String nodeToolTip) {
        this(nodeName);
        this.nodeToolTip = nodeToolTip;
      }

      public V getValue() {
        return this.value.orElse(null);
      }

      public void setValue(V value) {
        this.value = Optional.ofNullable(value);
      }

      public Node<V> findAndAddChild(Node<V> nodeToFind) {
        Optional<Node<V>> foundNode = childrenNodes.stream()
            .filter(node -> node.equals(nodeToFind))
            .findFirst();
        if (foundNode.isPresent()) {
          return foundNode.get();
        } else {
          addChild(nodeToFind);
        }
        return nodeToFind;
      }

      public void addChild(Node<V> node) {
        childrenNodes.add(node);
      }

      @Override
      public boolean equals(Object o) {
        if (this == o) {
          return true;
        }
        if (o == null || getClass() != o.getClass()) {
          return false;
        }
        return Objects.equals(nodeName, ((Node<?>) o).nodeName);
      }

      @Override
      public int hashCode() {
        return Objects.hash(nodeName);
      }
    }
  }

}
