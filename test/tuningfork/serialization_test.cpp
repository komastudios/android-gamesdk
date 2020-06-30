/*
 * Copyright 2019 The Android Open Source Project
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

#include "tf_test_utils.h"

#include <gtest/gtest.h>
#include "http_backend/json_serializer.h"
#include "core/tuningfork_utils.h"

#include <chrono>

namespace serialization_test {

using namespace tuningfork;
using namespace json11;
using namespace test;
using namespace std::chrono;

RequestInfo test_device_info {"expt", {}, "sess", 2387, 349587, "fing", "6.3", {1,2,3}, "packname",
                                  0, 10, "MODEL", "BRAND", "PRODUCT", "DEVICE"};
std::string test_device_info_ser = R"TF({
  "brand": "BRAND",
  "build_version": "6.3",
  "cpu_core_freqs_hz": [1, 2, 3],
  "device": "DEVICE",
  "fingerprint": "fing",
  "gles_version": {
    "major": 5, "minor": 21907
  },
  "model": "MODEL",
  "product": "PRODUCT",
  "total_memory_bytes": 2387
})TF";

void CheckDeviceInfo(const RequestInfo& info) {
    EXPECT_EQ(json_utils::GetResourceName(info),
              "applications/packname/apks/0") << "GetResourceName";
    EXPECT_TRUE(CompareIgnoringWhitespace(Json(json_utils::DeviceSpecJson(info)).dump(),
                                          test_device_info_ser))<< "DeviceSpecJson";
}

TEST(SerializationTest, DeviceInfo) {
    CheckDeviceInfo(test_device_info);
}

std::string report_start = R"TF({
  "name": "applications/packname/apks/0",
  "session_context": {
    "device": {
      "brand": "BRAND",
      "build_version": "6.3",
      "cpu_core_freqs_hz": [1, 2, 3],
      "device": "DEVICE",
      "fingerprint": "fing",
      "gles_version": {
        "major": 5,
        "minor": 21907
      },
      "model": "MODEL",
      "product": "PRODUCT",
      "total_memory_bytes": 2387
    },
    "game_sdk_info": {
      "session_id": "sess",
      "version": "1.0"
    },
    "time_period": {
      "end_time": "1970-01-01T00:00:00.000000Z",
      "start_time": "1970-01-01T00:00:00.000000Z"
    }
  },
  "telemetry": [)TF";
std::string report_end = "]}";

std::string single_tick_with_loading = R"TF({
  "context": {
    "annotations": "",
    "duration": "0.755s",
    "tuning_parameters": {
      "experiment_id": "expt",
      "serialized_fidelity_parameters": ""
    }
  },
  "report": {
    "loading": {
      "loading_events": {
        "times_ms": [1500]
      }
    },
    "rendering": {
      "render_time_histogram": [{"counts": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      "instrument_id": 1
      }]
    }
  }
})TF";

TEST(SerializationTest, SerializationWithLoading) {
    Session prong_cache(2/*size*/, 2/*max_instrumentation_keys*/,
                           { Settings::DefaultHistogram(1) },
                           [](uint64_t){ return SerializedAnnotation(); },
                           [](uint64_t id){ return id == 0; },
                           nullptr);
    std::string evt_ser;
    JsonSerializer::SerializeEvent(prong_cache, test_device_info, evt_ser);
    auto empty_report = report_start + report_end;
    EXPECT_TRUE(CompareIgnoringWhitespace(evt_ser, empty_report))
        << evt_ser << "\n!=\n" << empty_report;

    // Fill in some data
    auto p1 = prong_cache.Get(0);
    EXPECT_NE(p1, nullptr);
    p1->Trace(milliseconds(1500));

    auto p2 = prong_cache.Get(1);
    EXPECT_NE(p2, nullptr);
    p2->Trace(milliseconds(10));

    JsonSerializer::SerializeEvent(prong_cache, test_device_info, evt_ser);
    auto report = report_start + single_tick_with_loading + report_end;
    EXPECT_TRUE(CompareIgnoringWhitespace(evt_ser, report)) << evt_ser << "\n!=\n" << report;
}

std::string single_tick = R"TF({
  "context": {
    "annotations": "",
    "duration": "0.03s",
    "tuning_parameters": {
      "experiment_id": "expt",
      "serialized_fidelity_parameters": ""
    }
  },
  "report": {
    "rendering": {
      "render_time_histogram": [{
        "counts": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        "instrument_id": 0
      }]
    }
  }
})TF";

class TestIdProvider : public IdProvider{
  public:
    uint64_t DecodeAnnotationSerialization(const ProtobufSerialization& ser,
                                           bool* loading) const override {
        return 0;
    }
    TuningFork_ErrorCode MakeCompoundId(InstrumentationKey k,
                               uint64_t annotation_id,
                               uint64_t& id) override {
        id = k;
        return TUNINGFORK_ERROR_OK;
    }
};


void CheckSessions(const Session& pc0, const Session& pc1) {
    Prong* p0 = pc0.Get(0);
    Prong* p1 = pc1.Get(0);
    EXPECT_TRUE(p0!=nullptr && p1!=nullptr);
    EXPECT_EQ(p0->histogram_, p1->histogram_);
}

Settings::Histogram DefaultHistogram() {
    return {-1,10,40,30};
}

TEST(SerializationTest, GEDeserialization) {
    Session prong_cache(1/*size*/, 1/*max_instrumentation_keys*/, {DefaultHistogram()},
                           [](uint64_t){ return SerializedAnnotation(); },
                           [](uint64_t){ return false; },
                           nullptr);
    std::string evt_ser;
    JsonSerializer::SerializeEvent(prong_cache, test_device_info, evt_ser);
    auto empty_report = report_start + report_end;
    EXPECT_TRUE(CompareIgnoringWhitespace(evt_ser, empty_report))
        << evt_ser << "\n!=\n" << empty_report;
    // Fill in some data
    auto p = prong_cache.Get(0);
    p->Trace(milliseconds(30));
    JsonSerializer::SerializeEvent(prong_cache, test_device_info, evt_ser);
    auto report = report_start + single_tick + report_end;
    EXPECT_TRUE(CompareIgnoringWhitespace(evt_ser, report)) << evt_ser << "\n!=\n" << report;
    Session pc(1/*size*/, 1/*max_instrumentation_keys*/, {DefaultHistogram()},
                  [](uint64_t){ return SerializedAnnotation(); },
                  [](uint64_t){ return false; },
                  nullptr);
    TestIdProvider id_provider;
    EXPECT_EQ(JsonSerializer::DeserializeAndMerge(evt_ser, id_provider, pc), TUNINGFORK_ERROR_OK)
            << "Deserialize single";
    CheckSessions(pc, prong_cache);
}

} // namespace serialization_test
