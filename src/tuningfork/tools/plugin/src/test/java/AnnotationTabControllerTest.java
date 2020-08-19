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

import com.intellij.ui.components.JBLabel;
import com.intellij.ui.table.JBTable;
import java.util.List;
import javax.swing.JTable;
import org.junit.Assert;
import org.junit.Test;

public class AnnotationTabControllerTest {

  @Test
  public void addOneAnnotation() {
    AnnotationTabController controller = new AnnotationTabController();
    AnnotationTableModel model = new AnnotationTableModel();
    model.addRow(new String[]{"enum1", "enumName1"});
    model.addRow(new String[]{"enum2", "enumName2"});
    model.addRow(new String[]{"enum3", "enumName3"});
    JTable table = new JBTable(model);
    controller.saveSettings(table, new JBLabel());

    MessageDataModel annotation = controller.getAnnotationDataModel();
    List<String> names = annotation.getFieldNames();
    List<String> values = annotation.getFieldValues();

    Assert.assertEquals(names.size(), 3);
    Assert.assertEquals(values.get(0), "enum1");
    Assert.assertEquals(values.get(1), "enum2");
    Assert.assertEquals(values.get(2), "enum3");
    Assert.assertEquals(names.get(0), "enumName1");
    Assert.assertEquals(names.get(1), "enumName2");
    Assert.assertEquals(names.get(2), "enumName3");
  }

  @Test
  public void twoEnumsSameName() {
    AnnotationTabController controller = new AnnotationTabController();
    AnnotationTableModel model = new AnnotationTableModel();
    model.addRow(new String[]{"enum1", "enumName1"});
    model.addRow(new String[]{"enum2", "enumName1"});
    model.addRow(new String[]{"enum3", "enumName3"});
    JTable table = new JBTable(model);

    Assert.assertFalse(controller.saveSettings(table, new JBLabel()));
  }
}
