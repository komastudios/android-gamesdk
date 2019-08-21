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

#include "ge_serializer.h"

#include "tuningfork_utils.h"

#include <set>
#include <sstream>

#include "modp_b64.h"

namespace tuningfork {

using namespace json11;
using namespace std::chrono;

Json::object GameSdkInfoJson(const ExtraUploadInfo& device_info) {
    std::stringstream version_str;
    version_str << TUNINGFORK_MAJOR_VERSION << "." << TUNINGFORK_MINOR_VERSION;
    return Json::object {
        {"version", version_str.str() },
        {"session_id", device_info.session_id}
    };
}

std::string TimeToRFC3339(system_clock::time_point tp) {
    std::stringstream str;
    // TODO(b/139924411): date serialization
    return str.str();
}

std::string DurationToSecondsString(Duration d) {
    std::stringstream str;
    str << (duration_cast<nanoseconds>(d).count()/1000000000.0) << 's';
    return str.str();
}

std::string B64Encode(const std::vector<uint8_t>& bytes) {
    if (bytes.size()==0) return "";
    std::string enc(modp_b64_encode_len(bytes.size()), ' ');
    size_t l = modp_b64_encode(const_cast<char*>(enc.c_str()),
                    reinterpret_cast<const char*>(&bytes[0]), bytes.size());
    enc.resize(l);
    return enc;
}

Json::object TelemetryContextJson(const ProngCache& prong_cache,
                                  const SerializedAnnotation& annotation,
                                  const ProtobufSerialization& fidelity_params,
                                  const ExtraUploadInfo& device_info,
                                  const Duration& duration) {
    return Json::object {
        {"annotations", B64Encode(annotation)},
        {"tuning_parameters", Json::object {
                {"experiment_id", device_info.experiment_id},
                {"serialized_fidelity_parameters", B64Encode(fidelity_params)}
            }
        },
        {"duration", DurationToSecondsString(duration)}
    };
}

Json::object TelemetryReportJson(const ProngCache& prong_cache,
                                 const SerializedAnnotation& annotation,
                                 bool& empty, Duration& duration) {
    std::vector<Json::object> histograms;
    duration = Duration::zero();
    for(auto& p: prong_cache.prongs()) {
        if (p->Count()>0 && p->annotation_==annotation) {
            std::vector<int32_t> counts;
            for(auto& c: p->histogram_.buckets())
                counts.push_back(static_cast<int32_t>(c));
            histograms.push_back(Json::object {
                    {"instrument_id", p->instrumentation_key_},
                    {"counts", counts}
                });
            duration += p->duration_;
        }
    }
    empty = (histograms.size()==0);
    // Use the average duration for this annotation
    if (!empty)
        duration /= histograms.size();
    return Json::object {
        {"rendering", Json::object {
                {"render_time_histogram", histograms }
            }
        }
    };
}

Json::object TelemetryJson(const ProngCache& prong_cache,
                           const SerializedAnnotation& annotation,
                           const ProtobufSerialization& fidelity_params,
                           const ExtraUploadInfo& device_info,
                           bool& empty)
{
    Duration duration;
    auto report = TelemetryReportJson(prong_cache, annotation, empty, duration);
    return Json::object {
        {"context", TelemetryContextJson(prong_cache, annotation,
                                         fidelity_params, device_info, duration)},
        {"report", report}
    };

}

/*static*/ void GESerializer::SerializeEvent(const ProngCache& prongs,
                                       const ProtobufSerialization& fidelity_params,
                                       const ExtraUploadInfo& device_info,
                                       std::string& evt_json_ser) {
    Json session_context = Json::object {
        {"device", json_utils::DeviceSpecJson(device_info)},
        {"game_sdk_info", GameSdkInfoJson(device_info)},
        {"time_period",
            Json::object {
                {"start_time", TimeToRFC3339(prongs.time().start)},
                {"end_time", TimeToRFC3339(prongs.time().end)}
            }
        }
    };
    std::vector<Json::object> telemetry;
    // Loop over unique annotations
    std::set<SerializedAnnotation> annotations;
    for(auto& p: prongs.prongs())
        annotations.insert(p->annotation_);
    for(auto& a: annotations) {
        bool empty;
        auto tel = TelemetryJson(prongs, a, fidelity_params, device_info, empty);
        if(!empty)
            telemetry.push_back(tel);
    }
    Json upload_telemetry_request = Json::object {
        {"name", json_utils::GetResourceName(device_info)},
        {"session_context", session_context},
        {"telemetry", telemetry}
    };
    evt_json_ser = upload_telemetry_request.dump();
}

} // namespace tuningfork
