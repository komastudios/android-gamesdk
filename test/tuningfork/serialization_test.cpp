#include <gtest/gtest.h>
#include "tuningfork/ge_serializer.h"
#include "tuningfork/tuningfork_utils.h"

namespace serialization_test {

using namespace tuningfork;
using namespace json11;

static std::string ReplaceReturns(std::string in) {
    std::replace( in.begin(), in.end(), '\n', ' ');
    return in;
}

void CheckDeviceInfo(const ExtraUploadInfo& info) {
    EXPECT_EQ(json_utils::GetResourceName(info),
              "applications/packname/apks/0") << "GetResourceName";
    EXPECT_EQ(Json(json_utils::DeviceSpecJson(info)).dump(),
ReplaceReturns(R"TF({"build_version": "6.3",
"cpu_core_freqs_hz": [1, 2, 3],
"fingerprint": "fing",
"gles_version": {"major": 5, "minor": 21907},
"total_memory_bytes": 2387})TF"))<< "DeviceSpecJson";
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
