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

import Model.EnumDataModel;
import Model.MessageDataModel;
import Model.QualityDataModel;
import Utils.Assets.AssetsWriter;
import Utils.Proto.CompilationException;
import Utils.Proto.ProtoCompiler;
import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.EnumDescriptor;
import com.google.protobuf.Descriptors.EnumValueDescriptor;
import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.Descriptors.FieldDescriptor.Type;
import com.google.protobuf.Descriptors.FileDescriptor;
import com.google.protobuf.DynamicMessage;
import org.junit.Assert;
import org.junit.BeforeClass;
import org.junit.ClassRule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map.Entry;
import java.util.Optional;

public class AssetsWriterTest {

  private static final File PROTOC_BINARY = ProtocBinary.get();
  @ClassRule public static TemporaryFolder temporaryFolder = new TemporaryFolder();
  private static EnumDataModel levelEnum;
  private static EnumDataModel loadingStateEnum;
  private static MessageDataModel annotationMessage;
  private static MessageDataModel fidelityMessage;
  private static Path devTuningforkProto, devFidelityText, devFidelityBinary;
  private final ProtoCompiler compiler = ProtoCompiler.getInstance(PROTOC_BINARY);

  private static ArrayList<String> asArrayList(String... strings) {
    ArrayList<String> arrayList = new ArrayList<>();
    Collections.addAll(arrayList, strings);
    return arrayList;
  }

  @BeforeClass
  public static void setup() {
    String tempPath = temporaryFolder.getRoot().getAbsolutePath();
    levelEnum = new EnumDataModel("Level", asArrayList("LEVEL_1", "LEVEL_2", "LEVEL_3"));
    loadingStateEnum = new EnumDataModel("LoadingState", asArrayList("NOT_LOADING", "LOADING"));
    annotationMessage = new MessageDataModel();
    annotationMessage.setMessageType(MessageDataModel.Type.ANNOTATION);
    fidelityMessage = new MessageDataModel();
    fidelityMessage.setMessageType(MessageDataModel.Type.FIDELITY);
    annotationMessage.addField("loading", "LoadingState");
    annotationMessage.addField("level", "Level");
    fidelityMessage.addField("num_sphere", "int32");
    fidelityMessage.addField("tesselation_percent", "float");
    fidelityMessage.addField("current_level", "Level");
    devTuningforkProto = Paths.get(tempPath + "/dev_tuningfork.proto");
    devFidelityText = Paths.get(tempPath + "/dev_tuningfork_fidelityparams_1.txt");
    devFidelityBinary = Paths.get(tempPath + "/dev_tuningfork_fidelityparams_1.bin");
  }

  private List<EnumDataModel> parseFileDescriptorEnums(FileDescriptor fileDescriptor) {
    ArrayList<EnumDataModel> enums = new ArrayList<>();
    for (EnumDescriptor enumDescriptor : fileDescriptor.getEnumTypes()) {
      String enumName = enumDescriptor.getName();
      enums.add(new EnumDataModel(enumName, enumDescriptor.getValues()));
    }
    return enums;
  }

  // TODO(mohanad): Use DataTransformer Class for parsing descriptors.
  private MessageDataModel parseFileDescriptorMessage(
      FileDescriptor fileDescriptor, MessageDataModel.Type messageType) {
    MessageDataModel messageDataModel = new MessageDataModel();
    messageDataModel.setMessageType(messageType);
    Descriptor descriptor = fileDescriptor.findMessageTypeByName(messageType.getMessageName());
    for (FieldDescriptor fieldDescriptor : descriptor.getFields()) {
      String fieldType;
      switch (fieldDescriptor.getType()) {
        case INT32:
          fieldType = "int32";
          break;
        case FLOAT:
          fieldType = "float";
          break;
        default:
          fieldType = fieldDescriptor.getEnumType().getName();
          break;
      }
      String fieldName = fieldDescriptor.getName();
      messageDataModel.addField(fieldName, fieldType);
    }
    return messageDataModel;
  }

