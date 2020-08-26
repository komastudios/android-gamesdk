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

package Utils;

import Controller.Enum.EnumController;
import Model.EnumDataModel;
import Model.MessageDataModel;
import Model.QualityDataModel;
import Utils.Assets.AssetsFinder;
import Utils.Proto.CompilationException;
import Utils.Proto.ProtoCompiler;
import View.Dialog.MainDialogWrapper;
import View.Fidelity.FidelityTableData;
import View.Fidelity.FieldType;
import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.EnumDescriptor;
import com.google.protobuf.Descriptors.EnumValueDescriptor;
import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.Descriptors.FieldDescriptor.JavaType;
import com.google.protobuf.Descriptors.FileDescriptor;
import com.google.protobuf.DynamicMessage;
import com.google.protobuf.ProtocolMessageEnum;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Optional;
import java.util.stream.Collectors;

public final class DataModelTransformer {

  private DataModelTransformer() {
  }

  public static List<EnumDataModel> getEnums(List<EnumDescriptor> enumDescriptors) {
    return enumDescriptors.stream()
        .map(descriptor -> new EnumDataModel(descriptor.getName(), descriptor.getValues()))
        .collect(Collectors.toList());
  }

  public static Optional<MessageDataModel> transformToAnnotation(Descriptor desc) {
    List<FieldDescriptor> fieldDescriptors = desc.getFields();
    MessageDataModel annotationDataModel = new MessageDataModel();
    annotationDataModel.setMessageType(MessageDataModel.Type.ANNOTATION);

    for (FieldDescriptor fieldDescriptor : fieldDescriptors) {
      if (!annotationDataModel.addField(fieldDescriptor.getName(),
          fieldDescriptor.getEnumType().getName())) {
        return Optional.empty();
      }
    }

    return Optional.of(annotationDataModel);
  }

  public static Optional<QualityDataModel> transformToQuality(DynamicMessage message) {
    Map<FieldDescriptor, Object> fieldsMap = message.getAllFields();
    QualityDataModel qualityDataModel = new QualityDataModel();

    for (Entry<FieldDescriptor, Object> entry : fieldsMap.entrySet()) {
      Object fieldValue = entry.getValue();
      FieldDescriptor fieldDescriptor = entry.getKey();

      if (fieldValue instanceof EnumValueDescriptor) {
        int index = ((EnumValueDescriptor) fieldValue).getIndex();
        fieldValue = fieldDescriptor.getEnumType().getValues().get(index).getName();
      }

      if (!qualityDataModel.addField(fieldDescriptor.getName(), fieldValue.toString())) {
        return Optional.empty();
      }
    }

    return Optional.of(qualityDataModel);
  }

  public static Optional<MessageDataModel> transformToFidelity(Descriptor desc) {
    List<FieldDescriptor> fieldDescriptors = desc.getFields();
    MessageDataModel fidelityModel = new MessageDataModel();
    fidelityModel.setMessageType(MessageDataModel.Type.FIDELITY);

    for (FieldDescriptor fieldDescriptor : fieldDescriptors) {
      JavaType type = fieldDescriptor.getJavaType();
      String typeToInsert = new String();

      switch (type) {
        case ENUM: {
          typeToInsert = fieldDescriptor.getEnumType().getName();
          break;
        }
        case INT: {
          typeToInsert = "int32";
          break;
        }
        case FLOAT: {
          typeToInsert = "float";
          break;
        }
        default:
          break;
      }

      if (!fidelityModel.addField(fieldDescriptor.getName(), typeToInsert)) {
        return Optional.empty();
      }
    }

    return Optional.of(fidelityModel);
  }

  public static List<String[]> initAnnotationData(String projectPath, ProtoCompiler compiler)
      throws IOException, CompilationException {
    File assetsDir = new File(
        AssetsFinder.findAssets(projectPath).getAbsolutePath());
    File devTuningfork = new File(assetsDir, "dev_tuningfork.proto");
    FileDescriptor fDesc = compiler.compile(devTuningfork, Optional.empty());
    Descriptor messageDesc = fDesc.findMessageTypeByName("Annotation");

    MessageDataModel annotationDataModel = DataModelTransformer
        .transformToAnnotation(messageDesc).get();

    List<String> enumNames = annotationDataModel.getFieldNames();
    List<String> enumValues = annotationDataModel.getFieldValues();
    List<String[]> data = new ArrayList<>();
    for (int i = 0; i < enumNames.size(); i++) {
      data.add(new String[]{enumValues.get(i), enumNames.get(i)});
    }
    return data;
  }

  public static List<FidelityTableData> initFidelityData(String projectPath, ProtoCompiler compiler)
      throws IOException, CompilationException {
    File assetsDir = new File(
        AssetsFinder.findAssets(projectPath).getAbsolutePath());
    File devTuningfork = new File(assetsDir, "dev_tuningfork.proto");
    FileDescriptor fDesc = compiler.compile(devTuningfork, Optional.empty());
    Descriptor messageDesc = fDesc.findMessageTypeByName("FidelityParams");

    MessageDataModel fidelityDataModel = DataModelTransformer
        .transformToFidelity(messageDesc).get();

    List<String> fieldNames = fidelityDataModel.getFieldNames();
    List<String> fieldValues = fidelityDataModel.getFieldValues();
    List<FidelityTableData> data = new ArrayList<>();

    for (int i = 0; i < fieldNames.size(); i++) {
      if (fieldValues.get(i).equals("int32")) {
        data.add(new FidelityTableData(FieldType.INT32, null, fieldNames.get(i)));
      } else if (fieldValues.get(i).equals("float")) {
        data.add(new FidelityTableData(FieldType.FLOAT, null, fieldNames.get(i)));
      } else {
        data.add(new FidelityTableData(FieldType.ENUM, fieldValues.get(i), fieldNames.get(i)));
      }
    }
    return data;
  }


  public static List<String> initEnumData(String projectPath, EnumController controller,
      ProtoCompiler compiler)
      throws IOException, CompilationException {
    File assetsDir = new File(
        AssetsFinder.findAssets(projectPath).getAbsolutePath());
    File devTuningfork = new File(assetsDir, "dev_tuningfork.proto");
    FileDescriptor fDesc = compiler.compile(devTuningfork, Optional.empty());
    List<EnumDescriptor> enumDescriptors = fDesc.getEnumTypes();

    List<EnumDataModel> enumsFromProto = DataModelTransformer
        .getEnums(enumDescriptors);
    for (EnumDataModel enumDataModel : enumsFromProto) {
      controller.addEnum(enumDataModel.getName(), enumDataModel.getOptions());
    }
    return enumsFromProto.stream().map(EnumDataModel::getName).collect(
        Collectors.toList());
  }
}
