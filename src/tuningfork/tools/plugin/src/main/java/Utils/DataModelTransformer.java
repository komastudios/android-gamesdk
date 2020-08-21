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
import Model.MessageDataModel;
import Model.QualityDataModel;
import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.EnumValueDescriptor;
import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.Descriptors.FieldDescriptor.JavaType;
import com.google.protobuf.DynamicMessage;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Optional;

public final class DataModelTransformer {
  private DataModelTransformer() {}

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
}
