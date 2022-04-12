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

struct FrameTimeMetric {
    FrameTimeMetric(uint32_t ikey_index = 0,
                    SerializedAnnotation annotation = {})
        : instrumentation_key_index_(ikey_index), annotation_(annotation) {}
    uint32_t instrumentation_key_index_;
    SerializedAnnotation annotation_;
};

struct FrameTimeMetricData : public MetricData {
    FrameTimeMetricData(MetricId metric_id, const Settings::Histogram& settings)
        : MetricData(MetricType()),
          metric_id_(metric_id),
          histogram_(settings, false /*isLoading*/),
          last_time_(TimePoint::min()),
          duration_(Duration::zero()) {}
    MetricId metric_id_;
    Histogram<double> histogram_;
    TimePoint last_time_;
    Duration duration_;
    void Tick(TimePoint t, bool record = true);
    void Record(Duration dt);
    virtual void Clear() override;
    virtual size_t Count() const override { return histogram_.Count(); }
    static Metric::Type MetricType() { return Metric::Type::FRAME_TIME; }
};

}  // namespace tuningfork
