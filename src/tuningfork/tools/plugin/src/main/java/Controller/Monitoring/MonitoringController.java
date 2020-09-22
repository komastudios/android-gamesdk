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
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.ArrayList;

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
  private LinkedHashMap<String, XYSeriesCollection> seriesCollectionMap;
  /*
   * Each fidelity setting received so far has a LinkedHashMap with merged count values
   * received so far, where Key = instrument id, Value = list of merged bucket counts.
   */
  private HashMap<ByteString, LinkedHashMap<String, List<Integer>>> renderTimeHistograms;
  private ArrayList<Integer> indexesNotToPlot;
  private LinkedHashSet<QualityDataModel> qualitySettingsList;
  private PropertyChangeSupport monitoringPropertyChange;
  private int currentQualityIndex;
  private boolean isNewQuality;
  private ByteString currentFidelitySettings;

  public MonitoringController() {
    renderTimeHistograms = new HashMap<>();
    qualitySettingsList = new LinkedHashSet<>();
    monitoringPropertyChange = new PropertyChangeSupport(this);
    seriesCollectionMap = new LinkedHashMap<>();
    indexesNotToPlot = new ArrayList<>();
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
        .parseFrom(fidelityParamsDesc, currentFidelitySettings.toByteArray());

    QualityDataModel newQuality = DataModelTransformer
        .transformToQuality(qualityMessage, fidelityParamsDesc.getFields()).get();

    if (!renderTimeHistograms.containsKey(currentFidelitySettings)) {
      renderTimeHistograms.put(currentFidelitySettings, new LinkedHashMap<>());
      qualitySettingsList.add(newQuality);
      monitoringPropertyChange.firePropertyChange("addFidelity", qualitySettingsList.size(), newQuality);
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

    currentFidelitySettings = telemetryRequest.getTelemetry(0).getContext()
            .getTuningParameters().getSerializedFidelityParameters();

    transformQualitySettings().ifPresent(index -> currentQualityIndex = index);
  }

  public void setRenderTimeHistograms(UploadTelemetryRequest telemetryRequest) {
    renderTimeHistograms.put(currentFidelitySettings,
        TelemetryProcessing.processTelemetryData(telemetryRequest));
  }

  public Set<String> getRenderTimeHistogramsKeys() {
    return renderTimeHistograms.get(currentFidelitySettings).keySet();
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
    XYSeriesCollection originalSeries = seriesCollectionMap.get(instrumentID);

    for (int i = 0; i < currentQualityIndex; i++) {
      seriesToModify.addSeries(originalSeries.getSeries(i));
    }
    seriesToModify.addSeries(histogramDataset);
    for (int i = currentQualityIndex + 1; i < qualitySettingsList.size(); i++) {
      seriesToModify.addSeries(originalSeries.getSeries(i));
    }

    seriesCollectionMap.put(instrumentID, seriesToModify);
  }

  public void createChartPanels() {
    for (Map.Entry<String, List<Integer>> entry : renderTimeHistograms.get(currentFidelitySettings).entrySet()) {
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
        replotCertainSeries(instrumentId, currentQualityIndex, histogramDataset);
      } else {
        seriesCollectionMap.get(instrumentId).addSeries(histogramDataset);
      }
    }
    removeQualitySettingsNotToPlot();
  }

  public void removeQualitySettingsNotToPlot() {
    for (Map.Entry<String, XYSeriesCollection> entry : seriesCollectionMap.entrySet()) {
      try {
        XYSeriesCollection datasets = (XYSeriesCollection) entry.getValue().clone();

        for (int pos = 0; pos < indexesNotToPlot.size(); pos++) {
          datasets.removeSeries(indexesNotToPlot.get(pos) - pos);
        }

        JFreeChart histogram = ChartFactory.createXYBarChart("Render time histogram",
            "frame time", false, "counts", datasets,
            PlotOrientation.VERTICAL, true, false, false);

        ChartPanel chartPanel = new ChartPanel(histogram);
        monitoringPropertyChange.firePropertyChange("addChart", entry.getKey(), chartPanel);
      } catch (CloneNotSupportedException e) {
        e.printStackTrace();
      }
    }
  }
}
