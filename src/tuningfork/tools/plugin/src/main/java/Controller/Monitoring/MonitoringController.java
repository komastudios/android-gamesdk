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

import Model.QualityDataModel;
import Utils.DataModelTransformer;
import Utils.Monitoring.TelemetryProcessing;
import com.google.android.performanceparameters.v1.PerformanceParameters.UploadTelemetryRequest;
import com.google.protobuf.ByteString;
import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.FileDescriptor;
import com.google.protobuf.DynamicMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

public class MonitoringController {

  /*
   * Key = instrument id, Value = XYSeriesCollection.
   */
  private HashMap<String, XYSeriesCollection> seriesCollectionMap;
  /*
   * Key = instrument id, Value = list of merged bucket counts.
   */
  private LinkedHashMap<String, List<Integer>> renderTimeHistograms;
  private HashSet<ByteString> previousQualitySettings;
  private LinkedHashSet<QualityDataModel> qualitySettingsList;
  private PropertyChangeSupport propertyChangeSupport;
  private int currentQualityIndex;
  private boolean isNewQuality;

  public MonitoringController() {
    renderTimeHistograms = new LinkedHashMap<>();
    previousQualitySettings = new HashSet<>();
    qualitySettingsList = new LinkedHashSet<>();
    propertyChangeSupport = new PropertyChangeSupport(this);
    seriesCollectionMap = new HashMap<>();
    currentQualityIndex = 0;
    isNewQuality = false;
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

  private Optional<Integer> transformQualitySettings(ByteString currentQualitySettings)
      throws InvalidProtocolBufferException {
    FileDescriptor devTuningForkDesc = DataModelTransformer.getDevTuningforkDesc();

    if (devTuningForkDesc == null) {
      return Optional.empty();
    }

    Descriptor fidelityParamsDesc = devTuningForkDesc.findMessageTypeByName("FidelityParams");
    DynamicMessage qualityMessage = DynamicMessage
        .parseFrom(fidelityParamsDesc, currentQualitySettings.toByteArray());

    QualityDataModel newQuality = DataModelTransformer
        .transformToQuality(qualityMessage, fidelityParamsDesc.getFields()).get();

    if (!previousQualitySettings.contains(currentQualitySettings)) {
      qualitySettingsList.add(newQuality);
      // TODO (@targintaru) fire property change and display info about Quality Settings in MonitoringTab
      previousQualitySettings.add(currentQualitySettings);
      isNewQuality = true;
    } else {
      isNewQuality = false;
      return Optional.of(getIndexOf(newQuality));
    }

    return Optional.of(qualitySettingsList.size() - 1);
  }

  public void checkFidelityParams(UploadTelemetryRequest telemetryRequest)
      throws InvalidProtocolBufferException {
    if (telemetryRequest.getTelemetry(0) == null) {
      return;
    }

    ByteString currentFidelityParams = telemetryRequest.getTelemetry(0).getContext()
        .getTuningParameters().getSerializedFidelityParameters();

    transformQualitySettings(currentFidelityParams).ifPresent(index -> currentQualityIndex = index);

    if (isNewQuality) {
      renderTimeHistograms = new LinkedHashMap<>();
    }
  }

  public void setRenderTimeHistograms(UploadTelemetryRequest telemetryRequest) {
    renderTimeHistograms = TelemetryProcessing.processTelemetryData(telemetryRequest);
  }

  public Set<String> getRenderTimeHistogramsKeys() {
    return renderTimeHistograms.keySet();
  }

  public void addPropertyChangeListener(PropertyChangeListener propertyChangeListener) {
    propertyChangeSupport.addPropertyChangeListener(propertyChangeListener);
  }

  public void createChartPanels() {
    for (Map.Entry<String, List<Integer>> entry : renderTimeHistograms.entrySet()) {
      String instrumentId = entry.getKey();
      XYSeries histogramDataset = new XYSeries("Quality settings " + (currentQualityIndex + 1));
      List<Integer> fpsList = entry.getValue();

      for (int i = 0; i < fpsList.size(); i++) {
        histogramDataset.add(i, fpsList.get(i));
      }

      if (!seriesCollectionMap.containsKey(instrumentId)) {
        seriesCollectionMap.put(instrumentId, new XYSeriesCollection());
      }

      if (!isNewQuality) {
        seriesCollectionMap.get(instrumentId).removeSeries(currentQualityIndex);
      }
      seriesCollectionMap.get(instrumentId).addSeries(histogramDataset);

      JFreeChart histogram = ChartFactory.createXYBarChart("Render time histogram",
          "frame time", false, "counts", seriesCollectionMap.get(instrumentId),
          PlotOrientation.VERTICAL, true, false, false);

      ChartPanel chartPanel = new ChartPanel(histogram);
      propertyChangeSupport.firePropertyChange("addChart", instrumentId, chartPanel);
    }
  }
}
