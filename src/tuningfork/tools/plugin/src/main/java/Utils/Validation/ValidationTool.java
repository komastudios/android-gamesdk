package Utils.Validation;

import Model.EnumDataModel;
import Model.QualityDataModel;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public final class ValidationTool {

  private static ArrayList[] getQualityForOneField(
      List<QualityDataModel> qualityDataModels) {
    int fieldCount = qualityDataModels.get(0).getFieldCount();
    ArrayList<String>[] qualityForOneField = new ArrayList[fieldCount];

    for (int i = 0; i < fieldCount; i++) {
      qualityForOneField[i] = new ArrayList<>();
    }

    for (QualityDataModel qualityDataModel : qualityDataModels) {
      List<String> values = qualityDataModel.getFieldValues();
      for (int field = 0; field < fieldCount; field++) {
        qualityForOneField[field].add(values.get(field));
      }
    }

    return qualityForOneField;
  }

  /*
   * Transforms optionsToBeTransformed (which are the names of the enum options that are
   * set) to a list of value's index in the options list of the enum they belong to.
   */
  private static List<String> getEnumIndexes(List<String> optionsToBeTransformed,
      List<QualityDataModel> qualityDataModels, List<String> currentOptions) {
    return IntStream
        .range(0, qualityDataModels.size() - 1)
        .mapToObj(settingsNumber -> IntStream
            .range(0, currentOptions.size())
            .filter(optionsIndex -> currentOptions.get(optionsIndex)
                .equals(optionsToBeTransformed.get(settingsNumber))).boxed()
            .collect(Collectors.toList())
            .get(0)
            .toString())
        .collect(Collectors.toList());
  }

  private static boolean hasZeroEnum(ArrayList<String> indexes) {
    return indexes.stream().filter(index -> index.equals("0")).collect(Collectors.toList()).size()
        > 0;
  }

  private static void transformEnumOptionsToIndex(List<QualityDataModel> qualityDataModels,
      List<EnumDataModel> fidelityEnums, ArrayList[] qualityParams, ErrorCollector errors) {
    int fieldCount = qualityDataModels.get(0).getFieldCount();
    for (int qualityColumn = 0; qualityColumn < fieldCount; qualityColumn++) {
      String firstValue = (String) qualityParams[qualityColumn].get(0);
      if (!firstValue.matches("[-+]?[0-9]*\\.?[0-9]+")) {
        String enumName = qualityDataModels.get(0).getFieldNames().get(qualityColumn);
        EnumDataModel currentEnum = fidelityEnums.stream()
            .filter(enumDataModel -> enumDataModel.getName().equals(enumName))
            .collect(Collectors.toList())
            .get(0);
        ArrayList<String> indexes = (ArrayList<String>) getEnumIndexes(qualityParams[qualityColumn],
            qualityDataModels,
            currentEnum.getOptions());

        if (hasZeroEnum(indexes)) {
          errors.addWarning(ErrorType.DEV_FIDELITY_PARAMETERS_ENUMS_ZERO,
              "Enum fields can't have 0 values");
        }

        qualityParams[qualityColumn] = indexes;
      }
    }
  }

  /*
   * Validate content of fidelity parameters from all dev_tuningfork_fidelityparams_*.bin files.
   * These should be in either increasing/decreasing order.
   */
  public static void validateDevFidelityParamsOrder(
      List<QualityDataModel> qualityDataModels,
      List<EnumDataModel> fidelityEnums,
      ErrorCollector errors) {
    ArrayList[] qualityParams = getQualityForOneField(qualityDataModels);
    transformEnumOptionsToIndex(qualityDataModels, fidelityEnums, qualityParams, errors);
    int qualitySettingsCount = qualityDataModels.size();
    int fidelityParamsCount = qualityParams.length;

    long isIncreasing = IntStream.range(0, fidelityParamsCount - 1)
        .filter(fidelityParamIndex -> IntStream.range(0, qualitySettingsCount - 2)
            .allMatch(
                setting -> Float.parseFloat((String) qualityParams[fidelityParamIndex].get(setting))
                    <= Float
                    .parseFloat((String) qualityParams[fidelityParamIndex].get(setting + 1))))
        .count();

    long isDecreasing = IntStream.range(0, fidelityParamsCount - 1)
        .filter(fidelityParamIndex -> IntStream.range(0, qualitySettingsCount - 2)
            .allMatch(
                setting -> Float.parseFloat((String) qualityParams[fidelityParamIndex].get(setting))
                    <= Float
                    .parseFloat((String) qualityParams[fidelityParamIndex].get(setting + 1))))
        .count();

    long allEqual = IntStream.range(0, fidelityParamsCount - 1)
        .filter(fidelityParamIndex -> IntStream.range(0, qualitySettingsCount - 2)
            .allMatch(
                setting -> Float.parseFloat((String) qualityParams[fidelityParamIndex].get(setting))
                    == Float
                    .parseFloat((String) qualityParams[fidelityParamIndex].get(setting + 1))))
        .count();

    if (isIncreasing + isDecreasing - allEqual != fidelityParamsCount) {
      errors
          .addWarning(ErrorType.DEV_FIDELITY_PARAMETERS_ORDER, "Fidelity parameters should be " +
              "in either increasing or decreasing order.");
    }
  }
}
