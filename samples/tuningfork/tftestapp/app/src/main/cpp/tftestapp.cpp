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
#include "tuningfork/tuningfork.h"
#include "swappy/swappyGL.h"
#include "swappy/swappyGL_extra.h"
#include "full/tuningfork.pb.h"
#include "full/dev_tuningfork.pb.h"
#include <sstream>
#include <condition_variable>
#include <jni.h>
#include <unistd.h>
#include <android/native_window_jni.h>

#define LOG_TAG "tftestapp"
#include "Log.h"
#include "Renderer.h"

using ::com::google::tuningfork::FidelityParams;
using ::com::google::tuningfork::Settings;
using ::com::google::tuningfork::Annotation;

namespace proto_tf = com::google::tuningfork;
namespace tf = tuningfork;
using namespace samples;

bool swappy_enabled = false;

namespace {

constexpr TFInstrumentKey TFTICK_CHOREOGRAPHER = TFTICK_USERDEFINED_BASE;

std::string ReplaceReturns(const std::string& s) {
    std::string r = s;
    for (int i=0; i<r.length(); ++i) {
        if (r[i]=='\n') r[i] = ',';
        if (r[i]=='\r') r[i] = ' ';
    }
    return r;
}

void SplitAndLog(const std::string& s) {
    std::stringstream to_log;
    const int max_line_len = 300;
    int nparts = s.length()/max_line_len+1;
    for (int i=0;i<nparts;++i) {
        std::stringstream msg;
        msg << "(TGE" << (i+1) << '/' << nparts << ')';
        msg << s.substr(i*max_line_len, max_line_len);
        ALOGI("%s", msg.str().c_str());
    }
}

void UploadCallback(const char *tuningfork_log_event, size_t n) {
    if(tuningfork_log_event) {
        std::string evt(tuningfork_log_event, n);
        SplitAndLog(evt);
    }
}

static int sLevel = proto_tf::LEVEL_1;
extern "C"
void SetAnnotations() {
    if(proto_tf::Level_IsValid(sLevel)) {
        Annotation a;
        a.set_level((proto_tf::Level)sLevel);
        auto next_level = sLevel + 1;
        a.set_next_level((proto_tf::Level)(next_level>proto_tf::Level_MAX?1:next_level));
        auto ser = tf::CProtobufSerialization_Alloc(a);
        TuningFork_setCurrentAnnotation(&ser);
        CProtobufSerialization_Free(&ser);
    }
}

std::mutex mutex;
std::condition_variable cv;
bool setFPs = false;
extern "C" void SetFidelityParams(const CProtobufSerialization* params) {
    FidelityParams p;
    // Set default values
    p.set_num_spheres(20);
    p.set_tesselation_percent(50);
    std::vector<uint8_t> params_ser(params->bytes, params->bytes + params->size);
    tf::Deserialize(params_ser, p);
    std::string s = p.DebugString();
    ALOGI("Using FidelityParams: %s", ReplaceReturns(s).c_str());
    int nSpheres = p.num_spheres();
    int tesselation = p.tesselation_percent();
    Renderer::getInstance()->setQuality(nSpheres, tesselation);
    setFPs = true;
    cv.notify_one();
}

void WaitForFidelityParams() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, []{ return setFPs;});
}

std::thread tf_thread;
jobject tf_activity;

void InitTf(JNIEnv* env, jobject activity) {
    SwappyGL_init(env, activity);
    swappy_enabled = SwappyGL_isEnabled();
    TFSettings settings {};
    if (swappy_enabled) {
        settings.swappy_tracer_fn = &SwappyGL_injectTracer;
    }
    settings.fidelity_params_callback = SetFidelityParams;
    TFErrorCode err = TuningFork_init(&settings, env, activity);
    if (err==TFERROR_OK) {
        TuningFork_setUploadCallback(UploadCallback);
        SetAnnotations();
    } else {
        ALOGW("Error initializing TuningFork: %d", err);
    }
    // If we don't wait for fidelity params here, the download thread will set them after we
    //   have already started rendering with a different set of parameters.
    // In a real game, we'd initialize all the other assets before waiting.
    WaitForFidelityParams();
}

void InitTfFromNewThread(JavaVM* vm) {
    JNIEnv *env;
    int status = vm->AttachCurrentThread(&env, NULL);
    InitTf(env, tf_activity);
    vm->DetachCurrentThread();
}
} // anonymous namespace

extern "C" {

// initFromNewThread parameter is for testing
JNIEXPORT void JNICALL
Java_com_tuningfork_demoapp_TFTestActivity_initTuningFork(
    JNIEnv *env, jobject activity, jboolean initFromNewThread) {
    if(initFromNewThread) {
        tf_activity = env->NewGlobalRef(activity);
        JavaVM* vm;
        env->GetJavaVM(&vm);
        tf_thread = std::thread(InitTfFromNewThread, vm);
    } else {
        InitTf(env, activity);
    }
}

JNIEXPORT void JNICALL
Java_com_tuningfork_demoapp_TFTestActivity_onChoreographer(JNIEnv */*env*/, jclass clz, jlong /*frameTimeNanos*/) {
    TuningFork_frameTick(TFTICK_CHOREOGRAPHER);
    // After 600 ticks, switch to the next level
    static int tick_count = 0;
    ++tick_count;
    if(tick_count>=600) {
        ++sLevel;
        if(sLevel>proto_tf::Level_MAX) sLevel = proto_tf::LEVEL_1;
        SetAnnotations();
        tick_count = 0;
    }
}
JNIEXPORT void JNICALL
Java_com_tuningfork_demoapp_TFTestActivity_resize(JNIEnv *env, jclass /*clz*/, jobject surface, jint width, jint height) {
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    Renderer::getInstance()->setWindow(window,
                                       static_cast<int32_t>(width),
                                       static_cast<int32_t>(height));
}
JNIEXPORT void JNICALL
Java_com_tuningfork_demoapp_TFTestActivity_clearSurface(JNIEnv */*env*/, jclass /*clz*/ ) {
    Renderer::getInstance()->setWindow(nullptr, 0, 0);
}
JNIEXPORT void JNICALL
Java_com_tuningfork_demoapp_TFTestActivity_start(JNIEnv */*env*/, jclass /*clz*/ ) {
    Renderer::getInstance()->start();
}
JNIEXPORT void JNICALL
Java_com_tuningfork_demoapp_TFTestActivity_stop(JNIEnv */*env*/, jclass /*clz*/ ) {
    Renderer::getInstance()->stop();
    // Call flush here to upload any histograms when the app goes to the background.
    auto ret = TuningFork_flush();
    ALOGI("TuningFork_flush returned %d", ret);
}

JNIEXPORT void JNICALL
Java_com_tuningfork_demoapp_TFTestActivity_raiseSignal(JNIEnv * env, jclass clz, jint signal) {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    ALOGI("raiseSignal %d: [pid: %d], [tid: %d], [thread_id: %s])",
            signal, getpid(), gettid(), ss.str().c_str());
    raise(signal);
}
}
