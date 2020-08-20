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

package Utils.Assets;

import Model.EnumDataModel;
import Model.MessageDataModel;
import Model.QualityDataModel;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;
import java.util.stream.IntStream;

public class AssetsWriter {

  private final String assetsDirectory;
  private static final String AUTO_GENERATED_PROTO =
      "// This file is auto generated -- DO NOT EDIT\n";
  private static final String AUTO_GENERATED_TEXTPROTO =
      "# This file is auto generated -- DO NOT EDIT\n";

  public AssetsWriter(String assetsDirectory) {
    this.assetsDirectory = assetsDirectory;
  }

  public boolean saveDevTuningForkProto(
      List<EnumDataModel> enums,
      MessageDataModel annotationMessage,
      MessageDataModel fidelityMessage) {
    File file = new File(assetsDirectory, "dev_tuningfork.proto");
    if (!file.mkdirs()) {
      return false;
    }
    try (FileWriter fileWriter = new FileWriter(file)) {
      fileWriter.write(AUTO_GENERATED_PROTO);
      fileWriter.write("syntax = \"proto3\";\n\n");
      for (EnumDataModel enumDataModel : enums) {
        fileWriter.write(enumDataModel.toString());
      }
      fileWriter.write(annotationMessage.toString());
      fileWriter.write(fidelityMessage.toString());
      fileWriter.write(AUTO_GENERATED_PROTO);
      return true;
    } catch (IOException e) {
      e.printStackTrace();
    }
    return false;
  }

  public boolean saveDevFidelityParams(List<QualityDataModel> qualityDataModel) {
    return IntStream.range(0, qualityDataModel.size())
            .filter(i -> saveDevFidelityParams(qualityDataModel.get(i), i + 1))
            .count()
        == qualityDataModel.size();
  }

  public boolean saveDevFidelityParams(QualityDataModel qualityDataModel, int fileNumber) {
    File file = new File(assetsDirectory, "dev_tuningfork_fidelityparams_" + fileNumber + ".txt");
    if (!file.mkdirs()) {
      return false;
    }
    try (FileWriter fileWriter = new FileWriter(file)) {
      fileWriter.write(AUTO_GENERATED_TEXTPROTO);
      fileWriter.write(qualityDataModel.toString());
      fileWriter.write(AUTO_GENERATED_TEXTPROTO);
      return true;
    } catch (IOException e) {
      e.printStackTrace();
    }
    return false;
  }
}
