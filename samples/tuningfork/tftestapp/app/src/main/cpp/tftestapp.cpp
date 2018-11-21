#include <jni.h>
#include "tuningfork/protobuf_util.h"
#include "tuningfork/tuningfork.h"
#include "full/tuningfork.pb.h"
#include "full/tuningfork_clearcut_log.pb.h"
#include "full/tuningfork_extensions.pb.h"
#include <android/log.h>

using ::com::google::tuningfork::FidelityParams;
using ::com::google::tuningfork::Settings;
using ::com::google::tuningfork::Annotation;
using ::logs::proto::tuningfork::TuningForkLogEvent;
using ::logs::proto::tuningfork::TuningForkHistogram;

namespace tf = tuningfork;

namespace {
struct HistogramSettings {
    float start, end;
    int nBuckets;
};
Settings TestSettings(Settings::AggregationStrategy::Submission method, int n_ticks, int n_keys,
                      std::vector<int> annotation_size,
                      const std::vector<HistogramSettings>& hists = {}) {
    // Make sure we set all required fields
    Settings s;
    s.mutable_aggregation_strategy()->set_method(method);
    s.mutable_aggregation_strategy()->set_intervalms_or_count(n_ticks);
    s.mutable_aggregation_strategy()->set_max_instrumentation_keys(n_keys);
    for(int i=0;i<annotation_size.size();++i)
        s.mutable_aggregation_strategy()->add_annotation_enum_size(annotation_size[i]);
    int i=0;
    for(auto& h: hists) {
        auto sh = s.add_histograms();
        sh->set_bucket_min(h.start);
        sh->set_bucket_max(h.end);
        sh->set_n_buckets(h.nBuckets);
        sh->set_instrument_key(i++);
    }
    return s;
}
class LogcatBackend : public tf::Backend {
public:
    ~LogcatBackend() override {}
    bool GetFidelityParams(tf::ProtobufSerialization &fidelity_params, size_t timeout_ms) override {
        FidelityParams p;
        p.SetExtension(lod, LOD_1);
        fidelity_params = tf::Serialize(p);
        return true;
    }
    bool Process(const tf::ProtobufSerialization &tuningfork_log_event) override {
        TuningForkLogEvent evt;
        tf::Deserialize(tuningfork_log_event, evt);
        std::string m = evt.DebugString();
        __android_log_print(ANDROID_LOG_INFO, "TuningFork", "Event (size=%zu) :\n %s",
                            tuningfork_log_event.size(), m.c_str());
        return true;
    }
};
LogcatBackend myBackend;

static int sLevel = Level_MIN;
void SetAnnotations() {
    __android_log_print(ANDROID_LOG_DEBUG, "TuningFork", "Setting level to %d", sLevel);
    if(Level_IsValid(sLevel)) {
        Annotation a;
        a.SetExtension(level, (Level)sLevel);
        tf::SetCurrentAnnotation(tf::Serialize(a));
    }
}

} // namespace {

extern "C" {

JNIEXPORT void JNICALL
Java_com_google_tuningfork_TFTestActivity_nInit(JNIEnv */*env*/, jobject /*activity*/) {
    Settings s = TestSettings(Settings::AggregationStrategy::TIME_BASED, 1000, 1, {4},
                              {{14, 19, 10}});
    tf::Init(tf::Serialize(s),&myBackend);
    tf::ProtobufSerialization params;
    if(!tf::GetFidelityParameters(params, 1000)) {
        __android_log_print(ANDROID_LOG_WARN, "TuningFork", "Could not get FidelityParams");
    }
    else {
        FidelityParams p;
        tf::Deserialize(params, p);
        std::string s = p.DebugString();
        __android_log_print(ANDROID_LOG_INFO, "TuningFork", "Got FidelityParams: %s", s.c_str());

    }
    SetAnnotations();
}
JNIEXPORT void JNICALL
Java_com_google_tuningfork_TFTestActivity_nOnChoreographer(JNIEnv */*env*/, jobject /*activity*/,
                                                           jlong /*frameTimeNanos*/) {
    tf::FrameTick(tf::TFTICK_SYSCPU);
    // After 600 ticks, switch to the next level
    static int tick_count = 0;
    ++tick_count;
    if(tick_count>=600) {
        ++sLevel;
        if(sLevel>Level_MAX) sLevel = Level_MIN;
        SetAnnotations();
        tick_count = 0;
    }
}

}
