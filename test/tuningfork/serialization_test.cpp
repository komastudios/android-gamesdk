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
#include "tuningfork/ge_serializer.h"
#include "tuningfork/tuningfork_utils.h"

#include <chrono>

namespace serialization_test {

using namespace tuningfork;
using namespace json11;
using namespace test;
using namespace std::chrono;

ExtraUploadInfo test_device_info {"expt", "sess", 2387, 349587, "fing", "6.3", {1,2,3}, "packname"};
std::string test_device_info_ser = R"TF({
  "build_version": "6.3",
  "cpu_core_freqs_hz": [1, 2, 3],
  "fingerprint": "fing",
  "gles_version": {
    "major": 5, "minor": 21907
  },
  "total_memory_bytes": 2387
})TF";

void CheckDeviceInfo(const ExtraUploadInfo& info) {
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
      "build_version": "6.3",
      "cpu_core_freqs_hz": [1, 2, 3],
      "fingerprint": "fing",
      "gles_version": {
        "major": 5,
        "minor": 21907
      },
      "total_memory_bytes": 2387
    },
    "game_sdk_info": {
      "session_id": "sess",
      "version": "0.4"
    },
    "time_period": {
      "end_time": "1970-01-01T00:00:00.000000Z",
      "start_time": "1970-01-01T00:00:00.000000Z"
    }
  },
  "telemetry": [)TF";
std::string report_end = "]}";

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
    TFErrorCode MakeCompoundId(InstrumentationKey k,
                               uint64_t annotation_id,
                               uint64_t& id) override {
        id = k;
        return TFERROR_OK;
    }
};


void CheckProngCaches(const ProngCache& pc0, const ProngCache& pc1) {
    Prong* p0 = pc0.Get(0);
    Prong* p1 = pc1.Get(0);
    EXPECT_TRUE(p0!=nullptr && p1!=nullptr);
    EXPECT_EQ(p0->histogram_, p1->histogram_);
}

TEST(SerializationTest, GEDeserialization) {
    ProngCache prong_cache(1/*size*/, 1/*max_instrumentation_keys*/, {DefaultHistogram()},
                           [](uint64_t){ return SerializedAnnotation(); },
                           [](uint64_t){ return false; });
    ProtobufSerialization fidelity_params;
    ExtraUploadInfo device_info;
    std::string evt_ser;
    GESerializer::SerializeEvent(prong_cache, fidelity_params, test_device_info, evt_ser);
    auto empty_report = report_start + report_end;
    EXPECT_TRUE(CompareIgnoringWhitespace(evt_ser, empty_report))
        << evt_ser << "\n!=\n" << empty_report;
    // Fill in some data
    auto p = prong_cache.Get(0);
    p->Trace(milliseconds(30));
    GESerializer::SerializeEvent(prong_cache, fidelity_params, test_device_info, evt_ser);
    auto report = report_start + single_tick + report_end;
    EXPECT_TRUE(CompareIgnoringWhitespace(evt_ser, report)) << evt_ser << "\n!=\n" << report;
    ProngCache pc(1/*size*/, 1/*max_instrumentation_keys*/, {DefaultHistogram()},
                  [](uint64_t){ return SerializedAnnotation(); },
                  [](uint64_t){ return false; });
    TestIdProvider id_provider;
    EXPECT_EQ(GESerializer::DeserializeAndMerge(evt_ser, id_provider, pc), TFERROR_OK)
            << "Deserialize single";
    CheckProngCaches(pc, prong_cache);
}

} // namespace serialization_test
