
#include "tuningfork_internal.h"

#include <sstream>

#define LOG_TAG "TuningFork:DebugUpload"
#include "Log.h"
#include "tuningfork_utils.h"
#include "web.h"

#include "../../third_party/json11/json11.hpp"
#include "modp_b64.h"

namespace tuningfork {

using namespace json11;

const char kRpcName[] = ":debugInfo";

const int kSuccessCodeMin = 200;
const int kSuccessCodeMax = 299;

const int MAX_N_FP_FILES = 32;

bool encode_b64(const ProtobufSerialization& params, std::string& result) {
    size_t len = params.size();
    std::string dest(modp_b64_encode_len(len), '\0');
    size_t encoded_len = modp_b64_encode(const_cast<char*>(dest.c_str()),
                                         reinterpret_cast<const char*>(params.data()),
                                         len);
    if (encoded_len != -1) {
        dest.resize(encoded_len);
        result = dest;
        return true;
    } else {
        return false;
    }
}
// Add a serialization of the params as 'field_name' in 'obj'.
static void add_params(const ProtobufSerialization& params, Json::object& obj,
    const std::string& field_name) {
    std::string dest;
    if (encode_b64(params, dest)) {
        obj[field_name] = dest;
    }
}


static std::string RequestJson() {
    Json::object request_obj = Json::object {};
    // Add in other info for the debug monitor
    ProtobufSerialization descriptor_ser;
    if (apk_utils::GetAssetAsSerialization("tuningfork/dev_tuningfork.descriptor",
                                           descriptor_ser)) {
        add_params(descriptor_ser, request_obj, "dev_tuningfork_descriptor");
    }
    ProtobufSerialization settings_ser;
    if (apk_utils::GetAssetAsSerialization("tuningfork/tuningfork_settings.bin",
                                           settings_ser)) {
        add_params(settings_ser, request_obj, "settings");
    }
    std::vector<std::string> fps;
    for(int i=0; i<MAX_N_FP_FILES; ++i) {
        std::stringstream str;
        str << "dev_tuningfork_fidelityparams_" << i << ".bin";
        ProtobufSerialization fp;
        if (FindFidelityParamsInApk(str.str(), fp)==TFERROR_OK) {
            std::string fp_b64;
            encode_b64(fp, fp_b64);
            fps.push_back(fp_b64);
        }
        else {
            // Allow starting at 0 or 1
            if (i!=0)
                break;
        }
    }
    if (fps.size()>0) {
        request_obj["fidelity_param_sets"] = Json(fps);
    }
    Json request_json = request_obj;
    auto result = request_json.dump();
    ALOGV("%s request body: %s", kRpcName, result.c_str());
    return result;
}

TFErrorCode UploadDebugInfo(Request& request) {
    int response_code;
    std::string body;
    TFErrorCode ret = request.Send(kRpcName, RequestJson(),
                                   response_code, body);
    if (ret!=TFERROR_OK)
        return ret;

    if (response_code>=kSuccessCodeMin && response_code<=kSuccessCodeMax)
        return TFERROR_OK;
    else
        return TFERROR_BAD_PARAMETER;
}

} // namespace tuningfork
