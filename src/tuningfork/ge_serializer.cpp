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

#ifdef ANDROID_GNUSTL
// This is needed for date.h when using gnustl in the NDK
namespace std {

template <typename T>
std::string to_string(T value)
{
    std::stringstream os;
    os << value;
    return os.str();
}
template <typename T>
std::wstring to_wstring(T value)
{
    std::wstringstream os;
    os << value;
    return os.str();
}
long double stold( const std::string& str, std::size_t* pos = 0 ) {
    long double d;
    std::stringstream is(str);
    auto p0 = is.tellg();
    is >> d;
    if (pos != nullptr) {
        *pos = is.tellg() - p0;
    }
    return d;
    }

} // namespace std
#endif
// TODO(b/140155101): Move the date library into aosp/external
#include "date/date.h"

namespace tuningfork {

using namespace json11;
using namespace std::chrono;
using namespace date;

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
    //"{year}-{month}-{day}T{hour}:{min}:{sec}[.{frac_sec}]Z"
    auto const dp = date::floor<days>(tp);
    str << year_month_day(dp) << 'T' << make_time(tp-dp) << 'Z';
    return str.str();
}

system_clock::time_point RFC3339ToTime(const std::string& s) {
    std::istringstream str(s);
    int y, m, d;
    char delim;
    str >> y >> delim >> m >> delim >> d >> delim;
    int hours, mins;
    double secs;
    str >> hours >> delim >> mins >> secs;
    return sys_days(year_month_day(year(y), month(m), day(d)))
            + microseconds(static_cast<uint64_t>((hours*3600 + mins*60 + secs)*1000000.0));
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
    std::vector<Json::object> render_histograms;
    std::vector<Json::object> loading_histograms;
    std::vector<Json::object> loading_events;
    duration = Duration::zero();
    for(auto& p: prong_cache.prongs()) {
        if (p.get()!=nullptr && p->Count()>0 && p->annotation_==annotation) {
            if (p->histogram_.IsAutoRanging()) {
                std::vector<int> events; // Ignore fractional milliseconds
                for(auto& c: p->histogram_.samples())
                    events.push_back(static_cast<int>(c));
                loading_events.push_back(Json::object{{"times_ms", events}});
            }
            else {
                std::vector<int32_t> counts;
                for (auto &c: p->histogram_.buckets())
                    counts.push_back(static_cast<int32_t>(c));
                Json::object o {
                    {"counts",        counts},
                    {"range",         Json::object{
                            {"start_ms", p->histogram_.StartMs()},
                            {"end_ms",   p->histogram_.EndMs()}
                        }
                    }};
                if (p->IsLoading()) {
                    loading_histograms.push_back(o);
                }
                else {
                    o["instrument_id"] = p->instrumentation_key_;
                    render_histograms.push_back(o);
                }
            }
            duration += p->duration_;
        }
    }
    int total_size = render_histograms.size() + loading_histograms.size() + loading_events.size();
    empty = (total_size==0);
    // Use the average duration for this annotation
    if (!empty)
        duration /= total_size;
    Json::object ret;
    if (render_histograms.size()>0) {
        ret["rendering"] = Json::object {
            {"render_time_histogram", render_histograms }
        };
    }
    if (loading_histograms.size()>0 || loading_events.size()>0) {
        Json::object loading;
        if (loading_histograms.size()>0) {
            loading["loading_time_histogram"] = loading_histograms;
        }
        if (loading_events.size()>0) {
            loading["loading_events"] = loading_events;
        }
        ret["loading"] = loading;
    }
    return ret;
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
    for(auto& p: prongs.prongs()) {
        if (p.get() != nullptr)
            annotations.insert(p->annotation_);
    }
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
