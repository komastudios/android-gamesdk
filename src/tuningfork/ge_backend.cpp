#include "ge_backend.h"

#define LOG_TAG "TuningFork.GE"
#include "Log.h"

#include "tuningfork/protobuf_util.h"

#include "jni_helper.h"
#include "runnable.h"
#include "web.h"
#include "tuningfork_utils.h"

namespace tuningfork {

class UltimateUploader : public Runnable {
    const TFCache* persister_;
    WebRequest request_;
public:
    UltimateUploader(const TFCache* persister, const WebRequest& request)
            : Runnable(1000), persister_(persister), request_(request) {}
    void DoWork() override {
        CheckUploadPending();
    }
    void Run() override {
        request_.AttachToThread();
        Runnable::Run();
        request_.DetachFromThread();
    }
    void CheckUploadPending() {
        CProtobufSerialization uploading_hists_ser;
        if (persister_->get(HISTOGRAMS_UPLOADING, &uploading_hists_ser,
                persister_->user_data)==TFERROR_OK) {
            std::string request_json = ToString(uploading_hists_ser);
            CProtobufSerialization_Free(&uploading_hists_ser);
            int response_code;
            std::string body;
            int timeout_ms = 10000;
            ALOGI("Got UPLOADING histograms: %s", request_json.c_str());
            TFErrorCode ret = request_.Send(request_json, response_code, body);
            ALOGI("Request returned %d\n%s", response_code, body.c_str());
            if (response_code==200)
                persister_->remove(HISTOGRAMS_UPLOADING, persister_->user_data);
        }
        else {
            ALOGV("No upload pending");
        }
    }
};

TFErrorCode GEBackend::Init(JNIEnv* env, jobject context, const Settings& settings,
                           const ExtraUploadInfo& extra_upload_info) {

    std::string base_uri = settings.base_uri;
    if (base_uri.empty())
        base_uri = "https://performanceparameters.googleapis.com/v1/";
    if (settings.api_key.empty()) {
        ALOGW("The API key in Tuning Fork TFSettings is invalid");
        return TFERROR_BAD_PARAMETER;
    }

    std::stringstream upload_uri;
    upload_uri << base_uri;
    upload_uri << json_utils::GetResourceName(extra_upload_info);
    upload_uri << ":uploadTelemetry";
    WebRequest rq(env, context, upload_uri.str(), settings.api_key, 10000);

    persister_ = settings.persistent_cache;

    // TODO(b/140367226): Initialize a Java JobScheduler if we can

    if( ultimate_uploader_.get() == nullptr) {
        ultimate_uploader_ = std::make_shared<UltimateUploader>(persister_, rq);
        ultimate_uploader_->Start();
    }

    return TFERROR_OK;
}

GEBackend::~GEBackend() {}

TFErrorCode GEBackend::Process(const std::string &evt_ser) {

    ALOGI("GEBackend::Process %s",evt_ser.c_str());

    // Save event to file
    CProtobufSerialization uploading_hists_ser;
    ToCProtobufSerialization(evt_ser, uploading_hists_ser);
    auto ret = persister_->set(HISTOGRAMS_UPLOADING, &uploading_hists_ser,
                               persister_->user_data);

    CProtobufSerialization_Free(&uploading_hists_ser);

    return ret;
}

} //namespace tuningfork {
