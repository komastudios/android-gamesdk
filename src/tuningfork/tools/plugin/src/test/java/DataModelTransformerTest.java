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

import Files.CompilationException;
import Files.ProtoCompiler;
import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.EnumValueDescriptor;
import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.Descriptors.FileDescriptor;
import com.google.protobuf.DynamicMessage;
import java.io.File;
import java.io.IOException;
import java.util.List;
import java.util.Optional;
import org.junit.Assert;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

public class DataModelTransformerTest {

  private static final File PROTOC_BINARY = ProtocBinary.get();
  private final ProtoCompiler compiler = ProtoCompiler.getInstance(PROTOC_BINARY);

  @Rule
  // Override default behavior to allow overwriting files.
  public TemporaryFolder tempFolder = new TemporaryFolder() {
    @Override
    public File newFile(String filename) {
      return new File(getRoot(), filename);
    }
  };

  private final TestDataHelper helper = new TestDataHelper(tempFolder);

  @Test
  public void transformToAnnotation() throws IOException, CompilationException {
    File file = helper.getFile("annotation_valid.proto");
    FileDescriptor fDesc = compiler.compile(file, Optional.empty());
    Descriptor messageDesc = fDesc.findMessageTypeByName("Annotation");

    MessageDataModel annotationDataModel = DataModelTransformer
        .transformToAnnotation(messageDesc).get();
    List<String> enumNames = annotationDataModel.getFieldNames();
    List<String> enums = annotationDataModel.getFieldValues();

    Assert.assertEquals(enumNames.size(), 4);
    Assert.assertEquals(enumNames.get(0), "loading_state");
    Assert.assertEquals(enumNames.get(1), "state");
    Assert.assertEquals(enumNames.get(2), "settings");
    Assert.assertEquals(enumNames.get(3), "nested_enum");
    Assert.assertEquals(enums.get(0), "LoadingState");
    Assert.assertEquals(enums.get(1), "State");
    Assert.assertEquals(enums.get(2), "QualitySettings");
    Assert.assertEquals(enums.get(3), "NestedEnum");
  }

  @Test
  public void transformToFidelity() throws IOException, CompilationException {
    File file = helper.getFile("fidelity_params_valid.proto");
    FileDescriptor fDesc = compiler.compile(file, Optional.empty());
    Descriptor messageDesc = fDesc.findMessageTypeByName("FidelityParams");

    MessageDataModel fidelityDataModel = DataModelTransformer.transformToFidelity(messageDesc)
        .get();
    List<String> fidelityValues = fidelityDataModel.getFieldValues();
    List<String> fidelityNames = fidelityDataModel.getFieldNames();

    Assert.assertEquals(fidelityValues.size(), 3);
    Assert.assertEquals(fidelityValues.get(0), "QualitySettings");
    Assert.assertEquals(fidelityValues.get(1), "int32");
    Assert.assertEquals(fidelityValues.get(2), "float");
    Assert.assertEquals(fidelityNames.get(0), "field1");
    Assert.assertEquals(fidelityNames.get(1), "field2");
    Assert.assertEquals(fidelityNames.get(2), "field3");
  }

  @Test
  public void transformToQuality() throws IOException, CompilationException {
    File file = helper.getFile("fidelity_params_valid.proto");
    FileDescriptor fDesc = compiler.compile(file, Optional.empty());
    Descriptor messageDesc = fDesc.findMessageTypeByName("FidelityParams");

    List<FieldDescriptor> fieldDescriptors = messageDesc.getFields();
    EnumValueDescriptor enumValue = fieldDescriptors.get(0).getEnumType().getValues().get(2);
    DynamicMessage dynamicMessage = DynamicMessage.newBuilder(messageDesc)
        .setField(fieldDescriptors.get(0), enumValue)
        .setField(fieldDescriptors.get(1), 10) // int field
        .setField(fieldDescriptors.get(2), (float) 1.2) // float field
        .build();
    QualityDataModel qualityDataModel = DataModelTransformer
        .transformToQuality(dynamicMessage).get();
    List<String> qualityValues = qualityDataModel.getFieldValues();

    Assert.assertEquals(qualityValues.size(), 3);
    Assert.assertEquals(qualityValues.get(0), "SIMPLE");
    Assert.assertEquals(qualityValues.get(1), "10");
    Assert.assertEquals(qualityValues.get(2), "1.2");
  }

  @Test
  public void twoFieldsSameName() throws IOException, CompilationException {
    File file = helper.getFile("fidelity_params_valid.proto");
    FileDescriptor fDesc = compiler.compile(file, Optional.empty());
    Descriptor messageDesc = fDesc.findMessageTypeByName("FidelityParams");

    List<FieldDescriptor> fieldDescriptors = messageDesc.getFields();
    EnumValueDescriptor enumValue = fieldDescriptors.get(0).getEnumType().getValues().get(2);
    DynamicMessage dynamicMessage = DynamicMessage.newBuilder(messageDesc)
        .setField(fieldDescriptors.get(0), enumValue)
        .setField(fieldDescriptors.get(1), 10) // int field
        .setField(fieldDescriptors.get(1), 12) // float field
        .build();
    QualityDataModel qualityDataModel = DataModelTransformer
        .transformToQuality(dynamicMessage).get();

    Assert.assertEquals(qualityDataModel.getFieldValues().size(), 2);
  }
}