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

package View.Decorator;

import java.awt.BorderLayout;
import java.awt.Component;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.plaf.basic.BasicComboBoxEditor;

public class ValidationComboBoxEditor extends BasicComboBoxEditor {

  private final JPanel jPanel;
  private final JLabel textLabel;
  private final JLabel errorLabel;

  public ValidationComboBoxEditor() {
    jPanel = new JPanel(new BorderLayout());
    textLabel = new JLabel();
    errorLabel = new JLabel();
    textLabel.setOpaque(false);
    errorLabel.setOpaque(false);
    jPanel.add(textLabel, BorderLayout.WEST);
    jPanel.add(errorLabel, BorderLayout.EAST);
    jPanel.setBackground(null);
    jPanel.setForeground(null);
    jPanel.setOpaque(false);
  }

  public JLabel getErrorLabel() {
    return errorLabel;
  }

  @Override
  public Component getEditorComponent() {
    return this.jPanel;
  }

  @Override
  public Object getItem() {
    return textLabel.getText();
  }

  @Override
  public void setItem(Object anObject) {
    if (anObject == null) {
      textLabel.setText("");
    } else {
      textLabel.setText(anObject.toString());
    }
  }
}
