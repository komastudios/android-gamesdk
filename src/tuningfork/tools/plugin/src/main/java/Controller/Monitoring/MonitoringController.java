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

import Controller.Monitoring.HistogramTree.Node;
import Model.MonitorFilterModel;
import Model.QualityDataModel;
import Utils.DataModelTransformer;
import View.Monitoring.MonitoringTab.JTreeNode;
import com.google.android.performanceparameters.v1.PerformanceParameters.Telemetry;
import com.google.android.performanceparameters.v1.PerformanceParameters.UploadTelemetryRequest;
import com.google.protobuf.ByteString;
import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.DynamicMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import com.intellij.util.ui.UIUtil;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

public class MonitoringController {

  private final PropertyChangeSupport monitoringPropertyChange;
  private HistogramTree histogramTree;
  private Map<ByteString, Integer> annotationMap, fidelityMap;
  private List<MonitorFilterModel> filterModels;

  public MonitoringController() {
    monitoringPropertyChange = new PropertyChangeSupport(this);
    histogramTree = new HistogramTree();
    annotationMap = new HashMap<>();
    fidelityMap = new HashMap<>();
    filterModels = new ArrayList<>();
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

  public void makeTree(UploadTelemetryRequest telemetryRequest) {
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

  public JTreeNode getFilterTree() {
    JTreeNode root = new JTreeNode();
    for (int i = 0; i < filterModels.size(); i++) {
      MonitorFilterModel model = filterModels.get(i);
      JTreeNode filterNode = new JTreeNode(String.format("Filter %d", i + 1));
      filterNode.add(new JTreeNode("Phone Model: " + model.getPhoneModel()));
      JTreeNode annotationsNode = new JTreeNode("Annotations");
      filterNode.add(annotationsNode);
      model.getAnnotations().forEach(annotationsNode::add);
      filterNode.add(model.getFidelity().asJTreeNode());
      root.add(filterNode);
    }
    return root;
  }

  public void addMonitoringPropertyChangeListener(PropertyChangeListener listener) {
    monitoringPropertyChange.addPropertyChangeListener(listener);
  }

  public void addFilter(MonitorFilterModel model) {
    filterModels.add(model);
  }

  public void removeFilter(int index) {
    filterModels.remove(index);
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
    jChart.getTitle().setPaint(UIUtil.getTextAreaForeground());
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
    String fidelity = model.getFidelity().getNodeName();
    Map<String, List<Integer>> singleFidelityBucket = new HashMap<>();
    model.getAnnotations().forEach(annotation -> {
      Node annotationNode = histogramTree.findNodeByName(annotation.getNodeName(), phoneNode);
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

}