  // TODO(mohanad): Use DataTransformer Class for parsing descriptors.
  private QualityDataModel parseDynamicMessage(DynamicMessage message) {
    QualityDataModel qualityDataModel = new QualityDataModel();
    for (Entry<FieldDescriptor, Object> entry : message.getAllFields().entrySet()) {
      FieldDescriptor fieldDescriptor = entry.getKey();
      String fieldName = entry.getKey().getName();
      String fieldValue = entry.getValue().toString();
      if (fieldDescriptor.getType().equals(Type.ENUM)) {
        int index = ((EnumValueDescriptor) entry.getValue()).getIndex();
        fieldValue = fieldDescriptor.getEnumType().getValues().get(index).getName();
      }
      qualityDataModel.addField(fieldName, fieldValue);
    }
    return qualityDataModel;
  }

  private FileDescriptor saveDevTuningFork() throws IOException, CompilationException {
    AssetsWriter assetsWriter = new AssetsWriter(temporaryFolder.getRoot().getAbsolutePath());
    assetsWriter.saveDevTuningForkProto(
            Arrays.asList(levelEnum, loadingStateEnum), annotationMessage, fidelityMessage);
    return compiler.compile(new File(devTuningforkProto.toString()), Optional.empty());
  }

  private DynamicMessage parseBinaryDynamicFile(
          FileDescriptor fileDescriptor, File devFile, String content)
          throws IOException, CompilationException {

    Descriptor descriptor = fileDescriptor.findMessageTypeByName("FidelityParams");
    compiler.encodeFromTextprotoFile(
            descriptor.getFullName(), devFile, content, devFidelityBinary.toString(), Optional.empty());
    return compiler.decodeFromBinary(descriptor, Files.readAllBytes(devFidelityBinary));
  }

  @Test
  public void testSaveDevTuningFork() throws IOException, CompilationException {
    AssetsWriter assetsWriter = new AssetsWriter(temporaryFolder.getRoot().getAbsolutePath());
    FileDescriptor fileDescriptor = saveDevTuningFork();
    String content = String.join("\n", Files.readAllLines(devTuningforkProto));
    List<EnumDataModel> enums = parseFileDescriptorEnums(fileDescriptor);
    MessageDataModel resultAnnotation =
            parseFileDescriptorMessage(fileDescriptor, MessageDataModel.Type.ANNOTATION);
    MessageDataModel resultFidelity =
        parseFileDescriptorMessage(fileDescriptor, MessageDataModel.Type.FIDELITY);
    assetsWriter.saveDevTuningForkProto(enums, resultAnnotation, resultFidelity);
    String resultContent = String.join("\n", Files.readAllLines(devTuningforkProto));
    Assert.assertEquals(content, resultContent);
  }

  @Test
  public void testSaveDevFidelityParams() throws IOException, CompilationException {
    FileDescriptor fileDescriptor = saveDevTuningFork();
    QualityDataModel qualityDataModel = new QualityDataModel();
    qualityDataModel.addField("num_sphere", "3");
    qualityDataModel.addField("tesselation_percent", "22.5");
    qualityDataModel.addField("current_level", "LEVEL_1");
    // Initialize asset writer and write qualityDataModel to disk
    AssetsWriter assetsWriter = new AssetsWriter(temporaryFolder.getRoot().getAbsolutePath());
    assetsWriter.saveDevFidelityParams(qualityDataModel, 1);

    // read quality data model using protocol buffer compiler
    File devFile = new File(devTuningforkProto.toString());
    String expectedMessage = String.join("\n", Files.readAllLines(devFidelityText));
    DynamicMessage message = parseBinaryDynamicFile(fileDescriptor, devFile, expectedMessage);

    // Parse the message read again to the disk.
    QualityDataModel parsedQualityModel = parseDynamicMessage(message);
    assetsWriter.saveDevFidelityParams(parsedQualityModel, 1);

    // Compare both messages.
    String actualMessage = String.join("\n", Files.readAllLines(devFidelityText));
    Assert.assertEquals(expectedMessage, actualMessage);
  }
}
