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

    assert (enumNames.size() == 4);
    assert (enumNames.get(0).equals("loading_state"));
    assert (enumNames.get(1).equals("state"));
    assert (enumNames.get(2).equals("settings"));
    assert (enumNames.get(3).equals("nested_enum"));
    assert (enums.get(0).equals("LoadingState"));
    assert (enums.get(1).equals("State"));
    assert (enums.get(2).equals("QualitySettings"));
    assert (enums.get(3).equals("NestedEnum"));
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

    assert (fidelityValues.size() == 3);
    assert (fidelityValues.get(0).equals("QualitySettings"));
    assert (fidelityValues.get(1).equals("int32"));
    assert (fidelityValues.get(2).equals("float"));
    assert (fidelityNames.get(0).equals("field1"));
    assert (fidelityNames.get(1).equals("field2"));
    assert (fidelityNames.get(2).equals("field3"));
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

    assert (qualityValues.size() == 3);
    assert (qualityValues.get(0).equals("SIMPLE"));
    assert (qualityValues.get(1).equals("10"));
    assert (qualityValues.get(2).equals("1.2"));
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

    assert (qualityDataModel.getFieldValues().size() == 2);
  }
}