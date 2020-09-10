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

package Action;


import Model.MessageDataModel;
import Utils.DataModelTransformer;
import Utils.Generation.TuningForkMethodsGeneration;
import Utils.Proto.CompilationException;
import Utils.Proto.ProtoCompiler;
import com.intellij.notification.Notification;
import com.intellij.notification.NotificationDisplayType;
import com.intellij.notification.NotificationGroup;
import com.intellij.notification.NotificationType;
import com.intellij.openapi.actionSystem.AnAction;
import com.intellij.openapi.actionSystem.AnActionEvent;
import com.intellij.openapi.actionSystem.CommonDataKeys;
import com.intellij.openapi.command.WriteCommandAction;
import com.intellij.openapi.editor.Document;
import com.intellij.openapi.editor.Editor;
import com.intellij.openapi.progress.ProgressIndicator;
import com.intellij.openapi.progress.ProgressManager;
import com.intellij.openapi.progress.Task;
import com.intellij.openapi.project.Project;
import com.intellij.psi.PsiDocumentManager;
import com.intellij.psi.PsiFile;
import com.jetbrains.cidr.lang.psi.OCFile;
import java.io.IOException;
import org.jetbrains.annotations.NotNull;

public class GenerateAnnotationAction extends AnAction {

  private final NotificationGroup NOTIFICATION_GROUP =
      new NotificationGroup("Android Performance Tuner", NotificationDisplayType.BALLOON, true);

  @Override
  public void update(@NotNull AnActionEvent e) {
    final Project project = e.getProject();
    final Editor editor = e.getData(CommonDataKeys.EDITOR);
    if (project == null || editor == null) {
      e.getPresentation().setEnabledAndVisible(false);
      return;
    }
    final Document document = editor.getDocument();
    PsiFile psiFile = PsiDocumentManager.getInstance(project).getPsiFile(document);
    if (!(psiFile instanceof OCFile)) {
      e.getPresentation().setEnabledAndVisible(false);
      return;
    }
    e.getPresentation().setEnabledAndVisible(true);
  }

  @Override
  public void actionPerformed(@NotNull AnActionEvent actionEvent) {
    final Editor editor = actionEvent.getRequiredData(CommonDataKeys.EDITOR);
    final Project project = actionEvent.getRequiredData(CommonDataKeys.PROJECT);
    final Document document = editor.getDocument();
    PsiFile psiFile = PsiDocumentManager.getInstance(project).getPsiFile(document);
    if (psiFile == null) {
      Notification notification = NOTIFICATION_GROUP.createNotification(
          "Unable to identify the current file.",
          NotificationType.INFORMATION);
      notification.notify(project);
      return;
    }
    runGenerationTask(project, psiFile);
  }

  private void runGenerationTask(Project project, PsiFile psiFile) {
    ProgressManager.getInstance()
        .run(
            new Task.Backgroundable(project, "Loading APT Data") {
              public void run(@NotNull ProgressIndicator progressIndicator) {
                progressIndicator.setIndeterminate(true);
                ProtoCompiler protoCompiler = ProtoCompiler.getInstance();
                String projectPath = project.getProjectFilePath().split(".idea")[0];
                try {
                  DataModelTransformer transformer = new DataModelTransformer(projectPath,
                      protoCompiler);
                  MessageDataModel annotationData = transformer.initAnnotationData();
                  MessageDataModel fidelityData = transformer.initFidelityData();
                  TuningForkMethodsGeneration tuningForkMethodsGeneration
                      = new TuningForkMethodsGeneration(psiFile, project);
                  progressIndicator.setText("Writing Methods");
                  generateCode(annotationData, fidelityData, tuningForkMethodsGeneration, project);
                  progressIndicator.setText("Brushing Up Imports");
                  WriteCommandAction
                      .runWriteCommandAction(project, tuningForkMethodsGeneration::generateImports);
                } catch (IOException | CompilationException e1) {
                  e1.printStackTrace();
                }
              }
            });
  }

  private void generateCode(MessageDataModel annotationData, MessageDataModel fidelityData,
      TuningForkMethodsGeneration tuningForkMethodsGeneration, Project project) {

    WriteCommandAction.runWriteCommandAction(project, () -> tuningForkMethodsGeneration
        .generateTuningForkCode(annotationData, fidelityData));
  }
}
