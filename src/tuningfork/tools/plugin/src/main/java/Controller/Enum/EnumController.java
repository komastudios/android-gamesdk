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

import Model.EnumDataModel;
import com.intellij.ui.table.JBTable;

import javax.swing.table.DefaultTableModel;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

public abstract class EnumController {

  public enum ChangeType {
    ADD,
    REMOVE,
    EDIT
  }

  private final List<EnumDataModel> enums;

  public EnumController() {
    enums = new ArrayList<>();
  }

  public List<EnumDataModel> getEnums() {
    return enums;
  }

  public boolean hasEnums() {
    return !getEnums().isEmpty();
  }

  public List<String> getEnumsNames() {
    return enums.stream().map(EnumDataModel::getName).collect(Collectors.toList());
  }

  public boolean addEnum(String name, ArrayList<String> options) {
    EnumDataModel enumDataModel = new EnumDataModel(name, options);
    if (enums.contains(enumDataModel)) {
      return false;
    }
    onEnumTableChanged(ChangeType.ADD, new String[]{name});
    enums.add(enumDataModel);
    return true;
  }

  public void removeEnum(int row) {
    onEnumTableChanged(ChangeType.REMOVE, new String[]{enums.get(row).getName()});
    enums.remove(row);
  }

  public boolean editEnum(int index, String name, ArrayList<String> options) {
    EnumDataModel enumDataModel = new EnumDataModel(name, options);
    boolean conflictingEnumNames =
            enums.stream().map(EnumDataModel::getName).anyMatch(enumName -> enumName.equals(name));
    if (!name.equals(enums.get(index).getName()) && conflictingEnumNames) {
      return false;
    }
    onEnumTableChanged(ChangeType.EDIT, new String[]{enums.get(index).getName(), name});
    enums.set(index, enumDataModel);
    return true;
  }

  public final void addEnumToTable(JBTable table) {
    DefaultTableModel model = (DefaultTableModel) table.getModel();
    model.addRow(new Object[]{enums.get(enums.size() - 1).getName()});
  }

  public final void removeEnumFromTable(JBTable table, int row) {
    DefaultTableModel model = (DefaultTableModel) table.getModel();
    model.removeRow(row);
  }

  public final void editEnumInTable(JBTable table, int row) {
    DefaultTableModel model = (DefaultTableModel) table.getModel();
    model.setValueAt(enums.get(row).getName(), row, 0);
  }

  public boolean canRemoveEnum(String enumName) {
    return true;
  }

  public abstract void onEnumTableChanged(ChangeType changeType, Object[] changeList);
}
