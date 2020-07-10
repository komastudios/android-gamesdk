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

#include <cstdint>
#include <unordered_map>

#include "memory_record_type.h"

namespace tuningfork {

struct Metric {
    enum Type : uint8_t { FRAME_TIME = 0, LOADING_TIME, MEMORY, ERROR = 0xff };
};

/*
 * Metrics stored in a session are keyed by a MetricId.
 * Each metric type can have an annotation and other specific information in the
 * detail field.
 */
struct MetricId {
    union {
        uint64_t base;
        struct {
            uint32_t annotation;
            union {
                struct {
                    uint16_t ikey;
                } frame_time;
                struct {
                } loading_time;
                struct {
                    uint8_t record_type;
                } memory;
            };
            Metric::Type type;
        } detail;
    };
    bool operator==(const MetricId& x) const { return base == x.base; }

    static MetricId FrameTime(AnnotationId aid, uint16_t ikey) {
        MetricId id{0};
        id.detail.type = Metric::FRAME_TIME;
        id.detail.frame_time.ikey = ikey;
        id.detail.annotation = aid;
        return id;
    }
    static MetricId LoadingTime(AnnotationId aid) {
        MetricId id{0};
        id.detail.type = Metric::LOADING_TIME;
        id.detail.annotation = aid;
        return id;
    }
    static MetricId Memory(MemoryRecordType record_type) {
        MetricId id{0};
        id.detail.type = Metric::MEMORY;
        id.detail.memory.record_type = record_type;
        return id;
    }
};

}  // namespace tuningfork

// Define hash function for MetricId
namespace std {
template <>
class hash<tuningfork::MetricId> {
   public:
    size_t operator()(const tuningfork::MetricId& x) const {
        return hash<uint64_t>()(x.base);
    }
};
}  // namespace std
