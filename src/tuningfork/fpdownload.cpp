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

#include "tuningfork_internal.h"

#include <sstream>
#include <string>

#define LOG_TAG "TuningFork:FPDownload"
#include "Log.h"
#include "tuningfork_utils.h"
#include "web.h"

#include "../../third_party/json11/json11.hpp"
#include "modp_b64.h"

namespace tuningfork {

using namespace json11;

const char url_rpcname[] = ":generateTuningParameters";

static void add_params(const ProtobufSerialization& params, Json::object& obj,
    const std::string& field_name) {
    size_t len = params.size();
    std::string dest(modp_b64_encode_len(len), '\0');
    size_t encoded_len = modp_b64_encode(const_cast<char*>(dest.c_str()),
                                         reinterpret_cast<const char*>(params.data()),
                                         len);
    if (encoded_len != -1) {
        dest.resize(encoded_len);
        obj[field_name] = dest;
    }
}

static std::string RequestJson(const ExtraUploadInfo& request_info,
                        const ProtobufSerialization* training_mode_params) {
    Json::object request_obj = Json::object {
        {"name", json_utils::GetResourceName(request_info)},
        {"device_spec", json_utils::DeviceSpecJson(request_info)}};
    if (training_mode_params != nullptr) {
        add_params(*training_mode_params, request_obj, "serialized_training_mode_parameters");
    }
    Json request = request_obj;
    auto result = request.dump();
    ALOGV("Request body: %s", result.c_str());
    return result;
}
static TFErrorCode DecodeResponse(const std::string& response, std::vector<uint8_t>& fps,
                           std::string& experiment_id) {
    using namespace json11;
    ALOGI("Response: %s", response.c_str());
    std::string err;
    Json jresponse = Json::parse(response, err);
    if (!err.empty()) {
        ALOGE("Parsing error: %s", err.c_str());
        return TFERROR_NO_FIDELITY_PARAMS;
    }
    ALOGV("Response, deserialized: %s", jresponse.dump().c_str());
    if (!jresponse.is_object()) {
        ALOGE("Response not object");
        return TFERROR_NO_FIDELITY_PARAMS;
    }
    auto& outer = jresponse.object_items();
    auto iparameters = outer.find("parameters");
    if (iparameters==outer.end()) {
        ALOGE("No parameters");
        return TFERROR_NO_FIDELITY_PARAMS;
    }
    auto& params = iparameters->second;
    if (!params.is_object()) {
        ALOGE("parameters not object");
        return TFERROR_NO_FIDELITY_PARAMS;
    }
    auto& inner = params.object_items();
    auto iexperiment_id = inner.find("experimentId");
    if (iexperiment_id==inner.end()) {
        ALOGE("No experimentId");
        return TFERROR_NO_FIDELITY_PARAMS;
    }
    if (!iexperiment_id->second.is_string()) {
        ALOGE("experimentId is not a string");
        return TFERROR_NO_FIDELITY_PARAMS;
    }
    experiment_id = iexperiment_id->second.string_value();
    auto ifps = inner.find("serializedFidelityParameters");
    if (ifps==inner.end()) {
        ALOGE("No serializedFidelityParameters");
        return TFERROR_NO_FIDELITY_PARAMS;
    }
    if (!ifps->second.is_string()) {
        ALOGE("serializedFidelityParameters is not a string");
        return TFERROR_NO_FIDELITY_PARAMS;
    }
    std::string sfps = ifps->second.string_value();
    fps.resize(modp_b64_decode_len(sfps.length()));
    if (modp_b64_decode((char*)fps.data(), sfps.c_str(), sfps.length())==-1) {
        ALOGE("Can't decode base 64 FPs");
        return TFERROR_NO_FIDELITY_PARAMS;
    }
    return TFERROR_OK;
}

static TFErrorCode DownloadFidelityParams(const JniCtx& jni, const std::string& uri,
                                   const std::string& api_key,
                                   const ExtraUploadInfo& request_info,
                                   int timeout_ms,
                                   const ProtobufSerialization* training_mode_fps,
                                   ProtobufSerialization& fps,
                                   std::string& experiment_id) {
    WebRequest wq(jni, uri, api_key, std::chrono::milliseconds(timeout_ms));
    int response_code;
    std::string body;
    TFErrorCode ret =wq.Send(RequestJson(request_info, training_mode_fps),
                             response_code, body);
    if (ret!=TFERROR_OK)
        return ret;

    if (response_code==200)
        ret = DecodeResponse(body, fps, experiment_id);
    else
        ret = TFERROR_NO_FIDELITY_PARAMS;

    return ret;
}

TFErrorCode ParamsLoader::GetFidelityParams(const JniCtx& jni,
                                            const ExtraUploadInfo& info,
                                            const std::string& base_url,
                                            const std::string& api_key,
                                            const ProtobufSerialization* training_mode_fps,
                                            ProtobufSerialization& fidelity_params,
                                            std::string& experiment_id,
                                            uint32_t timeout_ms) {
    std::stringstream url;
    url << base_url;
    url << json_utils::GetResourceName(info);
    url << url_rpcname;
    return DownloadFidelityParams(jni, url.str(), api_key, info, timeout_ms,
                                  training_mode_fps, fidelity_params, experiment_id);
}

} // namespace tuningfork
