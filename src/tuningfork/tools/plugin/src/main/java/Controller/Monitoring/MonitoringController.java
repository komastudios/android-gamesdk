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

import Utils.Monitoring.TelemetryProcessing;
import com.google.android.performanceparameters.v1.PerformanceParameters.UploadTelemetryRequest;
import com.google.protobuf.ByteString;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

public class MonitoringController {

  private LinkedHashMap<String, List<Integer>> renderTimeHistograms;
  private ByteString previousFidelityParams;
  private PropertyChangeSupport propertyChangeSupport;

  public MonitoringController() {
    renderTimeHistograms = new LinkedHashMap<>();
    previousFidelityParams = null;
    propertyChangeSupport = new PropertyChangeSupport(this);
  }

  public boolean checkFidelityParams(UploadTelemetryRequest telemetryRequest) {
    if (telemetryRequest.getTelemetry(0) == null) {
      return true;
    }

    ByteString currentFidelityParams = telemetryRequest.getTelemetry(0).getContext()
        .getTuningParameters().getSerializedFidelityParameters();

    if (previousFidelityParams == null) {
      previousFidelityParams = currentFidelityParams;
    } else if (!previousFidelityParams.equals(currentFidelityParams)) {
      renderTimeHistograms = new LinkedHashMap<>();
      previousFidelityParams = currentFidelityParams;
      return false;
    }
    return true;
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
      XYSeries histogramDataset = new XYSeries("");
      List<Integer> fpsList = entry.getValue();
      for (int i = 0; i < fpsList.size(); i++) {
        histogramDataset.add(i, fpsList.get(i));
      }

      JFreeChart histogram = ChartFactory.createXYBarChart("Render time histogram",
          "frame time", false, "counts", new XYSeriesCollection(histogramDataset),
          PlotOrientation.VERTICAL, true, false, false);

      ChartPanel chartPanel = new ChartPanel(histogram);
      propertyChangeSupport.firePropertyChange("addChart", entry.getKey(), chartPanel);
    }
  }
}
