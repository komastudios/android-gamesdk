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
import Controller.Quality.QualityTabController;
import Model.EnumDataModel;
import Model.MessageDataModel;
import Model.QualityDataModel;
import Utils.Assets.AssetsFinder;
import Utils.Assets.AssetsWriter;
import Utils.Proto.ProtoCompiler;
import View.PluginLayout;
import com.intellij.notification.Notification;
import com.intellij.notification.NotificationDisplayType;
import com.intellij.notification.NotificationGroup;
import com.intellij.notification.NotificationType;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.ui.DialogWrapper;
import java.io.File;
import java.util.List;
import javax.swing.JComponent;
import org.jetbrains.annotations.Nullable;

public class MainDialogWrapper extends DialogWrapper {

  private static PluginLayout pluginLayout;
  private final Project project;
  private final NotificationGroup NOTIFICATION_GROUP =
      new NotificationGroup("Android Performance Tuner", NotificationDisplayType.BALLOON, true);
  private AnnotationTabController annotationTabController;
  private FidelityTabController fidelityTabController;
  private QualityTabController qualityTabController;
  private MessageDataModel annotationData;
  private MessageDataModel fidelityData;
  private List<EnumDataModel> enumData;
  private List<QualityDataModel> qualityData;
  private ProtoCompiler compiler;

  private void addNotification(String errorMessage) {
    Notification notification = NOTIFICATION_GROUP.createNotification(
        errorMessage,
        NotificationType.ERROR);
    notification.notify(project);
  }

  private void deleteExtraFiles(int initialNumber, int finalNumber) {
    for (int fileIndex = finalNumber + 1; fileIndex < initialNumber; fileIndex++) {
      String assetsDir =
          AssetsFinder.findAssets(project.getProjectFilePath().split(".idea")[0]).getAbsolutePath();
      File fileToRemove = new File(assetsDir,
          "/dev_tuningfork_fidelityparams_" + fileIndex + ".bin");
      fileToRemove.delete();
    }
  }

  @Override
  protected void doOKAction() {
    AssetsWriter assetsWriter = new AssetsWriter(
        AssetsFinder.findAssets(project.getProjectFilePath().split(".idea")[0]).getAbsolutePath());
    annotationTabController = pluginLayout.getAnnotationTabController();
    fidelityTabController = pluginLayout.getFidelityTabController();
    qualityTabController = pluginLayout.getQualityTabController();
    int initialQualityLevelsNumber = QualityTabController.getFilesCount();

    List<EnumDataModel> annotationEnums = annotationTabController.getEnums();

    if (annotationEnums == null) {
      addNotification(
          "Unable to write annotation and quality settings back to .proto files. You must add some settings first.1");
      return;
    }

    if (!pluginLayout.saveSettings()) {
      addNotification(
          "Unable to write annotation and quality settings back to .proto files. Different fields can't have the same name.");
      return;
    }

    MessageDataModel fidelityModel = fidelityTabController.getFidelityData();
    MessageDataModel annotationModel = annotationTabController.getAnnotationDataModel();
    boolean writeOK = true;
    if (!assetsWriter.saveDevTuningForkProto(annotationEnums, annotationModel, fidelityModel)) {
      addNotification("Unable to write annotation and fidelity settings back to .proto files.");
      writeOK = false;
    }

    List<QualityDataModel> qualityDataModels = qualityTabController.getQualityDataModels();
    int finalQualityLevelsNumber = QualityTabController.getFilesCount();
    assetsWriter.saveDevFidelityParams(compiler, qualityDataModels);
    deleteExtraFiles(initialQualityLevelsNumber, finalQualityLevelsNumber);

    if (writeOK) {
      Notification notification = NOTIFICATION_GROUP.createNotification(
          "Android Performance Tuner settings saved successfully!",
          NotificationType.INFORMATION);
      notification.notify(project);
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
    init();
  }

  @Override
  protected @Nullable
  JComponent createCenterPanel() {
    pluginLayout = new PluginLayout(annotationData, fidelityData, enumData, qualityData, project);
    return pluginLayout;
  }
}
