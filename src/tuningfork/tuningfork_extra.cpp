
#include "tuningfork/tuningfork_extra.h"
#include "tuningfork_internal.h"

#include <cinttypes>
#include <dlfcn.h>
#include <memory>
#include <vector>

#define LOG_TAG "TuningFork"
#include "Log.h"
#include "swappy/swappy_extra.h"
#include <android/asset_manager.h> 
#include <android/asset_manager_jni.h>

namespace {

using PFN_Swappy_initTracer = void (*)(const SwappyTracer* tracer);

class DynamicSwappy {
    typedef void* Handle;
    Handle lib_;
    PFN_Swappy_initTracer inject_tracer_;
public:
    DynamicSwappy(const char* libraryName) {
        static char defaultLibNames[][20] = {"libgamesdk.so", "libswappy.so", "libunity.so"};
        static std::vector<const char*> libNames = {libraryName, NULL, defaultLibNames[0], defaultLibNames[1], defaultLibNames[2]};
        for(auto libName: libNames) {
            lib_ = dlopen(libName, RTLD_NOW);
            if( lib_ ) {
                inject_tracer_ = (PFN_Swappy_initTracer)dlsym(lib_, "Swappy_injectTracer");
                if(inject_tracer_) return;
            }
        }
        ALOGW("Couldn't find Swappy_injectTracer");
        lib_ = nullptr;
    }
    void injectTracer(const SwappyTracer* tracer) const {
        if(inject_tracer_)
            inject_tracer_(tracer);
    }
    bool valid() const { return lib_ != nullptr; }
};

class SwappyTuningFork {
    DynamicSwappy swappy_;
    SwappyTracer trace_;
    void (*annotation_callback_)();
public:
    SwappyTuningFork(const CProtobufSerialization& settings_ser, JNIEnv* env, jobject activity, void (*cbk)(), const char* libName) : swappy_(libName), trace_({}), annotation_callback_(cbk) {
        trace_.startFrame = startFrame;
        trace_.userData = this;
        if(swappy_.valid()) {
            TuningFork_init(&settings_ser, env, activity);
            swappy_.injectTracer(&trace_);
        }
    }
    bool valid() const { return swappy_.valid(); }

    // Swappy trace callbacks
    static void startFrame(void* userPtr, int currentFrame, long currentFrameTimeStampMillis) {
        SwappyTuningFork* _this = (SwappyTuningFork*)userPtr;
        _this->annotation_callback_();
        TuningFork_frameTick(TFTICK_SYSCPU);
    }

    // Static methods
    static std::unique_ptr<SwappyTuningFork> s_instance_;

    static bool GetSettingsSerialization(JNIEnv* env, jobject activity, CProtobufSerialization& settings_ser) {
        jclass cls = env->FindClass("android/content/Context");
        jmethodID get_assets = env->GetMethodID(cls, "getAssets", "()Landroid/content/res/AssetManager;");
        if(get_assets==nullptr) {
            ALOGE("No Context.getAssets() method");
            return false;
        }
        auto javaMgr = env->CallObjectMethod(activity, get_assets);
        if (javaMgr == nullptr) {
            ALOGE("No java asset manager");
            return false;
        }
        AAssetManager* mgr = AAssetManager_fromJava(env, javaMgr);
        if (mgr == nullptr) {
            ALOGE("No asset manager");
            return false;
        }
        AAsset* asset = AAssetManager_open(mgr, "tuningfork/tuningfork_settings.bin", AASSET_MODE_BUFFER);
        if (asset == nullptr) {
            ALOGW("Can't find tuningfork/tuningfork_settings.bin in APK");
            return false;
        }
        ALOGI("Using settings from tuningfork/tuningfork_settings.bin");
        // Get serialized settings from assets
        uint64_t size = AAsset_getLength64(asset);
        settings_ser.bytes = (uint8_t*)::malloc(size);
        memcpy(settings_ser.bytes, AAsset_getBuffer(asset), size);
        settings_ser.size = size;
        settings_ser.dealloc = ::free;
        AAsset_close(asset);
        return true;
    }
    static bool Init(const CProtobufSerialization* settings, JNIEnv* env,
                     jobject activity, const char* libName, void (*annotation_callback)()) {
        s_instance_ = std::unique_ptr<SwappyTuningFork>(
            new SwappyTuningFork(*settings, env, activity, annotation_callback, libName));
        return s_instance_->valid();
    }
};

std::unique_ptr<SwappyTuningFork> SwappyTuningFork::s_instance_;

} // anonymous namespace

extern "C"
bool TuningFork_findSettingsInAPK(JNIEnv* env, jobject activity, CProtobufSerialization* settings_ser) {
    if(settings_ser)
        return SwappyTuningFork::GetSettingsSerialization(env, activity, *settings_ser);
    else
        return false;
}

extern "C"
bool TuningFork_initWithSwappy(const CProtobufSerialization* settings, JNIEnv* env,
                               jobject activity, const char* libraryName,
                               void (*annotation_callback)()) {
    return SwappyTuningFork::Init(settings, env, activity, libraryName, annotation_callback);
}

extern "C"
void TuningFork_setUploadCallback(void(*cbk)(const CProtobufSerialization*)) {
    tuningfork::SetUploadCallback(cbk);
}
