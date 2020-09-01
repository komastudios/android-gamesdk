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
import Model.QualityDataModel;
import Utils.Validation.ValidationTool;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import org.junit.Assert;
import org.junit.Test;

public class ValidationToolTest {

  @Test
  public void validateDevFidelityParams() {
    List<QualityDataModel> devFidelityParams = new ArrayList<>();
    List<EnumDataModel> fidelityEnums = new ArrayList<>();

    EnumDataModel enum1 = new EnumDataModel("QualitySettings", new ArrayList<>(
        Arrays.asList("UNKNOWN", "FAST", "SIMPLE", "GOOD", "BEAUTIFUL", "FANTASTIC")));
    EnumDataModel enum2 = new EnumDataModel("Speed",
        new ArrayList<>(Arrays.asList("UNKNOWN", "MODERATE", "FAST", "SUPER FAST", "EXTREME")));
    fidelityEnums.add(enum1);
    fidelityEnums.add(enum2);

    QualityDataModel devParams1 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings", "Speed"),
        Arrays.asList("1", "1.21", "SIMPLE", "MODERATE"));
    QualityDataModel devParams2 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings"),
        Arrays.asList("1.2", "1.212", "SIMPLE", "FAST"));
    QualityDataModel devParams3 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings"),
        Arrays.asList("1.2", "1.213", "SIMPLE", "FAST"));
    QualityDataModel devParams4 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings"),
        Arrays.asList("1.3", "1.213", "SIMPLE", "MODERATE"));
    devFidelityParams.add(devParams1);
    devFidelityParams.add(devParams2);
    devFidelityParams.add(devParams3);
    devFidelityParams.add(devParams4);

    Assert.assertTrue(ValidationTool.isIncreasingSingleField(devFidelityParams, fidelityEnums, 0));
    Assert.assertFalse(ValidationTool.isDecreasingSingleField(devFidelityParams, fidelityEnums, 0));

    Assert.assertTrue(ValidationTool.isIncreasingSingleField(devFidelityParams, fidelityEnums, 1));
    Assert.assertFalse(ValidationTool.isDecreasingSingleField(devFidelityParams, fidelityEnums, 1));

    Assert.assertTrue(ValidationTool.isIncreasingSingleField(devFidelityParams, fidelityEnums, 2));
    Assert.assertTrue(ValidationTool.isDecreasingSingleField(devFidelityParams, fidelityEnums, 2));

    Assert.assertFalse(ValidationTool.isIncreasingSingleField(devFidelityParams, fidelityEnums, 3));
    Assert.assertFalse(ValidationTool.isDecreasingSingleField(devFidelityParams, fidelityEnums, 3));
  }
}
