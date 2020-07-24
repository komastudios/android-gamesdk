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
    LoadingTimeMetricData(MetricId metric_id)
        : MetricData(MetricType()),
          metric_id_(metric_id),
          histogram_(Settings::Histogram{}, true),
          duration_(Duration::zero()) {}
    MetricId metric_id_;
    // TODO (willosborn): refactor histogram into separate time-series class
    Histogram<double> histogram_;
    Duration duration_;
    void Record(Duration dt);
    virtual void Clear() override { histogram_.Clear(); }
    virtual size_t Count() const override { return histogram_.Count(); }
    static Metric::Type MetricType() { return Metric::Type::LOADING_TIME; }
};

}  // namespace tuningfork

namespace std {

// Define hash function for LoadingTimeMetadata
template <class T>
inline void hash_combine(std::size_t& s, const T& v) {
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}
template <>
class hash<LoadingTimeMetadata> {
   public:
    size_t operator()(const LoadingTimeMetadata& x) const {
        size_t result = 0;
        hash_combine(result, x.loadingState);
        hash_combine(result, x.source);
        hash_combine(result, x.compressionLevel);
        hash_combine(result, x.networkConnectivity);
        hash_combine(result, x.networkTransferSpeed_bps);
        hash_combine(result, x.networkLatency_ns);
        return result;
    }
};
}  // namespace std

// Operator== for custom LoadingTimeMetadata struct
inline bool operator==(const LoadingTimeMetadata& x,
                       const LoadingTimeMetadata& y) {
    return x.loadingState == y.loadingState && x.source == y.source &&
           x.compressionLevel == y.compressionLevel &&
           x.networkConnectivity == y.networkConnectivity &&
           x.networkTransferSpeed_bps == y.networkTransferSpeed_bps &&
           x.networkLatency_ns == y.networkLatency_ns;
}
