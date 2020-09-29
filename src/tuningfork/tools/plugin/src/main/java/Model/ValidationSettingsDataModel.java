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

package Model;


import Controller.Annotation.AnnotationTabController;

import java.util.List;
import java.util.stream.Collectors;

public class ValidationSettingsDataModel {
    private final List<String> fieldNames;
    private final List<List<String>> validationSettings;
    private final String api_key;
    private final String method;
    private final int intervalms_or_count;
    private final List<EnumDataModel> enumDataModelList;

    public ValidationSettingsDataModel(List<String> fieldNames,
                                       List<List<String>> validationSettings,
                                       String api_key, String method,
                                       String intervalms_or_count,
                                       List<EnumDataModel> enumDataModelList) {
        this.fieldNames = fieldNames.stream().map((s) ->
                s.split(" ")[0]).collect(Collectors.toList());
        this.validationSettings = validationSettings;
        this.api_key = api_key;
        this.method = method;
        // TODO: add check that it is int or recreate another ui for that
        this.intervalms_or_count = Integer.parseInt(intervalms_or_count);
        this.enumDataModelList = enumDataModelList;
    }

    private StringBuilder getAggregationStrategy() {
        StringBuilder result = new StringBuilder();
        result.append("aggregation_strategy: {");
        result.append("method: ").append(this.method).append(", ");
        result.append("intervalms_or_count: ").append(this.intervalms_or_count).append(", ");
        int max_instrumentation_keys = 10;
        result.append("max_instrumentation_keys: ").append(max_instrumentation_keys).append(", ");
        result.append("annotation_enum_size: [");
        for (int i = 0; i < enumDataModelList.size(); i++) {
            result.append(enumDataModelList.get(i).size());
            if (i != enumDataModelList.size() - 1) {
                result.append(", ");
            }
        }
        result.append("]}");
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