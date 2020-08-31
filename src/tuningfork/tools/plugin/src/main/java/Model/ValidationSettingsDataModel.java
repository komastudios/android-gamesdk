package Model;

import android.os.SystemPropertiesProto;

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

    private String getAggregationStrategy() {
        String result = "";
        result += "aggregation_strategy: {";
        result += String.format("method: %s, ", this.method);
        int intervalms_or_count = 10000;
        result += String.format("intervalms_or_count: %d, ", intervalms_or_count);
        int max_instrumentation_keys = 10;
        result += String.format("max_instrumentation_keys: %d, ", max_instrumentation_keys);
        int first = 3;
        int second = 4;
        result += String.format("annotation_enum_size: [%d,%d]", first, second);
        result += "}";
        return result;
    }

    private String getHistograms() {
        String result = "histograms: [";
        for (int i = 0; i < validationSettings.size(); i++) {
            result += "{";
            for (int j = 0; j < fieldNames.size(); j++) {
                result += fieldNames.get(j) + ": " + validationSettings.get(i).get(j);
                if (j != fieldNames.size() - 1) {
                    result += ", ";
                }
            }
            result += "}";
            if (i != validationSettings.size() - 1) {
                result += ", ";
            }
        }
        result += "]\n";
        return result;
    }

    private String getOtherSettings() {
        String result = "";
        String base_url = "https://performanceparameters.googleapis.com/v1/";
        result += String.format("base_url: %s,\n", base_url);
        result += String.format("api_key: %s,\n", this.api_key);
        String default_val_set_params_filename = "dev_tuningfork_validationsettingsparams.bin";
        result += String.format("default_fidelity_parameters_filename: \"%s\"\n",
                default_val_set_params_filename);
        int initial_request_timeout_ms = 1000;
        result += String.format("initial_request_timeout_ms: %d,\n", initial_request_timeout_ms);
        int ultimate_request_timeout_ms = 10000;
        result += String.format("ultimate_request_timeout_ms: %d,\n", ultimate_request_timeout_ms);
        int loading_annotation_index = 1;
        result += String.format("loading_annotation_index: %d,\n", loading_annotation_index);
        int level_annotation_index = 5;
        result += String.format("level_annotation_index: %d\n", level_annotation_index);
        return result;
    }

    public String toString() {
        String result = "";
        result += getAggregationStrategy() + "\n";
        result += getHistograms();
        result += getOtherSettings();
        System.out.println(result);
        return result;
    }
}
