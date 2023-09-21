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

#include <stdlib.h>

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

static std::string RequestJson(const RequestInfo& request_info) {
    Json::object request_obj =
        Json::object{{"name", json_utils::GetResourceName(request_info)},
                     {"device_spec", json_utils::DeviceSpecJson(request_info)}};

    Json request = request_obj;
    auto result = request.dump();
    ALOGV("Request body: %s", result.c_str());
    return result;
}

static TuningFork_ErrorCode DecodeResponse(const std::string& response,
                                           QLTimePredictions& predictions) {
    using namespace json11;
    if (response.empty()) {
        ALOGW("Empty response to predictQualityLevels");
        return TUNINGFORK_ERROR_PREDICT_QUALITY_LEVELS_RESPONSE_ERROR;
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

    if (jresponse.find("qualityLevels") == jresponse.end()) {
        ALOGE("Quality levels not found in response");
        return TUNINGFORK_ERROR_PREDICT_QUALITY_LEVELS_INSUFFICIENT_DATA;
    }

    Json::array quality_levels = jresponse["qualityLevels"].array_items();
    if (quality_levels.empty()) {
        ALOGE("No quality levels reported");
        return TUNINGFORK_ERROR_PREDICT_QUALITY_LEVELS_INSUFFICIENT_DATA;
    }
    for (auto& quality_level : quality_levels) {
        // Fetch the fidelity parameters first for this level
        ProtobufSerialization fps;
        std::string sfps;
        Json::object ql_response = quality_level.object_items();

        if (ql_response.find("serializedFidelityParameters") !=
            ql_response.end()) {
            sfps = ql_response["serializedFidelityParameters"].string_value();
        }
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

        // Fetch the predicted frame time for this level
        std::string s_time;

        if (ql_response.find("expectedFrameTime") != ql_response.end()) {
            s_time = ql_response["expectedFrameTime"].string_value();
        }

        double parsed_dur_s = atof(s_time.c_str());
        if (parsed_dur_s > 1.0f || parsed_dur_s < 0.0f) {
            ALOGE("Invalid prediction times received");
            return TUNINGFORK_ERROR_PREDICT_QUALITY_LEVELS_PARSE_ERROR;
        }
        // Convert to micro seconds.
        uint32_t pred_frame_time_us = (parsed_dur_s * 1000000.0);

        predictions.emplace_back(fps, pred_frame_time_us);
    }
    return TUNINGFORK_ERROR_OK;
}

TuningFork_ErrorCode HttpBackend::PredictQualityLevels(
    HttpRequest& request, QLTimePredictions& predictions) {
    int response_code = 200;
    std::string body;
    TuningFork_ErrorCode ret = request.Send(
        kRpcName, RequestJson(RequestInfo::CachedValue()), response_code, body);

    if (ret != TUNINGFORK_ERROR_OK) return ret;
    if (response_code < kSuccessCodeMin || response_code > kSuccessCodeMax)
        return TUNINGFORK_ERROR_PREDICT_QUALITY_LEVELS_RESPONSE_ERROR;

    return DecodeResponse(body, predictions);
}

}  // namespace tuningfork
