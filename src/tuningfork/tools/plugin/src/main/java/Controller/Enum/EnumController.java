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
import Utils.Assets.AssetsFinder;
import Utils.DataModelTransformer;
import Utils.Proto.CompilationException;
import Utils.Proto.ProtoCompiler;
import com.google.protobuf.Descriptors.EnumDescriptor;
import com.google.protobuf.Descriptors.FileDescriptor;
import com.intellij.ui.table.JBTable;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;
import javax.swing.table.DefaultTableModel;

public abstract class EnumController {

  private String projectPath;

  private List<EnumDataModel> enums;

  private final ProtoCompiler compiler = ProtoCompiler.getInstance();

  public EnumController() {
    enums = new ArrayList<>();
  }

  public EnumController(String projectPath) {
    enums = new ArrayList<>();
    this.projectPath = projectPath;
  }

  public List<EnumDataModel> getEnums() {
    return enums;
  }

  public boolean hasEnums() {
    return !getEnums().isEmpty();
  }

  public List<String> getEnumsNames() {
    return enums.stream().map(EnumDataModel::getName).collect(
        Collectors.toList());
  }

  public boolean addEnum(String name, List<String> options) {
    EnumDataModel enumDataModel = new EnumDataModel(name, (ArrayList<String>) options);
    if (enums.contains(enumDataModel)) {
      return false;
    }
    enums.add(enumDataModel);
    return true;
  }

  public boolean removeEnum(int index) {
    enums.remove(index);
    return true;
  }

  public boolean editEnum(int index, String name, ArrayList<String> options) {
    EnumDataModel enumDataModel = new EnumDataModel(name, options);
    if (!name.equals(enums.get(index).getName()) && enums.contains(enumDataModel)) {
      return false;
    }
    enums.set(index, enumDataModel);
    return true;
  }

  public void addEnumToTable(JBTable table) {
    EnumTableModel model = (EnumTableModel) table.getModel();
    model.addRow(new Object[]{enums.get(enums.size() - 1).getName()});
    onEnumTableChanged();
  }

  public void removeEnumFromTable(JBTable table, int row) {
    EnumTableModel model = (EnumTableModel) table.getModel();
    model.removeRow(row);
    onEnumTableChanged();
  }

  public void editEnumInTable(JBTable table, int row) {
    EnumTableModel model = (EnumTableModel) table.getModel();
    model.setValueAt(enums.get(row).getName(), row, 0);
    onEnumTableChanged();
  }

  public abstract void onEnumTableChanged();

  public List<String> initEnumData() throws IOException, CompilationException {
    System.out.println(projectPath);
    File assetsDir = new File(
        AssetsFinder.findAssets(projectPath).getAbsolutePath());
    File devTuningfork = new File(assetsDir, "dev_tuningfork.proto");
    FileDescriptor fDesc = compiler.compile(devTuningfork, Optional.empty());
    List<EnumDescriptor> enumDescriptors = fDesc.getEnumTypes();

    List<EnumDataModel> enumsFromProto = DataModelTransformer
        .getEnums(enumDescriptors).get();
    enumsFromProto.stream().map(enumModel -> addEnum(enumModel.getName(), enumModel.getOptions()));
    enums.addAll(enumsFromProto);
    return enumsFromProto.stream().map(enumDataModel -> enumDataModel.getName()).collect(
        Collectors.toList());
  }
}
