/*
 * Copyright (C) 2019 The Android Open Source Project
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

import com.intellij.ui.JBColor;
import com.intellij.util.ui.JBUI;
import com.intellij.util.ui.UIUtil;
import java.awt.Color;
import java.awt.Component;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Insets;
import java.awt.RenderingHints;
import java.awt.Shape;
import java.awt.geom.Area;
import java.awt.geom.Rectangle2D;
import java.awt.geom.RoundRectangle2D;
import javax.swing.border.AbstractBorder;

public final class RoundedCornerBorder extends AbstractBorder {
    private final Color ALPHA_ZERO = UIUtil.getWindowColor();

    @Override
    public void paintBorder(Component c, Graphics g, int x, int y, int width, int height) {
        Graphics2D g2 = (Graphics2D) g.create();
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        Shape border = getBorderShape(x, y, width - 1, height - 1);
        g2.setPaint(ALPHA_ZERO);
        Area corner = new Area(new Rectangle2D.Double(x, y, width, height));
        corner.subtract(new Area(border));
        g2.fill(corner);
        g2.setPaint(JBColor.GRAY);
        g2.draw(border);
        g2.dispose();
    }

    public Shape getBorderShape(int x, int y, int w, int h) {
        int r = h;
        return new RoundRectangle2D.Double(x, y, w, h, r, r);
    }

    @Override
    public Insets getBorderInsets(Component c) {
        return JBUI.insets(4, 8);
    }

    @Override
    public Insets getBorderInsets(Component c, Insets insets) {
        insets.set(4, 8, 4, 8);
        return insets;
    }
}
