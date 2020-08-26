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

package Controller.Enum;

import Utils.DataModelTransformer;
import Utils.Proto.CompilationException;
import Utils.Proto.ProtoCompiler;
import java.io.IOException;
import java.util.List;
import javax.swing.table.AbstractTableModel;

public class EnumTableModel extends AbstractTableModel {

  private final String[] columnNames = {"Options"};
  private List<String> data;
  private ProtoCompiler compiler;

  public EnumTableModel(EnumController controller, ProtoCompiler compiler)
      throws IOException, CompilationException {
    this.compiler = compiler;
    data = DataModelTransformer.initEnumData(controller, compiler);
  }

  public void removeRow(int row) {
    data.remove(row);
    fireTableDataChanged();
  }

  @Override
  public int getRowCount() {
    return data.size();
  }

  @Override
  public int getColumnCount() {
    return 1;
  }

  @Override
  public Object getValueAt(int i, int i1) {
    return data.get(i);
  }

  @Override
  public String getColumnName(int column) {
    return columnNames[column];
  }

  public void addRow(Object[] objects) {
    data.add((String) objects[0]);
    fireTableRowsInserted(getRowCount() - 1, getRowCount());
  }
}
