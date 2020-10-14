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

import Controller.Monitoring.MonitoringController.HistogramTree.Node;
import Model.MonitorFilterModel;
import Model.QualityDataModel;
import Utils.DataModelTransformer;
import Utils.Monitoring.TelemetryProcessing;
import View.Monitoring.MonitoringTab.JTreeNode;
import com.google.android.performanceparameters.v1.PerformanceParameters.Telemetry;
import com.google.android.performanceparameters.v1.PerformanceParameters.UploadTelemetryRequest;
import com.google.protobuf.ByteString;
import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.FileDescriptor;
import com.google.protobuf.DynamicMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import com.intellij.util.ui.UIUtil;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;
import javax.swing.tree.TreePath;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
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
  private HistogramTree histogramTree;
  private Map<ByteString, Integer> annotationMap, fidelityMap;
  private List<MonitorFilterModel> filterModels;

  public MonitoringController() {
    renderTimeHistograms = new HashMap<>();
    qualitySettingsList = new LinkedHashSet<>();
    monitoringPropertyChange = new PropertyChangeSupport(this);
    seriesCollectionMap = new LinkedHashMap<>();
    indexesNotToPlot = new ArrayList<>();
    histogramTree = new HistogramTree();
    currentQualityIndex = 0;
    isNewQuality = false;
    annotationMap = new HashMap<>();
    fidelityMap = new HashMap<>();
    filterModels = new ArrayList<>();
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
  }

  private String getAnnotation(ByteString bytes) {
    try {
      DynamicMessage dynamicMessage = DynamicMessage
          .parseFrom(
              DataModelTransformer.getDevTuningforkDesc().findMessageTypeByName("Annotation"),
              bytes);
      return splitMessageIntoHTML(dynamicMessage.toString());
    } catch (InvalidProtocolBufferException e) {
      e.printStackTrace();
    }
    return "";
  }

  private String getFidelity(ByteString bytes) {
    try {
      Descriptor descriptor = DataModelTransformer.getDevTuningforkDesc()
          .findMessageTypeByName("FidelityParams");
      DynamicMessage dynamicMessage = DynamicMessage.parseFrom(descriptor, bytes);
      QualityDataModel newQuality = DataModelTransformer
          .transformToQuality(dynamicMessage, descriptor.getFields()).get();
      return splitMessageIntoHTML(newQuality.toString());
    } catch (InvalidProtocolBufferException e) {
      e.printStackTrace();
    }
    return "";
  }

  private String getFidelityNodeName(ByteString bytes) {
    if (fidelityMap.containsKey(bytes)) {
      return "Fidelity " + fidelityMap.get(bytes);
    }
    int sz = fidelityMap.size() + 1;
    fidelityMap.put(bytes, sz);
    return getFidelityNodeName(bytes);
  }

  private String getAnnotationNodeName(ByteString bytes) {
    if (annotationMap.containsKey(bytes)) {
      return "Annotation " + annotationMap.get(bytes);
    }
    // Starting at 1-indexed counting.
    int sz = annotationMap.size() + 1;
    annotationMap.put(bytes, sz);
    return getAnnotationNodeName(bytes);
  }

  private String splitMessageIntoHTML(String message) {
    String[] stringChunks = message.split("\n");
    StringBuilder newMessage = new StringBuilder("<html>");
    for (String chunk : stringChunks) {
      newMessage.append(chunk).append("<br>");
    }
    newMessage.append("</html>");
    return newMessage.toString();
  }

  public HistogramTree getHistogramTree() {
    return histogramTree;
  }

  private void makeTree(UploadTelemetryRequest telemetryRequest) {
    String deviceName = telemetryRequest.getSessionContext().getDevice().getBrand() + "/" +
        telemetryRequest.getSessionContext().getDevice().getModel();
    Node deviceNode = new Node(deviceName);
    for (Telemetry telemetry : telemetryRequest.getTelemetryList()) {
      ByteString annotationByteString = telemetry.getContext().getAnnotations();
      ByteString fidelityByteString = telemetry.getContext().getTuningParameters()
          .getSerializedFidelityParameters();

      Node annotation = new Node(getAnnotationNodeName(annotationByteString),
          getAnnotation(annotationByteString));
      Node fidelity = new Node(getFidelityNodeName(fidelityByteString),
          getFidelity(fidelityByteString));
      annotation.addChild(fidelity);
      deviceNode.addChild(annotation);
      telemetry.getReport().getRendering().getRenderTimeHistogramList().forEach(
          histogram -> {
            Node instrumentNode = new Node(String.valueOf(histogram.getInstrumentId()));
            instrumentNode.setValue(histogram.getCountsList());
            fidelity.addChild(instrumentNode);
          }
      );
    }
    histogramTree.addPath(deviceNode);
  }

  public JTreeNode getTree() {
    return histogramTree.getAsTreeNode();
  }

  public void addFilter(MonitorFilterModel model) {
    filterModels.add(model);
  }

  public Set<String> prepareFiltering() {
    List<Map<String, List<Integer>>> allModelsHistograms = new ArrayList<>();
    filterModels.forEach(model -> allModelsHistograms.add(getModelHistogram(model)));
    Set<String> allInstrumentKeys = getAllKeys(allModelsHistograms);
    for (String key : allInstrumentKeys) {
      XYSeriesCollection collection = new XYSeriesCollection();
      for (int i = 0; i < allModelsHistograms.size(); i++) {
        Map<String, List<Integer>> filterData = allModelsHistograms.get(i);
        List<Integer> histogram = filterData.getOrDefault(key, new ArrayList<>());
        collection.addSeries(makeSeries(histogram, i));
      }
      JFreeChart histogram = ChartFactory.createXYBarChart("Render Time Histogram",
          "Frame Time(ms)", false, "Count", collection,
          PlotOrientation.VERTICAL, true, false, false);
      ChartPanel chartPanel = new ChartPanel(histogram);
      setTheme(histogram, chartPanel);
      monitoringPropertyChange.firePropertyChange("addChart", key, chartPanel);
    }
    return allInstrumentKeys;
  }

  private void setTheme(JFreeChart jChart, ChartPanel chartPanel) {
    chartPanel.getChart().setBackgroundPaint(UIUtil.getWindowColor());
    jChart.getPlot().setBackgroundPaint(UIUtil.getWindowColor());
    ((XYPlot) jChart.getPlot()).setDomainGridlinePaint(UIUtil.getTextAreaForeground());
    ((XYPlot) jChart.getPlot()).setRangeGridlinePaint(UIUtil.getTextAreaForeground());
    ((XYPlot) jChart.getPlot()).getDomainAxis().setTickLabelPaint(UIUtil.getTextAreaForeground());
    ((XYPlot) jChart.getPlot()).getRangeAxis().setTickLabelPaint(UIUtil.getTextAreaForeground());
    ((XYPlot) jChart.getPlot()).getDomainAxis().setLabelPaint(UIUtil.getTextAreaForeground());
    ((XYPlot) jChart.getPlot()).getRangeAxis().setLabelPaint(UIUtil.getTextAreaForeground());
    jChart.getLegend().setBackgroundPaint(UIUtil.getWindowColor());
    jChart.getLegend().setItemPaint(UIUtil.getTextAreaForeground());
  }

  private XYSeries makeSeries(List<Integer> seriesData, int filterNumber) {
    XYSeries xySeries = new XYSeries(
        String.format("Filter %d", filterNumber + 1));
    int totalSum = seriesData.stream().mapToInt(val -> val).sum();
    for (int i = 0; i < seriesData.size(); i++) {
      xySeries.add(i, (1.00 * seriesData.get(i)) / (double) (totalSum));
    }
    return xySeries;
  }

  private Set<String> getAllKeys(List<Map<String, List<Integer>>> allModels) {
    Set<String> keys = new HashSet<>();
    allModels.forEach(mp -> keys.addAll(mp.keySet()));
    return keys;
  }

  /*
   * This method returns a List<Map<String, List<Integer> >, the outer list represents fidelity
   * while Each Map<String, List<> > Represents a different chosen fidelity,
   * and the map entry set is the merged instrument keys
   * So if you choose [Annotation1, Annotation2], Fidelity 1, then The list size will be just 1
   * and Map will have all instrument key, and each instrument key represent the merging of
   * Annotation1-fidelity1 + Annotation2-fidelity1
   */
  private Map<String, List<Integer>> getModelHistogram(MonitorFilterModel model) {
    Node phoneNode = histogramTree.findNodeByName(model.getPhoneModel());
    String fidelity = model.getFidelity();
    Map<String, List<Integer>> singleFidelityBucket = new HashMap<>();
    model.getAnnotations().forEach(annotation -> {
      Node annotationNode = histogramTree.findNodeByName(annotation, phoneNode);
      Map<String, List<Integer>> buckets = histogramTree
          .getFidelityCount(annotationNode, fidelity);
      mergeMaps(singleFidelityBucket, buckets);
    });
    return singleFidelityBucket;
  }

  private void mergeMaps(Map<String, List<Integer>> mainMap, Map<String, List<Integer>> toMerge) {
    toMerge.forEach((key, value) -> mainMap.merge(key, value, (v1, v2) -> {
      List<Integer> mergedList = new ArrayList<>();
      for (int i = 0; i < v1.size(); i++) {
        mergedList.add(v1.get(i) + v2.get(i));
      }
      return mergedList;
    }));
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


      } catch (CloneNotSupportedException e) {
        e.printStackTrace();
      }
    }
  }


  public static final class HistogramTree {

    private Node root = new Node("");

    public HistogramTree() {
    }

    public void addPath(Node node) {
      addPath(node, root);
    }

    private void addPath(Node currentNode, Node parentNode) {
      Optional<Node> foundNode = parentNode.findChild(currentNode);
      List<Node> children = currentNode.childrenNodes;
      currentNode = currentNode.getNoChildrenCopy();
      if (foundNode.isPresent()) {
        mergeNodeValue(currentNode, foundNode.get());
        currentNode = foundNode.get();
      } else {
        parentNode.addChild(currentNode);
      }
      for (Node child : children) {
        addPath(child, currentNode);
      }
    }

    // Merges the new node into the old node
    private void mergeNodeValue(Node newNode, Node oldNode) {
      List<Integer> newValue = newNode.getValue();
      List<Integer> oldValue = oldNode.getValue();
      for (int i = 0; i < oldValue.size(); i++) {
        oldValue.set(i, oldValue.get(i) + newValue.get(i));
      }
    }

    public Node findNodeByName(String name) {
      return findNodeByName(root, name);
    }

    public Node findNodeByName(String name, Node startNode) {
      return findNodeByName(startNode, name);
    }

    private Node findNodeByName(Node currentNode, String name) {
      if (currentNode.getNodeName().equals(name)) {
        return currentNode;
      }
      Node foundNode;
      for (Node childNode : currentNode.childrenNodes) {
        if ((foundNode = findNodeByName(childNode, name)) != null) {
          return foundNode;
        }
      }
      return null;
    }

    public JTreeNode getAsTreeNode(int maxDepth) {
      return getAsTreeNode(root, new JTreeNode(), maxDepth, 0);
    }

    public JTreeNode getAsTreeNode() {
      return getAsTreeNode(root, new JTreeNode(), Integer.MAX_VALUE, 0);
    }

    public JTreeNode getAsTreeNode(Node currentNode, JTreeNode treeNode, int maxDepth,
        int currentDepth) {
      if (currentDepth >= maxDepth) {
        return treeNode;
      }
      for (Node childNode : currentNode.childrenNodes) {
        JTreeNode childTreeNode = new JTreeNode(childNode);
        treeNode.add(childTreeNode);
        getAsTreeNode(childNode, childTreeNode, maxDepth, currentDepth + 1);
      }
      return treeNode;
    }

    public Map<String, List<Integer>> getFidelityCount(Node annotationNode, String fidelity) {
      Optional<Node> fidelityNode = annotationNode.childrenNodes.stream()
          .filter(node -> node.getNodeName().equals(fidelity)).findFirst();
      if (!fidelityNode.isPresent()) {
        return new HashMap<>();
      }
      Map<String, List<Integer>> bucketList = new HashMap<>();
      fidelityNode.get().childrenNodes.forEach(instrumentNode -> {
        String key = instrumentNode.getNodeName();
        List<Integer> value = instrumentNode.getValue();
        bucketList.put(key, value);
      });
      return bucketList;
    }

    public List<Node> getAllPhoneModels() {
      return new ArrayList<>(root.childrenNodes);
    }

    public List<Node> getAllAnnotations(Node phoneModel) {
      return new ArrayList<>(phoneModel.childrenNodes);
    }

    public Set<Node> getAllFidelity(List<Node> annotations) {
      Set<Node> fidelitySet = new HashSet<>();
      annotations.forEach(annotationNode -> fidelitySet.addAll(annotationNode.childrenNodes));
      return fidelitySet;
    }

    public void toggleLastNode(TreePath path) {
      Node currentNode = root;
      for (int i = 1; i < path.getPathCount(); i++) {
        JTreeNode currentJTreeNode = (JTreeNode) path.getPathComponent(i);
        currentNode = currentNode.findChild(new Node(currentJTreeNode.getNodeName())).get();
      }
      currentNode.toggleSelectedState();
    }

    private void setRoot(Node node) {
      this.root = node;
    }

    public HistogramTree getCopy() {
      HistogramTree copyTree = new HistogramTree();
      copyTree.setRoot(copyTree.root.getDeepCopy());
      return copyTree;
    }

    public static final class Node {

      private final List<Node> childrenNodes;
      private final String nodeName;
      private String nodeToolTip;
      private List<Integer> value;
      private boolean selectedState;

      public Node(String nodeName) {
        this.childrenNodes = new ArrayList<>();
        this.nodeName = nodeName;
        this.nodeToolTip = "";
        this.value = new ArrayList<>();
        selectedState = false;
      }

      public Node(String nodeName, String nodeToolTip) {
        this(nodeName);
        this.nodeToolTip = nodeToolTip;
      }

      public List<Integer> getValue() {
        return this.value;
      }

      public void setValue(List<Integer> value) {
        this.value = value;
      }

      public void toggleSelectedState() {
        selectedState = !selectedState;
      }

      public Optional<Node> findChild(Node nodeToFind) {
        return childrenNodes.stream()
            .filter(node -> node.equals(nodeToFind))
            .findFirst();
      }

      // Gets a copy of the node without children
      public Node getNoChildrenCopy() {
        Node copyNode = new Node(nodeName, nodeToolTip);
        copyNode.setValue(new ArrayList<>(this.value));
        return copyNode;
      }

      public Node getDeepCopy() {
        Node copyNode = new Node(nodeName, nodeToolTip);
        copyNode.setValue(new ArrayList<>(this.value));
        for (int i = 0; i < childrenNodes.size(); i++) {
          Node childCopy = childrenNodes.get(i).getDeepCopy();
          copyNode.childrenNodes.add(childCopy);
        }
        return copyNode;
      }

      public String getNodeName() {
        return nodeName;
      }

      public String getNodeToolTip() {
        return nodeToolTip;
      }

      public boolean getNodeState() {
        return selectedState;
      }

      public void addChild(Node node) {
        childrenNodes.add(node);
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
        return nodeName.equals(((Node) o).nodeName);
      }

      @Override
      public int hashCode() {
        return Objects.hash(nodeName);
      }
    }
  }

}
