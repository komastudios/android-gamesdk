package Model;


import java.util.List;
import java.util.stream.Collectors;

public class ValidationSettingsDataModel {
    private List<String> fieldNames;
    private List<List<String>> validationSettings;
    private String api_key;
    private String method;

    public ValidationSettingsDataModel(List<String> fieldNames,
                                       List<List<String>> validationSettings,
                                       String api_key, String method) {
        this.fieldNames = fieldNames.stream().map((s) ->
                s.split(" ")[0]).collect(Collectors.toList());
        this.validationSettings = validationSettings;
        this.api_key = api_key;
        this.method = method;
    }

    private StringBuilder getAggregationStrategy() {
        StringBuilder result = new StringBuilder();
        result.append("aggregation_strategy: {");
        result.append("method: ").append(this.method).append(", ");
        int intervalms_or_count = 10000;
        result.append("intervalms_or_count: ").append(intervalms_or_count).append(", ");
        int max_instrumentation_keys = 10;
        result.append("max_instrumentation_keys: ").append(max_instrumentation_keys).append(", ");
        int first = 3;
        int second = 4;
        result.append("annotation_enum_size: [").append(first).append(", ").append(second).append("], ");
        result.append("}");
        return result;
    }

    private StringBuilder getHistograms() {
        StringBuilder result = new StringBuilder();
        result.append("histograms: [");
        for (int i = 0; i < validationSettings.size(); i++) {
            result.append("{");
            for (int j = 0; j < fieldNames.size(); j++) {
                result.append(fieldNames.get(j)).append(": ").append(validationSettings.get(i).get(j));
                if (j != fieldNames.size() - 1) {
                    result.append(", ");
                }
            }
            result.append("}");
            if (i != validationSettings.size() - 1) {
                result.append(", ");
            }
        }
        result.append("]\n");
        return result;
    }

    private StringBuilder getOtherSettings() {
        StringBuilder result = new StringBuilder();
        result.append("base_url: ").append("https://performanceparameters.googleapis.com/v1/").append(", \n");
        result.append("api_key: ").append(this.api_key).append(", \n");
        result.append("default_fidelity_parameters_filename: ")
                .append("dev_tuningfork_validationsettingsparams.bin").append(", \n");
        int initial_request_timeout_ms = 1000;
        result.append("initial_request_timeout_ms: ").append(initial_request_timeout_ms).append(", \n");
        int ultimate_request_timeout_ms = 10000;
        result.append("ultimate_request_timeout_ms: ").append(ultimate_request_timeout_ms).append(", \n");
        int loading_annotation_index = 1;
        result.append("loading_annotation_index: ").append(loading_annotation_index).append(", \n");
        int level_annotation_index = 5;
        result.append("level_annotation_index: ").append(level_annotation_index).append("\n");
        return result;
    }

    public String toString() {
        StringBuilder result = new StringBuilder();
        result.append(getAggregationStrategy()).append("\n");
        result.append(getHistograms());
        result.append(getOtherSettings());
        System.out.println(result.toString());
        return result.toString();
    }
}
