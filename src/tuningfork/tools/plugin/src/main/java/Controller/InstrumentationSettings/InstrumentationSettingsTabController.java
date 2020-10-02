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

package Controller.InstrumentationSettings;

import com.google.tuningfork.Tuningfork.Settings;
import com.google.tuningfork.Tuningfork.Settings.AggregationStrategy;
import com.google.tuningfork.Tuningfork.Settings.AggregationStrategy.Submission;
import com.google.tuningfork.Tuningfork.Settings.Histogram;
import javax.swing.JTable;

public class InstrumentationSettingsTabController {

  private Settings settingsModel;

  public InstrumentationSettingsTabController(
      Settings settingsModel) {
    this.settingsModel = settingsModel;
  }

  public static void addRowAction(JTable jtable) {
    InstrumentationSettingsTableModel model = (InstrumentationSettingsTableModel) jtable.getModel();
    model.addRow();
  }

  public static void removeRowAction(JTable jtable) {
    InstrumentationSettingsTableModel model = (InstrumentationSettingsTableModel) jtable.getModel();
    int row = jtable.getSelectedRow();
    if (jtable.getCellEditor() != null) {
      jtable.getCellEditor().stopCellEditing();
    }
    model.removeRow(row);
  }

  public void addNewHistogram() {
    Settings.Builder settingsBuilder = settingsModel.toBuilder();
    settingsModel = settingsBuilder.addHistograms(Histogram.newBuilder().build()).build();
  }

  public void removeHistogram(int row) {
    Settings.Builder settingsBuilder = settingsModel.toBuilder();
    settingsModel = settingsBuilder.removeHistograms(row).build();
  }

  public void setHistogramInstrumentID(int histogramRow, String id) {
    Settings.Builder settingsBuilder = settingsModel.toBuilder();
    Histogram histogram = settingsBuilder.getHistograms(histogramRow);
    settingsModel = settingsModel.toBuilder().setHistograms(histogramRow,
        histogram.toBuilder().setInstrumentKey(Integer.parseInt(id))).build();
  }

  public void setHistogramMinimumBucketSize(int histogramRow, String bucketSize) {
    Settings.Builder settingsBuilder = settingsModel.toBuilder();
    Histogram histogram = settingsBuilder.getHistograms(histogramRow);
    settingsModel = settingsModel.toBuilder().setHistograms(histogramRow,
        histogram.toBuilder().setBucketMin(Float.parseFloat(bucketSize))).build();
  }

  public void setHistogramMaximumBucketSize(int histogramRow, String bucketSize) {
    Settings.Builder settingsBuilder = settingsModel.toBuilder();
    Histogram histogram = settingsBuilder.getHistograms(histogramRow);
    settingsModel = settingsModel.toBuilder().setHistograms(histogramRow,
        histogram.toBuilder().setBucketMax(Float.parseFloat(bucketSize))).build();
  }

  public void setHistogramNumberOfBuckets(int histogramRow, String buckets) {
    Settings.Builder settingsBuilder = settingsModel.toBuilder();
    Histogram histogram = settingsBuilder.getHistograms(histogramRow);
    settingsModel = settingsModel.toBuilder().setHistograms(histogramRow,
        histogram.toBuilder().setNBuckets(Integer.parseInt(buckets))).build();
  }

  public void setAggregationMethod(String uploadMethod) {
    Settings.Builder settingsBuilder = settingsModel.toBuilder();
    AggregationStrategy.Builder strategy = settingsBuilder.getAggregationStrategy().toBuilder();
    Submission submission = (uploadMethod.equals("Time Based") ? Submission.TIME_BASED
        : Submission.TICK_BASED);
    settingsModel = settingsBuilder.setAggregationStrategy(strategy.setMethod(submission).build())
        .build();
  }

  public void setApiKey(String key) {
    Settings.Builder settingsBuilder = settingsModel.toBuilder();
    settingsModel = settingsBuilder.setApiKey(key).build();
  }

  public void setUploadInterval(Integer uploadInterval) {
    Settings.Builder settingsBuilder = settingsModel.toBuilder();
    AggregationStrategy.Builder strategy = settingsBuilder.getAggregationStrategy().toBuilder();
    settingsModel = settingsBuilder
        .setAggregationStrategy(strategy.setIntervalmsOrCount(uploadInterval))
        .build();
  }

  public Settings getDataModel() {
    return settingsModel;
  }
}