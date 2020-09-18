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

import Controller.Annotation.AnnotationTabController;
import Controller.Fidelity.FidelityTabController;
import Controller.InstrumentationSettings.InstrumentationSettingsTabController;
import Controller.Quality.QualityTabController;
import Model.EnumDataModel;
import Model.MessageDataModel;
import Model.QualityDataModel;
import Utils.Assets.AssetsFinder;
import Utils.Assets.AssetsWriter;
import Utils.Monitoring.RequestServer;
import Utils.Proto.ProtoCompiler;
import View.PluginLayout;
import com.google.tuningfork.Tuningfork.Settings;
import com.intellij.notification.Notification;
import com.intellij.notification.NotificationDisplayType;
import com.intellij.notification.NotificationGroup;
import com.intellij.notification.NotificationType;
import com.intellij.openapi.Disposable;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.ui.DialogWrapper;
import com.intellij.openapi.ui.Messages;
import com.intellij.openapi.util.Disposer;
import java.io.IOException;
import com.intellij.openapi.ui.Messages;
import java.util.List;
import javax.swing.JComponent;
import org.jetbrains.annotations.Nullable;

public class MainDialogWrapper extends DialogWrapper {

  private static PluginLayout pluginLayout;
  private final Project project;
  private final NotificationGroup NOTIFICATION_GROUP =
      new NotificationGroup("Android Performance Tuner", NotificationDisplayType.BALLOON, true);
  private final MessageDataModel annotationData;
  private final MessageDataModel fidelityData;
  private final List<EnumDataModel> enumData;
  private final List<QualityDataModel> qualityData;
  private ProtoCompiler compiler;

  private void addNotification(String errorMessage) {
    Notification notification = NOTIFICATION_GROUP.createNotification(
        errorMessage,
        NotificationType.ERROR);
    notification.notify(project);
  }


  @Override
  public void doCancelAction() {
    try {
      RequestServer.stopListening();
    } catch (IOException e) {
      e.printStackTrace();
    }
    super.doCancelAction();
  }

  @Override
  protected void doOKAction() {
    pluginLayout.saveSettings();
    if (!pluginLayout.isViewValid()) {
      Messages.showErrorDialog("Please Fix the errors first", "Unable To Close");
      return;
    }
    pluginLayout.saveSettings();
    AssetsWriter assetsWriter = new AssetsWriter(
        AssetsFinder.findAssets(project.getProjectFilePath().split(".idea")[0]).getAbsolutePath());
    AnnotationTabController annotationTabController = pluginLayout.getAnnotationTabController();
    FidelityTabController fidelityTabController = pluginLayout.getFidelityTabController();
    QualityTabController qualityTabController = pluginLayout.getQualityTabController();
    InstrumentationSettingsTabController instrumentationController = pluginLayout
        .getInstrumentationSettingsTabController();
    List<EnumDataModel> annotationEnums = annotationTabController.getEnums();

    if (annotationEnums == null) {
      addNotification(
          "Unable to write annotation and quality settings back to .proto files. You must add some settings first.1");
      return;
    }

    MessageDataModel fidelityModel = fidelityTabController.getFidelityData();
    MessageDataModel annotationModel = annotationTabController.getAnnotationData();
    boolean writeOK = true;

    if (!assetsWriter.saveDevTuningForkProto(annotationEnums, annotationModel, fidelityModel)) {
      addNotification("Unable to write annotation and fidelity settings back to .proto files.");
      writeOK = false;
    }
    Settings dataModel = instrumentationController.getDataModel();
    if (!assetsWriter.saveInstrumentationSettings(dataModel)) {
      addNotification("Unable to write instrumentation settings to txt files.");
      writeOK = false;
    }
    List<QualityDataModel> qualityDataModels = qualityTabController.getQualityDataModels();
    assetsWriter.saveDevFidelityParams(compiler, qualityDataModels);

    if (writeOK) {
      Notification notification = NOTIFICATION_GROUP.createNotification(
          "Android Performance Tuner settings saved successfully!",
          NotificationType.INFORMATION);
      notification.notify(project);

      try {
        RequestServer.stopListening();
      } catch (IOException e) {
        e.printStackTrace();
      }

      super.doOKAction();
    }
  }

  public MainDialogWrapper(@Nullable Project project, MessageDataModel annotationData,
      MessageDataModel fidelityData, List<EnumDataModel> enumData,
      List<QualityDataModel> qualityData, ProtoCompiler compiler) {
    super(project);

    this.annotationData = annotationData;
    this.enumData = enumData;
    this.fidelityData = fidelityData;
    this.qualityData = qualityData;
    this.project = project;
    this.compiler = compiler;

    setTitle("Android Performance Tuner Plugin");

    Disposer.register(this.getDisposable(), () -> {
      try {
        RequestServer.stopListening();
      } catch (IOException e) {
        e.printStackTrace();
      }
    });

    init();
  }

  @Override
  @Nullable
  protected JComponent createCenterPanel() {
    pluginLayout = new PluginLayout(annotationData, fidelityData, enumData, qualityData,
        getDisposable());
    return pluginLayout;
  }
}
