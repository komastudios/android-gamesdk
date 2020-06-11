/*
 * Copyright 2020 The Android Open Source Project
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
 * limitations under the License.
 */

#pragma once

#include "core/common.h"
#include <string>
#include <vector>

namespace tuningfork {

struct Settings {
    struct Histogram {
        int32_t instrument_key;
        float bucket_min;
        float bucket_max;
        int32_t n_buckets;
    };
    struct AggregationStrategy {
        enum class Submission {
            TICK_BASED,
            TIME_BASED
        };
        Submission method;
        uint32_t intervalms_or_count;
        uint32_t max_instrumentation_keys;
        std::vector<uint32_t> annotation_enum_size;
    };
    TuningFork_Settings c_settings;
    AggregationStrategy aggregation_strategy;
    std::vector<Histogram> histograms;
    std::string base_uri;
    std::string api_key;
    std::string default_fidelity_parameters_filename;
    uint32_t initial_request_timeout_ms;
    uint32_t ultimate_request_timeout_ms;
    int32_t loading_annotation_index;
    int32_t level_annotation_index;

    std::string EndpointUri() const {
        std::string uri;
        if (c_settings.endpoint_uri_override==nullptr)
            uri = base_uri;
        else
            uri = c_settings.endpoint_uri_override;
        if (!uri.empty() && uri.back()!='/')
            uri += '/';
        return uri;
    }

    // The default histogram that is used if the user doesn't specify one in Settings
    static Settings::Histogram DefaultHistogram(InstrumentationKey ikey);
};

} // namespace tuningfork
