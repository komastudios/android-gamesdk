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

#include "histogram.h"
#include "metricdata.h"
#include "settings.h"

namespace tuningfork {

struct LoadingTimeMetric {
    LoadingTimeMetric(SerializedAnnotation annotation = {})
        : annotation_(annotation) {}
    SerializedAnnotation annotation_;
};

struct LoadingTimeMetricData : public MetricData {
    LoadingTimeMetricData(LoadingTimeMetric metric)
        : MetricData(MetricDataType()),
          metric_(metric),
          histogram_(Settings::Histogram{}, true),
          duration_(Duration::zero()) {}
    LoadingTimeMetric metric_;
    // TODO (willosborn): refactor histogram into separate time-series class
    Histogram<double> histogram_;
    Duration duration_;
    void Record(Duration dt);
    virtual void Clear() override { histogram_.Clear(); }
    virtual size_t Count() const override { return histogram_.Count(); }
    static MetricData::Type MetricDataType() {
        return MetricData::Type::LOADING_TIME;
    }
};

}  // namespace tuningfork
