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

package View.Fidelity;

import com.intellij.openapi.ui.cellvalidators.CellComponentProvider.TableProvider;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.event.MouseEvent;
import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JTable;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

public class FidelityCellComponentProvider extends TableProvider {

  protected FidelityCellComponentProvider(
      @NotNull JTable owner) {
    super(owner);
  }

  private JPanel getTextFieldPanel(JComponent jPanel) {
    for (Component component : jPanel.getComponents()) {
      if (component instanceof JPanel) {
        return (JPanel) component;
      }
    }
    return null;
  }

  @Nullable
  @Override
  public JComponent getCellRendererComponent(@NotNull MouseEvent e) {
    JComponent jComponent = super.getCellRendererComponent(e);
    if (jComponent instanceof JPanel) {
      JComponent textFieldPanel = getTextFieldPanel(jComponent);
      if (textFieldPanel instanceof FidelityValidatableTextField) {
        return textFieldPanel;
      }
    }
    return jComponent;
  }

  @NotNull
  @Override
  public Rectangle getCellRect(@NotNull MouseEvent e) {
    JComponent jComponent = super.getCellRendererComponent(e);
    JComponent textFieldPanel;
    if (jComponent instanceof JPanel && (textFieldPanel = getTextFieldPanel(jComponent)) != null) {
      Rectangle cellRect = super.getCellRect(e);
      Dimension dimension = textFieldPanel.getPreferredSize();
      return new Rectangle((int) (cellRect.getX() + (cellRect.getWidth() / 2)),
          (int) cellRect.getY(),
          (int) dimension.getWidth(), (int) dimension.getHeight());
    }
    return super.getCellRect(e);
  }
}
