package Model;

import android.os.SystemPropertiesProto;

import java.util.List;
import java.util.stream.Collectors;

public class ValidationSettingsDataModel {
    private List<String> fieldNames;
    private List<List<String>> validationSettings;
    private String api_key;
    private String method;

    public ValidationSettingsDataModel(List<String> fieldNames, List<List<String>> validationSettings,
                                       String api_key, String method) {
        this.fieldNames = fieldNames.stream().map((s) -> s.split(" ")[0]).collect(Collectors.toList());
        this.validationSettings = validationSettings;
        this.api_key = api_key;
        this.method = method;
    }

    private StringBuilder getAggregationStrategy() {
        StringBuilder result = new StringBuilder();
        result.append("aggregation_strategy: {");

        {
            result.append(String.format("method: %s, ", this.method));
        }

        {
            int intervalms_or_count = 10000;
            result.append(String.format("intervalms_or_count: %d, ", intervalms_or_count));
        }

        {
            int max_instrumentation_keys = 10;
            result.append(String.format("max_instrumentation_keys: %d, ", max_instrumentation_keys));
        }

        {
            int first = 3;
            int second = 4;
            result.append(String.format("annotation_enum_size: [%d,%d]", first, second));
        }

        result.append("}");
        return result;
    }

    private StringBuilder getHistograms() {
        StringBuilder result = new StringBuilder();
        result.append("histograms: [");
        for (int i = 0; i < validationSettings.size(); i++) {
            result.append("{");
            for (int j = 0; j < fieldNames.size(); j++) {
                result.append(fieldNames.get(j) + ": " + validationSettings.get(i).get(j));
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
        {
            String base_url = "https://performanceparameters.googleapis.com/v1/";
            result.append(String.format("base_url: %s,\n", base_url));
        }

        {
            result.append(String.format("api_key: %s,\n", this.api_key));
        }

        {
            String default_val_set_params_filename = "dev_tuningfork_validationsettingsparams.bin";
            result.append(String.format("default_fidelity_parameters_filename: \"%s\"\n",
                    default_val_set_params_filename));
        }
        {
            int initial_request_timeout_ms = 1000;
            result.append(String.format("initial_request_timeout_ms: %d,\n", initial_request_timeout_ms));

        }
        {
            int ultimate_request_timeout_ms = 10000;
            result.append(String.format("ultimate_request_timeout_ms: %d,\n", ultimate_request_timeout_ms));
        }
        {
            int loading_annotation_index = 1;
            result.append(String.format("loading_annotation_index: %d,\n", loading_annotation_index));
        }
        {
            int level_annotation_index = 5;
            result.append(String.format("level_annotation_index: %d\n", level_annotation_index));
        }
        return result;
    }

    public String toString() {
        StringBuilder result = new StringBuilder();
        result.append(getAggregationStrategy());
        result.append('\n');
        result.append(getHistograms());
        result.append(getOtherSettings());
        System.out.println(result.toString());
        return result.toString();
    }
}
