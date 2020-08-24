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
import Utils.Validation.ErrorCollector;
import Utils.Validation.ErrorType;
import Utils.Validation.ValidationTool;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;
import org.junit.Assert;
import org.junit.Test;

public class ValidationToolTest {

  @Test
  public void correctDevFidelityParams() {
    List<QualityDataModel> devFidelityParams = new ArrayList<>();
    List<EnumDataModel> fidelityEnums = new ArrayList<>();

    EnumDataModel enum1 = new EnumDataModel("QualitySettings", new ArrayList<>(
        Arrays.asList("UNKNOWN", "FAST", "SIMPLE", "GOOD", "BEAUTIFUL", "FANTASTIC")));
    EnumDataModel enum2 = new EnumDataModel("Speed",
        new ArrayList<>(Arrays.asList("UNKNOWN", "MODERATE", "FAST", "SUPER FAST", "EXTREME")));
    fidelityEnums.add(enum1);
    fidelityEnums.add(enum2);

    QualityDataModel devParams1 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings", "field4", "Speed"),
        Arrays.asList("1", "1.21", "SIMPLE", "-3", "MODERATE"));
    QualityDataModel devParams2 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings", "field4", "Speed"),
        Arrays.asList("1.2", "1.212", "SIMPLE", "-3.2", "MODERATE"));
    QualityDataModel devParams3 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings", "field4", "Speed"),
        Arrays.asList("1.2", "1.213", "SIMPLE", "-5", "MODERATE"));
    QualityDataModel devParams4 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings", "field4", "Speed"),
        Arrays.asList("1.3", "1.213", "SIMPLE", "-100", "FAST"));
    devFidelityParams.add(devParams1);
    devFidelityParams.add(devParams2);
    devFidelityParams.add(devParams3);
    devFidelityParams.add(devParams4);

    ErrorCollector errors = new ErrorCollector();
    ValidationTool.validateDevFidelityParamsOrder(devFidelityParams, fidelityEnums, errors);

    Assert.assertEquals(errors.getErrorCount(), 0);
    Assert.assertEquals(errors.getWarningCount(), 0);
  }

  @Test
  public void enumZero() {
    List<QualityDataModel> devFidelityParams = new ArrayList<>();
    List<EnumDataModel> fidelityEnums = new ArrayList<>();

    EnumDataModel enum1 = new EnumDataModel("QualitySettings", new ArrayList<>(
        Arrays.asList("UNKNOWN", "FAST", "SIMPLE", "GOOD", "BEAUTIFUL", "FANTASTIC")));
    EnumDataModel enum2 = new EnumDataModel("Speed",
        new ArrayList<>(Arrays.asList("UNKNOWN", "MODERATE", "FAST", "SUPER FAST", "EXTREME")));
    fidelityEnums.add(enum1);
    fidelityEnums.add(enum2);

    QualityDataModel devParams1 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings", "field4", "Speed"),
        Arrays.asList("1", "1.21", "SIMPLE", "-3", "UNKNOWN"));
    QualityDataModel devParams2 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings", "field4", "Speed"),
        Arrays.asList("1.2", "1.212", "UNKNOWN", "-3.2", "MODERATE"));
    devFidelityParams.add(devParams1);
    devFidelityParams.add(devParams2);

    ErrorCollector errors = new ErrorCollector();
    ValidationTool.validateDevFidelityParamsOrder(devFidelityParams, fidelityEnums, errors);

    Assert.assertTrue(errors.getWarningCount(ErrorType.DEV_FIDELITY_PARAMETERS_ENUMS_ZERO) > 0);
  }

  @Test
  public void incorrectOrder() {
    List<QualityDataModel> devFidelityParams = new ArrayList<>();
    List<EnumDataModel> fidelityEnums = new ArrayList<>();

    EnumDataModel enum1 = new EnumDataModel("QualitySettings", new ArrayList<>(
        Arrays.asList("UNKNOWN", "FAST", "SIMPLE", "GOOD", "BEAUTIFUL", "FANTASTIC")));
    EnumDataModel enum2 = new EnumDataModel("Speed",
        new ArrayList<>(Arrays.asList("UNKNOWN", "MODERATE", "FAST", "SUPER FAST", "EXTREME")));
    fidelityEnums.add(enum1);
    fidelityEnums.add(enum2);

    QualityDataModel devParams1 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings", "field4", "Speed"),
        Arrays.asList("1", "1.21", "FAST", "-3", "MODERATE"));
    QualityDataModel devParams2 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings", "field4", "Speed"),
        Arrays.asList("1.2", "1.212", "SIMPLE", "-3.2", "MODERATE"));
    QualityDataModel devParams3 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings", "field4", "Speed"),
        Arrays.asList("1.2", "1.213", "SIMPLE", "-5", "MODERATE"));
    QualityDataModel devParams4 = new QualityDataModel(Arrays.asList("field1", "field2",
        "QualitySettings", "field4", "Speed"),
        Arrays.asList("1.3", "1.213", "FAST", "-100", "FAST"));
    devFidelityParams.add(devParams1);
    devFidelityParams.add(devParams2);
    devFidelityParams.add(devParams3);
    devFidelityParams.add(devParams4);

    ErrorCollector errors = new ErrorCollector();
    ValidationTool.validateDevFidelityParamsOrder(devFidelityParams, fidelityEnums, errors);

    Assert.assertEquals(errors.getWarningCount(ErrorType.DEV_FIDELITY_PARAMETERS_ORDER), 1);
  }
}
