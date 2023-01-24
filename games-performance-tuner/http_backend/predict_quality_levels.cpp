/*
 * Copyright 2023 The Android Open Source Project
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

#include "../../third_party/json11/json11.hpp"
#include "Log.h"
#include "core/tuningfork_internal.h"
#include "core/tuningfork_utils.h"
#include "http_backend.h"
#include "http_request.h"
#include "json_serializer.h"
#include "modp_b64.h"

namespace tuningfork {

using namespace json11;

const char kRpcName[] = ":predictQualityLevels";

const int kSuccessCodeMin = 200;
const int kSuccessCodeMax = 299;

static std::string RequestJson(const RequestInfo& request_info,
                               uint32_t target_frame_time_ms) {
    Json::object request_obj = Json::object{
        {"name", json_utils::GetResourceName(request_info)},
        {"device_spec", json_utils::DeviceSpecJson(request_info)},
        {"target_frame_time",
         JsonSerializer::DurationJsonFromMillis(target_frame_time_ms)}};

    Json request = request_obj;
    auto result = request.dump();
    ALOGV("Request body: %s", result.c_str());
    return result;
}

static TuningFork_ErrorCode DecodeResponse(const std::string& response,
                                           ProtobufArray& fidelity_params) {
    using namespace json11;
    ALOGE("response is: %s", response.c_str());
    if (response.empty()) {
        ALOGW("Empty response to predictQualityLevels");
        return TUNINGFORK_ERROR_NO_FIDELITY_PARAMS;
    } else {
        ALOGI("Response to predictQualityLevels: %s",
              g_verbose_logging_enabled ? response.c_str()
                                        : LOGGING_PLACEHOLDER_TEXT);
    }
    std::string err;
    Json::object jresponse = Json::parse(response, err).object_items();
    if (!err.empty()) {
        ALOGE("Parsing error: %s", err.c_str());
        return TUNINGFORK_ERROR_PREDICT_QUALITY_LEVELS_PARSE_ERROR;
    }

    if (jresponse.find("quality_levels") == jresponse.end()) {
        ALOGE("Quality levels not found in response");
        return TUNINGFORK_ERROR_PREDICT_QUALITY_LEVELS_PARSE_ERROR;
    }

    Json::array quality_levels = jresponse["quality_levels"].array_items();
    for (auto& quality_level : quality_levels) {
        quality_level.object_items()
            .at("serialized_fidelity_parameters")
            .string_value();
        ProtobufSerialization fps;

        std::string sfps = quality_level.object_items()
                               .at("serialized_fidelity_parameters")
                               .string_value();
        fps.resize(modp_b64_decode_len(sfps.length()));
        int sz =
            modp_b64_decode((char*)fps.data(), sfps.c_str(), sfps.length());
        if (sz == -1) {
            ALOGE("Can't decode base 64 FPs");
            return TUNINGFORK_ERROR_PREDICT_QUALITY_LEVELS_PARSE_ERROR;
        } else {
            // Delete extra trailing zero bytes at the end
            fps.erase(fps.begin() + sz, fps.end());
        }
        fidelity_params.push_back(fps);
    }
    return TUNINGFORK_ERROR_OK;
}

TuningFork_ErrorCode HttpBackend::PredictQualityLevels(
    HttpRequest& request, ProtobufArray& fidelity_params,
    uint32_t target_frame_time_ms) {
    int response_code = 200;
    std::string body;
    TuningFork_ErrorCode ret = request.Send(
        kRpcName, RequestJson(RequestInfo::CachedValue(), target_frame_time_ms),
        response_code, body);
    if (ret != TUNINGFORK_ERROR_OK) return ret;

    if (response_code >= kSuccessCodeMin && response_code <= kSuccessCodeMax)
        ret = DecodeResponse(body, fidelity_params);
    else
        ret = TUNINGFORK_ERROR_GENERATE_TUNING_PARAMETERS_RESPONSE_NOT_SUCCESS;

    return ret;
}

}  // namespace tuningfork