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

namespace serialization_test {

using namespace tuningfork;
using namespace json11;
using namespace test;

void CheckDeviceInfo(const ExtraUploadInfo& info) {
    EXPECT_EQ(json_utils::GetResourceName(info),
              "applications/packname/apks/0") << "GetResourceName";
    EXPECT_TRUE(CompareIgnoringWhitespace(Json(json_utils::DeviceSpecJson(info)).dump(),
R"TF({
  "build_version": "6.3",
  "cpu_core_freqs_hz": [1, 2, 3],
  "fingerprint": "fing",
  "gles_version": {
    "major": 5, "minor": 21907
  },
  "total_memory_bytes": 2387
})TF")) << "DeviceSpecJson";
}
void CheckFull(const ProngCache& pc,
               const ProtobufSerialization& fps,
               const ExtraUploadInfo& device_info) {
    std::string evt_json_ser;
    GESerializer::SerializeEvent(pc, fps, device_info, evt_json_ser);
    EXPECT_EQ(evt_json_ser, "") << "SerializeEvent";
}

TEST(SerializationTest, DeviceInfo) {
    CheckDeviceInfo({"expt", "sess", 2387, 349587, "fing", "6.3", {1,2,3}, "packname"});
}

} // namespace serialization_test
