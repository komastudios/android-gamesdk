/*
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

#include "tuningfork/protobuf_util.h"
#include "tuningfork/tuningfork_extra.h"
#include "swappy/swappy.h"
#include "full/tuningfork.pb.h"
#include "full/tuningfork_clearcut_log.pb.h"
#include "full/dev_tuningfork.pb.h"
#include <sstream>
#include <jni.h>
#include <android/native_window_jni.h>

#define LOG_TAG "tftestapp"
#include "Log.h"
#include "Renderer.h"

using ::com::google::tuningfork::FidelityParams;
using ::com::google::tuningfork::Settings;
using ::com::google::tuningfork::Annotation;
using ::logs::proto::tuningfork::TuningForkLogEvent;
using ::logs::proto::tuningfork::TuningForkHistogram;

namespace proto_tf = com::google::tuningfork;
namespace tf = tuningfork;
using namespace samples;

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

std::string ReplaceReturns(const std::string& s) {
    std::string r = s;
    for (int i = 0; i < r.length(); ++i) {
        if (r[i] == '\n') r[i] = ',';
        if (r[i] == '\r') r[i] = ' ';
    }
    return r;
}

std::string PrettyPrintTuningForkLogEvent(const TuningForkLogEvent& evt) {
    std::stringstream eventStr;
    eventStr << "TuningForkLogEvent {\n";
    if (evt.has_fidelityparams()) {
        FidelityParams p;
        p.ParseFromArray(evt.fidelityparams().c_str(), evt.fidelityparams().length());
        eventStr << "  fidelityparams : " << ReplaceReturns(p.DebugString()) << "\n";
    }
    for (int i = 0; i < evt.histograms_size(); ++i) {
        auto &h = evt.histograms(i);
        Annotation ann;
        ann.ParseFromArray(h.annotation().c_str(), h.annotation().length());
        bool first = true;
        eventStr << "  histogram {\n";
        eventStr << "    instrument_id : " << h.instrument_id() << "\n";
        eventStr << "    annotation : " << ReplaceReturns(ann.DebugString()) << "\n    counts : ";
        eventStr << "[";
        for (int j = 0; j < h.counts_size(); ++j) {
            if (first)
                first = false;
            else
                eventStr << ",";
            eventStr << h.counts(j);
        }
        eventStr << "]\n  }\n";
    }
    if (evt.has_experiment_id()) {
        eventStr << "  experiment_id : " << evt.experiment_id() << "\n";
    }
    eventStr << "}";
    return eventStr.str();
}

void UploadCallback(const CProtobufSerialization *tuningfork_log_event) {
    if(tuningfork_log_event) {
        TuningForkLogEvent evt;
        evt.ParseFromArray(tuningfork_log_event->bytes, tuningfork_log_event->size);
        ALOGI("%s", PrettyPrintTuningForkLogEvent(evt).c_str());
    }
}

static int sLevel = proto_tf::Level_MIN;
extern "C"
void SetAnnotations() {
    if(proto_tf::Level_IsValid(sLevel)) {
        Annotation a;
        a.set_level((proto_tf::Level)sLevel);
        a.set_next_level((proto_tf::Level)sLevel);
        auto ser = tf::CSerialize(a);
        TuningFork_setCurrentAnnotation(&ser);
        if(ser.dealloc) ser.dealloc(ser.bytes);
    }
}

void SetFidelityParams(const CProtobufSerialization& params) {
    FidelityParams p;
    // Set default values
    p.set_num_spheres(20);
    p.set_tesselation_percent(50);
    std::vector<uint8_t> params_ser(params.bytes, params.bytes + params.size);
    tf::Deserialize(params_ser, p);
    std::string s = p.DebugString();
    ALOGI("Using FidelityParams: %s", ReplaceReturns(s).c_str());
    int nSpheres = p.num_spheres();
    int tesselation = p.tesselation_percent();
    Renderer::getInstance()->setQuality(nSpheres, tesselation);
}

} // anonymous namespace

extern "C" {

JNIEXPORT void JNICALL
Java_com_google_tuningfork_TFTestActivity_initTuningFork(JNIEnv *env, jobject activity) {
    Settings s = TestSettings(Settings::AggregationStrategy::TICK_BASED,
                              100, // Time in ms between events
                              3, // Number of instrumentation keys (SYSCPU and SYSGPU and choreographer)
                              {4, 4}, // annotation enum sizes (4 levels)
                              {{14, // histogram minimum delta time in ms
                                19, // histogram maximum delta time in ms
                                70} // number of buckets between the max and min (there will be
                                  //   2 more for out-of-bounds ticks, too)
                              });
    CProtobufSerialization ser;
    if(!TuningFork_findSettingsInAPK(env, activity, &ser)) {
        ALOGW("Falling back to default tuningfork settings");
        ser = tf::CSerialize(s);
    }

    Swappy_init(env, activity);
    if(!(Swappy_isEnabled() && TuningFork_initWithSwappy(&ser, env, activity, "libnative-lib.so", SetAnnotations)))
        TuningFork_init(&ser, env, activity);
    TuningFork_setUploadCallback(UploadCallback);
    if (ser.dealloc) ser.dealloc(ser.bytes);

    int nfps=0;
    CProtobufSerialization defaultParams = {};
    // Try to use the middle fidelity params in the APK as the default
    TuningFork_findFidelityParamsInAPK(env, activity, NULL, &nfps);
    if (nfps>0) {
        std::vector<CProtobufSerialization> fps(nfps);
        TuningFork_findFidelityParamsInAPK(env, activity, fps.data(), &nfps);
        int chosen = nfps/2;
        ALOGI("Using params from dev_tuningfork_fidelityparams_%d.bin as default", chosen);
        for (int i=0;i<nfps;++i) {
            if (i == chosen) defaultParams = fps[i];
            else if (fps[i].dealloc) fps[i].dealloc(fps[i].bytes);
        }
    }
    CProtobufSerialization params = {};
    if (!TuningFork_getFidelityParameters(&defaultParams, &params, 1000)) {
        ALOGW("Could not get FidelityParams from server");
        SetFidelityParams(defaultParams);
        if (defaultParams.dealloc) defaultParams.dealloc(defaultParams.bytes);
    }
    else {
        SetFidelityParams(params);
        if (params.dealloc) params.dealloc(params.bytes);
    }
    SetAnnotations();
}
JNIEXPORT void JNICALL
Java_com_google_tuningfork_TFTestActivity_onChoreographer(JNIEnv */*env*/, jclass clz, jlong /*frameTimeNanos*/) {
    TuningFork_frameTick(2);
    // After 600 ticks, switch to the next level
    static int tick_count = 0;
    ++tick_count;
    if(tick_count>=600) {
        ++sLevel;
        if(sLevel>proto_tf::Level_MAX) sLevel = proto_tf::Level_MIN;
        SetAnnotations();
        tick_count = 0;
    }
}
JNIEXPORT void JNICALL
Java_com_google_tuningfork_TFTestActivity_resize(JNIEnv *env, jclass /*clz*/, jobject surface, jint width, jint height) {
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    Renderer::getInstance()->setWindow(window,
                                       static_cast<int32_t>(width),
                                       static_cast<int32_t>(height));
}
JNIEXPORT void JNICALL
Java_com_google_tuningfork_TFTestActivity_clearSurface(JNIEnv */*env*/, jclass /*clz*/ ) {
    Renderer::getInstance()->setWindow(nullptr, 0, 0);
}
JNIEXPORT void JNICALL
Java_com_google_tuningfork_TFTestActivity_start(JNIEnv */*env*/, jclass /*clz*/ ) {
    Renderer::getInstance()->start();
}
JNIEXPORT void JNICALL
Java_com_google_tuningfork_TFTestActivity_stop(JNIEnv */*env*/, jclass /*clz*/ ) {
    Renderer::getInstance()->stop();
}

}
