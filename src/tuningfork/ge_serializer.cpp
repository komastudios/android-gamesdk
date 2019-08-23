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
#include "annotation_util.h"

#define LOG_TAG "TuningFork"
#include "Log.h"

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

system_clock::time_point RFC3339ToTime(const std::string& s) {
    // TODO(b/139924411): date deserialization
    return {};
}

std::string DurationToSecondsString(Duration d) {
    std::stringstream str;
    str << (duration_cast<nanoseconds>(d).count()/1000000000.0) << 's';
    return str.str();
}
Duration StringToDuration(const std::string& s) {
    double d;
    std::stringstream str(s);
    str >> d;
    return nanoseconds(static_cast<uint64_t>(d*1000000000));
}

std::string B64Encode(const std::vector<uint8_t>& bytes) {
    if (bytes.size()==0) return "";
    std::string enc(modp_b64_encode_len(bytes.size()), ' ');
    size_t l = modp_b64_encode(const_cast<char*>(enc.c_str()),
                               reinterpret_cast<const char*>(bytes.data()), bytes.size());
    enc.resize(l);
    return enc;
}
std::vector<uint8_t> B64Decode(const std::string& s) {
    if (s.length()==0) return std::vector<uint8_t>();
    std::vector<uint8_t> ret(modp_b64_decode_len(s.length()));
    size_t l = modp_b64_decode(reinterpret_cast<char*>(ret.data()), s.c_str(), s.length());
    ret.resize(l);
    return ret;
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
        {"rendering",
            Json::object {
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

std::string Serialize(std::vector<uint32_t> vs) {
    std::stringstream str;
    str << "[";
    for(auto& v: vs) {
        str << " " << v;
    }
    str << "]";
    return str.str();
}
namespace {
struct Hist {
    ProtobufSerialization annotation;
    ProtobufSerialization fidelity_params;
    uint64_t instrument_id;
    Duration duration;
    std::vector<uint32_t> counts;
};
}

/*static*/ TFErrorCode GESerializer::DeserializeAndMerge(const std::string& evt_json_ser,
                                                         IdProvider& id_provider,
                                                         ProngCache& pc) {
    std::string err;
    Json in = Json::parse(evt_json_ser, err);
    if (!err.empty()) {
        ALOGE("Failed to deserialize %s\n%s", evt_json_ser.c_str(), err.c_str());
        return TFERROR_BAD_PARAMETER;
    }

    // Deserialize
    auto start = RFC3339ToTime(in["session_context"]["time_period"]["start_time"].string_value());
    auto end = RFC3339ToTime(in["session_context"]["time_period"]["start_time"].string_value());
    std::vector<Hist> hists;
    for (auto& telemetry : in["telemetry"].array_items()) {
        // Context
        auto& context = telemetry["context"];
        if (context.is_null())
            return TFERROR_BAD_PARAMETER;
        auto annotation = B64Decode(context["annotations"].string_value());
        auto fps = B64Decode(context["tuning_parameters"]["serialized_fidelity_parameters"]
                                 .string_value());
        auto duration = StringToDuration(context["duration"].string_value());
        // Report
        auto& report = telemetry["report"]["rendering"];
        if (report.is_null())
            return TFERROR_BAD_PARAMETER;
        for(auto& histogram: report["render_time_histogram"].array_items()) {
            uint64_t instrument_id = histogram["instrument_id"].int_value();
            std::vector<uint32_t> cs;
            for(auto& c: histogram["counts"].array_items()) {
                cs.push_back( c.int_value() );
            }
            if (cs.size()>0)
                hists.push_back({annotation, fps, instrument_id, duration, cs});
        }
    }

    // Merge
    for(auto& h: hists) {
        uint64_t id;
        auto annotation_id = id_provider.DecodeAnnotationSerialization(h.annotation);
        if (annotation_id== annotation_util::kAnnotationError)
            return TFERROR_BAD_PARAMETER;
        auto r = id_provider.MakeCompoundId(h.instrument_id, annotation_id, id);
        if (r!=TFERROR_OK)
            return r;
        auto p = pc.Get(id);
        if (p==nullptr)
            return TFERROR_BAD_PARAMETER;
        auto& orig_counts = p->histogram_.buckets();
        p->histogram_.AddCounts(h.counts);
    }
    return TFERROR_OK;
}

} // namespace tuningfork
