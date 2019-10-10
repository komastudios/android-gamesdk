#include "ge_backend.h"

#define LOG_TAG "TuningFork.GE"
#include "Log.h"

#include "tuningfork/protobuf_util.h"

#include "jni_helper.h"
#include "runnable.h"
#include "web.h"
#include "tuningfork_utils.h"

namespace tuningfork {

constexpr int kUploadCheckIntervalMs = 1000;
constexpr int kRequestTimeoutMs = 10000;

class UltimateUploader : public Runnable {
    const TFCache* persister_;
    WebRequest request_;
public:
    UltimateUploader(const TFCache* persister, const WebRequest& request)
            : Runnable(kUploadCheckIntervalMs), persister_(persister), request_(request) {}
    void DoWork() override {
        CheckUploadPending();
    }
    void Run() override {
        Runnable::Run();
    }
    void CheckUploadPending() {
        CProtobufSerialization uploading_hists_ser;
        if (persister_->get(HISTOGRAMS_UPLOADING, &uploading_hists_ser,
                persister_->user_data)==TFERROR_OK) {
            std::string request_json = ToString(uploading_hists_ser);
            CProtobufSerialization_Free(&uploading_hists_ser);
            int response_code = -1;
            std::string body;
            ALOGV("Got UPLOADING histograms: %s", request_json.c_str());
            TFErrorCode ret = request_.Send(request_json, response_code, body);
            if (ret==TFERROR_OK) {
                ALOGI("UPLOAD request returned %d %s", response_code, body.c_str());
                if (response_code==200)
                    persister_->remove(HISTOGRAMS_UPLOADING, persister_->user_data);
            }
            else
                ALOGW("Error %d when sending UPLOAD request\n%s", ret, request_json.c_str());
        }
        else {
            ALOGV("No upload pending");
        }
    }
};

TFErrorCode GEBackend::Init(const JniCtx& jni, const Settings& settings,
                           const ExtraUploadInfo& extra_upload_info) {

    if (settings.base_uri.empty()) {
        ALOGW("The base URI in Tuning Fork TFSettings is invalid");
        return TFERROR_BAD_PARAMETER;
    }
    if (settings.api_key.empty()) {
        ALOGW("The API key in Tuning Fork TFSettings is invalid");
        return TFERROR_BAD_PARAMETER;
    }

    std::stringstream upload_uri;
    upload_uri << settings.base_uri;
    upload_uri << json_utils::GetResourceName(extra_upload_info);
    upload_uri << ":uploadTelemetry";
    WebRequest rq(jni, upload_uri.str(), settings.api_key, kRequestTimeoutMs);

    persister_ = settings.c_settings.persistent_cache;

    // TODO(b/140367226): Initialize a Java JobScheduler if we can

    if( ultimate_uploader_.get() == nullptr) {
        ultimate_uploader_ = std::make_shared<UltimateUploader>(persister_, rq);
        ultimate_uploader_->Start();
    }

    return TFERROR_OK;
}

GEBackend::~GEBackend() {}

TFErrorCode GEBackend::Process(const std::string &evt_ser) {

    ALOGV("GEBackend::Process %s",evt_ser.c_str());

    // Save event to file
    CProtobufSerialization uploading_hists_ser;
    ToCProtobufSerialization(evt_ser, uploading_hists_ser);
    auto ret = persister_->set(HISTOGRAMS_UPLOADING, &uploading_hists_ser,
                               persister_->user_data);

    CProtobufSerialization_Free(&uploading_hists_ser);

    return ret;
}

} //namespace tuningfork {
