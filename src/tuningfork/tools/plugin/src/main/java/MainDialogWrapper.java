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

import com.intellij.openapi.project.Project;
import com.intellij.openapi.ui.DialogWrapper;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;
import java.awt.*;

public class MainDialogWrapper extends DialogWrapper {
  private static PluginLayout pluginLayout;

  protected MainDialogWrapper(@Nullable Project project) {
    super(project);
    init();
    setTitle("Android Performance Tuner Plugin");
  }

  @Override
  protected @Nullable JComponent createCenterPanel() {
    pluginLayout = new PluginLayout();
    return pluginLayout;
  }
}
