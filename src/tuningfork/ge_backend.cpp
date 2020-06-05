#include "ge_backend.h"

#define LOG_TAG "TuningFork.GE"
#include "Log.h"

#include "protobuf_util.h"

#include "runnable.h"
#include "web.h"
#include "tuningfork_utils.h"

namespace tuningfork {

constexpr Duration kUploadCheckInterval = std::chrono::seconds(10);
constexpr Duration kRequestTimeout = std::chrono::seconds(10);

const char kRpcName[] = ":uploadTelemetry";

class UltimateUploader : public Runnable {
    const TuningFork_Cache* persister_;
    WebRequest request_;
    public:
    UltimateUploader(const TuningFork_Cache* persister, const WebRequest& request)
            : Runnable(), persister_(persister), request_(request) {}
    Duration DoWork() override {
        CheckUploadPending();
        return kUploadCheckInterval;
    }
    void Run() override {
        Runnable::Run();
    }
    bool CheckUploadPending() {
        TuningFork_CProtobufSerialization uploading_hists_ser;
        if (persister_->get(HISTOGRAMS_UPLOADING, &uploading_hists_ser,
                persister_->user_data)==TUNINGFORK_ERROR_OK) {
            std::string request_json = ToString(uploading_hists_ser);
            TuningFork_CProtobufSerialization_free(&uploading_hists_ser);
            int response_code = -1;
            std::string body;
            ALOGV("Got UPLOADING histograms: %s", request_json.c_str());
            TuningFork_ErrorCode ret = request_.Send(kRpcName, request_json, response_code, body);
            if (ret==TUNINGFORK_ERROR_OK) {
                ALOGI("UPLOAD request returned %d %s", response_code, body.c_str());
                if (response_code==200) {
                    persister_->remove(HISTOGRAMS_UPLOADING, persister_->user_data);
                    return true;
                }
            }
            else
                ALOGW("Error %d when sending UPLOAD request\n%s", ret, request_json.c_str());
        }
        else {
            ALOGV("No upload pending");
            return true;
        }
        return false;
    }
};

TuningFork_ErrorCode GEBackend::Init(const Settings& settings,
                           const ExtraUploadInfo& extra_upload_info) {

    if (settings.EndpointUri().empty()) {
        ALOGW("The base URI in Tuning Fork TuningFork_Settings is invalid");
        return TUNINGFORK_ERROR_BAD_PARAMETER;
    }
    if (settings.api_key.empty()) {
        ALOGW("The API key in Tuning Fork TuningFork_Settings is invalid");
        return TUNINGFORK_ERROR_BAD_PARAMETER;
    }

    Request rq(extra_upload_info, settings.EndpointUri(), settings.api_key, kRequestTimeout);
    WebRequest web_request(rq);

    persister_ = settings.c_settings.persistent_cache;

    // TODO(b/140367226): Initialize a Java JobScheduler if we can

    if( ultimate_uploader_.get() == nullptr) {
        ultimate_uploader_ = std::make_shared<UltimateUploader>(persister_, web_request);
        ultimate_uploader_->Start();
    }

    return TUNINGFORK_ERROR_OK;
}

GEBackend::~GEBackend() {}

TuningFork_ErrorCode GEBackend::Process(const std::string &evt_ser) {

    ALOGV("GEBackend::Process %s",evt_ser.c_str());

    // Save event to file
    TuningFork_CProtobufSerialization uploading_hists_ser;
    ToCProtobufSerialization(evt_ser, uploading_hists_ser);
    auto ret = persister_->set(HISTOGRAMS_UPLOADING, &uploading_hists_ser,
                               persister_->user_data);

    TuningFork_CProtobufSerialization_free(&uploading_hists_ser);

    return ret;
}

void GEBackend::KillThreads() {
    if (ultimate_uploader_)
        ultimate_uploader_->Stop();
}

} //namespace tuningfork {
