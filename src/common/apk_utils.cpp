#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#define LOG_TAG "TuningForkUtils"

#include "Log.h"
#include "apk_utils.h"
#include "jni/jni_wrap.h"

namespace apk_utils {

NativeAsset::NativeAsset(const char* name) {
    auto java_asset_manager = gamesdk::jni::AppContext().getAssets();
    AAssetManager* mgr = AAssetManager_fromJava(
        gamesdk::jni::Env(), (jobject)java_asset_manager.obj_);
    asset = AAssetManager_open(mgr, name, AASSET_MODE_BUFFER);
    if (asset == nullptr) {
        ALOGW("Can't find %s in APK", name);
    }
}
NativeAsset::NativeAsset(NativeAsset&& a) : asset(a.asset) {
    a.asset = nullptr;
}
NativeAsset& NativeAsset::operator=(NativeAsset&& a) {
    asset = a.asset;
    a.asset = nullptr;
    return *this;
}
NativeAsset::~NativeAsset() {
    if (asset != nullptr) {
        AAsset_close(asset);
    }
}
bool NativeAsset::IsValid() { return asset != nullptr; }
NativeAsset::operator AAsset*() { return asset; }

}  // namespace apk_utils